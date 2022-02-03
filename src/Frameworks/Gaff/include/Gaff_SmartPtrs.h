/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class AllocatorDeleter
{
public:
	AllocatorDeleter(void) = default;
	GAFF_COPY_DEFAULT(AllocatorDeleter);
	GAFF_MOVE_DEFAULT(AllocatorDeleter);

	void operator()(T* ptr) const
	{
		if (ptr) {
			GAFF_FREET(ptr, _allocator);
		}
	}

private:
	mutable Allocator _allocator;
};


template <class T>
using SharedPtr = eastl::shared_ptr<T>;

template <class T, class Allocator, class... Args>
SharedPtr<T> MakeShared(const Allocator& allocator = Allocator(), Args&&... args)
{
	return eastl::allocate_shared<T>(allocator, std::forward(args)...);
}

template <class T, class Allocator>
SharedPtr<T> MakeShared(T* value, const Allocator& allocator = Allocator())
{
	return SharedPtr<T>(value, AllocatorDeleter<T, Allocator>(), allocator);
}


template <class T, class Allocator = DefaultAllocator>
using UniquePtr = eastl::unique_ptr< T, AllocatorDeleter<T, Allocator> >;

NS_END
