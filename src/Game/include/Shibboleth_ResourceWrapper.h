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

#include <Shibboleth_ResourceManager.h>

NS_SHIBBOLETH

template <class T>
class ResourceWrapper
{
public:
	ResourceWrapper(const ResourceWrapper<T>& wrapper):
		_res_ptr(wrapper._res_ptr)
	{
	}

	ResourceWrapper(ResourceWrapper<T>&& wrapper):
		_res_ptr(wrapper._res_ptr)
	{
		wrapper._res_ptr = nullptr;
	}

	ResourceWrapper(const ResourcePtr& res_ptr):
		_res_ptr(res_ptr)
	{
	}

	ResourceWrapper(void)
	{
	}

	~ResourceWrapper(void)
	{
	}

	bool operator==(const ResourceWrapper<T>& rhs) const
	{
		return _res_ptr == rhs._res_ptr;
	}

	bool operator==(const ResourcePtr& rhs) const
	{
		return _res_ptr == rhs;
	}

	bool operator!=(const ResourceWrapper<T>& rhs) const
	{
		return _res_ptr != rhs._res_ptr;
	}

	bool operator!=(const ResourcePtr& rhs) const
	{
		return _res_ptr != rhs;
	}

	const ResourceWrapper<T>& operator=(const ResourceWrapper<T>& rhs)
	{
		_res_ptr = rhs._res_ptr;
		return *this;
	}

	const ResourceWrapper<T>& operator=(const ResourceWrapper<T>&& rhs)
	{
		_res_ptr = rhs._res_ptr;
		rhs._res_ptr = nullptr;
		return *this;
	}

	const ResourceWrapper<T>& operator=(const ResourcePtr& rhs)
	{
		_res_ptr = rhs;
		return *this;
	}

	const T* operator->(void) const
	{
		return _res_ptr->getResourcePtr<T>();
	}

	T* operator->(void)
	{
		return _res_ptr->getResourcePtr<T>();
	}

	const T& operator*(void) const
	{
		return *_res_ptr->getResourcePtr<T>();
	}

	T& operator*(void)
	{
		return *_res_ptr->getResourcePtr<T>();
	}

	const ResourcePtr& getResourcePtr(void) const
	{
		return _res_ptr;
	}

	bool valid(void) const
	{
		return _res_ptr.get() != nullptr;
	}

	operator bool(void) const
	{
		return valid();
	}

private:
	ResourcePtr _res_ptr;
};

template <class T> using ResWrap = ResourceWrapper<T>;

NS_END
