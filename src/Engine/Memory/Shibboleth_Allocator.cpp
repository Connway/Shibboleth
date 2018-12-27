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

//////////////////////////////////////////////////////////////////////////
// This file probably should get cleaned up to remove all the #ifdef's.	//
// Potentially add some sort of system to add modules instead?			//
//////////////////////////////////////////////////////////////////////////

#include "Shibboleth_Allocator.h"
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <EASTL/algorithm.h>
#include <rpmalloc.h>

#ifdef _MSC_VER
	// Disable "structure was padded due to alignment specifier" warning
	#pragma warning(push)
	#pragma warning(disable: 4324)
#endif

//#define GATHER_ALLOCATION_STACKTRACE

#ifdef GATHER_ALLOCATION_STACKTRACE
	#include <Gaff_StackTrace.h>
#endif

namespace coherent_rpmalloc
{
	void* rpmalloc_allocate_memory_external(size_t bytes)
	{
		return malloc(bytes);
	}

	void rpmalloc_deallocate_memory_external(void* ptr)
	{
		free(ptr);
	}
}

NS_SHIBBOLETH

// Enforce the header being 16-byte aligned so that data falls on 16-byte boundary.
struct alignas(16) AllocationHeader
{
	size_t alloc_size;
	size_t pool_index;
	char file[256] = { 0 };
	int line = 0;

	static constexpr int32_t s_num_free_callbacks = 4;
	Allocator::OnFreeCallback free_callbacks[s_num_free_callbacks] = { nullptr };

	AllocationHeader* next = nullptr;
	AllocationHeader* prev = nullptr;

#ifdef GATHER_ALLOCATION_STACKTRACE
	Gaff::StackTrace trace;
#endif
};


Allocator::Allocator(void):
	_list_head(nullptr)
{
	MemoryPoolInfo& mem_pool_info = _tagged_pools[0];
	mem_pool_info.total_bytes_allocated = 0;
	mem_pool_info.num_allocations = 0;
	mem_pool_info.num_frees = 0;
	strncpy(mem_pool_info.pool_name, "Untagged", POOL_NAME_SIZE - 1);

	coherent_rpmalloc::rpmalloc_initialize();
}

Allocator::~Allocator(void)
{
	writeAllocationLog();
	writeLeakLog();

	coherent_rpmalloc::rpmalloc_finalize();
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

void Allocator::addOnFreeCallback(OnFreeCallback callback, void* data)
{
	AllocationHeader* const header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<char*>(data) - sizeof(AllocationHeader));

	for (int32_t i = 0; i < AllocationHeader::s_num_free_callbacks; ++i) {
		if (!header->free_callbacks[i]) {
			header->free_callbacks[i] = callback;
			return;
		}
	}

	GAFF_ASSERT_MSG(false, "Exceeded number of on free callbacks!");
}

void Allocator::removeOnFreeCallback(OnFreeCallback callback, void* data)
{
	AllocationHeader* const header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<char*>(data) - sizeof(AllocationHeader));

	for (int32_t i = 0; i < AllocationHeader::s_num_free_callbacks; ++i) {
		if (header->free_callbacks[i] == callback) {
			header->free_callbacks[i] = nullptr;
			break;
		}
	}
}

int32_t Allocator::getPoolIndex(const char* pool_name)
{
	Gaff::Hash32 alloc_tag = Gaff::FNV1aHash32(pool_name, strlen(pool_name));
	auto it = eastl::find(_tag_ids.begin(), _tag_ids.end(), alloc_tag);

	if (it == _tag_ids.end()) {
		GAFF_ASSERT(!_tag_ids.full());

		_tag_ids.push_back(alloc_tag);
		it = _tag_ids.end() - 1;

		strncpy(_tagged_pools[_tag_ids.size()].pool_name, pool_name, POOL_NAME_SIZE - 1);
	}

	int32_t index = static_cast<int32_t>(eastl::distance(_tag_ids.begin(), it));
	return index + 1;
}

void* Allocator::alloc(size_t size_bytes, size_t alignment, int32_t pool_index, const char* file, int line)
{
	void* data = coherent_rpmalloc::rpaligned_alloc(alignment, size_bytes + sizeof(AllocationHeader));

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
	void* data = coherent_rpmalloc::rpmalloc(size_bytes + sizeof(AllocationHeader));

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
	if (!data) {
		return;
	}

	//GAFF_ASSERT(data);

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

	for (OnFreeCallback callback : header->free_callbacks) {
		if (callback) {
			callback(data);
		}
	}

	coherent_rpmalloc::rpfree(header);
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

void Allocator::setLogDir(const char* log_dir)
{
	strncpy(_log_dir, log_dir, ARRAY_SIZE(_log_dir) - 1);
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
	strncpy_s(header->file, file, ARRAY_SIZE(header->file) - 1);
	header->pool_index = pool_index;
	header->line = line;
	header->next = header->prev = nullptr;
	memset(header->free_callbacks, 0, sizeof(AllocationHeader::free_callbacks));

#ifdef GATHER_ALLOCATION_STACKTRACE
	header->trace.captureStack("", 16, 3);
#endif

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
	if (!Gaff::CreateDir(_log_dir, 0777)) {
		return;
	}

	char log_file_name[64] = { 0 };
	snprintf(log_file_name, ARRAY_SIZE(log_file_name), "%s/AllocationLog.txt", _log_dir);

	Gaff::File log;

	if (!log.open(log_file_name, Gaff::File::WRITE)) {
		return;
	}

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
	}

	log.printf("Total Bytes Allocated: %zu\n", total_bytes);
	log.printf("Total Allocations: %zu\n", total_allocs);
	log.printf("Total Frees: %zu\n", total_frees);

	if (total_allocs != total_frees) {
		log.printf("\n===========================================================\n");
		log.printf("WARNING: Application has a memory leak(s)!\n");
		log.printf("         See 'LeakLog.txt' for allocation callstacks.\n");
		log.printf("===========================================================\n");
	}
}

void Allocator::writeLeakLog(void) const
{
	if (!_list_head || !Gaff::CreateDir(_log_dir, 0777)) {
		return;
	}

	char log_file_name[64] = { 0 };
	snprintf(log_file_name, ARRAY_SIZE(log_file_name), "%s/LeakLog.txt", _log_dir);

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
		const char* const pool_name = _tagged_pools[header->pool_index].pool_name;

#ifdef GATHER_ALLOCATION_STACKTRACE
		log.printf("Address 0x%p [%s]:\n", reinterpret_cast<const char*>(header) + sizeof(AllocationHeader), pool_name);

		const int32_t frames = header->trace.getNumCapturedFrames();

		for (int32_t i = 0; i < frames; ++i) {
			log.printf(
				"\t(0x%llX) [%s:(%u)] %s\n",
				header->trace.getAddress(i),
				header->trace.getFileName(i),
				header->trace.getLineNumber(i),
				header->trace.getSymbolName(i)
			);
		}

		log.printf("\n");
#else
		log.printf("%s:(%i) [%s]\n", header->file, header->line, pool_name);
#endif

		AllocationHeader* old_header = header;
		header = header->next;

		coherent_rpmalloc::rpfree(old_header);
	}
}

NS_END

#ifdef _MSC_VER
	#pragma warning(pop)
#endif
