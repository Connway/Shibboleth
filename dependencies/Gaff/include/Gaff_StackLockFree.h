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

#include "Gaff_DefaultAllocator.h"
#include "Gaff_IncludeAssert.h"
#include "Gaff_Atomic.h"

NS_GAFF

/*!
	\brief A LIFO stack implemented using single-producer, single-consumer, atomic operations.
	\note Does not dynamically change size. Once it is full, elements cannot be pushed until a slot is freed.
*/
template<class T, class Allocator = DefaultAllocator>
class StackLockFree
{
public:
	StackLockFree(const Allocator& allocator = Allocator()) :
		_allocator(allocator), _used(0),
		_size(0), _data(nullptr)
	{
	}

	~StackLockFree(void)
	{
		_allocator.template freeArrayT<T>(_data, _size);
	}

	bool init(unsigned int size)
	{
		assert(!_data);

		_data = _allocator.template allocArrayT<T>(size);

		if (_data) {
			_size = size;
			return true;
		}

		return false;
	}

	/*!
		\brief Pushes a value to the top of the stack.
		\return Whether the value was successfully pushed onto the stack.
	*/
	bool push(const T& value)
	{
		unsigned int old_size = 0;

		// Reserve a slot for us to write to
		do {
			// Fail if the queue is full
			if (_used == _size) {
				return false;
			}

			old_size = _used;
		} while (AtomicCompareExchange(&_used, old_size + 1, old_size) != old_size);

		// Write data
		_data[old_size] = value;

		return true;
	}

	/*!
		\brief Pops a value off the top of the stack.
		\param value Where we store the popped value.
		\return Whether the value was successfully popped off the stack.
	*/
	bool pop(T& value)
	{
		unsigned int old_size = 0;

		do {
			// Queue is empty, nothing to pop
			if (!_used) {
				return false;
			}

			old_size = _used;

			value = _data[old_size - 1]; // can make this operation expensive if T is a complex object
		} while (AtomicCompareExchange(&_used, old_size - 1, old_size) != old_size);

		deconstruct(_data + old_size - 1);

		return true;
	}

	// This is ok, we never change size
	unsigned int capacity(void) const
	{
		return _size;
	}

	// Will not always be accurate
	unsigned int size(void) const
	{
		return _used;
	}

private:
	Allocator _allocator;
	volatile unsigned int _used;
	unsigned int _size;
	T* _data;

	GAFF_NO_COPY(StackLockFree);
	GAFF_NO_MOVE(StackLockFree);
};

/*!
	\brief Same as StackLockFree, but uses a staic array instead of a dynamically allocated array.
*/
template<class T, unsigned int stack_size>
class StackLockFreeNoAlloc
{
public:
	StackLockFreeNoAlloc(void) :
		_write_index(0), _read_index(0),
		_used(0), _data(nullptr)
	{
	}

	~StackLockFreeNoAlloc(void)
	{
	}

	bool push(const T& value)
	{
		unsigned int old_size = 0;

		// Reserve a slot for us to write to
		do {
			// Fail if the queue is full
			if (_used == _size) {
				return false;
			}

			old_size = _used;
		} while (AtomicCompareExchange(&_used, old_size + 1, old_size) != old_size);

		// Write data
		_data[old_size] = value;

		return true;
	}

	bool pop(T& value)
	{
		unsigned int old_size = 0;

		do {
			// Queue is empty, nothing to pop
			if (!_used) {
				return false;
			}

			old_size = _used;

			value = _data[old_size - 1]; // can make this operation expensive if T is a complex object
		} while (AtomicCompareExchange(&_used, old_size - 1, old_size) != old_size);

		deconstruct(&_data[old_size - 1]);

		return true;
	}

	// This is ok, we never change size
	unsigned int capacity(void) const
	{
		return stack_size;
	}

	// Will not always be accurate
	unsigned int size(void) const
	{
		return _used;
	}

private:
	volatile unsigned int _used;
	T _data[stack_size];

	GAFF_NO_COPY(StackLockFreeNoAlloc);
	GAFF_NO_MOVE(StackLockFreeNoAlloc);
};

NS_END
