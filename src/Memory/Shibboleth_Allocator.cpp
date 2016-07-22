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

//////////////////////////////////////////////////////////////////////////
// This file probably should get cleaned up to remove all the #ifdef's.	//
// Potentially add some sort of system to add modules instead?			//
//////////////////////////////////////////////////////////////////////////

#include "Shibboleth_Allocator.h"
#include <Gaff_Thread.h>
#include <Gaff_Atomic.h>
#include <Gaff_Event.h>
#include <Gaff_Utils.h>
#include <Gaff_File.h>

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
	size_t pool_index;
	char file[256] = { 0 };
	int line = 0;

	AllocationHeader* next = nullptr;
	AllocationHeader* prev = nullptr;

#ifdef GATHER_ALLOCATION_STACKTRACE
		Gaff::StackTrace stack_trace;
#endif
};

#ifdef GATHER_ALLOCATION_STACKTRACE
	static Gaff::Array<AllocationHeader*, Gaff::DefaultAlignedAllocator> g_allocs[NUM_TAG_POOLS + 1];
	static Gaff::SpinLock g_pt_locks[NUM_TAG_POOLS + 1];
#endif

struct AllocThreadInfo
{
	AllocationHeader* allocs[256] = { nullptr };
	AllocationHeader* frees[256] = { nullptr };
	AllocationHeader* list_head = nullptr;

	volatile unsigned int alloc_index = 0;
	volatile unsigned int free_index = 0;

	Gaff::Event alloc_event;
	bool running = true;
};

static AllocThreadInfo g_thread_info;
static Gaff::Thread g_thread;

static Gaff::Thread::ReturnType THREAD_CALLTYPE AllocInfoThread(void* data)
{
	AllocThreadInfo* ati = reinterpret_cast<AllocThreadInfo*>(data);

	unsigned int curr_alloc_index = 0;
	unsigned int curr_free_index = 0;

	while (ati->running) {
		ati->alloc_event.wait();

		while (ati->allocs[curr_alloc_index]) {
			// Grab the info pointer
			AllocationHeader* header = ati->allocs[curr_alloc_index];

			// Free up the slot
			ati->allocs[curr_alloc_index] = nullptr;
			curr_alloc_index = (curr_alloc_index + 1) % 256;

			// Process the allocation
			header->next = ati->list_head;

			if (ati->list_head) {
				ati->list_head->prev = header;
			}

			ati->list_head = header;
		}

		while (ati->frees[curr_free_index]) {
			// Grab the info pointer
			AllocationHeader* header = ati->frees[curr_free_index];

			// If we are an allocation that hasn't been added yet, then come back later.
			if (!header->prev && !header->next && header != ati->list_head) {
				break;
			}

			// Free up the slot
			ati->frees[curr_free_index] = nullptr;
			curr_free_index = (curr_free_index + 1) % 256;

			// Process the free
			if (header->prev) {
				header->prev->next = header->next;
			}

			if (header->next) {
				header->next->prev = header->prev;
			}

			if (header == ati->list_head) {
				ati->list_head = header->next;
			}

			Gaff::AlignedFree(header);
		}
	}

	return 0;
}

Allocator::Allocator(size_t alignment):
	_alignment(alignment), _next_tag(0)
{
	MemoryPoolInfo& mem_pool_info = _tagged_pools[0];
	mem_pool_info.total_bytes_allocated = 0;
	mem_pool_info.num_allocations = 0;
	mem_pool_info.num_frees = 0;
	strncpy(mem_pool_info.pool_name, "Untagged", POOL_NAME_SIZE);

	GAFF_ASSERT(g_thread.create(AllocInfoThread, &g_thread_info));
}

Allocator::~Allocator(void)
{
	g_thread_info.running = false;
	g_thread_info.alloc_event.set();
	g_thread.wait();
	g_thread.close();

	writeAllocationLog();
	writeLeakLog();
}

size_t Allocator::getPoolIndex(const char* pool_name)
{
	unsigned int alloc_tag = Gaff::FNV1aHash32(pool_name, strlen(pool_name));
	size_t index = _tag_ids.linearSearch(0, _tag_ids.size(), alloc_tag);

	// This is going to fail if we get multiple calls to this with the same alloc_tag at the same time.
	// Unlikely that this should happen, but it is possible.
	if (index == SIZE_T_FAIL) {
		GAFF_ASSERT(_next_tag < NUM_TAG_POOLS);
		unsigned int tag_index = AtomicIncrement(&_next_tag) - 1;

		_tag_ids[tag_index] = alloc_tag;
		index = tag_index;

		strncpy(_tagged_pools[index + 1].pool_name, pool_name, POOL_NAME_SIZE);
	}

	return index + 1;
}

void* Allocator::alloc(size_t size_bytes, size_t pool_index, const char* file, int line)
{
	MemoryPoolInfo& mem_pool_info = _tagged_pools[pool_index];

	void* data = Gaff::AlignedMalloc(size_bytes + sizeof(AllocationHeader), _alignment);

	if (data) {
		AtomicUAdd(&mem_pool_info.total_bytes_allocated, size_bytes);
		AtomicIncrement(&mem_pool_info.num_allocations);

		AllocationHeader* header = reinterpret_cast<AllocationHeader*>(data);
		header->pool_index = pool_index;
		strncpy_s(header->file, file, 256);
		header->pool_index = pool_index;
		header->line = line;
		header->next = header->prev = nullptr;

#ifdef GATHER_ALLOCATION_STACKTRACE
		g_pt_locks[pool_index].lock();

		header->stack_trace.captureStack(APP_NAME);

		g_allocs[pool_index].emplacePush(header);
		g_pt_locks[pool_index].unlock();
#endif

		unsigned int next_index = (AtomicIncrement(&g_thread_info.alloc_index) - 1) % 256;

		void* orig = AtomicCompareExchangePointer(
			reinterpret_cast<void* volatile*>(g_thread_info.allocs + next_index),
			reinterpret_cast<void*>(header),
			nullptr
		);

		while (orig != nullptr) {
			orig = AtomicCompareExchangePointer(
				reinterpret_cast<void* volatile*>(g_thread_info.allocs + next_index),
				reinterpret_cast<void*>(header),
				nullptr
			);
		}

		g_thread_info.alloc_event.set();

		return reinterpret_cast<char*>(data) + sizeof(AllocationHeader);
	}

	return nullptr;
}

void* Allocator::alloc(size_t size_bytes, const char* file, int line)
{
	return alloc(size_bytes, 0, file, line);
}

void Allocator::free(void* data)
{
	GAFF_ASSERT(data);

	AllocationHeader* header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<char*>(data) - sizeof(AllocationHeader));

	MemoryPoolInfo& mem_pool_info = _tagged_pools[header->pool_index];
	AtomicIncrement(&mem_pool_info.num_frees);

#ifdef GATHER_ALLOCATION_STACKTRACE
	auto& allocs = g_allocs[header->pool_index];
	g_pt_locks[header->pool_index].lock();

	auto it = allocs.linearSearch(header);
	GAFF_GAFF_ASSERT(it != allocs.end());
	allocs.fastErase(it);

	g_pt_locks[header->pool_index].unlock();
#endif

	unsigned int next_index = (AtomicIncrement(&g_thread_info.free_index) - 1) % 256;

	void* orig = AtomicCompareExchangePointer(
		reinterpret_cast<void* volatile*>(g_thread_info.frees + next_index),
		reinterpret_cast<void*>(header), nullptr
	);

	while (orig != nullptr) {
		orig = AtomicCompareExchangePointer(
			reinterpret_cast<void* volatile*>(g_thread_info.frees + next_index),
			reinterpret_cast<void*>(header), nullptr
		);
	}

	g_thread_info.alloc_event.set();
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

	for (size_t i = 0; i < _next_tag + 1; ++i) {
		const MemoryPoolInfo& mem_pool_info = _tagged_pools[i];

		log.printf("%s:\n", mem_pool_info.pool_name);
		log.printf("\tBytes Allocated: %zu\n", mem_pool_info.total_bytes_allocated);
		log.printf("\tAllocations: %zu\n", mem_pool_info.num_allocations);
		log.printf("\tFrees: %zu\n\n", mem_pool_info.num_frees);

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
	if (!g_thread_info.list_head) {
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

	for (AllocationHeader* header = g_thread_info.list_head; header; header = header->next) {
		log.printf("%s:(%i) [%s]\n", header->file, header->line, _tagged_pools[header->pool_index].pool_name);
		Gaff::AlignedFree(header);
	}
}

NS_END

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif
