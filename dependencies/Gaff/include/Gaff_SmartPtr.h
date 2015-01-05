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

NS_GAFF

/*!
	\brief A simple "smart" pointer that just de-allocates when the destructor is called.
*/
template <class T, class Allocator = DefaultAllocator>
class SmartPtr
{
public:
	explicit SmartPtr(T* data = nullptr, const Allocator& allocator = Allocator()):
		_allocator(allocator), _data(data)
	{
	}

	SmartPtr(const SmartPtr<T, Allocator>& data):
		_allocator(data._allocator), _data(data._data)
	{
		((SmartPtr<T, Allocator>&)data)._data = nullptr;
	}

	~SmartPtr(void)
	{
		destroy();
	}

	bool operator==(const SmartPtr<T, Allocator>& rhs) const
	{
		return _data == rhs._data;
	}

	bool operator!=(const SmartPtr<T, Allocator>& rhs) const
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

	const SmartPtr<T, Allocator>& operator=(const SmartPtr<T, Allocator>& rhs)
	{
		destroy();
		_data = rhs._data;
		((SmartPtr<T, Allocator>&)rhs)._data = nullptr;
		return *this;
	}

	const SmartPtr<T, Allocator>& operator=(T* data)
	{
		destroy();
		_data = data;
		return *this;
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

	const T* get(void) const
	{
		return _data;
	}

	T* get(void)
	{
		return _data;
	}

	/*!
		\brief Releases the pointer and returns ownership back to the caller.
		\return The internal pointer.
	*/
	T* release(void)
	{
		T* data = _data;
		_data = nullptr;
		return data;
	}

	bool valid(void) const
	{
		return _data != nullptr;
	}

	operator bool(void) const
	{
		return valid();
	}

private:
	Allocator _allocator;
	T* _data;

	void destroy(void)
	{
		if (_data) {
			_allocator.freeT(_data);
		}
	}
};

NS_END
