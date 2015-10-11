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

#include "Gaff_Defines.h"
#include <new>

NS_GAFF

template <class T, class... Args>
T* construct(T* data, Args&&... args)
{
	return new (data) T(std::forward<Args>(args)...);
}

// This is giving a warning saying data is unreferenced ... what?
template <class T>
void deconstruct(T* data)
{
	// To get rid of the stupid unreferenced parameter warning in VS.
#if defined(_WIN32) || defined(_WIN64)
	(data);
#endif

	data->~T();
}

/*!
	\brief Interface that all allocators inherit from.
*/
class IAllocator
{
public:
	virtual ~IAllocator(void) {}

	virtual void* alloc(size_t size_bytes) = 0;
	virtual void free(void* data) = 0;

	template <class T, class... Args>
	T* allocArrayT(size_t count, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count));

		for (size_t i = 0; i < count; ++i) {
			construct(data + i, std::forward<Args>(args)...);
		}

		return data;
	}

	template <class T, class... Args>
	T* allocT(Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T)));
		return construct(data, std::forward<Args>(args)...);
	}

	template <class T>
	void freeArrayT(T* data, size_t count)
	{
		for (size_t i = 0; i < count; ++i) {
			(data + i)->~T();
		}

		free((void*)data);
	}

	template <class T>
	void freeT(T* data)
	{
		data->~T();
		free((void*)data);
	}
};

NS_END
