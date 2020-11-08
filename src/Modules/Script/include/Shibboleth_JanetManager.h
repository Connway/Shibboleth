/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include <Shibboleth_Reflection.h>
#include <Shibboleth_IManager.h>
#include <EAThread/eathread_futex.h>
#include <janet.h>

struct JanetTraversalNode;
struct JanetScratch;

NS_SHIBBOLETH

class JanetManager final : public IManager
{
public:
	static constexpr const char* const k_loaded_chunks_name = "__loaded_chunks";
	static constexpr const char* k_thread_pool_name = "Janet";
	static constexpr Gaff::Hash32 k_thread_pool = Gaff::FNV1aHash32Const(k_thread_pool_name);
	static constexpr int32_t k_default_num_threads = 4;

	~JanetManager(void);

	bool initAllModulesLoaded(void) override;
	bool initThread(uintptr_t thread_id_int) override;
	void destroyThread(uintptr_t thread_id_int) override;

	bool loadBuffer(uintptr_t thread_id_int, const char* buffer, size_t size, const char* name);
	void unloadBuffer(uintptr_t thread_id_int, const char* name);

	//lua_State* requestState(void);
	//void returnState(lua_State* state);

private:
	struct JanetStateData final
	{
		UniquePtr<EA::Thread::Futex> lock;
		JanetTable* env;

		// Thread local variables to copy when occupying a thread.

		/* Top level dynamic bindings */
		JanetTable* janet_vm_top_dyns;

		/* Cache the core environment */
		JanetTable* janet_vm_core_env;

		/* How many VM stacks have been entered */
		int janet_vm_stackn;

		/* The current running fiber on the current thread.
		 * Set and unset by janet_run. */
		JanetFiber* janet_vm_fiber;
		JanetFiber* janet_vm_root_fiber;

		/* The current pointer to the inner most jmp_buf. The current
		 * return point for panics. */
		jmp_buf* janet_vm_jmp_buf;
		Janet* janet_vm_return_reg;

		/* The global registry for c functions. Used to store meta-data
		 * along with otherwise bare c function pointers. */
		JanetTable* janet_vm_registry;

		/* Registry for abstract abstract types that can be marshalled.
		 * We need this to look up the constructors when unmarshalling. */
		JanetTable* janet_vm_abstract_registry;

		/* Immutable value cache */
		const uint8_t** janet_vm_cache;
		uint32_t janet_vm_cache_capacity;
		uint32_t janet_vm_cache_count;
		uint32_t janet_vm_cache_deleted;

		/* Garbage collection */
		void* janet_vm_blocks;
		size_t janet_vm_gc_interval;
		size_t janet_vm_next_collection;
		size_t janet_vm_block_count;
		int janet_vm_gc_suspend;

		/* GC roots */
		Janet* janet_vm_roots;
		size_t janet_vm_root_count;
		size_t janet_vm_root_capacity;

		/* Scratch memory */
		JanetScratch** janet_scratch_mem;
		size_t janet_scratch_cap;
		size_t janet_scratch_len;

		uint8_t gensym_counters[8];
		uint32_t janet_depth;
		size_t orig_rootcount;

		JanetTraversalNode* janet_vm_traversal;
		JanetTraversalNode* janet_vm_traversal_top;
		JanetTraversalNode* janet_vm_traversal_base;

		JanetMailbox* janet_vm_mailbox;
		JanetThread* janet_vm_thread_current;
		JanetTable* janet_vm_thread_decode;

		JanetRNG janet_vm_rng;
	};

	VectorMap<EA::Thread::ThreadId, JanetStateData> _states{ ProxyAllocator("Janet") };

	//static void* alloc(void*, void* ptr, size_t, size_t new_size);
	//static int panic(lua_State* L);

	//bool loadJanetManager(const char* file_name, IFile* file);

	SHIB_REFLECTION_CLASS_DECLARE(JanetManager);
};

NS_END

SHIB_REFLECTION_DECLARE(JanetManager)
