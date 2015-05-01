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
#include "Gaff_Atomic.h"

NS_GAFF

/*!
	\brief A reference counted pointer where the reference count is separate from the object.
*/
template <class T, class Allocator = DefaultAllocator>
class SharedPtr
{
public:
	SharedPtr(const Allocator& allocator = Allocator()) :
		_allocator(allocator), _count(nullptr), _data(nullptr)
	{
	}

	explicit SharedPtr(T* data, const Allocator& allocator = Allocator()) :
		_allocator(allocator), _count(nullptr), _data(nullptr)
	{
		*this = data;
	}

	SharedPtr(const SharedPtr<T, Allocator>& data):
		_allocator(data._allocator), _count(nullptr), _data(nullptr)
	{
		*this = data;
	}

	SharedPtr(SharedPtr<T, Allocator>&& data):
		_allocator(data._allocator), _count(data._count), _data(data._data)
	{
		data._count = nullptr;
		data._data = nullptr;
	}

	~SharedPtr(void)
	{
		clear();
	}

	const SharedPtr<T, Allocator>& operator=(const SharedPtr<T, Allocator>& rhs)
	{
		clear();

		_allocator = rhs._allocator;
		_count = rhs._count;
		_data = rhs._data;

		if (_count) {
			AtomicIncrement(_count);
		}

		return *this;
	}

	const SharedPtr<T, Allocator>& operator=(SharedPtr<T, Allocator>&& rhs)
	{
		clear();

		_allocator = rhs._allocator;
		_count = rhs._count;
		_data = rhs._data;

		rhs._count = nullptr;
		rhs._data = nullptr;

		return *this;
	}

	const SharedPtr<T, Allocator>& operator=(T* rhs)
	{
		clear();

		_data = rhs;

		if (rhs) {
			_count = reinterpret_cast<volatile unsigned int*>(_allocator.alloc(sizeof(volatile unsigned int)));
			*_count = 1;
		}

		return *this;
	}

	bool operator==(const SharedPtr<T, Allocator>& rhs) const
	{
		return _data == rhs._data;
	}

	bool operator!=(const SharedPtr<T, Allocator>& rhs) const
	{
		return _data != rhs._data;
	}

	bool operator==(const T* rhs) const
	{
		return _data == rhs;
	}

	bool operator!=(const T* rhs) const
	{
		return _data != rhs;
	}

	const T* operator->(void) const
	{
		return _data;
	}

	T* operator->(void)
	{
		return _data;
	}

	const T& operator*(void) const
	{
		return *_data;
	}

	T& operator*(void)
	{
		return *_data;
	}

	operator bool(void) const
	{
		return valid();
	}

	bool valid(void) const
	{
		return _data && _count;
	}

	void clear(void)
	{
		if (_count) {
			unsigned int new_count = AtomicDecrement(_count);

			if (!new_count) {
				_allocator.free((void*)_count);

				if (_data) {
					_allocator.freeT(_data);
				}
			}
		}

		_count = nullptr;
		_data = nullptr;
	}

	const T* get(void) const
	{
		return _data;
	}

	T* get(void)
	{
		return _data;
	}

	unsigned int getCount(void) const
	{
		return *_count;
	}

private:
	Allocator _allocator;
	volatile unsigned int* _count;
	T* _data;
};

NS_END
