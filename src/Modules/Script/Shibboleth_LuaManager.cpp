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

#include "Shibboleth_LuaManager.h"
#include "Shibboleth_LuaHelpers.h"
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_Math.h>
#include <Gaff_JSON.h>
#include <lua.hpp>

SHIB_REFLECTION_DEFINE_BEGIN(LuaManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(LuaManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(LuaManager)

static ProxyAllocator g_allocator("Lua");

LuaManager::~LuaManager(void)
{
	for (LuaStateData& data : _states) {
		lua_close(data.state);
	}
}

bool LuaManager::initAllModulesLoaded(void)
{
	IApp& app = GetApp();
	app.getLogManager().addChannel("Lua", "LuaLog");

	const Gaff::JSON script_threads = app.getConfigs()["script_threads"];
	const int32_t num_threads = script_threads.getInt32(k_default_num_threads);

	_states.resize(static_cast<size_t>(num_threads));

	const auto ref_defs = app.getReflectionManager().getReflectionWithAttribute<RegisterWithScriptAttribute>();

	for (int32_t i = 0; i < num_threads; ++i) {
		lua_State* const state = lua_newstate(alloc, nullptr);

		if (!state) {
			// $TODO: Log error.
			return false;
		}

		_states[i].lock.reset(SHIB_ALLOCT(EA::Thread::Futex, g_allocator));
		_states[i].state = state;

		lua_atpanic(state, &LuaManager::panic);

		luaL_requiref(state, "base", luaopen_base, 1);
		lua_pop(state, 1);

		luaL_requiref(state, "coroutine", luaopen_coroutine, 1);
		lua_pop(state, 1);

		luaL_requiref(state, "debug", luaopen_debug, 1);
		lua_pop(state, 1);

		luaL_requiref(state, "math", luaopen_math, 1);
		lua_pop(state, 1);

		luaL_requiref(state, "package", luaopen_package, 1);
		lua_pop(state, 1);

		luaL_requiref(state, "table", luaopen_table, 1);
		lua_pop(state, 1);

		luaL_requiref(state, "string", luaopen_string, 1);
		lua_pop(state, 1);

		luaL_requiref(state, "utf8", luaopen_utf8, 1);
		lua_pop(state, 1);

		lua_createtable(state, 0, 0);
		lua_setglobal(state, k_loaded_chunks_name);

		RegisterBuiltIns(state);

		for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
			RegisterType(state, *ref_def);
		}
	}

//	constexpr const char* const test =
//R"(
//local v = Vec3.new(1, 2, 3)
//local v2 = Vec3.new(v)
//local x = v2.x
//local v3 = v + v2
//x = v3[2]
//v3.x = 200
//print(v)
//print(v3)
//print(tostring(1) .. " test ")
//print(tostring(1) .. " test " .. tostring(v3))
//)";
//
//	const int32_t err = luaL_loadstring(_states[0].state, test);
//
//	if (err) {
//		const char* const error = lua_tostring(_states[0].state, -1);
//		GAFF_REF(error);
//	} else {
//		lua_pcall(_states[0].state, 0, 0, 0);
//	}

	return true;
}

bool LuaManager::loadBuffer(const char* buffer, size_t size, const char* name)
{
	bool success = true;

	for (LuaStateData& data: _states) {
		EA::Thread::AutoFutex lock(*data.lock);

		const int32_t err = luaL_loadbuffer(data.state, buffer, size, name);

		if (err != LUA_OK) {
			// $TODO: Get error message from top of stack and log.
			const char* const error = lua_tostring(data.state, -1);
			success = false;

			GAFF_REF(error);
			continue;
		}

		lua_getglobal(data.state, k_loaded_chunks_name);
		lua_getfield(data.state, -1, name);

		if (!lua_isnoneornil(data.state, -1)) {
			// $TODO: Log error.
			lua_pop(data.state, 3); // top -> bottom: something, chunk_table, func
			success = false;
			continue;
		}

		lua_pop(data.state, 1); // Pop off the nil.
		lua_pushvalue(data.state, -2); // top -> bottom: chunk_table, func
		lua_pcall(data.state, 0, 1, 0); // Call the func. The function will return a table.
		luaL_checktype(data.state, -1, LUA_TTABLE); // top -> bottom: table, chunk_table, func
		lua_setfield(data.state, -2, name); // Set the table to the chunk_table.

		lua_pop(data.state, 2); // Pop off the chunk table and function.
	}

	return success;
}

lua_State* LuaManager::requestState(void)
{
	lua_State* state = nullptr;

	while (!state) {
		for (LuaStateData& data : _states) {
			if (!data.lock->TryLock()) {
				continue;
			}

			state = data.state;
			break;
		}

		if (!state) {
			EA::Thread::ThreadSleep();
		}
	}

	return state;
}

void LuaManager::returnState(lua_State* state)
{
	for (LuaStateData& data : _states) {
		if (data.state == state) {
			data.lock->Unlock();
			break;
		}
	}
}

void* LuaManager::alloc(void*, void* ptr, size_t, size_t new_size)
{
	if (new_size == 0) {
		SHIB_FREE(ptr, g_allocator);
		return nullptr;

	} else {
		return SHIB_REALLOC(ptr, new_size, g_allocator);
	}
}

int LuaManager::panic(lua_State* L)
{
	const char* const message = lua_tostring(L, -1);

	if (message) {
		GetApp().getLogManager().logMessage(LogType::Error, k_lua_log_channel, message);
	}

	return 0;
}

NS_END
