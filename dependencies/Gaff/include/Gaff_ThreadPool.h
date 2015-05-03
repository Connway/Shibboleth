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

/*! \file */

#pragma once

#include "Gaff_SharedPtr.h"
#include "Gaff_SpinLock.h"
#include "Gaff_Thread.h"
#include "Gaff_Queue.h"
#include "Gaff_Utils.h"
#include "Gaff_ITask.h"
#include "Gaff_Atomic.h"

NS_GAFF

/*!
	\brief A collection of threads that execute tasks that are added to a queue.
	\tparam Allocator The allocator used for the task queue.
*/
template <class Allocator = DefaultAllocator>
class ThreadPool
{
public:
	ThreadPool(const Allocator& allocator = Allocator()):
		_task_pools(allocator), _threads(allocator),
		_allocator(allocator)
	{
	}

	~ThreadPool(void)
	{
		destroy();
	}

	/*!
		\brief Creates the threads.

		\param num_pools
			The number of task pools to allocate.

		\param num_threads
			The number of threads to allocate for the ThreadPool.
			Defaults to the number of physics cores minus one.
		
		\return Whether the threads were successfully created.

		\note
			Task pools represent tasks that share a similar bottleneck, making parallelizing them difficult.
			Place tasks that would be contesting each other for a lock in the same pool for optimal thread idle time.

			Pool zero is a special pool. These tasks are not considered sharing data and will run any number of them in parallel.
			Tasks that are placed in any other pool will run one at a time until the pool is empty.
	*/
	bool init(unsigned int num_pools = 7, unsigned int num_threads = static_cast<unsigned int>(GetNumberOfCores()) - 1)
	{
		_task_pools.resize(num_pools + 1);

		_thread_data.thread_pool = this;
		_thread_data.terminate = false;
		_threads.resize(num_threads);

		for (unsigned long i = 0; i < num_threads; ++i) {
			if (!_threads[i].create(TaskRunner, &_thread_data)) {
				destroy();
				return false;
			}
		}

		return true;
	}

	/*!
		\brief Tells all threads to terminate, waits for them to return, and then destroys them.
	*/
	void destroy(void)
	{
		_thread_data.terminate = true;

		for (unsigned int i = 0; i < _threads.size(); ++i) {
			_threads[i].wait();
		}

		_threads.clear();
	}

	//volatile unsigned int* addTasks(const Array< TaskPtr<Allocator> >& tasks, unsigned int pool = 0)
	//{
	//	return nullptr;
	//}

	void addTask(ITask<Allocator>* task, unsigned int pool = 0)
	{
		addTask(TaskPtr<Allocator>(task), pool);
	}

	void addTask(const TaskPtr<Allocator>& task, unsigned int pool = 0)
	{
		assert(pool < _task_pools.size());
		_task_pools[pool].read_write_lock.lock();
		_task_pools[pool].tasks.emplacePush(task);
		_task_pools[pool].read_write_lock.unlock();
	}

	void doATask(void)
	{
		for (unsigned int i = 1; i < _task_pools.size(); ++i) {
			TaskQueue& task_queue = _task_pools[i];

			if (task_queue.thread_lock.tryLock()) {
				if (!task_queue.tasks.empty()) {
					task_queue.read_write_lock.lock();

					TaskPtr<Allocator> task = Move(task_queue.tasks.first());
					task_queue.tasks.pop();

					task_queue.read_write_lock.unlock();

					DoTask(task, this, i);

					task_queue.thread_lock.unlock();
					return;
				}

				task_queue.thread_lock.unlock();
			}
		}

		TaskQueue& task_queue = _task_pools[0];

		if (!task_queue.tasks.empty()) {
			task_queue.read_write_lock.lock();
			TaskPtr<Allocator> task;

			if (!task_queue.tasks.empty()) {
				task = Move(task_queue.tasks.first());
				task_queue.tasks.pop();
			}

			task_queue.read_write_lock.unlock();

			if (task) {
				DoTask(task, this, 0);
			}
		}
	}

	/*!
		\brief Helps finish tasks until there are no more free tasks to process.
		\note
			This function only makes one pass. If there is a task that adds another task earlier in the pass,
			this function will still terminate. Use this function if you are sure that the tasks in the pool
			will not add more tasks. Also, just because this function returns, does not mean all the tasks
			have been finished. Other threads may have claimed them and are potentially still running.
	*/
	void helpUntilNoTasks(void)
	{
		// Start with the other pools first. Presumably they don't lead to pool zero never getting reached.
		for (unsigned int i = 1; i < _task_pools.size(); ++i) {
			TaskQueue& task_queue = _task_pools[i];

			if (task_queue.thread_lock.tryLock()) {
				ProcessTaskQueue(task_queue, this, i);
				task_queue.thread_lock.unlock();
			}
		}

		// Conversely, hopefully the main pool doesn't constantly get new tasks pushed to it and stop the other pools from being processed.
		TaskQueue& task_queue = _task_pools[0];
		ProcessMainTaskQueue(task_queue, this, 0);
	}

	/*!
		\brief Returns the number of threads that aren't yielding to the scheduler.
	*/
	unsigned int getNumActiveThreads(void) const
	{
		return _active_threads;
	}

	/*!
		\brief Returns the total number of worker threads.
	*/
	size_t getNumTotalThreads(void) const
	{
		return _threads.size();
	}

	/*!
		\brief Fills \a out with the thread IDs. \a out is assumed to be the size that \a getNumTotalThreads() returns.
		\param out The output buffer we will fill with the thread IDs.
	*/
	void getThreadIDs(unsigned int* out) const
	{
		for (unsigned int i = 0; i < _threads.size(); ++i) {
			out[i] = _threads[i].getID();
		}
	}

private:
	struct ThreadData
	{
		ThreadPool<Allocator>* thread_pool;
		bool terminate;
	};

	struct TaskQueue
	{
		Queue<TaskPtr<Allocator>, Allocator> tasks;
		SpinLock read_write_lock;
		SpinLock thread_lock;
	};

	Array<TaskQueue, Allocator> _task_pools;
	Array<Thread, Allocator> _threads;
	ThreadData _thread_data;

	Allocator _allocator;

	volatile unsigned int _active_threads;

	void DoTask(TaskPtr<Allocator>& task, ThreadPool<Allocator>* thread_pool, unsigned int pool)
	{
		const typename ITask<Allocator>::DependentTaskData& dep_tasks = task->getDependentTasks();
		task->doTask();
		task->finished();

		// Don't want to waste CPU cycles by locking when it is empty.
		if (!dep_tasks.empty()) {
			for (unsigned int i = 0; i < dep_tasks.size(); ++i) {
				TaskQueue& task_queue = thread_pool->_task_pools[(dep_tasks[i].second == SIZE_T_FAIL) ? pool : dep_tasks[i].second];

				task_queue.read_write_lock.lock();
				task_queue.tasks.emplaceMovePush(Move(dep_tasks[i].first));
				task_queue.read_write_lock.unlock();
			}
		}

		task = nullptr; // Free the task
	}

	void ProcessMainTaskQueue(TaskQueue& task_queue, ThreadPool<Allocator>* thread_pool, unsigned int pool)
	{
		TaskPtr<Allocator> task;

		// This is probably a race condition, but if the read says that it is empty, when it isn't,
		// then we'll just get to it on the next iteration.
		while (!task_queue.tasks.empty()) {
			task_queue.read_write_lock.lock();

			if (!task_queue.tasks.empty()) {
				task = Move(task_queue.tasks.first());
				task_queue.tasks.pop();
			}

			task_queue.read_write_lock.unlock();

			if (task) {
				DoTask(task, thread_pool, pool);
			}
		}
	}

	void ProcessTaskQueue(TaskQueue& task_queue, ThreadPool<Allocator>* thread_pool, unsigned int pool)
	{
		TaskPtr<Allocator> task;

		// This is probably a race condition, but if the read says that it is empty, when it isn't,
		// then we'll just get to it on the next iteration.
		while (!task_queue.tasks.empty()) {
			task_queue.read_write_lock.lock();

			task = Move(task_queue.tasks.first());
			task_queue.tasks.pop();

			task_queue.read_write_lock.unlock();

			DoTask(task, thread_pool, pool);
		}
	}

	static Thread::ReturnType THREAD_CALLTYPE TaskRunner(void* thread_data)
	{
		ThreadData* td = reinterpret_cast<ThreadData*>(thread_data);
		ThreadPool<Allocator>* thread_pool = td->thread_pool;

		while (!td->terminate) {
			AtomicIncrement(&thread_pool->_active_threads);
			thread_pool->helpUntilNoTasks();
			YieldThread(); // Probably a good idea to give some time back to the CPU for other stuff.
			AtomicDecrement(&thread_pool->_active_threads);
		}

		return 0;
	}

	friend Thread::ReturnType THREAD_CALLTYPE TaskRunner(void* thread_data);

	GAFF_NO_COPY(ThreadPool);
	GAFF_NO_MOVE(ThreadPool);
};

NS_END
