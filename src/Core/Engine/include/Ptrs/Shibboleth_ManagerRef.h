/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Shibboleth_AppUtils.h"

NS_SHIBBOLETH

template <class T, bool k_use_fast_getter = true>
class ManagerRef final
{
public:
	ManagerRef(void):
		_manager(k_use_fast_getter ? GetManagerTFast<T>() : GetManagerT<T>())
	{
	}

	const T* operator->(void) const
	{
		return get();
	}

	T* operator->(void)
	{
		return get();
	}

	const T& operator*(void) const
	{
		return _manager;
	}

	T& operator*(void)
	{
		return _manager;
	}

	const T* get(void) const
	{
		return &_manager;
	}

	T* get(void)
	{
		return &_manager;
	}

	operator const T*(void) const
	{
		return get();
	}

	operator T*(void)
	{
		return get();
	}

private:
	T& _manager;
};

template <class T>
using ManagerRefSafe = ManagerRef<T, false>;

NS_END
