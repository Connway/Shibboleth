/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

	_thread_data.job_pool = this;
	_thread_data.terminate = false;
	_threads.reserve(num_threads);

	for (unsigned long i = 0; i < num_threads; ++i) {
		_threads.emplacePush();

		if (!_threads[i].create(JobThread, &_thread_data)) {
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
		_threads[i].wait();
	}

	_threads.clear();
}

template <class Allocator>
void JobPool<Allocator>::addJobs(JobData* jobs, size_t num_jobs, Counter** counter, unsigned int pool)
{
	assert(pool < _job_pools.size());
	assert(num_jobs);

	Counter* cnt = nullptr;

	if (counter) {
		if (*counter) {
			cnt = *counter;
		} else {
			*counter = cnt = _allocator.template allocT<Counter>();
		}

		AtomicExchange(&cnt->count, static_cast<unsigned int>(num_jobs));
	}

	typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[pool];

	job_queue.read_write_lock.lock();
	job_queue.jobs.reserve(job_queue.jobs.size() + num_jobs);

	for (size_t i = 0; i < num_jobs; ++i) {
		assert(jobs[i].job_func);
		job_queue.jobs.emplacePush(JobPair(jobs[i], cnt));
	}

	job_queue.read_write_lock.unlock();
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
	assert(counter);

	while (counter->count) {
		YieldThread();
	}
}

template <class Allocator>
void JobPool<Allocator>::freeCounter(Counter* counter)
{
	assert(counter);
	_allocator.free(counter);
}

template <class Allocator>
void JobPool<Allocator>::helpWhileWaiting(Counter* counter)
{
	assert(counter);

	while (counter->count) {
		doAJob();
	}
}

template <class Allocator>
void JobPool<Allocator>::helpUntilNoJobs(void)
{
	// Start with the other pools first. Presumably they don't lead to pool zero never getting reached.
	for (unsigned int i = 1; i < _job_pools.size(); ++i) {
		typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[i];

		if (job_queue.thread_lock.tryLock()) {
			ProcessJobQueue(job_queue);
			job_queue.thread_lock.unlock();
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

		if (job_queue.thread_lock.tryLock()) {
			job_queue.read_write_lock.lock();

			if (!job_queue.jobs.empty()) {
				job = job_queue.jobs.first();
				job_queue.jobs.pop();

				job_queue.read_write_lock.unlock();
				DoJob(job);

				job_queue.thread_lock.unlock();
				return;

			} else {
				job_queue.thread_lock.unlock();
			}
		}
	}

	typename JobPool<Allocator>::JobQueue& job_queue = _job_pools[0];

	job_queue.read_write_lock.lock();

	if (!job_queue.jobs.empty()) {
		job = job_queue.jobs.first();
		job_queue.jobs.pop();

		job_queue.read_write_lock.unlock();
		DoJob(job);

	} else {
		job_queue.read_write_lock.unlock();
	}
}

template <class Allocator>
unsigned int JobPool<Allocator>::getNumActiveThreads(void) const
{
	return _active_threads;
}

template <class Allocator>
size_t JobPool<Allocator>::getNumTotalThreads(void) const
{
	return _threads.size();
}

template <class Allocator>
void JobPool<Allocator>::getThreadIDs(unsigned int* out) const
{
	for (unsigned int i = 0; i < _threads.size(); ++i) {
		out[i] = _threads[i].getID();
	}
}

template <class Allocator>
void JobPool<Allocator>::ProcessMainJobQueue(typename JobPool<Allocator>::JobQueue& job_queue)
{
	JobPair job;

	// This is probably a race condition, but if the read says that it is empty, when it isn't,
	// then we'll just get to it on the next iteration.
	for (;;) {
		job_queue.read_write_lock.lock();

		// this may be getting a stale value
		if (!job_queue.jobs.empty()) {
			job = job_queue.jobs.first();
			job_queue.jobs.pop();

			job_queue.read_write_lock.unlock();
			DoJob(job);

		} else {
			job_queue.read_write_lock.unlock();
			break;
		}
	}
}

template <class Allocator>
void JobPool<Allocator>::ProcessJobQueue(typename JobPool<Allocator>::JobQueue& job_queue)
{
	JobPair job;

	// This is probably a race condition, but if the read says that it is empty, when it isn't,
	// then we'll just get to it on the next iteration.
	while (!job_queue.jobs.empty()) {
		job_queue.read_write_lock.lock();

		job = job_queue.jobs.first();
		job_queue.jobs.pop();

		job_queue.read_write_lock.unlock();

		DoJob(job);
	}
}

template <class Allocator>
void JobPool<Allocator>::DoJob(JobPair& job)
{
	job.first.job_func(job.first.job_data);

	if (job.second) {
		//Gaff::DebugPrintf("FUNC: %p\n", job.first);

		//if (!job.second->count) {
		//	Gaff::DebugPrintf("JOB FAIL(%d) - %p:(%d)\n", Gaff::Thread::GetCurrentThreadID(), job.second, job.second->count);
		//} else {
		//	Gaff::DebugPrintf("JOB(%d) - %p:(%d)\n", Gaff::Thread::GetCurrentThreadID(), job.second, job.second->count);
		//}

		assert(job.second->count);
		AtomicDecrement(&job.second->count);
	}
}

template <class Allocator>
Thread::ReturnType THREAD_CALLTYPE JobPool<Allocator>::JobThread(void* thread_data)
{
	ThreadData* td = reinterpret_cast<ThreadData*>(thread_data);
	JobPool<Allocator>* job_pool = td->job_pool;

	while (!td->terminate) {
		AtomicIncrement(&job_pool->_active_threads);
		job_pool->helpUntilNoJobs();
		YieldThread(); // Probably a good idea to give some time back to the CPU for other stuff.
		AtomicDecrement(&job_pool->_active_threads);
	}

	return 0;
}
