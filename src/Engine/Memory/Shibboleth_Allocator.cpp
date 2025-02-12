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

//////////////////////////////////////////////////////////////////////////
// This file probably should get cleaned up to remove all the #ifdef's.	//
// Potentially add some sort of system to add modules instead?			//
//////////////////////////////////////////////////////////////////////////

#include "Shibboleth_Allocator.h"
#include <Gaff_IncludeTracy.h>
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <EASTL/algorithm.h>
#include <mimalloc.h>

MSVC_DISABLE_WARNING_PUSH(4324)

#if defined(CHECK_FOR_LEAKS) && defined(GATHER_ALLOCATION_STACKTRACE)
	#include <Gaff_StackTrace.h>
#endif


NS_SHIBBOLETH

// Should we change this to being a footer? If the data comes at the end of the data,
// then we don't have to ensure the header/footer data is aligned.

// Enforce the header being 16-byte aligned so that data falls on 16-byte boundary.
struct alignas(16) AllocationHeader
{
	size_t alloc_size;
	size_t pool_index;
	char file[256] = { 0 };
	int line = 0;

	// Not a huge fan of adding this to the header data.
	static constexpr int32_t k_num_free_callbacks = 4;
	Allocator::OnFreeCallback free_callbacks[k_num_free_callbacks] = { nullptr };

#if REQUIRES_HEADER_LIST
	AllocationHeader* next = nullptr;
	AllocationHeader* prev = nullptr;
#endif

#ifdef GATHER_ALLOCATION_STACKTRACE
	Gaff::StackTrace trace;
#endif
};


Allocator::Allocator(void)
{
	MemoryPoolInfo& mem_pool_info = _tagged_pools[0];
	mem_pool_info.total_bytes_allocated = 0;
	mem_pool_info.num_allocations = 0;
	mem_pool_info.num_frees = 0;
	strncpy(mem_pool_info.pool_name, "Untagged", POOL_NAME_SIZE - 1);

	_tag_ids.push_back(Gaff::FNV1aHash32Const("Untagged"));
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

void Allocator::addOnFreeCallback(OnFreeCallback callback, void* data)
{
	AllocationHeader* const header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<int8_t*>(data) - sizeof(AllocationHeader));

	for (int32_t i = 0; i < AllocationHeader::k_num_free_callbacks; ++i) {
		if (!header->free_callbacks[i]) {
			header->free_callbacks[i] = callback;
			return;
		}
	}

	GAFF_ASSERT_MSG(false, "Exceeded number of on free callbacks!");
}

void Allocator::removeOnFreeCallback(OnFreeCallback callback, void* data)
{
	AllocationHeader* const header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<int8_t*>(data) - sizeof(AllocationHeader));

	for (int32_t i = 0; i < AllocationHeader::k_num_free_callbacks; ++i) {
		if (header->free_callbacks[i] == callback) {
			header->free_callbacks[i] = nullptr;
			break;
		}
	}
}

int32_t Allocator::getPoolIndex(const char* pool_name)
{
	Gaff::Hash32 alloc_tag = Gaff::FNV1aHash32String(pool_name);
	auto it = eastl::find(_tag_ids.begin(), _tag_ids.end(), alloc_tag);

	if (it == _tag_ids.end()) {
		GAFF_ASSERT(!_tag_ids.full());

		_tag_ids.push_back(alloc_tag);
		it = _tag_ids.end() - 1;

		strncpy(_tagged_pools[_tag_ids.size() - 1].pool_name, pool_name, POOL_NAME_SIZE - 1);
	}

	int32_t index = static_cast<int32_t>(eastl::distance(_tag_ids.begin(), it));
	return index;
}

size_t Allocator::getUsableSize(const void* data) const
{
	const AllocationHeader* const header = reinterpret_cast<const AllocationHeader*>(reinterpret_cast<const int8_t*>(data) - sizeof(AllocationHeader));
	return mi_usable_size(header);
}

void* Allocator::alloc(size_t size_bytes, size_t alignment, int32_t pool_index, const char* file, int line)
{
	void* data = mi_malloc_aligned(size_bytes + sizeof(AllocationHeader), alignment);
	TracyAllocN(data, size_bytes, getPoolName(pool_index));

	if (data) {
		setHeaderData(
			reinterpret_cast<AllocationHeader*>(data),
			pool_index,
			size_bytes,
			file,
			line
		);

		return reinterpret_cast<int8_t*>(data) + sizeof(AllocationHeader);
	}

	return nullptr;
}

void* Allocator::alloc(size_t size_bytes, int32_t pool_index, const char* file, int line)
{
	void* data = mi_malloc(size_bytes + sizeof(AllocationHeader));
	TracyAllocN(data, size_bytes, getPoolName(pool_index));

	if (data) {
		setHeaderData(
			reinterpret_cast<AllocationHeader*>(data),
			pool_index,
			size_bytes,
			file,
			line
		);

		return reinterpret_cast<int8_t*>(data) + sizeof(AllocationHeader);
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

#if defined(CHECK_FOR_MISALIGNED_POINTER)
	AllocationHeader* header = nullptr;
#else
	AllocationHeader* const header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<int8_t*>(data) - sizeof(AllocationHeader));
#endif

#if REQUIRES_HEADER_LIST
	_alloc_lock.Lock();

	#ifdef CHECK_FOR_MISALIGNED_POINTER
	// Looping over the alloc list is a terrible way of determining if a call to delete is valid.
	for (AllocationHeader* list = _list_head; list; list = list->next) {
		// Checking data range because we could be requesting a free from an interface pointer.
		if (data >= reinterpret_cast<int8_t*>(list) + sizeof(AllocationHeader) &&
			data < reinterpret_cast<int8_t*>(list) + list->alloc_size + sizeof(AllocationHeader)) {

			header = list;
			break;
		}
	}

	#ifdef CHECK_FOR_DOUBLE_FREE
	GAFF_ASSERT(header);
	#endif

	// Check if we did not free on header boundary.
	const AllocationHeader* const header_from_pointer = reinterpret_cast<AllocationHeader*>(reinterpret_cast<int8_t*>(data) - sizeof(AllocationHeader));
	GAFF_ASSERT(header == header_from_pointer);

	#elif defined(CHECK_FOR_DOUBLE_FREE)
	bool found = false;

	// Looping over the alloc list is a terrible way of determining if a call to delete is valid.
	for (AllocationHeader* list = _list_head; list; list = list->next) {
		if (list == header) {
			found = true;
			break;
		}
	}

	GAFF_ASSERT(found);
#endif

	// Process the free.
	if (header->prev) {
		header->prev->next = header->next;
	}

	if (header->next) {
		header->next->prev = header->prev;
	}

	if (header == _list_head) {
		_list_head = header->next;
	}

	_alloc_lock.Unlock();
#endif

	MemoryPoolInfo& mem_pool_info = _tagged_pools[header->pool_index];
	mem_pool_info.curr_bytes_allocated -= header->alloc_size;
	++mem_pool_info.num_frees;

	for (OnFreeCallback callback : header->free_callbacks) {
		if (callback) {
			callback(data);
		}
	}

	mi_free(header);

	TracyFreeN(header, mem_pool_info.pool_name);
}

void* Allocator::calloc(size_t num_members, size_t member_size, size_t alignment, int32_t pool_index, const char* file, int line)
{
	const size_t total_size = num_members * member_size;
	void* const buffer = alloc(total_size, alignment, pool_index, file, line);

	std::memset(buffer, 0, total_size);
	return buffer;
}

void* Allocator::calloc(size_t num_members, size_t member_size, size_t alignment, const char* file, int line)
{
	return calloc(num_members, member_size, alignment, static_cast<int32_t>(0), file, line);
}

void* Allocator::calloc(size_t num_members, size_t member_size, int32_t pool_index, const char* file, int line)
{
	const size_t total_size = num_members * member_size;
	void* const buffer = alloc(total_size, pool_index, file, line);

	std::memset(buffer, 0, total_size);
	return buffer;
}

void* Allocator::calloc(size_t num_members, size_t member_size, const char* file, int line)
{
	return calloc(num_members, member_size, static_cast<int32_t>(0), file, line);
}

void* Allocator::realloc(void* old_ptr, size_t new_size, size_t alignment, int32_t pool_index, const char* file, int line)
{
	if (!old_ptr) {
		return alloc(new_size, alignment, pool_index, file, line);
	}

	AllocationHeader* const header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<int8_t*>(old_ptr) - sizeof(AllocationHeader));

	if (header->alloc_size >= new_size) {
		return old_ptr;
	}

	void* const buffer = alloc(new_size, alignment, pool_index, file, line);
	std::memcpy(buffer, old_ptr, header->alloc_size);
	free(old_ptr);

	return buffer;
}

void* Allocator::realloc(void* old_ptr, size_t new_size, size_t alignment, const char* file, int line)
{
	return realloc(old_ptr, new_size, alignment, static_cast<int32_t>(0), file, line);
}

void* Allocator::realloc(void* old_ptr, size_t new_size, int32_t pool_index, const char* file, int line)
{
	if (!old_ptr) {
		return alloc(new_size, pool_index, file, line);
	}

	AllocationHeader* const header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<int8_t*>(old_ptr) - sizeof(AllocationHeader));

	if (header->alloc_size >= new_size) {
		return old_ptr;
	}

	void* const buffer = alloc(new_size, pool_index, file, line);
	std::memcpy(buffer, old_ptr, header->alloc_size);
	free(old_ptr);

	return buffer;
}

void* Allocator::realloc(void* old_ptr, size_t new_size, const char* file, int line)
{
	return realloc(old_ptr, new_size, static_cast<int32_t>(0), file, line);
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

const char* Allocator::getPoolName(int32_t pool_index) const
{
	return _tagged_pools[pool_index].pool_name;
}

void Allocator::setLogDir(const char8_t* log_dir)
{
	const size_t in_len = eastl::CharStrlen(log_dir) + 1;
	const size_t log_dir_len = ARRAY_SIZE(_log_dir);
	const size_t len = (log_dir_len < in_len) ? log_dir_len : in_len;

	memcpy(_log_dir, log_dir, len);
	_log_dir[len - 1] = 0;
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
	mem_pool_info.curr_bytes_allocated += size_bytes;
	++mem_pool_info.num_allocations;

	// Set the header data.
	header->alloc_size = size_bytes;
	header->pool_index = pool_index;
	strncpy(header->file, file, ARRAY_SIZE(header->file) - 1);
	header->pool_index = pool_index;
	header->line = line;
	memset(header->free_callbacks, 0, sizeof(AllocationHeader::free_callbacks));

#if REQUIRES_HEADER_LIST
	header->next = header->prev = nullptr;
#endif

#ifdef GATHER_ALLOCATION_STACKTRACE
	header->trace.captureStack("", 16, 3);
#endif

#if REQUIRES_HEADER_LIST
	// Add to the leak list.
	_alloc_lock.Lock();

	header->next = _list_head;

	if (_list_head) {
		_list_head->prev = header;
	}

	_list_head = header;

	_alloc_lock.Unlock();
#endif
}

void Allocator::writeAllocationLog(void) const
{
	if (!Gaff::CreateDir(_log_dir, 0777)) {
		return;
	}

	char log_file_name[512] = { 0 };
	snprintf(log_file_name, ARRAY_SIZE(log_file_name), "%s/AllocationLog.txt", reinterpret_cast<const char*>(_log_dir));

	Gaff::File log;

	if (!log.open(log_file_name, Gaff::File::OpenMode::Write)) {
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

	for (size_t i = 0; i < _tag_ids.size(); ++i) {
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
#ifdef CHECK_FOR_LEAKS
	if (!_list_head || !Gaff::CreateDir(_log_dir, 0777)) {
		return;
	}

	char log_file_name[512] = { 0 };
	snprintf(log_file_name, ARRAY_SIZE(log_file_name), "%s/LeakLog.txt", reinterpret_cast<const char*>(_log_dir));

	Gaff::File log;

	if (!log.open(log_file_name, Gaff::File::OpenMode::Write)) {
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
		log.printf("Address 0x%p [%s]:\n", reinterpret_cast<const int8_t*>(header) + sizeof(AllocationHeader), pool_name);

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

		AllocationHeader* const old_header = header;
		header = header->next;

		mi_free(old_header);
	}
#endif
}

NS_END

MSVC_DISABLE_WARNING_POP()
