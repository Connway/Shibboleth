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

#include "Shibboleth_Allocator.h"
#include <Gaff_Atomic.h>
#include <Gaff_Utils.h>
#include <Gaff_File.h>

NS_SHIBBOLETH

Allocator::Allocator(size_t alignment):
	_tagged_pools(Gaff::DefaultAlignedAllocator(16)), _alignment(alignment), _global_allocator(alignment)
{
	MemoryPoolInfo& mem_pool_info = _tagged_pools[0];
	mem_pool_info.total_bytes_allocated = 0;
	mem_pool_info.num_allocations = 0;
	mem_pool_info.num_frees = 0;

#ifdef TRACK_POINTER_ALLOCATIONS
	mem_pool_info.pa_lock = _global_allocator.template allocT<Gaff::SpinLock>();
	mem_pool_info.wf_lock = _global_allocator.template allocT<Gaff::SpinLock>();
#endif

	strncpy(mem_pool_info.pool_name, "Untagged Allocations", POOL_NAME_SIZE);
}

Allocator::~Allocator(void)
{
	char log_file_name[64] = { 0 };
	Gaff::GetCurrentTimeString(log_file_name, 64, "Logs/AllocationLog %Y-%m-%d %H-%M-%S.txt");

	if (!Gaff::CreateDir("./Logs", 0777)) {
		return;
	}

	Gaff::File log;

	if (!log.open(log_file_name, Gaff::File::WRITE)) {
		return;
	}

	size_t total_bytes = 0;
	unsigned int total_allocs = 0;
	unsigned int total_frees = 0;

	log.printf("===========================================================\n");
	log.printf("Tagged Memory Allocations Log\n");
	log.printf("===========================================================\n\n");

	for (auto it = _tagged_pools.begin(); it != _tagged_pools.end(); ++it) {
#ifdef TRACK_POINTER_ALLOCATIONS
		it->second.pointers_allocated.clear();
		_global_allocator.freeT(it->second.pa_lock);
		_global_allocator.freeT(it->second.wf_lock);
#endif

		log.printf("%s:\n", it->second.pool_name);
		log.printf("\tBytes Allocated: %i\n", it->second.total_bytes_allocated);
		log.printf("\tAllocations: %i\n", it->second.num_allocations);
		log.printf("\tFrees: %i\n\n", it->second.num_frees);

		total_bytes += it->second.total_bytes_allocated;
		total_allocs += it->second.num_allocations;
		total_frees += it->second.num_frees;

#ifdef TRACK_POINTER_ALLOCATIONS
		if (!it->second.wrong_free.empty()) {
			log.printf("\t===========================================================\n");
			log.printf("\tERROR: This pool freed memory that belonged to another pool!\n");
			log.printf("\t===========================================================\n");

			for (auto it_wf = it->second.wrong_free.begin(); it_wf != it->second.wrong_free.end(); ++it_wf) {
				log.printf("\t\t%s: %i\n", _tagged_pools[it_wf->first].pool_name, it_wf->second);
			}

			log.printf("\n");

			it->second.wrong_free.clear();
		}
#else
		if (it->second.num_allocations < it->second.num_frees) {
			log.printf("\t===========================================================\n");
			log.printf("\tWARNING: Memory was potentially allocated by another pool, but freed by this one!\n");
			log.printf("\t===========================================================\n\n");
		}
#endif

		if (it->second.num_allocations > it->second.num_frees) {
			log.printf("\t===========================================================\n");
			log.printf("\tWARNING: A memory leak was caused by this pool!\n");
			log.printf("\t===========================================================\n\n");

		}
	}

	log.printf("Total Bytes Allocated: %i\n", total_bytes);
	log.printf("Total Allocations: %i\n", total_allocs);
	log.printf("Total Frees: %i\n", total_frees);

	if (total_allocs != total_frees) {
		log.printf("\n===========================================================\n");
		log.printf("WARNING: Application has a memory leak(s)!\n");
		log.printf("===========================================================\n");
	}
}

void Allocator::createMemoryPool(const char* pool_name, unsigned int alloc_tag)
{
	if (!_tagged_pools.hasElementWithKey(alloc_tag)) {
		MemoryPoolInfo& mem_pool_info = _tagged_pools[alloc_tag];
		mem_pool_info.total_bytes_allocated = 0;
		mem_pool_info.num_allocations = 0;
		mem_pool_info.num_frees = 0;

		strncpy(mem_pool_info.pool_name, pool_name, POOL_NAME_SIZE);

#ifdef TRACK_POINTER_ALLOCATIONS
		mem_pool_info.pa_lock = _global_allocator.template allocT<Gaff::SpinLock>();
		mem_pool_info.wf_lock = _global_allocator.template allocT<Gaff::SpinLock>();
#endif
	}
}

void* Allocator::alloc(size_t size_bytes, unsigned int alloc_tag)
{
	assert(_tagged_pools.hasElementWithKey(alloc_tag));
	MemoryPoolInfo& mem_pool_info = _tagged_pools[alloc_tag];

	void* data = _aligned_malloc(size_bytes, _alignment);

	if (data) {
		AtomicUAdd(&mem_pool_info.total_bytes_allocated, size_bytes);
		AtomicIncrement(&mem_pool_info.num_allocations);

#ifdef TRACK_POINTER_ALLOCATIONS
		mem_pool_info.pa_lock->lock();
		mem_pool_info.pointers_allocated.push(data);
		mem_pool_info.pa_lock->unlock();
#endif
	}

	return data;
}

void Allocator::free(void* data, unsigned int alloc_tag)
{
	assert(_tagged_pools.hasElementWithKey(alloc_tag));
	assert(data);

	MemoryPoolInfo& mem_pool_info = _tagged_pools[alloc_tag];
	AtomicIncrement(&mem_pool_info.num_frees);

	_aligned_free(data);

#ifdef TRACK_POINTER_ALLOCATIONS
	mem_pool_info.pa_lock->lock();

	auto it_ptr = mem_pool_info.pointers_allocated.linearSearch(data);

	if (it_ptr == mem_pool_info.pointers_allocated.end()) {
		mem_pool_info.pa_lock->unlock();

		for (auto it_pool = _tagged_pools.begin(); it_pool != _tagged_pools.end(); ++it_pool) {
			it_pool->second.pa_lock->lock();

			it_ptr = it_pool->second.pointers_allocated.linearSearch(data);

			if (it_ptr != it_pool->second.pointers_allocated.end()) {
				mem_pool_info.wf_lock->lock();
				++mem_pool_info.wrong_free[it_pool->first];
				// get stack trace
				mem_pool_info.wf_lock->unlock();
			}

			it_pool->second.pa_lock->unlock();
		}

	} else {
		mem_pool_info.pointers_allocated.fastErase(it_ptr);
		mem_pool_info.pa_lock->unlock();
	}
#endif
}

void* Allocator::alloc(size_t size_bytes)
{

	return alloc(size_bytes, 0);
}

void Allocator::free(void* data)
{
	free(data, 0);
}

size_t Allocator::getTotalBytesAllocated(unsigned int alloc_tag) const
{
	return _tagged_pools[alloc_tag].total_bytes_allocated;
}

unsigned int Allocator::getNumAllocations(unsigned int alloc_tag) const
{
	return _tagged_pools[alloc_tag].num_allocations;
}

unsigned int Allocator::getNumFrees(unsigned int alloc_tag) const
{
	return _tagged_pools[alloc_tag].num_frees;
}

NS_END
