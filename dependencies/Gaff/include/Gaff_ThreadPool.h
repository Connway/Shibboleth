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

#include "Gaff_SpinLock.h"
#include "Gaff_Thread.h"
#include "Gaff_Queue.h"
#include "Gaff_Utils.h"
#include "Gaff_ITask.h"

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
		_tasks(allocator), _threads(allocator),
		_allocator(allocator)
	{
	}

	~ThreadPool(void)
	{
		destroy();
	}

	/*!
		\brief Creates the threads.
		\param num_threads
			The number of threads to allocate for the ThreadPool.
			Defaults to the number of physics cores minus one.
		\return Whether the threads were successfully craeted.
	*/
	bool init(unsigned int num_threads = (unsigned int)GetNumberOfCores() - 1)
	{
		_thread_data.thread_pool = this;
		_thread_data.terminate = false;

		_threads.reserve(num_threads);

		for (unsigned long i = 0; i < num_threads; ++i) {
			Thread thread;

			if (!thread.create(TaskRunner, &_thread_data)) {
				destroy();
				return false;
			}

			_threads.movePush(Move(thread));
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

	void addTask(const TaskPtr<Allocator>& task)
	{
		_lock.lock();
		_tasks.push(task);
		_lock.unlock();
	}

	//unsigned int getNumActiveThreads(void) const
	//{
	//}

	unsigned int getNumTotalThreads(void) const
	{
		return _threads.size();
	}

private:
	struct ThreadData
	{
		ThreadPool<Allocator>* thread_pool;
		bool terminate;
	};

	Queue<TaskPtr<Allocator>, Allocator> _tasks;
	Array<Thread, Allocator> _threads;
	ThreadData _thread_data;
	SpinLock _lock;

	Allocator _allocator;

	//volatile unsigned int _active_threads;

	static Thread::ReturnType THREAD_CALLTYPE TaskRunner(void* thread_data)
	{
		ThreadData* td = (ThreadData*)thread_data;
		ThreadPool<Allocator>* tp = td->thread_pool;

		TaskPtr<Allocator> task;

		while (!td->terminate) {
			tp->_lock.lock();
				if (!tp->_tasks.empty()) {
					task = tp->_tasks.first();
					tp->_tasks.pop();
				}
			tp->_lock.unlock();

			if (task) {
				// Atomic increment _active_threads

				const Array<TaskPtr<Allocator>, Allocator>& dep_tasks = task->getDependentTasks();
				task->doTask();
				task->setFinished(true);

				if (!dep_tasks.empty()) {
					tp->_lock.lock();
						for (unsigned int i = 0; i < dep_tasks.size(); ++i) {
							tp->_tasks.push(dep_tasks[i]);
						}
					tp->_lock.unlock();
				}

				task = nullptr; // Free the task

				// Atomic decrement _active_threads
			} /*else {*/
				// Is this a better idea than yielding the thread every iteration?
				// Only yield when we have nothing to do?
				// No idea ...
				YieldThread();
			//}
		}

		return 0;
	}

	friend Thread::ReturnType THREAD_CALLTYPE TaskRunner(void*);

	GAFF_NO_COPY(ThreadPool);
	GAFF_NO_MOVE(ThreadPool);
};

NS_END
