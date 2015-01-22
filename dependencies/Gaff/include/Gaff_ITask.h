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

#include "Gaff_IRefCounted.h"
#include "Gaff_RefPtr.h"
#include "Gaff_Array.h"
#include "Gaff_Pair.h"

NS_GAFF

/*!
	\brief Interface for tasks used with the ThreadPool.
*/
template <class Allocator = DefaultAllocator>
class ITask : public IRefCounted
{
public:
	typedef RefPtr< ITask<Allocator> > TaskPointer;
	typedef Array<Gaff::Pair<TaskPointer, unsigned int>, Allocator> DependentTaskData;

	ITask(const Allocator& allocator = Allocator()):
		_dependent_tasks(allocator), _finished(false)
	{
	}

	virtual ~ITask(void) {}

	virtual void doTask(void) = 0;

	void setFinished(bool finished)
	{
		_finished = finished;
	}

	bool isFinished(void) const
	{
		return _finished;
	}

	/*!
		\brief Block thread until the task has finished. While waiting, yield the thread to the scheduler.
	*/
	void yieldWait(void) const
	{
		while (!_finished) {
			YieldThread();
		}
	}

	/*!
		\brief Block thread until the task has finished. CPU is still spinning while waiting.
	*/
	void spinWait(void) const
	{
		while (!_finished) {
		}
	}

	/*!
		\brief
			Adds a task that is dependent on this task.
			Dependent tasks will be added to the ThreadPool when this task finishes.

		\param task The task that is dependent on this task.
		\param pool
			The task pool this dependent task should be added to.
			A value of UINT_FAIL means to be added to the same pool that this task is a part of.
	*/
	void addDependentTask(const TaskPointer& task, unsigned int pool = 0)
	{
		_dependent_tasks.emplacePush(task, pool);
	}

	const DependentTaskData& getDependentTasks(void) const
	{
		return _dependent_tasks;
	}

private:
	DependentTaskData _dependent_tasks;
	unsigned int _pool;
	bool _finished;
};

template <class Allocator = DefaultAllocator> using TaskPtr = RefPtr< ITask<Allocator> >;

NS_END
