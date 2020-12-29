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


#include "Shibboleth_JanetProcess.h"
#include "Shibboleth_StateMachineReflection.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_JanetManager.h>
#include <lua.hpp>

SHIB_REFLECTION_DEFINE_BEGIN(JanetProcess)
	.BASE(Esprit::IProcess)
	.ctor<>()

	.var("script", &JanetProcess::_script)
SHIB_REFLECTION_DEFINE_END(JanetProcess)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(JanetProcess)

bool JanetProcess::init(const Esprit::StateMachine& owner)
{
	_janet_mgr = &GetApp().getManagerTFast<JanetManager>();
	GetApp().getManagerTFast<ResourceManager>().waitForResource(*_script);

	if (!_script->isLoaded()) {
		return false;
	}

	IJanetManager* const janet_mgr_interface = _janet_mgr;

	JanetState* const state = janet_mgr_interface->requestState();
	JanetTable* const env = state->getEnv();
	state->restore();

	const Janet loaded_chunks = janet_table_get(env, janet_ckeywordv(JanetManager::k_loaded_chunks_name));

	if (!janet_checktype(loaded_chunks, JANET_TABLE)) {
		if (_log_error) {
			// $TODO: Log error once.
			_log_error = false;
		}

		state->save();
		_janet_mgr->returnState(state);
		return false;
	}

	const Janet type_table = janet_table_get(janet_unwrap_table(loaded_chunks), janet_ckeywordv(_script->getFilePath().getBuffer()));

	if (!janet_checktype(type_table, JANET_TABLE)) {
		if (_log_error) {
			// $TODO: Log error once.
			_log_error = false;
		}

		state->save();
		_janet_mgr->returnState(state);
		return false;
	}

	JanetTable* const type_table_unwrap = janet_unwrap_table(type_table);
	const Janet init_func = janet_table_get(type_table_unwrap, janet_ckeywordv("init"));

	if (janet_checktype(type_table, JANET_NIL)) {
		SaveTable(*type_table_unwrap, _table_state);

		state->save();
		_janet_mgr->returnState(state);
		return true;
	}

	if (!janet_checktype(init_func, JANET_FUNCTION)) {
		if (_log_error) {
			// $TODO: Log error once.
			_log_error = false;
		}

		state->save();
		_janet_mgr->returnState(state);
		return false;
	}

	bool success = false;
	Janet result;

	const Janet args[] = {
		type_table,
		PushUserTypeReference(owner, *_janet_mgr)
	};

	if (janet_pcall(janet_unwrap_function(init_func), 2, args, &result, nullptr) != JANET_SIGNAL_OK) {
		if (_log_error) {
			_log_error = false;

			const char* const error = reinterpret_cast<const char*>(janet_unwrap_string(result));
			int i = 0;
			i += 5;

			// $TODO: Log error.
			GAFF_REF(error);
		}

	} else {
		success = !janet_checktype(result, JANET_BOOLEAN) || janet_unwrap_boolean(result);
	}

	SaveTable(*type_table_unwrap, _table_state);
	state->save();
	_janet_mgr->returnState(state);

	return success;
}

void JanetProcess::update(const Esprit::StateMachine& owner, Esprit::VariableSet::Instance& variables)
{
	if (!_script->isLoaded()) {
		return;
	}

	IJanetManager* const janet_mgr_interface = _janet_mgr;
	JanetState* const state = janet_mgr_interface->requestState();
	JanetTable* const env = state->getEnv();
	state->restore();

	const Janet loaded_chunks = janet_table_get(env, janet_ckeywordv(JanetManager::k_loaded_chunks_name));

	if (!janet_checktype(loaded_chunks, JANET_TABLE)) {
		if (_log_error) {
			// $TODO: Log error once.
			_log_error = false;
		}

		_janet_mgr->returnState(state);
		return;
	}

	const Janet type_table = janet_table_get(janet_unwrap_table(loaded_chunks), janet_ckeywordv(_script->getFilePath().getBuffer()));

	if (!janet_checktype(type_table, JANET_TABLE)) {
		if (_log_error) {
			// $TODO: Log error once.
			_log_error = false;
		}

		_janet_mgr->returnState(state);
		return;
	}

	JanetTable* const type_table_unwrap = janet_unwrap_table(type_table);
	const Janet update_func = janet_table_get(type_table_unwrap, janet_ckeywordv("update"));

	if (!janet_checktype(update_func, JANET_FUNCTION)) {
		if (_log_error) {
			// $TODO: Log error once.
			_log_error = false;
		}

		_janet_mgr->returnState(state);
		return;
	}

	RestoreTable(*type_table_unwrap, _table_state);

	// Call the function.
	const Janet args[] = {
		type_table,
		PushUserTypeReference(owner, *_janet_mgr),
		PushUserTypeReference(variables, *_janet_mgr)
	};

	Janet result;

	if (janet_pcall(janet_unwrap_function(update_func), 2, args, &result, nullptr) != JANET_SIGNAL_OK) {
		if (_log_error) {
			// $TODO: Log error once.
			_log_error = false;

			// Error string is in result.
		}
	}

	SaveTable(*type_table_unwrap, _table_state);
	_janet_mgr->returnState(state);
}

NS_END
