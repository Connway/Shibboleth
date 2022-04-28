/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
	_threads(allocator),
	_allocator(allocator)
{
	_thread_data.job_pool = this;
	_thread_data.running = true;
	_thread_data.pause = true;
}

template <class Allocator>
JobPool<Allocator>::~JobPool(void)
{
	destroy();
}

template <class Allocator>
bool JobPool<Allocator>::init(int32_t num_threads, ThreadInitOrShutdownFunc init, ThreadInitOrShutdownFunc shutdown)
{
	// Add the default queue.
	JobQueue& job_queue = _job_pools[HashString32<Allocator>(_allocator)];
	job_queue.jobs = Queue<JobPair, Allocator>(_allocator);
	job_queue.read_write_lock = UniquePtr<EA::Thread::Mutex, Allocator>(GAFF_ALLOCT(EA::Thread::Mutex, _allocator));

	_main_thread_jobs.jobs = Queue<JobPair, Allocator>(_allocator);
	_main_thread_jobs.read_write_lock = UniquePtr<EA::Thread::Mutex, Allocator>(GAFF_ALLOCT(EA::Thread::Mutex, _allocator));

	_thread_data.init_func = init;
	_thread_data.shutdown_func = shutdown;
	_threads.resize(num_threads);

	for (int32_t i = 0; i < num_threads; ++i) {
		U8String<Allocator> thread_name(_allocator);
		thread_name.sprintf(u8"Job Pool Worker Thread %i", i);

		EA::Thread::ThreadParameters thread_params;
		thread_params.mbDisablePriorityBoost = false;
		thread_params.mnAffinityMask = EA::Thread::kThreadAffinityMaskAny;
		thread_params.mnPriority = EA::Thread::kThreadPriorityDefault;
		thread_params.mnProcessor = EA::Thread::kProcessorDefault;
		thread_params.mnStackSize = 0;
		thread_params.mpName = reinterpret_cast<const char*>(thread_name.data());
		thread_params.mpStack = nullptr;

		const EA::Thread::ThreadId thread_id = _threads[i].Begin(JobThread, &_thread_data, &thread_params);

		if (thread_id == EA::Thread::kThreadIdInvalid) {
			destroy();
			return false;
		}
	}

	_main_thread_id = EA::Thread::GetThreadId();
	return true;
}

template <class Allocator>
void JobPool<Allocator>::destroy(void)
{
	_thread_data.running = false;
	notifyThreads();

	for (EA::Thread::Thread& thread : _threads) {
		thread.WaitForEnd();
	}

	_threads.clear();
	_job_pools.erase(_job_pools.begin(), _job_pools.end());
}

template <class Allocator>
bool JobPool<Allocator>::isRunning(void) const
{
	return !_thread_data.pause;
}

template <class Allocator>
bool JobPool<Allocator>::isPaused(void) const
{
	return _thread_data.pause;
}

template <class Allocator>
void JobPool<Allocator>::pause(void)
{
	_thread_data.pause = true;
	notifyThreads();
}

template <class Allocator>
void JobPool<Allocator>::run(void)
{
	_thread_data.pause = false;
	notifyThreads();
}

template <class Allocator>
void JobPool<Allocator>::addPool(const HashStringView32<>& name, int32_t max_concurrent_threads)
{
	HashString32<Allocator> copy(name);
	GAFF_ASSERT(_job_pools.find(copy) == _job_pools.end());

	JobQueue& job_queue = _job_pools[copy];
	job_queue.jobs = Queue<JobPair, Allocator>(_allocator);
	job_queue.read_write_lock = UniquePtr<EA::Thread::Mutex, Allocator>(GAFF_ALLOCT(EA::Thread::Mutex, _allocator));
	job_queue.thread_lock = UniquePtr<EA::Thread::Semaphore, Allocator>(GAFF_ALLOCT(EA::Thread::Semaphore, _allocator, max_concurrent_threads));
}

template <class Allocator>
void JobPool<Allocator>::addMainThreadJobs(const JobData* jobs, int32_t num_jobs, Counter** counter)
{
	GAFF_ASSERT(jobs && num_jobs > 0);

	Counter* cnt = nullptr;

	if (counter) {
		if (*counter) {
			cnt = *counter;
		} else {
			*counter = cnt = GAFF_ALLOCT(Counter, _allocator, 0);
		}

		*cnt += num_jobs;
	}

	_main_thread_jobs.read_write_lock->Lock();

	for (int32_t i = 0; i < num_jobs; ++i) {
		GAFF_ASSERT(jobs[i].job_func);
		_main_thread_jobs.jobs.emplace(JobPair(jobs[i], cnt));
	}

	_main_thread_jobs.read_write_lock->Unlock();

	_num_main_thread_jobs += num_jobs;
	_num_jobs += num_jobs;

	// Main thread is not part of the thread pool. Don't post to the pool threads.
	//_thread_lock.Post(num_jobs);
}

template <class Allocator>
void JobPool<Allocator>::addMainThreadJobs(const JobData* jobs, int32_t num_jobs, Counter& counter)
{
	Counter* cnt = &counter;
	addMainThreadJobs(jobs, num_jobs, &cnt);
}

template <class Allocator>
void JobPool<Allocator>::addJobs(const JobData* jobs, int32_t num_jobs, Counter** counter, Hash32 pool)
{
	GAFF_ASSERT(jobs && num_jobs > 0);

	const auto it = _job_pools.find_as(pool, eastl::less_2< Hash32, const HashString32<Allocator> >());
	GAFF_ASSERT(it != _job_pools.end());
	JobQueue& job_queue = it->second;

	Counter* cnt = nullptr;

	if (counter) {
		if (*counter) {
			cnt = *counter;
		} else {
			*counter = cnt = GAFF_ALLOCT(Counter, _allocator, 0);
		}

		*cnt += num_jobs;
	}

	job_queue.read_write_lock->Lock();

	for (int32_t i = 0; i < num_jobs; ++i) {
		GAFF_ASSERT(jobs[i].job_func);
		job_queue.jobs.emplace(JobPair(jobs[i], cnt));
	}

	job_queue.read_write_lock->Unlock();

	_num_jobs += num_jobs;
	_thread_lock.Post(num_jobs);
}

template <class Allocator>
void JobPool<Allocator>::addJobs(const JobData* jobs, int32_t num_jobs, Counter& counter, Hash32 pool)
{
	Counter* cnt = &counter;
	addJobs(jobs, num_jobs, &cnt, pool);
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
	while (counter > 0 && _thread_data.running) {
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
void JobPool<Allocator>::waitForAllJobsToFinish(EA::Thread::ThreadId thread_id)
{
	// Do main thread jobs.
	if (thread_id == _main_thread_id) {
		while (doJobFromQueue(thread_id, _main_thread_jobs)) {
			--_num_main_thread_jobs;
			EA::Thread::ThreadSleep();
		}
	}

	while (_num_jobs > 0) {
		EA::Thread::ThreadSleep();
	}
}

template <class Allocator>
void JobPool<Allocator>::helpWhileWaiting(EA::Thread::ThreadId thread_id, const Counter& counter)
{
	while (counter > 0) {
		if (_thread_lock.Wait(EA::Thread::kTimeoutImmediate) >= 0) {
			if (!doAJob(thread_id)) {
				// We didn't do a job, put the count back.
				_thread_lock.Post();
			}
		}

		EA::Thread::ThreadSleep();
	}
}

template <class Allocator>
void JobPool<Allocator>::helpWhileWaiting(const Counter& counter)
{
	helpWhileWaiting(EA::Thread::GetThreadId(), counter);
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
	helpAndFreeCounter(EA::Thread::GetThreadId(), counter);
}

template <class Allocator>
void JobPool<Allocator>::help(EA::Thread::ThreadId thread_id, eastl::chrono::milliseconds ms)
{
	const auto start = eastl::chrono::high_resolution_clock::now();
	auto curr = start;

	do {
		if (_thread_lock.Wait(EA::Thread::kTimeoutImmediate) >= 0) {
			if (!doAJob(thread_id)) {
				// We didn't do a job, put the count back.
				_thread_lock.Post();
			}

		// Do main thread jobs.
		} else if (thread_id == _main_thread_id) {
			if (doJobFromQueue(thread_id, _main_thread_jobs)) {
				--_num_main_thread_jobs;
			}
		}

		curr = eastl::chrono::high_resolution_clock::now();
		EA::Thread::ThreadSleep();
	} while ((curr - start) < ms);
}

template <class Allocator>
void JobPool<Allocator>::help(eastl::chrono::milliseconds ms)
{
	help(EA::Thread::GetThreadId(), ms);
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
void JobPool<Allocator>::notifyThreads(void)
{
	_thread_lock.Post(static_cast<int>(_threads.size()));
}

template <class Allocator>
bool JobPool<Allocator>::doJobFromQueue(EA::Thread::ThreadId thread_id, JobQueue& job_queue)
{
	if (!job_queue.thread_lock || job_queue.thread_lock->Wait(EA::Thread::kTimeoutImmediate) >= 0) {
		job_queue.read_write_lock->Lock();

		if (job_queue.jobs.empty()) {
			job_queue.read_write_lock->Unlock();

			if (job_queue.thread_lock) {
				job_queue.thread_lock->Post();
			}

		} else {
			JobPair job = job_queue.jobs.front();
			job_queue.jobs.pop();
			job_queue.read_write_lock->Unlock();

			doJob(thread_id, job);

			if (job_queue.thread_lock) {
				job_queue.thread_lock->Post();
			}

			return true;
		}
	}

	return false;
}

template <class Allocator>
void JobPool<Allocator>::doJob(EA::Thread::ThreadId thread_id, JobPair& job)
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

	--_num_jobs;
}

template <class Allocator>
bool JobPool<Allocator>::doAJob(EA::Thread::ThreadId thread_id)
{
	// Do main thread jobs.
	if (thread_id == _main_thread_id) {
		if (doJobFromQueue(thread_id, _main_thread_jobs)) {
			--_num_main_thread_jobs;
			return true;
		}
	}

	for (auto& pair : _job_pools) {
		if (doJobFromQueue(thread_id, pair.second)) {
			return true;
		}
	}

	return false;
}

template <class Allocator>
intptr_t JobPool<Allocator>::JobThread(void* data)
{
	ThreadData& thread_data = *reinterpret_cast<ThreadData*>(data);
	JobPool<Allocator>* job_pool = thread_data.job_pool;

	const EA::Thread::ThreadId thread_id = EA::Thread::GetThreadId();
	const uintptr_t id_int = (uintptr_t)&thread_id;

	// Wait for the job pool to start.
	while (thread_data.pause) {
		EA::Thread::ThreadSleep();
	}

	if (thread_data.init_func) {
		thread_data.init_func(id_int);
	}

	while (thread_data.running) {
		while (thread_data.pause) {
			EA::Thread::ThreadSleep();
		}

		// Count of the semaphore is number of jobs, every time we do a job, grab a count from the semaphore.
		// No timout, so this shouldn't ever return < 0.
		if (job_pool->_thread_lock.Wait() >= 0) {
			// If _num_jobs is 0, then we have nothing to do and were woken up for some other reason.
			// Don't post the job count back.
			while (job_pool->_num_jobs > 0 && !job_pool->doAJob(thread_id)) {
				// We didn't do a job, put the count back.
				//job_pool->_thread_lock.Post();
			}
		}

		// Should we sleep here? Help keep CPU from being pegged all the time?
	}

	if (thread_data.shutdown_func) {
		thread_data.shutdown_func(id_int);
	}

	return 0;
}
