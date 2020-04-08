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


#include "Shibboleth_LuaProcess.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_LuaManager.h>
#include <lua.hpp>

SHIB_REFLECTION_DEFINE_BEGIN(LuaProcess)
	.BASE(Esprit::IProcess)
	.ctor<>()

	.var("script", &LuaProcess::_script)
SHIB_REFLECTION_DEFINE_END(LuaProcess)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(LuaProcess);

bool LuaProcess::init(const Esprit::StateMachine& owner)
{
	_lua_mgr = &GetApp().getManagerTFast<LuaManager>();

	GAFF_REF(owner);
	return true;
}

void LuaProcess::update(const Esprit::StateMachine& owner, Esprit::VariableSet::Instance& variables)
{
	GAFF_REF(owner, variables);

	if (!_script->isLoaded()) {
		return;
	}

	lua_State* const state = _lua_mgr->requestState();

	lua_getglobal(state, LuaManager::k_loaded_chunks_name);
	lua_getfield(state, -1, _script->getFilePath().getBuffer());

	// Should never happen, but check anyways.
	if (!lua_istable(state, -1)) {
		if (_log_error) {
			// $TODO: Log error once.
			_log_error = false;
		}

		lua_pop(state, 2);
		_lua_mgr->returnState(state);
		return;
	}

	lua_getfield(state, -1, "update");

	if (!lua_isfunction(state, -1)) {
		if (_log_error) {
			// $TODO: Log error once.
			_log_error = false;
		}

		lua_pop(state, 3);
		_lua_mgr->returnState(state);
		return;
	}

	// Restore state of table.

	if (lua_isfunction(state, -1)) {
		// Push Args
		lua_pcall(state, 2, 0, 0);
	}

	lua_pop(state, 3);
	_lua_mgr->returnState(state);
}

NS_END
