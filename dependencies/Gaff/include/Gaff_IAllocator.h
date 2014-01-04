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

#include "Gaff_Defines.h"
#include <new>

NS_GAFF

class IAllocator
{
public:
	virtual ~IAllocator(void) {}

	virtual void* alloc(unsigned int size_bytes) = 0;
	virtual void free(void* data) = 0;

	template <class T>
	T* allocT(unsigned int count = 1)
	{
		return (T*)alloc(sizeof(T) * count);
	}
};

//template <class T>
//T* construct(T* data)
//{
//	return new (data) T();
//}
//
//template <class T, class ARG1>
//T* construct(T* data, ARG1 arg1)
//{
//	return new (data) T(arg1);
//}
//
//template <class T, class ARG1, class ARG2>
//T* construct(T* data, ARG1 arg1, ARG2 arg2)
//{
//	return new (data)T(arg1, arg2);
//}
//
//template <class T, class ARG1, class ARG2, class ARG3>
//T* construct(T* data, ARG1 arg1, ARG2 arg2, ARG3 arg3)
//{
//	return new (data)T(arg1, arg2, arg3);
//}

template <class T, class... Args>
T* construct(T* data, Args... args)
{
	return new (data)T(args...);
}

// this is giving a warning saying data is unreferenced ... what?
template <class T>
void deconstruct(T* data)
{
	data->~T();
}

NS_END
