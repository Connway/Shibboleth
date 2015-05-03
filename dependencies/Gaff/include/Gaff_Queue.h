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

NS_GAFF

/*!
	\brief A FIFO quque implemented with a circular buffer.
	\tparam T The data type we are storing.
	\tparam Allocator The memory allocator we will use.
*/
template <class T, class Allocator = DefaultAllocator>
class Queue
{
public:
	explicit Queue(const Allocator& allocator = Allocator());
	explicit Queue(size_t start_alloc, const Allocator& allocator = Allocator());
	Queue(const T* data, size_t size, const Allocator& allocator = Allocator());
	Queue(const Queue<T, Allocator>& rhs);
	Queue(Queue<T, Allocator>&& rhs);
	~Queue(void);

	const Queue<T, Allocator>& operator=(const Queue<T, Allocator>& rhs);
	const Queue<T, Allocator>& operator=(Queue<T, Allocator>&& rhs);

	bool operator==(const Queue<T, Allocator>& rhs) const;
	bool operator!=(const Queue<T, Allocator>& rhs) const;

	void clear(void);
	void reserve(size_t new_size);

	void movePush(T&& data);
	void push(const T& data);
	void pop(void);

	template <class... Args>
	void emplacePush(Args&&... args);

	template <class... Args>
	void emplaceMovePush(Args&&... args);

	const T& first(void) const;
	T& first(void);
	const T& last(void) const;
	T& last(void);

	size_t size(void) const;
	size_t capacity(void) const;

	bool empty(void) const;

	void setAllocator(const Allocator& allocator);

private:
	Allocator _allocator;
	size_t _used, _size;
	T* _array;
	T* _begin;
	T* _end;

	void increment(T** element, T* array = nullptr, size_t array_size = 0) const;
	void decrement(const T** element, T* array = nullptr, size_t array_size = 0) const;
	void decrement(T** element, T* array = nullptr, size_t array_size = 0);
};

#include "Gaff_Queue.inl"

NS_END
