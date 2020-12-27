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

#include "Shibboleth_JanetManager.h"
#include "Shibboleth_JanetHelpers.h"
#include <Shibboleth_LogManager.h>
#include <Shibboleth_JobPool.h>
#include <Gaff_JSON.h>

SHIB_REFLECTION_DEFINE_BEGIN(JanetManager)
	.base<IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(JanetManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(JanetManager)

static ProxyAllocator g_allocator("Janet");

JanetManager::~JanetManager(void)
{
	for (JanetStateData& state : _states) {
		state.lock->Lock();
	}

	for (const auto& type_info : _types) {
		SHIB_FREE(const_cast<char*>(type_info.second.name), g_allocator);
	}

	for (JanetStateData& state : _states) {
		state.state.restore();
		janet_deinit();
	}

	for (JanetStateData& state : _states) {
		state.lock->Unlock();
	}
}

bool JanetManager::initAllModulesLoaded(void)
{
	IApp& app = GetApp();
	app.getLogManager().addChannel(HashStringView32<>("Janet"), "JanetLog");

	const Gaff::JSON script_threads = app.getConfigs()["script_threads"];
	const int32_t num_threads = script_threads.getInt32(k_default_num_threads);

	_states.resize(static_cast<size_t>(num_threads));

	const auto* const ref_defs = app.getReflectionManager().getTypeBucket(CLASS_HASH(*));
	const auto enum_ref_defs = app.getReflectionManager().getEnumReflection();

	for (JanetStateData& state : _states) {
		state.lock.reset(SHIB_ALLOCT(EA::Thread::Futex, g_allocator));

		janet_init();

		JanetTable* const env = janet_core_env(nullptr);
		state.env = env;

		// Add loaded chunks table to the global environment.
		janet_table_put(env, janet_wrap_keyword(k_loaded_chunks_name), janet_wrap_table(janet_table(0)));

		RegisterBuiltIns(env);

		for (const Gaff::IEnumReflectionDefinition* enum_ref_def : enum_ref_defs) {
			RegisterEnum(env, *enum_ref_def);
		}

		if (ref_defs) {
			for (const Gaff::IReflectionDefinition* ref_def : *ref_defs) {
				RegisterType(env, *ref_def, *this);
			}

			_types.shrink_to_fit();

			for (const auto& type_info : _types) {
				RegisterTypeFinalize(env, *type_info.first, type_info.second);
			}
		}


		constexpr const char* k_test_string = R"(
			(Print Gleam/ISamplerState/Wrap/def)
			(Print "Gleam::ISamplerState::Wrap::Repeat - " Gleam/ISamplerState/Wrap/Repeat)
			(Print "Reverse - " (get Gleam/ISamplerState/Wrap/def 1))
			(def TestType (New Gleam/Vec3))
		)";

		janet_dostring(env, k_test_string, nullptr, nullptr);

		state.state.save();
	}

	return true;
}

bool JanetManager::loadBuffer(const char* buffer, size_t size, const char* name)
{
	if (!name) {
		// $TODO: Log error.
		return false;
	}

	for (JanetStateData& state : _states) {
		EA::Thread::AutoFutex lock(*state.lock);
		state.state.restore();

		const Janet loaded_chunks = janet_table_get(state.env, janet_wrap_string(k_loaded_chunks_name));
		const Janet key = janet_wrap_string(name);
		GAFF_ASSERT(janet_checktype(loaded_chunks, JANET_TABLE));

		JanetTable* const loaded_chunks_table = janet_unwrap_table(loaded_chunks);

		// Something is already in this slot.
		if (!janet_checktype(janet_table_get(loaded_chunks_table, key), JANET_NIL)) {
			// $TODO: Log error.
			return false;
		}

		Janet value;
		const int result = janet_dobytes(state.env, reinterpret_cast<const uint8_t*>(buffer), static_cast<int32_t>(size), nullptr, &value);

		if (result) {
			if (janet_checktype(value, JANET_STRING)) {
				// $TODO: Log error.
			}

			return false;
		}

		janet_table_put(loaded_chunks_table, key, value);
		state.state.save();
	}

	return true;
}

void JanetManager::unloadBuffer(const char* name)
{
	if (!name) {
		// $TODO: Log error.
		return;
	}

	for (JanetStateData& state : _states) {
		EA::Thread::AutoFutex lock(*state.lock);
		state.state.restore();

		const Janet loaded_chunks = janet_table_get(state.env, janet_wrap_string(k_loaded_chunks_name));
		GAFF_ASSERT(janet_checktype(loaded_chunks, JANET_TABLE));
	
		janet_table_remove(janet_unwrap_table(loaded_chunks), janet_wrap_string(name));
		state.state.save();
	}
}

JanetState* JanetManager::requestState(void)
{
	JanetState* state = nullptr;

	while (!state) {
		for (JanetStateData& data : _states) {
			if (!data.lock->TryLock()) {
				continue;
			}

			state = &data.state;
			break;
		}

		if (!state) {
			EA::Thread::ThreadSleep();
		}
	}

	return state;
}

void JanetManager::returnState(JanetState* state)
{
	for (JanetStateData& data : _states) {
		if (&data.state == state) {
			data.lock->Unlock();
			break;
		}
	}
}

void JanetManager::registerType(const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info)
{
	if (_types.find(&ref_def) == _types.end()) {
		_types.emplace(&ref_def, type_info);
	}
}

NS_END
