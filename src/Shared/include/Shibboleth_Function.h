/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_ProxyAllocator.h"
#include <EASTL/functional.h>

NS_SHIBBOLETH

template <class T, class Ret, class... Args>
class ConstMemFuncBinder
{
public:
	using MemFunc = Ret (T::*)(Args...) const;

	ConstMemFuncBinder(T* obj, MemFunc func):
		_func(func), _obj(obj)
	{
	}

	Ret operator()(Args&&... args)
	{
		return (_obj->*_func)(std::forward<Args>(args)...);
	}

private:
	MemFunc _func;
	T* _obj;
};

template <class T, class Ret, class... Args>
class MemFuncBinder
{
public:
	using MemFunc = Ret (T::*)(Args...);

	MemFuncBinder(T* obj, MemFunc func):
		_func(func), _obj(obj)
	{
	}

	Ret operator()(Args&&... args)
	{
		return (_obj->*_func)(std::forward<Args>(args)...);
	}

private:
	MemFunc _func;
	T* _obj;
};


template <class Fn, class T>
eastl::function<Fn> Func(const ProxyAllocator& allocator, T functor)
{
	static_assert(std::is_function<Fn>::value, "Shibboleth::Func requires template argument Func to be a function type!");
	return eastl::function<Fn>(eastl::allocator_arg, allocator, functor);
}

template <class Fn, class T>
eastl::function<Fn> Func(T functor)
{
	ProxyAllocator allocator;
	return Func<Fn>(allocator, functor);
}

template <class T, class Ret, class... Args>
eastl::function<Ret (Args...)> MemberFunc(const T* obj, Ret (T::*func)(Args...) const, const ProxyAllocator& allocator = ProxyAllocator())
{
	ConstMemFuncBinder<T, Ret, Args...> mem_func_binder(obj, func);
	return Func<Ret (Args...)>(allocator, mem_func_binder);
}

template <class T, class Ret, class... Args>
eastl::function<Ret (Args...)> MemberFunc(T* obj, Ret (T::*func)(Args...), const ProxyAllocator& allocator = ProxyAllocator())
{
	MemFuncBinder<T, Ret, Args...> mem_func_binder(obj, func);
	return Func<Ret (Args...)>(allocator, mem_func_binder);
}

NS_END
