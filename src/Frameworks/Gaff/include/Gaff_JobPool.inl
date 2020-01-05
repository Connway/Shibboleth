/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

template <class Allocator>
JobPool<Allocator>::JobPool(const Allocator& allocator):
	_job_pools(allocator), _threads(allocator),
	_allocator(allocator)
{
}

template <class Allocator>
JobPool<Allocator>::~JobPool(void)
{
	destroy();
}

template <class Allocator>
bool JobPool<Allocator>::init(int32_t num_pools, int32_t num_threads, ThreadInitFunc init)
{
	_job_pools.resize(num_pools + 1);

	for (JobQueue& job_queue : _job_pools) {
		job_queue.jobs = Queue<JobPair, Allocator>(_allocator);
		job_queue.read_write_lock = UniquePtr<EA::Thread::Futex, Allocator>(GAFF_ALLOCT(EA::Thread::Futex, _allocator));
		job_queue.thread_lock = UniquePtr<EA::Thread::Futex, Allocator>(GAFF_ALLOCT(EA::Thread::Futex, _allocator));
	}

	_thread_data.job_pool = this;
	_thread_data.init_func = init;
	_thread_data.terminate = false;
	_threads.resize(num_threads);

	for (int32_t i = 0; i < num_threads; ++i) {
		U8String<Allocator> thread_name(_allocator);
		thread_name.sprintf("Job Pool Worker Thread %i", i);

		EA::Thread::ThreadParameters thread_params;
		thread_params.mbDisablePriorityBoost = false;
		thread_params.mnAffinityMask = EA::Thread::kThreadAffinityMaskAny;
		thread_params.mnPriority = EA::Thread::kThreadPriorityDefault;
		thread_params.mnProcessor = EA::Thread::kProcessorDefault;
		thread_params.mnStackSize = 0;
		thread_params.mpName = thread_name.data();
		thread_params.mpStack = nullptr;

		const bool success = _threads[i].Begin(JobThread, &_thread_data, &thread_params) != EA::Thread::kThreadIdInvalid;

		if (!success) {
			destroy();
			return false;
		}
	}

	return true;
}

template <class Allocator>
void JobPool<Allocator>::destroy(void)
{
	_thread_data.terminate = true;

	for (EA::Thread::Thread& thread : _threads) {
		thread.WaitForEnd();
	}

	_threads.clear();
	_job_pools.clear();
}

template <class Allocator>
void JobPool<Allocator>::addJobs(const JobData* jobs, size_t num_jobs, Counter** counter, int32_t pool)
{
	GAFF_ASSERT(pool < _job_pools.size());
	GAFF_ASSERT(num_jobs);

	Counter* cnt = nullptr;

	if (counter) {
		if (*counter) {
			cnt = *counter;
		} else {
			*counter = cnt = GAFF_ALLOCT(Counter, _allocator, 0);
		}

		*cnt += static_cast<int32_t>(num_jobs);
	}

	typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[pool];

	job_queue.read_write_lock->Lock();
	//job_queue.jobs.reserve(job_queue.jobs.size() + num_jobs);

	for (size_t i = 0; i < num_jobs; ++i) {
		GAFF_ASSERT(jobs[i].job_func);
		job_queue.jobs.emplace_back(JobPair(jobs[i], cnt));
	}

	job_queue.read_write_lock->Unlock();
}

template <class Allocator>
void JobPool<Allocator>::addJobs(const JobData* jobs, size_t num_jobs, Counter& counter, int32_t pool)
{
	GAFF_ASSERT(pool < _job_pools.size());
	GAFF_ASSERT(num_jobs);

	counter += static_cast<int32_t>(num_jobs);

	typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[pool];

	job_queue.read_write_lock->Lock();
	//job_queue.jobs.reserve(job_queue.jobs.size() + num_jobs);

	for (size_t i = 0; i < num_jobs; ++i) {
		GAFF_ASSERT(jobs[i].job_func);
		job_queue.jobs.emplace_back(JobPair(jobs[i], &counter));
	}

	job_queue.read_write_lock->Unlock();
}

template <class Allocator>
void JobPool<Allocator>::waitForAndFreeCounter(Counter* counter)
{
	waitForCounter(counter);
	freeCounter(counter);
}

template <class Allocator>
void JobPool<Allocator>::waitForCounter(const Counter* counter)
{
	GAFF_ASSERT(counter);

	while (*counter > 0 && !_thread_data.terminate) {
		EA::Thread::ThreadSleep();
	}
}

template <class Allocator>
void JobPool<Allocator>::freeCounter(Counter* counter)
{
	GAFF_ASSERT(counter);
	GAFF_FREE(counter, _allocator);
}

template <class Allocator>
void JobPool<Allocator>::helpWhileWaiting(const Counter* counter)
{
	GAFF_ASSERT(counter);

	while (*counter > 0) {
		doAJob();
	}
}

template <class Allocator>
void JobPool<Allocator>::helpAndFreeCounter(Counter* counter)
{
	helpWhileWaiting(counter);
	freeCounter(counter);
}

template <class Allocator>
void JobPool<Allocator>::help(eastl::chrono::milliseconds ms)
{
	// Do jobs until we can't do any more.
	bool earlied_out = false;

	// Start with the other pools first. Presumably they don't lead to pool zero never getting reached.
	for (size_t i = 1; i < _job_pools.size(); ++i) {
		typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[i];

		if (job_queue.thread_lock->TryLock()) {
			earlied_out = ProcessJobQueue(job_queue, ms);
			job_queue.thread_lock->Unlock();
		}
	}

	if (!earlied_out) {
		// Conversely, hopefully the main pool doesn't constantly get new tasks pushed to it and stop the other pools from being processed.
		typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[0];
		ProcessJobQueue(job_queue, ms);
	}
}

template <class Allocator>
void JobPool<Allocator>::doAJob(void)
{
	JobPair job;

	for (size_t i = 1; i < _job_pools.size(); ++i) {
		typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[i];

		if (job_queue.thread_lock->TryLock()) {
			if (job_queue.jobs.empty()) {
				job_queue.thread_lock->Unlock();

			} else {
				job = job_queue.jobs.front();
				job_queue.jobs.pop();
				DoJob(job);

				job_queue.thread_lock->Unlock();
				return;
			}
		}
	}

	typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[0];

	job_queue.read_write_lock->Lock();

	if (job_queue.jobs.empty()) {
		job_queue.read_write_lock->Unlock();

	} else {
		job = job_queue.jobs.front();
		job_queue.jobs.pop();

		job_queue.read_write_lock->Unlock();
		DoJob(job);
	}
}

template <class Allocator>
int32_t JobPool<Allocator>::getNumTotalThreads(void) const
{
	return static_cast<int32_t>(_threads.size());
}

template <class Allocator>
void JobPool<Allocator>::getThreadIDs(EA::Thread::ThreadId* out) const
{
	for (size_t i = 0; i < _threads.size(); ++i) {
		out[i] = _threads[i].GetId();
	}
}

template <class Allocator>
bool JobPool<Allocator>::ProcessJobQueue(typename JobPool<Allocator>::JobQueue& job_queue, eastl::chrono::milliseconds ms)
{
	auto start = eastl::chrono::high_resolution_clock::now();
	JobPair job;

	for (;;) {
		job_queue.read_write_lock->Lock();

		// No more jobs in the job queue.
		if (job_queue.jobs.empty()) {
			job_queue.read_write_lock->Unlock();
			break;

		// Process the job.
		} else {
			job = job_queue.jobs.front();
			job_queue.jobs.pop();

			job_queue.read_write_lock->Unlock();
			DoJob(job);
		}

		if (ms > eastl::chrono::milliseconds::zero() && (eastl::chrono::high_resolution_clock::now() - start) >= ms) {
			job_queue.read_write_lock->Unlock();
			return true;
		}
	}

	return false;
}

template <class Allocator>
void JobPool<Allocator>::DoJob(JobPair& job)
{
	job.first.job_func(job.first.job_data);

	if (job.second) {
		//Gaff::DebugPrintf("FUNC: %p\n", job.front);

		//if (!job.second->count) {
		//	Gaff::DebugPrintf("JOB FAIL(%d) - %p:(%d)\n", Gaff::Thread::GetCurrentThreadID(), job.second, job.second->count);
		//} else {
		//	Gaff::DebugPrintf("JOB(%d) - %p:(%d)\n", Gaff::Thread::GetCurrentThreadID(), job.second, job.second->count);
		//}

		GAFF_ASSERT(*job.second > 0);
		--(*job.second);
	}
}

template <class Allocator>
intptr_t JobPool<Allocator>::JobThread(void* data)
{
	ThreadData& thread_data = *reinterpret_cast<ThreadData*>(data);
	JobPool<Allocator>* job_pool = thread_data.job_pool;

	if (thread_data.init_func) {
		thread_data.init_func();
	}

	while (!thread_data.terminate) {
		job_pool->help();
		EA::Thread::ThreadSleep(); // Probably a good idea to give some time back to the CPU for other stuff.
	}

	return 0;
}
