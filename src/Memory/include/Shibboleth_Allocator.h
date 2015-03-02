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

#pragma once

#include "Shibboleth_IAllocator.h"
#include <Gaff_DefaultAlignedAllocator.h>
#include <Gaff_IAllocator.h>
#include <Gaff_SpinLock.h>
#include <Gaff_Map.h>

#define POOL_NAME_SIZE 32

#define TRACK_POINTER_ALLOCATIONS
//#define GATHER_ALLOCATION_STACKTRACE

#if defined(SYMBOL_BUILD) && defined(GATHER_ALLOCATION_STACKTRACE)
	#include <Gaff_StackTrace.h>
#endif

NS_SHIBBOLETH

class Allocator : public IAllocator
{
public:
	Allocator(size_t alignment = 16);
	~Allocator(void);

	void createMemoryPool(const char* pool_name, unsigned int alloc_tag);
	void* alloc(size_t size_bytes, unsigned int alloc_tag);
	void free(void* data, unsigned int alloc_tag);

	void* alloc(size_t size_bytes);
	void free(void* data);

	INLINE size_t getTotalBytesAllocated(unsigned int alloc_tag) const;
	INLINE size_t getNumAllocations(unsigned int alloc_tag) const;
	INLINE size_t getNumFrees(unsigned int alloc_tag) const;

private:
	struct MemoryPoolInfo
	{
		volatile size_t total_bytes_allocated;
		volatile size_t num_allocations;
		volatile size_t num_frees;
		char pool_name[POOL_NAME_SIZE];

#ifdef TRACK_POINTER_ALLOCATIONS
		Gaff::Map<unsigned int, unsigned int, Gaff::DefaultAlignedAllocator> wrong_free = Gaff::Map<unsigned int, unsigned int, Gaff::DefaultAlignedAllocator>(Gaff::DefaultAlignedAllocator(16));
		Gaff::Array<void*, Gaff::DefaultAlignedAllocator> pointers_allocated = Gaff::Array<void*, Gaff::DefaultAlignedAllocator>(Gaff::DefaultAlignedAllocator(16));
		Gaff::SpinLock* wf_lock;
		Gaff::SpinLock* pa_lock;
#endif
#if defined(SYMBOL_BUILD) && defined(GATHER_ALLOCATION_STACKTRACE)
		Gaff::Map<void*, Gaff::StackTrace, Gaff::DefaultAlignedAllocator> stack_traces = Gaff::Map<void*, Gaff::StackTrace, Gaff::DefaultAlignedAllocator>(Gaff::DefaultAlignedAllocator(16));
		Gaff::SpinLock* st_lock;
#endif
	};

	Gaff::Map<unsigned int, MemoryPoolInfo, Gaff::DefaultAlignedAllocator> _tagged_pools;
	size_t _alignment;

	Gaff::DefaultAlignedAllocator _global_allocator;

	GAFF_NO_MOVE(Allocator);
};

NS_END
