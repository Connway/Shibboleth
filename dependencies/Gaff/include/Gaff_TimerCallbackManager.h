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

#include "Gaff_Function.h"
#include "Gaff_Array.h"

NS_GAFF

template <class Allocator>
class TimerCallbackManager
{
public:
	TimerCallbackManager(const Allocator& allocator = Allocator());
	~TimerCallbackManager(void);

	void update(double dt);

	// Function
	unsigned int registerForCallback(void (*function)(unsigned int), double period, bool periodic = false);

	// Member Function
	template <class T>
	unsigned int registerForCallback(T* object, void (T::*function)(unsigned int), double period, bool periodic = false);

	// Functor
	template <class T>
	unsigned int registerForCallback(const T& object, double period, bool periodic = false);

	void removeCallback(unsigned int id);

private:
	struct CallbackData
	{
		FunctionBinder<void> callback;
		double elapsed;
		double period;
		unsigned int id;
		bool once;

		// for binarySearch()
		bool operator<(unsigned int rhs) const
		{
			return id < rhs;
		}
	};

	Array<CallbackData, Allocator> _callbacks;
	unsigned int _nextID;

	unsigned int registerCallback(FunctionBinder<void, unsigned int>& callback, double period, bool periodic);
};

#include "Gaff_TimerCallbackManager.inl"

NS_END

