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

#if defined (_WIN32) || defined(_WIN64)
	#pragma warning(push)
	#pragma warning(disable: 4100 4244 4267 4800)
#endif

#include <LuaState.h>

#if defined (_WIN32) || defined(_WIN64)
	#pragma warning(pop)
#endif

NS_SHIBBOLETH

REF_IMPL_REQ(LuaManager);
SHIB_REF_IMPL(LuaManager)
.addBaseClassInterfaceOnly<LuaManager>()
;

LuaManager::LuaManager(void)
{
}

LuaManager::~LuaManager(void)
{
}

const char* LuaManager::getName(void) const
{
	return "Lua Manager";
}

void LuaManager::addRegistrant(const Gaff::FunctionBinder<void, lua::State&>& registrant)
{
	_registrants.push(registrant);
}

lua::State* LuaManager::createNewState(void)
{
	lua::State* state = GetAllocator()->template allocT<lua::State>();

	if (state) {
		for (auto it = _registrants.begin(); it != _registrants.end(); ++it) {
			(*it)(*state);
		}
	}

	return state;
}

NS_END
