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
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Math.h>
#include <lua.hpp>

SHIB_REFLECTION_DEFINE_BEGIN(LuaManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(LuaManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(LuaManager)


static constexpr Gaff::Hash32 k_lua_log_channel = Gaff::FNV1aHash32Const("Lua");
static ProxyAllocator g_allocator("Lua");

LuaManager::~LuaManager(void)
{
	if (_state) {
		lua_close(_state);
	}
}

bool LuaManager::initAllModulesLoaded(void)
{
	GetApp().getLogManager().addChannel("Lua", "LuaLog");

	_state = lua_newstate(alloc, nullptr);

	if (!_state) {
		// $TODO: Log error.
		return false;
	}

	lua_atpanic(_state, &LuaManager::panic);

	luaL_requiref(_state, "base", luaopen_base, 1);
	lua_pop(_state, 1);

	luaL_requiref(_state, "coroutine", luaopen_coroutine, 1);
	lua_pop(_state, 1);

	luaL_requiref(_state, "debug", luaopen_debug, 1);
	lua_pop(_state, 1);

	luaL_requiref(_state, "math", luaopen_math, 1);
	lua_pop(_state, 1);

	luaL_requiref(_state, "package", luaopen_package, 1);
	lua_pop(_state, 1);

	luaL_requiref(_state, "table", luaopen_table, 1);
	lua_pop(_state, 1);

	luaL_requiref(_state, "string", luaopen_string, 1);
	lua_pop(_state, 1);

	luaL_requiref(_state, "utf8", luaopen_utf8, 1);
	lua_pop(_state, 1);

	RegisterBuiltIns(_state);

	Reflection<glm::vec3>::Init();
	RegisterType(_state, Reflection<glm::vec3>::GetReflectionDefinition());

	luaL_loadstring(_state, "local v = Vec3.new(1, 2, 3)\nlocal v2 = Vec3.new(v)\nlocal x = v2.x\nlocal v3 = v + v2\nx = v3[2]\nv3 = Vec3.length()");
	lua_pcall(_state, 0, 0, 0);

	return true;
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
	size_t size;
	const char* const message = lua_tolstring(L, -1, &size);

	if (message) {
		GetApp().getLogManager().logMessage(LogType::Error, k_lua_log_channel, message);
	}

	lua_settop(L, 0);
	return -1;
}

NS_END
