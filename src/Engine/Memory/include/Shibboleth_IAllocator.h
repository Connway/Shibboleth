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

#include <Shibboleth_Defines.h>
#include <Gaff_IAllocator.h>

#define SHIB_ALLOC_GLOBAL_CAST GAFF_ALLOC_CAST
#define SHIB_ALLOC_CAST(Type, pool_index, allocator) reinterpret_cast<Type*>(SHIB_ALLOC(sizeof(Type), pool_index, allocator))
#define SHIB_ALLOC_ALIGNED(size, alignment, pool_index, allocator) (allocator).alloc(size, alignment, pool_index, __FILE__, __LINE__)
#define SHIB_ALLOC(size, pool_index, allocator) (allocator).alloc(size, pool_index, __FILE__, __LINE__)
#define SHIB_ALLOC_GLOBAL GAFF_ALLOC
#define SHIB_ALLOC_ARRAYT GAFF_ALLOC_ARRAYT
#define SHIB_ALLOCT GAFF_ALLOCT
#define SHIB_FREE_ARRAYT GAFF_FREE_ARRAYT
#define SHIB_FREET GAFF_FREET
#define SHIB_FREE GAFF_FREE

#define SHIB_ALLOCT_POOL(Type, pool_index, allocator, ...) Gaff::Construct(SHIB_ALLOC_CAST(Type, pool_index, allocator), __VA_ARGS__);

NS_SHIBBOLETH

class IAllocator : public Gaff::IAllocator
{
public:
	IAllocator(void) {}
	virtual ~IAllocator(void) {}

	virtual int32_t getPoolIndex(const char* pool_name) = 0;
	virtual void* alloc(size_t size_bytes, size_t alignment, int32_t pool_index, const char* file, int line) = 0;
	virtual void* alloc(size_t size_bytes, int32_t pool_index, const char* file, int line) = 0;

	virtual void* alloc(size_t size_bytes, size_t alignment, const char* file, int line) = 0;
	virtual void* alloc(size_t size_bytes, const char* file, int line) = 0;

	GAFF_NO_COPY(IAllocator);
	GAFF_NO_MOVE(IAllocator);
};

NS_END
