/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
#include "Shibboleth_StateMachineReflection.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ScriptLogging.h>
#include <Shibboleth_ScriptConfigs.h>
#include <Shibboleth_LuaManager.h>
#include <Shibboleth_LuaHelpers.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <lua.hpp>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::LuaProcess)
	.BASE(Esprit::IProcess)
	.ctor<>()

	.var("script", &Shibboleth::LuaProcess::_script)
SHIB_REFLECTION_DEFINE_END(Shibboleth::LuaProcess)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(LuaProcess)

bool LuaProcess::init(const Esprit::StateMachine& owner)
{
	_lua_mgr = &GetApp().getManagerTFast<LuaManager>();
	GetApp().getManagerTFast<ResourceManager>().waitForResource(*_script);

	if (!_script->isLoaded()) {
		return false;
	}

	lua_State* const state = _lua_mgr->requestState();

	lua_getglobal(state, k_config_script_loaded_chunks_name);

	// Should never happen, but check anyways.
	if (!lua_istable(state, -1)) {
		if (_log_error) {
			_log_error = false;

			size_t len = 0;
			const char* const error = lua_tolstring(state, -1, &len);

			LogErrorScript("%s", error);
		}

		lua_pop(state, 2);
		_lua_mgr->returnState(state);
		return false;
	}

	lua_getfield(state, -1, reinterpret_cast<const char*>(_script->getFilePath().getBuffer()));

	// Should never happen, but check anyways.
	if (!lua_istable(state, -1)) {
		if (_log_error) {
			_log_error = false;

			size_t len = 0;
			const char* const error = lua_tolstring(state, -1, &len);

			LogErrorScript("%s", error);
		}

		lua_pop(state, 2);
		_lua_mgr->returnState(state);
		return false;
	}

	lua_getfield(state, -1, "init");

	if (lua_isnoneornil(state, -1)) {
		SaveTable(state, _table_state);

		lua_pop(state, 3);
		_lua_mgr->returnState(state);
		return true;
	}

	if (!lua_isfunction(state, -1)) {
		if (_log_error) {
			_log_error = false;

			size_t len = 0;
			const char* const error = lua_tolstring(state, -1, &len);

			LogErrorScript("%s", error);
		}

		lua_pop(state, 3);
		_lua_mgr->returnState(state);
		return false;
	}

	lua_pushvalue(state, -2);
	PushUserTypeReference(state, owner);
	const int32_t err = lua_pcall(state, 2, 1, 0);
	bool success = false;

	if (err != LUA_OK) {
		size_t len = 0;
		const char* const error = lua_tolstring(state, -1, &len);

		LogErrorScript("%s", error);

	} else {
		success = lua_toboolean(state, -1);
	}

	lua_pop(state, 1); // bool/err_string, table, chunk_table

	SaveTable(state, _table_state);

	lua_pop(state, 2); // table, chunk_table
	_lua_mgr->returnState(state);

	return success;
}

void LuaProcess::update(const Esprit::StateMachine& owner, Esprit::VariableSet::Instance& variables)
{
	if (!_script->isLoaded()) {
		return;
	}

	lua_State* const state = _lua_mgr->requestState();

	lua_getglobal(state, k_config_script_loaded_chunks_name);
	lua_getfield(state, -1, reinterpret_cast<const char*>(_script->getFilePath().getBuffer()));

	// Should never happen, but check anyways.
	if (!lua_istable(state, -1)) {
		if (_log_error) {
			_log_error = false;

			size_t len = 0;
			const char* const error = lua_tolstring(state, -1, &len);

			LogErrorScript("%s", error);
		}

		lua_pop(state, 2);
		_lua_mgr->returnState(state);
		return;
	}

	lua_getfield(state, -1, "update"); // top -> bottom: function, table, chunk_table

	if (!lua_isfunction(state, -1)) {
		if (_log_error) {
			_log_error = false;

			size_t len = 0;
			const char* const error = lua_tolstring(state, -1, &len);

			LogErrorScript("%s", error);
		}

		lua_pop(state, 3);
		_lua_mgr->returnState(state);
		return;
	}

	// Restore state of table.
	lua_pushvalue(state, -2); // Push table to the top of the stack.
	RestoreTable(state, _table_state);

	// Call the function.
	PushUserTypeReference(state, owner);
	PushUserTypeReference(state, variables);
	const int32_t err = lua_pcall(state, 3, 0, 0);

	if (err != LUA_OK) {
		if (_log_error) {
			_log_error = false;

			size_t len = 0;
			const char* const error = lua_tolstring(state, -1, &len);

			LogErrorScript("%s", error);
		}

		lua_pop(state, 1);
	}

	// Save the state of the table.
	SaveTable(state, _table_state);

	lua_pop(state, 2);
	_lua_mgr->returnState(state);
}

NS_END
