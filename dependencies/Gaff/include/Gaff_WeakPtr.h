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

#include "Gaff_WeakObject.h"

NS_GAFF

template < class T, class WeakObj = WeakObject<DefaultAllocator> >
class WeakPtr
{
public:
	typedef WeakPtr<T, WeakObj> WeakPtrType;

	WeakPtr(const WeakPtrType& rhs)
	{
		*this = rhs;
	}

	WeakPtr(T* data = nullptr)
	{
		*this = data;
	}

	~WeakPtr(void)
	{
	}

	const WeakPtrType& operator=(const WeakPtrType& rhs)
	{
		_weak_ref = rhs._weak_ref;
		return *this;
	}

	const WeakPtrType& operator=(const T* rhs)
	{
		_weak_ref = rhs->getWeakRef();
		return *this;
	}

	bool operator==(const WeakPtrType& rhs) const
	{
		return _weak_ref == rhs;
	}

	bool operator!=(const WeakPtrType& rhs) const
	{
		return _weak_ref != rhs;
	}

	bool operator==(const T* rhs) const
	{
		return _weak_ref == rhs;
	}

	bool operator!=(const T* rhs) const
	{
		return _weak_ref != rhs;
	}

	const T* operator->(void) const
	{
		return (T*)_weak_ref.get();
	}

	T* operator->(void)
	{
		return (T*)_weak_ref.get();
	}

	const T& operator*(void) const
	{
		return *get();
	}

	T& operator*(void)
	{
		return *get();
	}

	const T* get(void) const
	{
		return (T*)_weak_ref.get();
	}

	T* get(void)
	{
		return (T*)_weak_ref.get();
	}

	void set(T* data)
	{
		_weak_ref = data;
	}

	operator bool() const
	{
		return valid();
	}

	bool valid(void) const
	{
		return _weak_ref != nullptr && _weak_ref->getObject() != nullptr;
	}

private:
	RefPtr<typename WeakObj::WeakReference> _weak_ref;
};

NS_END
