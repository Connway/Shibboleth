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

#include "Shibboleth_LuaManager.h"
#include <Shibboleth_IApp.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable: 4100 4244 4267 4800)
#endif

#include <LuaState.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif

NS_SHIBBOLETH

static ProxyAllocator g_lua_allocator("Lua");

static void* LuaAllocator(void*, void* ptr, size_t original_size, size_t new_size)
{
	if (original_size >= new_size) {
		return ptr;
	}

	if (!ptr) {
		return SHIB_ALLOC_GLOBAL(new_size, g_lua_allocator);
	}

	if (!new_size) {
		SHIB_FREE(ptr, g_lua_allocator);
		return nullptr;
	}

	void* new_ptr = SHIB_ALLOC_GLOBAL(new_size, g_lua_allocator);
	memcpy_s(new_ptr, new_size, ptr, original_size);
	SHIB_FREE(ptr, g_lua_allocator);

	return new_ptr;

	//if (ptr) {
	//	SHIB_FREE(ptr, g_lua_allocator);
	//}

	//return (nsize) ? SHIB_ALLOC_GLOBAL(nsize, g_lua_allocator) : nullptr;
}

REF_IMPL_REQ(LuaManager);
SHIB_REF_IMPL(LuaManager)
.addBaseClassInterfaceOnly<LuaManager>()
;

const char* LuaManager::GetFriendlyName(void)
{
	return "Lua Manager";
}

LuaManager::LuaManager(void)
{
}

LuaManager::~LuaManager(void)
{
}

const char* LuaManager::getName(void) const
{
	return GetFriendlyName();
}

void LuaManager::addRegistrant(const Gaff::FunctionBinder<void, lua::State&>& registrant)
{
	_registrants.push(registrant);
}

lua::State* LuaManager::createNewState(void)
{
	// Until LuaJIT 64-bit builds support custom allocators, we have to leave this commented out. :(
	//lua::State* state = SHIB_ALLOCT(lua::State, g_lua_allocator, lua_newstate(LuaAllocator, nullptr));
	lua::State* state = SHIB_ALLOCT(lua::State, g_lua_allocator);

	if (state) {
		for (auto it = _registrants.begin(); it != _registrants.end(); ++it) {
			(*it)(*state);
		}
	}

	return state;
}

NS_END
