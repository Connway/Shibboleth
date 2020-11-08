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

#include "Shibboleth_JanetManager.h"
#include <Shibboleth_JobPool.h>
#include <janet.h>

SHIB_REFLECTION_DEFINE_BEGIN(JanetManager)
	.base<IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(JanetManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(JanetManager)

static ProxyAllocator g_allocator("Janet");

JanetManager::~JanetManager(void)
{
}

bool JanetManager::initAllModulesLoaded(void)
{
	return true;
}

// Janet, unlike Lua, has no analog to lua_State. It only has global, thread local variables.
bool JanetManager::initThread(uintptr_t thread_id_int)
{
	janet_init();

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	JanetTable* const env = janet_core_env(nullptr);

	// Add loaded chunks table to the global environment.
	janet_table_put(env, janet_wrap_string(k_loaded_chunks_name), janet_wrap_table(janet_table(0)));

	JanetStateData& state = _states[thread_id];
	state.lock.reset(SHIB_ALLOCT(EA::Thread::Futex, g_allocator));
	state.env = env;

	return true;
}

void JanetManager::destroyThread(uintptr_t /*thread_id*/)
{
	janet_deinit();
}

bool JanetManager::loadBuffer(uintptr_t thread_id_int, const char* buffer, size_t size, const char* name)
{
	if (!name) {
		// $TODO: Log error.
		return false;
	}

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	JanetStateData& state = _states[thread_id];
	EA::Thread::AutoFutex lock(*state.lock);

	const Janet loaded_chunks = janet_table_get(state.env, janet_wrap_string(k_loaded_chunks_name));
	const Janet key = janet_wrap_string(name);
	GAFF_ASSERT(janet_checktype(loaded_chunks, JANET_TABLE));

	JanetTable* const loaded_chunks_table = janet_unwrap_table(loaded_chunks);

	// Something is already in this slot.
	if (!janet_checktype(janet_table_get(loaded_chunks_table, key), JANET_NIL)) {
		// $TODO: Log error.
		return false;
	}

	Janet value;
	const int result = janet_dobytes(state.env, reinterpret_cast<const uint8_t*>(buffer), static_cast<int32_t>(size), nullptr, &value);

	if (result) {
		if (janet_checktype(value, JANET_STRING)) {
			// $TODO: Log error.
		}

		return false;
	}

	janet_table_put(loaded_chunks_table, key, value);
	return true;
}

void JanetManager::unloadBuffer(uintptr_t thread_id_int, const char* name)
{
	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	JanetStateData& state = _states[thread_id];
	EA::Thread::AutoFutex lock(*state.lock);

	const Janet loaded_chunks = janet_table_get(state.env, janet_wrap_string(k_loaded_chunks_name));
	GAFF_ASSERT(janet_checktype(loaded_chunks, JANET_TABLE));
	
	janet_table_remove(janet_unwrap_table(loaded_chunks), janet_wrap_string(name));
}

NS_END
