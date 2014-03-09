/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#pragma once

#include "Gaff_SmartPtr.h"
#include "Gaff_Array.h"

NS_GAFF

template <class Allocator = DefaultAllocator>
class ITask
{
public:
	typedef SmartPtr<ITask<Allocator>, Allocator> TaskPointer;

	ITask(const Allocator& allocator = Allocator()):
		_dependent_tasks(allocator)
	{
	}

	virtual ~ITask(void) {}

	virtual void doTask(void) = 0;

	void addDependentTask(const TaskPointer& task)
	{
		_dependent_tasks.push(task);
	}

	const Array<TaskPointer, Allocator>& getDependentTasks(void) const
	{
		return _dependent_tasks;
	}

private:
	Array<TaskPointer, Allocator> _dependent_tasks;
};

template <class Allocator = DefaultAllocator> using TaskPtr = SmartPtr<ITask<Allocator>, Allocator>;


NS_END
