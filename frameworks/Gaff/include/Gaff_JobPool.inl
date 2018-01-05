/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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
		job_queue.read_write_lock = UniquePtr<SpinLock, Allocator>(GAFF_ALLOCT(Gaff::SpinLock, _allocator));
		job_queue.thread_lock = UniquePtr<SpinLock, Allocator>(GAFF_ALLOCT(Gaff::SpinLock, _allocator));
	}

	_thread_data.job_pool = this;
	_thread_data.init_func = init;
	_thread_data.terminate = false;
	_threads.resize(num_threads);

	for (int32_t i = 0; i < num_threads; ++i) {
		if (uv_thread_create(&_threads[i], JobThread, &_thread_data)) {
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

	for (uv_thread_t thread : _threads) {
		uv_thread_join(&thread);
	}

	_threads.clear();
	_job_pools.clear();
}

template <class Allocator>
void JobPool<Allocator>::addJobs(JobData* jobs, size_t num_jobs, Counter** counter, int32_t pool)
{
	GAFF_ASSERT(pool < _job_pools.size());
	GAFF_ASSERT(num_jobs);

	Counter* cnt = nullptr;

	if (counter) {
		if (*counter) {
			cnt = *counter;
		} else {
			*counter = cnt = GAFF_ALLOCT(Counter, _allocator);
		}

		*cnt = static_cast<int32_t>(num_jobs);
	}

	typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[pool];

	job_queue.read_write_lock->lock();
	//job_queue.jobs.reserve(job_queue.jobs.size() + num_jobs);

	for (size_t i = 0; i < num_jobs; ++i) {
		GAFF_ASSERT(jobs[i].job_func);
		job_queue.jobs.emplace_back(JobPair(jobs[i], cnt));
	}

	job_queue.read_write_lock->unlock();
}

template <class Allocator>
void JobPool<Allocator>::waitForAndFreeCounter(Counter* counter)
{
	waitForCounter(counter);
	freeCounter(counter);
}

template <class Allocator>
void JobPool<Allocator>::waitForCounter(Counter* counter)
{
	GAFF_ASSERT(counter);

	while (*counter > 0 && !_thread_data.terminate) {
		YieldThread();
	}
}

template <class Allocator>
void JobPool<Allocator>::freeCounter(Counter* counter)
{
	GAFF_ASSERT(counter);
	GAFF_FREE(counter, _allocator);
}

template <class Allocator>
void JobPool<Allocator>::helpWhileWaiting(Counter* counter)
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
void JobPool<Allocator>::helpUntilNoJobs(void)
{
	// Start with the other pools first. Presumably they don't lead to pool zero never getting reached.
	for (unsigned int i = 1; i < _job_pools.size(); ++i) {
		typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[i];

		if (job_queue.thread_lock->tryLock()) {
			ProcessJobQueue(job_queue);
			job_queue.thread_lock->unlock();
		}
	}

	// Conversely, hopefully the main pool doesn't constantly get new tasks pushed to it and stop the other pools from being processed.
	typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[0];
	ProcessMainJobQueue(job_queue);
}

template <class Allocator>
void JobPool<Allocator>::doAJob(void)
{
	JobPair job;

	for (size_t i = 1; i < _job_pools.size(); ++i) {
		typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[i];

		if (job_queue.thread_lock->tryLock()) {
			if (!job_queue.jobs.empty()) {
				job = job_queue.jobs.front();
				job_queue.jobs.pop();
				DoJob(job);

				job_queue.thread_lock->unlock();
				return;

			} else {
				job_queue.thread_lock->unlock();
			}
		}
	}

	typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[0];

	job_queue.read_write_lock->lock();

	if (!job_queue.jobs.empty()) {
		job = job_queue.jobs.front();
		job_queue.jobs.pop();

		job_queue.read_write_lock->unlock();
		DoJob(job);

	} else {
		job_queue.read_write_lock->unlock();
	}
}

template <class Allocator>
int32_t JobPool<Allocator>::getNumActiveThreads(void) const
{
	return _active_threads;
}

template <class Allocator>
size_t JobPool<Allocator>::getNumTotalThreads(void) const
{
	return _threads.size();
}

template <class Allocator>
void JobPool<Allocator>::ProcessMainJobQueue(typename JobPool<Allocator>::JobQueue& job_queue)
{
	JobPair job;

	for (;;) {
		job_queue.read_write_lock->lock();

		if (!job_queue.jobs.empty()) {
			job = job_queue.jobs.front();
			job_queue.jobs.pop();

			job_queue.read_write_lock->unlock();
			DoJob(job);

		} else {
			job_queue.read_write_lock->unlock();
			break;
		}
	}
}

template <class Allocator>
void JobPool<Allocator>::ProcessJobQueue(typename JobPool<Allocator>::JobQueue& job_queue)
{
	JobPair job;

	while (!job_queue.jobs.empty()) {
		job = job_queue.jobs.front();
		job_queue.jobs.pop();
		DoJob(job);
	}
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
void JobPool<Allocator>::JobThread(void* arg)
{
	ThreadData& td = *reinterpret_cast<ThreadData*>(arg);
	JobPool<Allocator>* job_pool = td.job_pool;

	if (td.init_func) {
		td.init_func();
	}

	while (!td.terminate) {
		++job_pool->_active_threads;
		job_pool->helpUntilNoJobs();
		YieldThread(); // Probably a good idea to give some time back to the CPU for other stuff.
		--job_pool->_active_threads;
	}
}
