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

struct lua_State;

NS_SHIBBOLETH

class LuaManager final : public IManager
{
public:
	static constexpr const char* const k_loaded_chunks_name = "__loaded_chunks";

	static constexpr const char* k_thread_pool_name = "Lua";
	static constexpr Gaff::Hash32 k_thread_pool = Gaff::FNV1aHash32Const(k_thread_pool_name);
	static constexpr int32_t k_default_num_threads = 4;

	~LuaManager(void);

	bool initAllModulesLoaded(void) override;

	bool loadBuffer(const char* buffer, size_t size, const char* name);

	lua_State* requestState(void);
	void returnState(lua_State* state);

private:
	struct LuaStateData final
	{
		UniquePtr<EA::Thread::Futex> lock;
		lua_State* state = nullptr;
	};

	Vector<LuaStateData> _states{ ProxyAllocator("Lua") };

	static void* alloc(void*, void* ptr, size_t, size_t new_size);
	static int panic(lua_State* L);

	SHIB_REFLECTION_CLASS_DECLARE(LuaManager);
};

NS_END

SHIB_REFLECTION_DECLARE(LuaManager)
