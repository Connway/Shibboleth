/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gaff_DefaultAllocator.h"
#include "Gaff_IncludeAssert.h"
#include <cstring>

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class Array
{
public:
	Array(const Allocator& allocator = Allocator()) : _allocator(allocator), _used(0), _size(0), _array(nullptr) {}

	explicit Array(unsigned int start_alloc, const Allocator& allocator = Allocator()) :
	_allocator(allocator), _used(0), _size(start_alloc)
	{
		_array = (T*)_allocator.alloc(sizeof(T) * start_alloc);

		for (unsigned int i = 0; i < start_alloc; ++i) {
			construct(_array + i);
		}
	}

	Array(unsigned int start_alloc, const T& init_val, const Allocator& allocator = Allocator()) :
		_allocator(allocator), _array(nullptr), _used(start_alloc), _size(start_alloc)
	{
		_array = (T*)_allocator.alloc(sizeof(T) * start_alloc);

		for (unsigned int i = 0; i < start_alloc; ++i) {
			construct(_array + i, init_val);
		}
	}

	Array(unsigned int size, const T* data, const Allocator& allocator = Allocator()) :
		_allocator(allocator), _array(nullptr), _used(size), _size(size)
	{
		_array = (T*)_allocator.alloc(sizeof(T) * size);

		for (unsigned int i = 0; i < size; ++i) {
			construct(_array + i, data[i]);
		}
	}

	Array(const Array<T, Allocator>& rhs):
		_allocator(rhs._allocator), _used(0), _size(0), _array(nullptr)
	{
		*this = rhs;
	}

	~Array(void)
	{
		clear();
	}

	const Array<T, Allocator>& operator=(const Array<T, Allocator>& rhs)
	{
		if (this == &rhs) {
			return *this;
		}

		clear();

		if (rhs._size == 0) {
			return *this;
		}

		if (rhs._size > 0) {
			_array = (T*)_allocator.alloc(sizeof(T) * rhs._size);

			for (unsigned int i = 0; i < rhs._used; ++i) {
				construct(_array + i, rhs._array[i]);
			}

		}

		_used = rhs._used;
		_size = rhs._size;

		return *this;
	}

	bool operator==(const Array<T, Allocator>& rhs)
	{
		if (_used != rhs._used) {
			return false;
		}

		for (unsigned int i = 0; i < _used; ++i) {
			if (_array[i] != rhs._array[i]) {
				return false;
			}
		}

		return true;
	}

	bool operator!=(const Array<T, Allocator>& rhs)
	{
		return !(*this == rhs);
	}

	const T& operator[](unsigned int index) const
	{
		assert(index < _used);
		return _array[index];
	}

	T& operator[](unsigned int index)
	{
		assert(index < _used);
		return _array[index];
	}

	void clear(void)
	{
		if (_array) {
			for (unsigned int i = 0; i < _used; ++i) {
				deconstruct(_array + i);
			}

			_allocator.free(_array);
			_used = _size = 0;
			_array = nullptr;
		}
	}

	void clearNoFree(void)
	{
		if (_array) {
			for (unsigned int i = 0; i < _used; ++i) {
				deconstruct(_array + i);
			}

			_used = 0;
		}
	}

	const T& last(void) const
	{
		assert(_used > 0);
		return _array[_used - 1];
	}

	T& last(void)
	{
		assert(_used > 0);
		return _array[_used - 1];
	}

	const T* getArray(void) const
	{
		return _array;
	}

	T* getArray(void)
	{
		return _array;
	}

	bool empty(void) const
	{
		return _used == 0;
	}

	void push(const T& data)
	{
		if (_used == _size) {
			if (_size == 0) {
				reserve(1);
			} else {
				reserve(_size * 2);
			}
		}

		construct(_array + _used, data);
		++_used;
	}

	void push(T& data)
	{
		if (_used == _size) {
			if (_size == 0) {
				reserve(1);
			}
			else {
				reserve(_size * 2);
			}
		}

		construct(_array + _used, data);
		++_used;
	}

	void pop(void)
	{
		assert(_used > 0);
		deconstruct(_array + _used - 1);
		--_used;
	}

	void insert(const T& data, unsigned int index)
	{
		if (_used + 1 > _size) {
			if (_size == 0) {
				reserve(1);
			} else {
				reserve(_size * 2);
			}
		}

		construct(_array + _used, _array[_used - 1]);

		for (unsigned int i = _used - 1; i > index; --i) {
			deconstruct(_array + i);
			construct(_array + i, _array[i - 1]);
		}

		construct(_array + index, data);
		++_used;
	}

	void erase(unsigned int index)
	{
		assert(index < _used && _used > 0);

		deconstruct(_array + index);
		memcpy(_array + index, _array + index + 1, (_used - index - 1) * sizeof(T));
		--_used;
	}

	void resize(unsigned int new_size)
	{
		if (new_size == _size) {
			return;
		}

		T* old_data = _array;

		_array = (T*)_allocator.alloc(sizeof(T) * new_size);

		for (unsigned int i = _used; i < new_size; ++i) {
			construct(_array + i);
		}

		if (old_data) {
			if (new_size < _size) {
				for (unsigned int i = new_size; i < _size; ++i) {
					deconstruct(old_data + i);
				}
			}

			memcpy(_array, old_data, sizeof(T) * _used);
			_allocator.free(old_data);
		}

		_size = new_size;
		_used = new_size;
	}

	void reserve(unsigned int reserve_size)
	{
		if (reserve_size <= _size) {
			return;
		}

		T* old_data = _array;

		_array = (T*)_allocator.alloc(sizeof(T) * reserve_size);
		_size = reserve_size;

		if (old_data) {
			memcpy(_array, old_data, sizeof(T) * _used);
			_allocator.free(old_data);
		}
	}

	int linearFind(const T& data) const
	{
		for (unsigned int i = 0; i < _used; ++i) {
			if (_array[i] == data) {
				return i;
			}
		}

		return -1;
	}

	unsigned int size(void) const
	{
		return _used;
	};

	unsigned int capacity(void) const
	{
		return _size;
	}

	Array(Array<T, Allocator>&& rhs):
		_allocator(rhs._allocator), _used(rhs._used),
		_size(rhs._size), _array(rhs._array)
	{
		rhs._used = rhs._size = 0;
		rhs._array = nullptr;
	}

	const Array<T, Allocator>& operator=(Array<T, Allocator>&& rhs)
	{
		clear();

		_used = rhs._used;
		_size = rhs._size;
		_array = rhs._array;

		rhs._used = rhs._size = 0;
		rhs._array = nullptr;

		return *this;
	}

private:
	Allocator _allocator;
	unsigned int _used, _size;
	T* _array;
};

NS_END
