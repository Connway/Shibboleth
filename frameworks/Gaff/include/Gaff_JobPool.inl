/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
bool JobPool<Allocator>::init(unsigned int num_pools, unsigned int num_threads)
{
	_job_pools.resize(num_pools + 1);

	for (JobQueue& job_queue : _job_pools) {
		job_queue.read_write_lock = UniquePtr<SpinLock>(GAFF_ALLOCT(Gaff::SpinLock, _allocator));
		job_queue.thread_lock = UniquePtr<SpinLock>(GAFF_ALLOCT(Gaff::SpinLock, _allocator));
	}

	_thread_data.job_pool = this;
	_thread_data.terminate = false;
	_threads.reserve(num_threads);

	for (unsigned long i = 0; i < num_threads; ++i) {
		_threads.emplace_back(JobThread, std::ref(_thread_data));

		if (_threads.back().get_id() == std::thread::id()) {
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

	for (unsigned int i = 0; i < _threads.size(); ++i) {
		_threads[i].join();
	}

	_threads.clear();
}

template <class Allocator>
void JobPool<Allocator>::addJobs(JobData* jobs, size_t num_jobs, Counter** counter, unsigned int pool)
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

		cnt->count = static_cast<int32_t>(num_jobs);
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

	while (counter->count && !_thread_data.terminate) {
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

	while (counter->count) {
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
void JobPool<Allocator>::getThreadIDs(size_t* out) const
{
	for (size_t i = 0; i < _threads.size(); ++i) {
		out[i] = std::hash<std::thread::id>{}(_threads[i].get_id());
	}
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

		GAFF_ASSERT(job.second->count);
		--job.second->count;
	}
}

template <class Allocator>
void JobPool<Allocator>::JobThread(ThreadData& td)
{
	JobPool<Allocator>* job_pool = td.job_pool;

	while (!td.terminate) {
		++job_pool->_active_threads;
		job_pool->helpUntilNoJobs();
		YieldThread(); // Probably a good idea to give some time back to the CPU for other stuff.
		--job_pool->_active_threads;
	}
}