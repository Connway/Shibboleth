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

//////////////////////////////////////////////////////////////////////////
// This file probably should get cleaned up to remove all the #ifdef's.	//
// Potentially add some sort of system to add modules instead?			//
//////////////////////////////////////////////////////////////////////////

#include "Shibboleth_Allocator.h"
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <jemalloc/jemalloc.h>
#include <eastl/algorithm.h>

#ifdef PLATFORM_WINDOWS
	// Disable "structure was padded due to alignment specifier" warning
	#pragma warning(push)
	#pragma warning(disable: 4324)
#endif

//#define GATHER_ALLOCATION_STACKTRACE

#ifdef GATHER_ALLOCATION_STACKTRACE
	#include <Gaff_DefaultAlignedAllocator.h>
	#include <Gaff_ScopedExit.h>
	#include <Gaff_StackTrace.h>
	#include <Gaff_SpinLock.h>
	#include <Gaff_Map.h>
#endif

NS_SHIBBOLETH

// Enforce the header being 16-byte aligned so that data falls on 16-byte boundary.
struct alignas(16) AllocationHeader
{
	size_t alloc_size;
	size_t pool_index;
	char file[256] = { 0 };
	int line = 0;

	AllocationHeader* next = nullptr;
	AllocationHeader* prev = nullptr;
};


Allocator::Allocator(void):
	_list_head(nullptr)
{
	MemoryPoolInfo& mem_pool_info = _tagged_pools[0];
	mem_pool_info.total_bytes_allocated = 0;
	mem_pool_info.num_allocations = 0;
	mem_pool_info.num_frees = 0;
	strncpy(mem_pool_info.pool_name, "Untagged", POOL_NAME_SIZE);
}

Allocator::~Allocator(void)
{
	writeAllocationLog();
	writeLeakLog();
}

void* Allocator::allocate(size_t n, int flags)
{
	GAFF_REF(flags);
	return alloc(n, __FILE__, __LINE__);
}

void* Allocator::allocate(size_t n, size_t alignment, size_t, int flags)
{
	GAFF_REF(flags);
	return alloc(n, alignment, __FILE__, __LINE__);
}

void Allocator::deallocate(void* p, size_t)
{
	return free(p);
}

const char* Allocator::get_name() const
{
	return "Shibboleth Global Allocator";
}

void Allocator::set_name(const char*)
{
}

int32_t Allocator::getPoolIndex(const char* pool_name)
{
	Gaff::Hash32 alloc_tag = Gaff::FNV1aHash32(pool_name, strlen(pool_name));
	auto it = eastl::find(_tag_ids.begin(), _tag_ids.end(), alloc_tag);

	if (it == _tag_ids.end()) {
		GAFF_ASSERT(!_tag_ids.full());

		_tag_ids.push_back(alloc_tag);
		it = _tag_ids.end() - 1;

		strncpy(_tagged_pools[_tag_ids.size()].pool_name, pool_name, POOL_NAME_SIZE);
	}

	int32_t index = static_cast<int32_t>(eastl::distance(_tag_ids.begin(), it));
	return index + 1;
}

void* Allocator::alloc(size_t size_bytes, size_t alignment, int32_t pool_index, const char* file, int line)
{
	void* data = je_aligned_alloc(alignment, size_bytes + sizeof(AllocationHeader));

	if (data) {
		setHeaderData(
			reinterpret_cast<AllocationHeader*>(data),
			pool_index,
			size_bytes,
			file,
			line
		);

		return reinterpret_cast<char*>(data) + sizeof(AllocationHeader);
	}

	return nullptr;
}

void* Allocator::alloc(size_t size_bytes, int32_t pool_index, const char* file, int line)
{
	void* data = je_malloc(size_bytes + sizeof(AllocationHeader));

	if (data) {
		setHeaderData(
			reinterpret_cast<AllocationHeader*>(data),
			pool_index,
			size_bytes,
			file,
			line
		);

		return reinterpret_cast<char*>(data) + sizeof(AllocationHeader);
	}

	return nullptr;
}

void* Allocator::alloc(size_t size_bytes, size_t alignment, const char* file, int line)
{
	return alloc(size_bytes, alignment, static_cast<int32_t>(0), file, line);
}

void* Allocator::alloc(size_t size_bytes, const char* file, int line)
{
	return alloc(size_bytes, static_cast<int32_t>(0), file, line);
}

void Allocator::free(void* data)
{
	GAFF_ASSERT(data);

	// Looping over the alloc list is a terrible way of determining if a call to delete is valid.

	//AllocationHeader* header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<char*>(data) - sizeof(AllocationHeader));
	AllocationHeader* header = nullptr;

	_alloc_lock.lock();

	//bool found = false;
	for (AllocationHeader* list = _list_head; list; list = list->next) {
		if (data >= reinterpret_cast<char*>(list) + sizeof(AllocationHeader) &&
			data < reinterpret_cast<char*>(list) + list->alloc_size + sizeof(AllocationHeader)) {

			header = list;
			break;
		}

		//if (header == list) {
		//	found = true;
		//	break;
		//}
	}

	//GAFF_ASSERT(found);
	GAFF_ASSERT(header);

	MemoryPoolInfo& mem_pool_info = _tagged_pools[header->pool_index];
	++mem_pool_info.num_frees;

#ifdef GATHER_ALLOCATION_STACKTRACE
	auto& allocs = g_allocs[header->pool_index];
	g_pt_locks[header->pool_index].lock();

	auto it = allocs.linearSearch(header);
	GAFF_GAFF_ASSERT(it != allocs.end());
	allocs.fastErase(it);

	g_pt_locks[header->pool_index].unlock();
#endif

	// Process the free
	if (header->prev) {
		header->prev->next = header->next;
	}

	if (header->next) {
		header->next->prev = header->prev;
	}

	if (header == _list_head) {
		_list_head = header->next;
	}

	_alloc_lock.unlock();

	je_free(header);
}

size_t Allocator::getTotalBytesAllocated(size_t pool_index) const
{
	return _tagged_pools[pool_index].total_bytes_allocated;
}

size_t Allocator::getNumAllocations(size_t pool_index) const
{
	return _tagged_pools[pool_index].num_allocations;
}

size_t Allocator::getNumFrees(size_t pool_index) const
{
	return _tagged_pools[pool_index].num_frees;
}

const char* Allocator::getPoolName(size_t pool_index) const
{
	return _tagged_pools[pool_index].pool_name;
}

void Allocator::setHeaderData(
	AllocationHeader* header,
	int32_t pool_index,
	size_t size_bytes,
	const char* file,
	int line)
{
	// Add the allocation statistics.
	MemoryPoolInfo& mem_pool_info = _tagged_pools[pool_index];
	mem_pool_info.total_bytes_allocated += size_bytes;
	++mem_pool_info.num_allocations;

	// Set the header data.
	header->alloc_size = size_bytes;
	header->pool_index = pool_index;
	strncpy_s(header->file, file, 256);
	header->pool_index = pool_index;
	header->line = line;
	header->next = header->prev = nullptr;

	// Add to the leak list.
	_alloc_lock.lock();

	header->next = _list_head;

	if (_list_head) {
		_list_head->prev = header;
	}

	_list_head = header;

	_alloc_lock.unlock();
}

void Allocator::writeAllocationLog(void) const
{
	char log_file_name[64] = { 0 };
	Gaff::GetCurrentTimeString(log_file_name, 64, "logs/AllocationLog %Y-%m-%d %H-%M-%S.txt");

	if (!Gaff::CreateDir("./logs", 0777)) {
		return;
	}

	Gaff::File log;

	if (!log.open(log_file_name, Gaff::File::WRITE)) {
		return;
	}

#ifdef GATHER_ALLOCATION_STACKTRACE
	Gaff::GetCurrentTimeString(log_file_name, 64, "logs/CallstackLog %Y-%m-%d %H-%M-%S.txt");
	Gaff::File callstack_log;

	if (callstack_log.open(log_file_name, Gaff::File::WRITE)) {
		callstack_log.printf("===================================\n");
		callstack_log.printf("    Leaked Memory Callstack Log\n");
		callstack_log.printf("===================================\n\n");
	}
#endif

	size_t total_bytes = 0;
	size_t total_allocs = 0;
	size_t total_frees = 0;

	log.printf(
		"===========================================================\n"
		"Tagged Memory Allocations Log\n"
		"===========================================================\n\n"
	);

	for (size_t i = 0; i < _tag_ids.size() + 1; ++i) {
		const MemoryPoolInfo& mem_pool_info = _tagged_pools[i];

		log.printf("%s:\n", mem_pool_info.pool_name);
		log.printf("\tBytes Allocated: %zu\n", mem_pool_info.total_bytes_allocated.load());
		log.printf("\tAllocations: %zu\n", mem_pool_info.num_allocations.load());
		log.printf("\tFrees: %zu\n\n", mem_pool_info.num_frees.load());

		total_bytes += mem_pool_info.total_bytes_allocated;
		total_allocs += mem_pool_info.num_allocations;
		total_frees += mem_pool_info.num_frees;

		if (mem_pool_info.num_allocations < mem_pool_info.num_frees) {
			log.printf("\t===========================================================\n");
			log.printf("\tWARNING: Memory was potentially allocated by another pool, but freed by this one!\n");
			log.printf("\t===========================================================\n\n");
		}

		if (mem_pool_info.num_allocations > mem_pool_info.num_frees) {
			log.printf("\t===========================================================\n");
			log.printf("\tWARNING: A memory leak was caused by this pool!\n");
			log.printf("\t===========================================================\n\n");
		}

#ifdef GATHER_ALLOCATION_STACKTRACE
		if (callstack_log.isOpen()) {
			for (auto it_st = g_allocs[i].begin(); it_st != g_allocs[i].end(); ++it_st) {
				callstack_log.printf("Address 0x%p:\n", *it_st);

				for (unsigned short j = 0; j < (*it_st)->stack_trace.getNumCapturedFrames(); ++j) {
					callstack_log.printf(
						"\t(0x%llX) [%s:(%u)] %s\n", (*it_st)->stack_trace.getAddress(j),
						(*it_st)->stack_trace.getFileName(j), (*it_st)->stack_trace.getLineNumber(j),
						(*it_st)->stack_trace.getSymbolName(j)
						);
				}

				callstack_log.printf("\n");
			}
		}
#endif
	}

	log.printf("Total Bytes Allocated: %zu\n", total_bytes);
	log.printf("Total Allocations: %zu\n", total_allocs);
	log.printf("Total Frees: %zu\n", total_frees);

	if (total_allocs != total_frees) {
		log.printf("\n===========================================================\n");
		log.printf("WARNING: Application has a memory leak(s)!\n");

#ifdef GATHER_ALLOCATION_STACKTRACE
		if (callstack_log.isOpen()) {
			log.printf("         See '%s' for allocation callstacks.\n", log_file_name);
		}
#endif

		log.printf("===========================================================\n");
	}
}

void Allocator::writeLeakLog(void) const
{
	if (!_list_head) {
		return;
	}

	char log_file_name[64] = { 0 };
	Gaff::GetCurrentTimeString(log_file_name, 64, "logs/LeakLog %Y-%m-%d %H-%M-%S.txt");

	Gaff::File log;

	if (!log.open(log_file_name, Gaff::File::WRITE)) {
		return;
	}

	log.printf(
		"===========================================================\n"
		"Leaked Memory Allocations Log\n"
		"===========================================================\n\n"
	);

	for (AllocationHeader* header = _list_head; header;) {
		log.printf("%s:(%i) [%s]\n", header->file, header->line, _tagged_pools[header->pool_index].pool_name);
		AllocationHeader* old_header = header;
		header = header->next;

		je_free(old_header);
	}
}

NS_END

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif
