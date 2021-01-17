/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
	_job_pools(allocator),
	_per_thread_jobs(allocator),
	_threads(allocator),
	_allocator(allocator)
{
	_main_queue.jobs = Queue<JobPair, Allocator>(allocator);
	_main_queue.read_write_lock = UniquePtr<EA::Thread::Futex, Allocator>(GAFF_ALLOCT(EA::Thread::Futex, _allocator));

	_thread_data.terminate = false;
	_thread_data.pause = true;
}

template <class Allocator>
JobPool<Allocator>::~JobPool(void)
{
	destroy();
}

template <class Allocator>
bool JobPool<Allocator>::init(int32_t num_threads, ThreadInitFunc init)
{
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

		JobQueue& thread_queue = _per_thread_jobs[_threads[i].GetId()];
		thread_queue.read_write_lock = UniquePtr<EA::Thread::Futex, Allocator>(GAFF_ALLOCT(EA::Thread::Futex, _allocator));
		thread_queue.jobs = Queue<JobPair, Allocator>(_allocator);
	}

	_main_thread_id = EA::Thread::GetThreadId();
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
	_job_pools.erase(_job_pools.begin(), _job_pools.end());
}

template <class Allocator>
void JobPool<Allocator>::run(void)
{
	_thread_data.pause = false;
}

template <class Allocator>
void JobPool<Allocator>::addPool(const HashStringView32<>& name, int32_t max_concurrent_threads)
{
	HashString32<Allocator> copy(name);
	GAFF_ASSERT(_job_pools.find(copy) == _job_pools.end());

	JobQueue& job_queue = _job_pools[copy];
	job_queue.jobs = Queue<JobPair, Allocator>(_allocator);
	job_queue.read_write_lock = UniquePtr<EA::Thread::Futex, Allocator>(GAFF_ALLOCT(EA::Thread::Futex, _allocator));
	job_queue.thread_lock = UniquePtr<EA::Thread::Semaphore, Allocator>(GAFF_ALLOCT(EA::Thread::Semaphore, _allocator, max_concurrent_threads));
}

template <class Allocator>
void JobPool<Allocator>::addJobs(const JobData* jobs, int32_t num_jobs, Counter** counter, Hash32 pool)
{
	if (!jobs || num_jobs == 0) {
		return;
	}

	JobQueue* job_queue = nullptr;

	if (pool.getHash() == 0) {
		job_queue = &_main_queue;

	} else {
		const auto it = _job_pools.find_as(pool, eastl::less_2< Hash32, const HashString32<Allocator> >());
		GAFF_ASSERT(it != _job_pools.end());
		job_queue = &it->second;
	}

	Counter* cnt = nullptr;

	if (counter) {
		if (*counter) {
			cnt = *counter;
		} else {
			*counter = cnt = GAFF_ALLOCT(Counter, _allocator, 0);
		}

		*cnt += num_jobs;
	}

	job_queue->read_write_lock->Lock();

	for (int32_t i = 0; i < num_jobs; ++i) {
		GAFF_ASSERT(jobs[i].job_func);
		job_queue->jobs.emplace(JobPair(jobs[i], cnt));
	}

	job_queue->read_write_lock->Unlock();
}

template <class Allocator>
void JobPool<Allocator>::addJobs(const JobData* jobs, int32_t num_jobs, Counter& counter, Hash32 pool)
{
	Counter* cnt = &counter;
	addJobs(jobs, num_jobs, &cnt, pool);
}

template <class Allocator>
void JobPool<Allocator>::addJobsForAllThreads(const JobData* jobs, int32_t num_jobs, Counter** counter)
{
	Counter* cnt = nullptr;

	if (counter) {
		if (*counter) {
			cnt = *counter;
		}
		else {
			*counter = cnt = GAFF_ALLOCT(Counter, _allocator, 0);
		}

		// +1 for main thread.
		*cnt += num_jobs * static_cast<int32_t>(_threads.size()) + 1;
	}

	for (auto& job_queue : _per_thread_jobs) {
		job_queue.second.read_write_lock->Lock();

		for (int32_t i = 0; i < num_jobs; ++i) {
			job_queue.second.jobs.emplace(JobPair(jobs[i], cnt));
		}

		job_queue.second.read_write_lock->Unlock();
	}

	_main_queue.read_write_lock->Lock();

	for (int32_t i = 0; i < num_jobs; ++i) {
		_main_queue.jobs.emplace(JobPair(jobs[i], cnt));
	}

	_main_queue.read_write_lock->Unlock();
}

template <class Allocator>
void JobPool<Allocator>::addJobsForAllThreads(const JobData* jobs, int32_t num_jobs, Counter& counter)
{
	Counter* cnt = &counter;
	addJobsForAllThreads(jobs, num_jobs, &cnt);
}

template <class Allocator>
void JobPool<Allocator>::waitForAndFreeCounter(Counter* counter)
{
	waitForCounter(*counter);
	freeCounter(counter);
}

template <class Allocator>
void JobPool<Allocator>::waitForCounter(const Counter& counter)
{
	while (counter > 0 && !_thread_data.terminate) {
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
void JobPool<Allocator>::helpWhileWaiting(EA::Thread::ThreadId thread_id, const Counter& counter)
{
	while (counter > 0) {
		doAJob(thread_id);
	}
}

template <class Allocator>
void JobPool<Allocator>::helpWhileWaiting(const Counter& counter)
{
	while (counter > 0) {
		doAJob();
	}
}

template <class Allocator>
void JobPool<Allocator>::helpAndFreeCounter(EA::Thread::ThreadId thread_id, Counter* counter)
{
	helpWhileWaiting(thread_id, *counter);
	freeCounter(counter);
}

template <class Allocator>
void JobPool<Allocator>::helpAndFreeCounter(Counter* counter)
{
	helpWhileWaiting(*counter);
	freeCounter(counter);
}

template <class Allocator>
bool JobPool<Allocator>::help(EA::Thread::ThreadId thread_id, eastl::chrono::milliseconds ms)
{
	// Do jobs until we can't do any more.

	// Do per-thread jobs first.
	JobQueue& per_thread_job_queue = _per_thread_jobs[thread_id];
	bool earlied_out = ProcessJobQueue(per_thread_job_queue, thread_id, ms);

	if (earlied_out) {
		return true;
	}

	// Start with the other pools first. Presumably they don't lead to pool zero never getting reached.
	for (auto& pair : _job_pools) {
		typename JobPool<Allocator>::JobQueue& job_queue = pair.second;

		if (job_queue.thread_lock->Wait(EA::Thread::kTimeoutImmediate) >= 0) {
			earlied_out = ProcessJobQueue(job_queue, thread_id, ms);
			job_queue.thread_lock->Post();
		}

		if (earlied_out) {
			return true;
		}
	}

	// Conversely, hopefully the main pool doesn't constantly get new tasks pushed to it and stop the other pools from being processed.
	return ProcessJobQueue(_main_queue, thread_id, ms);
}

template <class Allocator>
bool JobPool<Allocator>::help(eastl::chrono::milliseconds ms)
{
	return help(EA::Thread::GetThreadId(), ms);
}

template <class Allocator>
void JobPool<Allocator>::doAJob(EA::Thread::ThreadId thread_id)
{
	for (auto& pair : _job_pools) {
		typename JobPool<Allocator>::JobQueue& job_queue = pair.second;

		if (job_queue.thread_lock->Wait(EA::Thread::kTimeoutImmediate) >= 0) {
			if (job_queue.jobs.empty()) {
				job_queue.thread_lock->Post();

			} else {
				JobPair job = job_queue.jobs.front();
				job_queue.jobs.pop();
				DoJob(thread_id, job);

				job_queue.thread_lock->Post();
				return;
			}
		}
	}

	_main_queue.read_write_lock->Lock();

	if (_main_queue.jobs.empty()) {
		_main_queue.read_write_lock->Unlock();

	} else {
		JobPair job = _main_queue.jobs.front();
		_main_queue.jobs.pop();

		_main_queue.read_write_lock->Unlock();
		DoJob(thread_id, job);
	}
}

template <class Allocator>
void JobPool<Allocator>::doAJob(void)
{
	doAJob(EA::Thread::GetThreadId());
}

template <class Allocator>
int32_t JobPool<Allocator>::getNumTotalThreads(void) const
{
	return static_cast<int32_t>(_threads.size() + 1);
}

template <class Allocator>
void JobPool<Allocator>::getThreadIDs(EA::Thread::ThreadId* out) const
{
	for (int32_t i = 0; i < static_cast<int32_t>(_threads.size()); ++i) {
		out[i] = _threads[i].GetId();
	}

	out[_threads.size()] = _main_thread_id;
}

template <class Allocator>
EA::Thread::ThreadId JobPool<Allocator>::getMainThreadID(void) const
{
	return _main_thread_id;
}

template <class Allocator>
bool JobPool<Allocator>::ProcessJobQueue(typename JobPool<Allocator>::JobQueue& job_queue, EA::Thread::ThreadId thread_id, eastl::chrono::milliseconds ms)
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
			DoJob(thread_id, job);
		}

		if (ms > eastl::chrono::milliseconds::zero() && (eastl::chrono::high_resolution_clock::now() - start) >= ms) {
			return true;
		}
	}

	return false;
}

template <class Allocator>
void JobPool<Allocator>::DoJob(EA::Thread::ThreadId thread_id, JobPair& job)
{
	const uintptr_t id_int = (uintptr_t)&thread_id;
	job.first.job_func(id_int, job.first.job_data);

	if (job.second) {
		//DebugPrintf("FUNC: %p\n", job.front);

		//if (!job.second->count) {
		//	DebugPrintf("JOB FAIL(%d) - %p:(%d)\n", Thread::GetCurrentThreadID(), job.second, job.second->count);
		//} else {
		//	DebugPrintf("JOB(%d) - %p:(%d)\n", Thread::GetCurrentThreadID(), job.second, job.second->count);
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

	const EA::Thread::ThreadId thread_id = EA::Thread::GetThreadId();
	const uintptr_t id_int = (uintptr_t)&thread_id;

	if (thread_data.init_func) {
		thread_data.init_func(id_int);
	}

	while (!thread_data.terminate) {
		if (!thread_data.pause) {
			job_pool->help(thread_id);
		}

		EA::Thread::ThreadSleep(); // Probably a good idea to give some time back to the CPU for other stuff.
	}

	return 0;
}
