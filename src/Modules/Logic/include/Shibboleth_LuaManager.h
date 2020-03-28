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

#define SOL_NO_EXCEPTIONS 1
#define SOL_PRINT_ERRORS 0
#include <sol/sol.hpp>

NS_SHIBBOLETH

class LuaManager final : public IManager
{
public:
	bool initAllModulesLoaded(void) override;

private:
	sol::state _state = sol::state{ panic, alloc, nullptr };

	static void* alloc(void*, void* ptr, size_t, size_t new_size);
	static int panic(lua_State* L);

	SHIB_REFLECTION_CLASS_DECLARE(LuaManager);
};

NS_END

SHIB_REFLECTION_DECLARE(LuaManager)
