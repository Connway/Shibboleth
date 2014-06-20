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

#include "Shibboleth_Defines.h"
#include <Gaff_DefaultAlignedAllocator.h>
#include <Gaff_IAllocator.h>
#include <Gaff_Map.h>

#define POOL_NAME_SIZE 32

NS_SHIBBOLETH

class Allocator : public Gaff::IAllocator
{
public:
	Allocator(size_t alignment = 16);
	~Allocator(void);

	void createMemoryPool(const char* pool_name, unsigned int alloc_tag);
	void* alloc(unsigned int size_bytes, unsigned int alloc_tag);
	void free(void* data, unsigned int alloc_tag);

	void* alloc(unsigned int size_bytes);
	void free(void* data);

	INLINE unsigned int getTotalBytesAllocated(unsigned int alloc_tag) const;
	INLINE unsigned int getNumAllocations(unsigned int alloc_tag) const;
	INLINE unsigned int getNumFrees(unsigned int alloc_tag) const;

private:
	struct MemoryPoolInfo
	{
		volatile unsigned int total_bytes_allocated;
		volatile unsigned int num_allocations;
		volatile unsigned int num_frees;
		char pool_name[POOL_NAME_SIZE];
	};

	Gaff::Map<unsigned int, MemoryPoolInfo, Gaff::DefaultAlignedAllocator> _tagged_pools;
	size_t _alignment;

	GAFF_NO_MOVE(Allocator);
};

INLINE void SetAllocator(Allocator& allocator);
INLINE Allocator& GetAllocator(void);

void* ShibbolethAllocate(size_t size);
void ShibbolethFree(void* data);

NS_END
