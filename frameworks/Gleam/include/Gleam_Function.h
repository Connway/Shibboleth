/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gleam_ProxyAllocator.h"
#include <Gaff_Function.h>

NS_GLEAM

template <class Fn, class T>
eastl::function<Fn> Func(T functor, const ProxyAllocator& allocator = ProxyAllocator())
{
	return Gaff::Func<Fn>(functor, allocator);
}

template <class T, class Ret, class... Args>
eastl::function<Ret (Args...)> MemberFunc(const T* obj, Ret (T::*func)(Args...) const, const ProxyAllocator& allocator = ProxyAllocator())
{
	return Gaff::MemberFunc(obj, func, allocator);
}

template <class T, class Ret, class... Args>
eastl::function<Ret (Args...)> MemberFunc(T* obj, Ret (T::*func)(Args...), const ProxyAllocator& allocator = ProxyAllocator())
{
	return Gaff::MemberFunc(obj, func, allocator);
}

NS_END