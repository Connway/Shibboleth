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

#include <Shibboleth_Defines.h>
#include <janet.h>

struct JanetTraversalNode;
struct JanetScratch;

extern "C"
{
	/* Top level dynamic bindings */
	extern JANET_THREAD_LOCAL JanetTable* janet_vm_top_dyns;

	/* Cache the core environment */
	extern JANET_THREAD_LOCAL JanetTable* janet_vm_core_env;

	/* How many VM stacks have been entered */
	extern JANET_THREAD_LOCAL int janet_vm_stackn;

	/* The current running fiber on the current thread.
	 * Set and unset by janet_run. */
	extern JANET_THREAD_LOCAL JanetFiber* janet_vm_fiber;
	extern JANET_THREAD_LOCAL JanetFiber* janet_vm_root_fiber;

	/* The current pointer to the inner most jmp_buf. The current
	 * return point for panics. */
	extern JANET_THREAD_LOCAL jmp_buf* janet_vm_jmp_buf;
	extern JANET_THREAD_LOCAL Janet* janet_vm_return_reg;

	/* The global registry for c functions. Used to store meta-data
	 * along with otherwise bare c function pointers. */
	extern JANET_THREAD_LOCAL JanetTable* janet_vm_registry;

	/* Registry for abstract abstract types that can be marshalled.
	 * We need this to look up the constructors when unmarshalling. */
	extern JANET_THREAD_LOCAL JanetTable* janet_vm_abstract_registry;

	/* Immutable value cache */
	extern JANET_THREAD_LOCAL const uint8_t** janet_vm_cache;
	extern JANET_THREAD_LOCAL uint32_t janet_vm_cache_capacity;
	extern JANET_THREAD_LOCAL uint32_t janet_vm_cache_count;
	extern JANET_THREAD_LOCAL uint32_t janet_vm_cache_deleted;

	/* Garbage collection */
	extern JANET_THREAD_LOCAL void* janet_vm_blocks;
	extern JANET_THREAD_LOCAL size_t janet_vm_gc_interval;
	extern JANET_THREAD_LOCAL size_t janet_vm_next_collection;
	extern JANET_THREAD_LOCAL size_t janet_vm_block_count;
	extern JANET_THREAD_LOCAL int janet_vm_gc_suspend;

	/* GC roots */
	extern JANET_THREAD_LOCAL Janet* janet_vm_roots;
	extern JANET_THREAD_LOCAL size_t janet_vm_root_count;
	extern JANET_THREAD_LOCAL size_t janet_vm_root_capacity;

	/* Scratch memory */
	extern JANET_THREAD_LOCAL JanetScratch** janet_scratch_mem;
	extern JANET_THREAD_LOCAL size_t janet_scratch_cap;
	extern JANET_THREAD_LOCAL size_t janet_scratch_len;

	extern JANET_THREAD_LOCAL uint8_t gensym_counter[8];

	extern JANET_THREAD_LOCAL JanetTraversalNode* janet_vm_traversal;
	extern JANET_THREAD_LOCAL JanetTraversalNode* janet_vm_traversal_top;
	extern JANET_THREAD_LOCAL JanetTraversalNode* janet_vm_traversal_base;
}

NS_SHIBBOLETH

class JanetState final
{
public:
	void restore(void) const
	{
		janet_vm_top_dyns = _janet_vm_top_dyns;
		janet_vm_core_env = _janet_vm_core_env;
		janet_vm_stackn = _janet_vm_stackn;
		janet_vm_fiber = _janet_vm_fiber;
		janet_vm_root_fiber = _janet_vm_root_fiber;
		janet_vm_jmp_buf = _janet_vm_jmp_buf;
		janet_vm_return_reg = _janet_vm_return_reg;
		janet_vm_registry = _janet_vm_registry;
		janet_vm_abstract_registry = _janet_vm_abstract_registry;
		janet_vm_cache = _janet_vm_cache;
		janet_vm_cache_capacity = _janet_vm_cache_capacity;
		janet_vm_cache_count = _janet_vm_cache_count;
		janet_vm_cache_deleted = _janet_vm_cache_deleted;
		janet_vm_blocks = _janet_vm_blocks;
		janet_vm_gc_interval = _janet_vm_gc_interval;
		janet_vm_next_collection = _janet_vm_next_collection;
		janet_vm_block_count = _janet_vm_block_count;
		janet_vm_gc_suspend = _janet_vm_gc_suspend;
		janet_vm_roots = _janet_vm_roots;
		janet_vm_root_count = _janet_vm_root_count;
		janet_vm_root_capacity = _janet_vm_root_capacity;
		janet_scratch_mem = _janet_scratch_mem;
		janet_scratch_cap = _janet_scratch_cap;
		janet_scratch_len = _janet_scratch_len;
		janet_vm_traversal = _janet_vm_traversal;
		janet_vm_traversal_top = _janet_vm_traversal_top;
		janet_vm_traversal_base = _janet_vm_traversal_base;
		*janet_default_rng() = _janet_vm_rng;
		janet_threads_set_state(_janet_vm_mailbox, _janet_vm_thread_current, _janet_vm_thread_decode);

		for (int32_t i = 0; i < 8; ++i) {
			gensym_counter[i] = _gensym_counter[i];
		}
	}

	void save(void)
	{
		_janet_vm_top_dyns = janet_vm_top_dyns;
		_janet_vm_core_env = janet_vm_core_env;
		_janet_vm_stackn = janet_vm_stackn;
		_janet_vm_fiber = janet_vm_fiber;
		_janet_vm_root_fiber = janet_vm_root_fiber;
		_janet_vm_jmp_buf = janet_vm_jmp_buf;
		_janet_vm_return_reg = janet_vm_return_reg;
		_janet_vm_registry = janet_vm_registry;
		_janet_vm_abstract_registry = janet_vm_abstract_registry;
		_janet_vm_cache = janet_vm_cache;
		_janet_vm_cache_capacity = janet_vm_cache_capacity;
		_janet_vm_cache_count = janet_vm_cache_count;
		_janet_vm_cache_deleted = janet_vm_cache_deleted;
		_janet_vm_blocks = janet_vm_blocks;
		_janet_vm_gc_interval = janet_vm_gc_interval;
		_janet_vm_next_collection = janet_vm_next_collection;
		_janet_vm_block_count = janet_vm_block_count;
		_janet_vm_gc_suspend = janet_vm_gc_suspend;
		_janet_vm_roots = janet_vm_roots;
		_janet_vm_root_count = janet_vm_root_count;
		_janet_vm_root_capacity = janet_vm_root_capacity;
		_janet_scratch_mem = janet_scratch_mem;
		_janet_scratch_cap = janet_scratch_cap;
		_janet_scratch_len = janet_scratch_len;
		_janet_vm_traversal = janet_vm_traversal;
		_janet_vm_traversal_top = janet_vm_traversal_top;
		_janet_vm_traversal_base = janet_vm_traversal_base;
		_janet_vm_rng = *janet_default_rng();
		janet_threads_get_state(&_janet_vm_mailbox, &_janet_vm_thread_current, &_janet_vm_thread_decode);

		for (int32_t i = 0; i < 8; ++i) {
			_gensym_counter[i] = gensym_counter[i];
		}
	}

private:
	/* Top level dynamic bindings */
	JanetTable* _janet_vm_top_dyns;

	/* Cache the core environment */
	JanetTable* _janet_vm_core_env;

	/* How many VM stacks have been entered */
	int _janet_vm_stackn;

	/* The current running fiber on the current thread.
	 * Set and unset by janet_run. */
	JanetFiber* _janet_vm_fiber;
	JanetFiber* _janet_vm_root_fiber;

	/* The current pointer to the inner most jmp_buf. The current
	 * return point for panics. */
	jmp_buf* _janet_vm_jmp_buf;
	Janet* _janet_vm_return_reg;

	/* The global registry for c functions. Used to store meta-data
	 * along with otherwise bare c function pointers. */
	JanetTable* _janet_vm_registry;

	/* Registry for abstract abstract types that can be marshalled.
	 * We need this to look up the constructors when unmarshalling. */
	JanetTable* _janet_vm_abstract_registry;

	/* Immutable value cache */
	const uint8_t** _janet_vm_cache;
	uint32_t _janet_vm_cache_capacity;
	uint32_t _janet_vm_cache_count;
	uint32_t _janet_vm_cache_deleted;

	/* Garbage collection */
	void* _janet_vm_blocks;
	size_t _janet_vm_gc_interval;
	size_t _janet_vm_next_collection;
	size_t _janet_vm_block_count;
	int _janet_vm_gc_suspend;

	/* GC roots */
	Janet* _janet_vm_roots;
	size_t _janet_vm_root_count;
	size_t _janet_vm_root_capacity;

	/* Scratch memory */
	JanetScratch** _janet_scratch_mem;
	size_t _janet_scratch_cap;
	size_t _janet_scratch_len;

	uint8_t _gensym_counter[8];

	JanetTraversalNode* _janet_vm_traversal;
	JanetTraversalNode* _janet_vm_traversal_top;
	JanetTraversalNode* _janet_vm_traversal_base;

	JanetMailbox* _janet_vm_mailbox;
	JanetThread* _janet_vm_thread_current;
	JanetTable* _janet_vm_thread_decode;

	JanetRNG _janet_vm_rng;
};

NS_END
