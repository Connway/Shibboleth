/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
#include <Gaff_StaticArray.h>
#include <Gaff_SpinLock.h>

#define NUM_TAG_POOLS 16
#define POOL_NAME_SIZE 32

NS_SHIBBOLETH

struct AllocationHeader;

class Allocator : public IAllocator
{
public:
	explicit Allocator(size_t alignment = 16);
	~Allocator(void);

	size_t getPoolIndex(const char* pool_name) override;
	void* alloc(size_t size_bytes, size_t pool_index, const char* file, int line) override;
	void* alloc(size_t size_bytes, const char* file, int line) override;
	void free(void* data) override;

	INLINE size_t getTotalBytesAllocated(size_t pool_index) const;
	INLINE size_t getNumAllocations(size_t pool_index) const;
	INLINE size_t getNumFrees(size_t pool_index) const;
	INLINE const char* getPoolName(size_t pool_index) const;

private:
	struct MemoryPoolInfo
	{
		MemoryPoolInfo(void):
			total_bytes_allocated(0), num_allocations(0),
			num_frees(0)
		{
			pool_name[0] = 0;
		}

		volatile size_t total_bytes_allocated;
		volatile size_t num_allocations;
		volatile size_t num_frees;
		char pool_name[POOL_NAME_SIZE];
	};

	AllocationHeader* _list_head;
	size_t _alignment;

	MemoryPoolInfo _tagged_pools[NUM_TAG_POOLS + 1];
	Gaff::StaticArray<unsigned int, NUM_TAG_POOLS> _tag_ids;
	Gaff::SpinLock _alloc_lock;
	volatile unsigned int _next_tag;

	void writeAllocationLog(void) const;
	void writeLeakLog(void) const;

	GAFF_NO_MOVE(Allocator);
};

NS_END
