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
	\brief Same as Queue, except that it uses a single-producer, single-consumer, atomic operations.
	\note Does not dynamically change size. Once it is full, elements cannot be pushed until a slot is freed.
*/
template<class T, class Allocator = DefaultAllocator>
class QueueLockFree
{
public:
	QueueLockFree(const Allocator& allocator = Allocator()):
		_allocator(allocator), _write_index(0),
		_read_index(0), _used(0),
		_size(0), _data(nullptr)
	{
	}

	~QueueLockFree(void)
	{
		clear();
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

	// Assumed to be called on main thread
	void clear(void)
	{
		if (_data) {
			for (unsigned int i = 0; i < _used; ++i) {
				deconstruct(_data + _read_index);
				_read_index = (_read_index + 1) % _size;
			}

			_allocator.free(_data);
		}

		_read_index = 0;
		_write_index = 0;
		_used = 0;
		_size = 0;
		_data = nullptr;
	}

	/*!
		\brief Pushes a value to the front of the queue.
		\return Whether the value was successfully pushed into the queue.
	*/
	bool push(const T& value)
	{
		unsigned int write_index = 0;

		// Reserve a slot for us to write to
		do {
			// Fail if the queue is full
			if (_used == _size) {
				return false;
			}

			write_index = _write_index;
		} while (AtomicCompareExchange(&_write_index, (write_index + 1) % _size, write_index) != write_index);

		// Write data
		_data[write_index] = value;
		AtomicIncrement(&_used);

		return true;
	}

	/*!
		\brief Pops a value off the front of the queue.
		\param value Where we store the popped value.
		\return Whether a value was popped off the queue.
	*/
	bool pop(T& value)
	{
		unsigned int read_index = 0;

		do {
			// Queue is empty, nothing to pop
			if (!_used) {
				return false;
			}

			read_index = _read_index;
		} while (AtomicCompareExchange(&_read_index, (read_index + 1) % _size, read_index) != read_index);

		value = _data[read_index];

		deconstruct(_data + read_index);
		AtomicDecrement(&_used);

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
	volatile unsigned int _write_index;
	volatile unsigned int _read_index;
	volatile unsigned int _used;
	unsigned int _size;
	T* _data;

	GAFF_NO_COPY(QueueLockFree);
	GAFF_NO_MOVE(QueueLockFree);
};

/*!
	\brief Same as QueueLockFree, but uses a staic array instead of a dynamically allocated array.
*/
template<class T, unsigned int queue_size>
class QueueLockFreeNoAlloc
{
public:
	QueueLockFreeNoAlloc(void):
		_write_index(0), _read_index(0),
		_used(0), _data(nullptr)
	{
	}

	~QueueLockFreeNoAlloc(void)
	{
	}

	bool push(const T& value)
	{
		unsigned int write_index = 0;

		// Reserve a slot for us to write to
		do {
			// Fail if the queue is full
			if (_used == queue_size) {
				return false;
			}

			write_index = _write_index;
		} while (AtomicCompareExchange(&_write_index, (write_index + 1) % queue_size, write_index) != write_index);

		// Write data
		_data[write_index] = value;
		AtomicIncrement(&_used);

		return true;
	}

	bool pop(T& value)
	{
		unsigned int read_index = 0;

		do {
			// Queue is empty, nothing to pop
			if (!_used) {
				return false;
			}

			read_index = _read_index;

			value = _data[read_index]; // can make this operation expensive if T is a complex object
		} while (AtomicCompareExchange(&_read_index, (read_index + 1) % queue_size, read_index) != read_index);

		deconstruct(&_data[read_index]);
		AtomicDecrement(&_used);

		return true;
	}

	// This is ok, we never change size
	unsigned int capacity(void) const
	{
		return queue_size;
	}

	// Will not always be accurate
	unsigned int size(void) const
	{
		return _used;
	}

private:
	volatile unsigned int _write_index;
	volatile unsigned int _read_index;
	volatile unsigned int _used;
	T _data[queue_size];

	GAFF_NO_COPY(QueueLockFreeNoAlloc);
	GAFF_NO_MOVE(QueueLockFreeNoAlloc);
};

NS_END
