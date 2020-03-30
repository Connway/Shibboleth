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
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Math.h>
#include <lua.hpp>

//#include <sol/sol.hpp>

SHIB_REFLECTION_DEFINE_BEGIN(LuaManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(LuaManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(LuaManager)

static constexpr Gaff::Hash32 k_lua_log_channel = Gaff::FNV1aHash32Const("Lua");
static ProxyAllocator g_allocator("Lua");

bool LuaManager::initAllModulesLoaded(void)
{
	GetApp().getLogManager().addChannel("Lua", "LuaLog");

	_state = lua_newstate(alloc, nullptr);

	if (!_state) {
		// $TODO: Log error.
		return false;
	}

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

	//registerMath();

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

//static int NewVec3(lua_State* state)
//{
//	const int32_t num_args = lua_gettop(state);
//
//	Gaff::Hash64 hash = Gaff::INIT_HASH64;
//
//	for (int32_t i = 0; i < num_args; ++i) {
//		const int32_t index = -i - 1;
//		const int32_t type = lua_type(state, index);
//
//		switch (type) {
//			case LUA_TNIL:
//				break;
//			case LUA_TBOOLEAN:
//				hash = Gaff::CalcTemplateHash<bool>(hash);
//				break;
//			case LUA_TLIGHTUSERDATA:
//				break;
//			case LUA_TNUMBER:
//				if (lua_isinteger(state, index)) {
//					hash = Gaff::CalcTemplateHash<int32_t>(hash);
//				} else {
//					hash = Gaff::CalcTemplateHash<float>(hash);
//				}
//				break;
//			case LUA_TSTRING:
//				hash = Gaff::CalcTemplateHash<const U8String&>(hash);
//				break;
//			case LUA_TTABLE:
//				break;
//			case LUA_TFUNCTION:
//				break;
//			case LUA_TUSERDATA:
//				break;
//			case LUA_TTHREAD:
//				break;
//		}
//	}
//
//	Reflection<glm::vec3>::GetReflectionDefinition().getConstructor(hash);
//
//	return 0;
//}
//
//void LuaManager::registerMath(void)
//{
//	static const luaL_Reg vec3_reg[] = {
//		{ "new", NewVec3 },
//		{ NULL, NULL }
//	};
//
//	//luaL_newmetatable(_state, "glm::vec3");
//	luaL_newlib(_state, vec3_reg);
//	lua_setglobal(_state, "Vec3");
//	//lua_register(_state, "vec3", vec3_reg);
//
//	luaL_loadstring(_state, "local v = Vec3.new(1, 2, 3)");
//	lua_pcall(_state, 0, 0, 0);
//}

NS_END
