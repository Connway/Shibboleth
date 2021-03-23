/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Shibboleth_InputManager.h"
#include "Shibboleth_InputReflection.h"
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_GameTime.h>
#include <Gleam_Window.h>
#include <Gaff_JSON.h>
#include <Gaff_Math.h>
#include <EASTL/sort.h>

SHIB_REFLECTION_DEFINE_BEGIN(InputManager)
	.base<IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(InputManager)


SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE(InputSystem, ISystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(InputManager)
SHIB_REFLECTION_CLASS_DEFINE(InputSystem)

constexpr char* const g_binding_cfg_schema =
R"({
	"type": "array",
	"items":
	{
		"type": "object",
		"properties":
		{
			"Alias": { "type": "string" },
			"Binding": { "type": ["array", "string"], "items": { "type": "string" } },
			"Scale": { "type": "number", "default": 1.0 },
			"Taps": { "type": "integer", "minimum": 1 },
			"Tap Interval": { "type": "number" },
			"Modes": { "type": ["array", "string"], "items": { "type": "string" } },

			"required": ["Alias", "Binding"]
		}
	}
})";



bool InputManager::initAllModulesLoaded(void)
{
	const IRenderManager& render_mgr = GetApp().GETMANAGERT(IRenderManager, RenderManager);

	_keyboard.reset(render_mgr.createKeyboard());
	_mouse.reset(render_mgr.createMouse());

	if (!_keyboard || !_keyboard->init()) {
		// $TODO: Log error.
		return false;
	}

	if (!_mouse || !_mouse->init()) {
		// $TODO: Log error.
		return false;
	}

	_keyboard->addInputHandler(Gaff::MemberFunc(this, &InputManager::handleKeyboardInput));
	_mouse->addInputHandler(Gaff::MemberFunc(this, &InputManager::handleMouseInput));

	// Load bindings.
	IFile* const input_file = GetApp().getFileSystem().openFile("cfg/input_bindings.cfg");

	if (!input_file) {
		LogErrorDefault("InputManager: Failed to open file 'cfg/input_bindings.cfg'");
		return false;
	}

	Gaff::JSON input_bindings;

	if (!input_bindings.parse(reinterpret_cast<char*>(input_file->getBuffer()), g_binding_cfg_schema)) {
		LogErrorDefault("InputManager: Failed to parse 'cfg/input_bindings.cfg' with error '%s'", input_bindings.getErrorText());
		return false;
	}


	VectorMap<Gaff::Hash32, Alias> alias_values(ProxyAllocator("Input"));

	// Aliases
	input_bindings.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		GAFF_ASSERT(value.isObject());

		const Gaff::JSON alias = value["Alias"];
		GAFF_ASSERT(alias.isString());

		const char* const alias_string = alias.getString();
		alias_values[Gaff::FNV1aHash32String(alias_string)] = Alias{};
		alias.freeString(alias_string);

		return false;
	});

	alias_values.shrink_to_fit();


	// Bindings
	input_bindings.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		GAFF_ASSERT(value.isObject());

		const Gaff::JSON tap_interval = value["Tap Interval"];
		const Gaff::JSON binding = value["Binding"];
		const Gaff::JSON modes = value["Modes"];
		const Gaff::JSON scale = value["Scale"];
		const Gaff::JSON alias = value["Alias"];
		const Gaff::JSON taps = value["Taps"];

		GAFF_ASSERT(tap_interval.isFloat() || tap_interval.isNull());
		GAFF_ASSERT(binding.isString() || binding.isArray());
		GAFF_ASSERT(modes.isArray() || modes.isString() || modes.isNull());
		GAFF_ASSERT(scale.isFloat() || scale.isNull());
		GAFF_ASSERT(taps.isInt8() || taps.isNull());
		GAFF_ASSERT(alias.isString());

		const auto& mouse_ref_def = Reflection<Gleam::MouseCode>::GetReflectionDefinition();
		const auto& key_ref_def = Reflection<Gleam::KeyCode>::GetReflectionDefinition();

		const char* const alias_str = alias.getString();
		const float scale_value = scale.getFloat(1.0f);

		const Gaff::Hash32 alias_hash = Gaff::FNV1aHash32String(alias_str);
		const auto alias_it = alias_values.find(alias_hash);

		GAFF_ASSERT(alias_values.find(alias_hash) != alias_values.end());

		const float tap_interval_value = tap_interval.getFloat(0.1f);
		const int8_t taps_value = taps.getInt8(0);

		Binding final_binding;
		final_binding.alias_index = static_cast<int32_t>(eastl::distance(alias_values.begin(), alias_it));
		final_binding.scale = scale_value;
		final_binding.tap_interval = tap_interval_value;
		final_binding.taps = taps_value;

		alias.freeString(alias_str);

		if (modes.isArray()) {
			const bool failed = modes.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
			{
				GAFF_ASSERT(value.isString());
				const char* const mode_string = value.getString();

				final_binding.modes.emplace_back(Gaff::FNV1aHash32String(mode_string));

				value.freeString(mode_string);
				return false;
			});

		} else {
			char mode_name[256] = { 0 };
			const char* const mode_string = modes.getString(mode_name, 256, "Default");

			final_binding.modes.emplace_back(Gaff::FNV1aHash32String(mode_string));
		}

		if (binding.isArray()) {
			const bool failed = binding.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
			{
				GAFF_ASSERT(value.isString());
				const char* const binding_str = value.getString();

				if (const int32_t value_kb = key_ref_def.getEntryValue(binding_str); value_kb != std::numeric_limits<int32_t>::min()) {
					final_binding.key_codes.emplace_back(static_cast<Gleam::KeyCode>(value_kb));

				} else if (const int32_t value_mouse = mouse_ref_def.getEntryValue(binding_str); value_mouse != std::numeric_limits<int32_t>::min()) {
					final_binding.mouse_codes.emplace_back(static_cast<Gleam::MouseCode>(value_mouse));

				} else {
					LogErrorDefault("InputManager: Invalid input binding '%s'.", binding_str);
				}

				value.freeString(binding_str);
				return false;
			});

			// Skip adding the binding.
			if (failed) {
				return false;
			}

		} else {
			const char* const binding_str = binding.getString();

			if (const int32_t value_kb = key_ref_def.getEntryValue(binding_str); value_kb != std::numeric_limits<int32_t>::min()) {
				final_binding.key_codes.resize(1);
				final_binding.key_codes[0] = static_cast<Gleam::KeyCode>(value_kb);

			} else if (const int32_t value_mouse = mouse_ref_def.getEntryValue(binding_str); value_mouse != std::numeric_limits<int32_t>::min()) {
				final_binding.mouse_codes.resize(1);
				final_binding.mouse_codes[0] = static_cast<Gleam::MouseCode>(value_mouse);

			} else {
				LogErrorDefault("InputManager: Invalid input binding '%s'.", binding_str);
			}

			binding.freeString(binding_str);
		}

		final_binding.mouse_codes.shrink_to_fit();
		final_binding.key_codes.shrink_to_fit();
		final_binding.modes.shrink_to_fit();

		// Probably unnecessary.
		eastl::sort(final_binding.mouse_codes.begin(), final_binding.mouse_codes.end());
		eastl::sort(final_binding.key_codes.begin(), final_binding.key_codes.end());
		eastl::sort(final_binding.modes.begin(), final_binding.modes.end());

		_bindings.emplace_back(final_binding);

		return false;
	});

	_bindings.shrink_to_fit();

	// Always have a player 0.
	_binding_instances.emplace();
	_alias_values.emplace();

	for (auto& binding_instance : _binding_instances) {
		binding_instance.resize(_bindings.size());
	}

	for (auto& av : _alias_values) {
		av = alias_values;
	}

	return true;
}

void InputManager::update(void)
{
	_keyboard->update();
	_mouse->update();

	using DoubleSeconds = eastl::chrono::duration<double>;
	_end = eastl::chrono::high_resolution_clock::now();

	DoubleSeconds delta = _end - _start;
	_start = _end;

	// Delta time is real-time.
	const float dt = static_cast<float>(delta.count());
	//const int32_t num_inputs = static_cast<int32_t>(_binding_instances.size());
	const int32_t num_inputs = k_max_local_players;
	const int32_t num_bindings = static_cast<int32_t>(_bindings.size());

	for (int32_t input_index = 0; input_index < num_inputs; ++input_index) {
		auto& input_instance = _binding_instances[input_index];

		for (int32_t binding_index = 0; binding_index < num_bindings; ++binding_index) {
			auto& binding_instance = input_instance[binding_index];
			const auto& binding = _bindings[binding_index];

			if (Gaff::Find(binding.modes, _curr_mode) == binding.modes.end()) {
				_alias_values[input_index].at(binding.alias_index).second.value = 0.0f;

				binding_instance.curr_tap_time = 0.0f;
				binding_instance.curr_tap = 0;
				binding_instance.first_frame = false;

				continue;
			}

			if (binding.taps > 0) {
				binding_instance.curr_tap_time += dt;

				// We have been set for at least one frame or we have exceeded the tap time. Reset.
				if (!binding_instance.first_frame && binding_instance.curr_tap == binding.taps) {
					_alias_values[input_index].at(binding.alias_index).second.value = 0.0f;
					binding_instance.curr_tap_time = 0.0f;
					binding_instance.curr_tap = 0;

				} else if (binding_instance.curr_tap_time > binding.tap_interval) {
					binding_instance.curr_tap_time = 0.0f;
					binding_instance.curr_tap = 0;
				}

				binding_instance.first_frame = false;
			}
		}
	}
}

void InputManager::resetTimer(void)
{
	_start = _end = eastl::chrono::high_resolution_clock::now();
}

float InputManager::getAliasValue(Gaff::Hash32 alias_name, int32_t player_id) const
{
	GAFF_ASSERT(Gaff::Between(player_id, 0, k_max_local_players));
	const auto it = _alias_values[player_id].find(alias_name);
	return (it == _alias_values[player_id].end()) ? 0.0f : it->second.value;
}

float InputManager::getAliasValue(const char* alias_name, int32_t player_id) const
{
	return getAliasValue(Gaff::FNV1aHash32String(alias_name), player_id);
}

float InputManager::getAliasValue(int32_t index, int32_t player_id) const
{
	GAFF_ASSERT(Gaff::Between(player_id, 0, k_max_local_players));
	GAFF_ASSERT(index < static_cast<int32_t>(_alias_values[player_id].size()));
	return _alias_values[player_id].at(index).second.value;
}

int32_t InputManager::getAliasIndex(Gaff::Hash32 alias_name) const
{
	const auto it = _alias_values[0].find(alias_name);
	return (it != _alias_values[0].end()) ? static_cast<int32_t>(eastl::distance(_alias_values[0].begin(), it)) : -1;
}

int32_t InputManager::getAliasIndex(const char* alias_name) const
{
	return getAliasIndex(Gaff::FNV1aHash32String(alias_name));
}

void InputManager::setKeyboardMousePlayerID(int32_t player_id)
{
	GAFF_ASSERT(Gaff::Between(player_id, 0, k_max_local_players));
	_km_player_id = player_id;
}

int32_t InputManager::getKeyboardMousePlayerID(void) const
{
	return _km_player_id;
}

Gaff::Hash32 InputManager::getPreviousMode(void) const
{
	return _prev_mode;
}

Gaff::Hash32 InputManager::getCurrentMode(void) const
{
	return _curr_mode;
}

void InputManager::setMode(Gaff::Hash32 mode)
{
	_prev_mode = _curr_mode;
	_curr_mode = mode;
}

void InputManager::setMode(const char* mode)
{
	setMode(Gaff::FNV1aHash32String(mode));
}

void InputManager::setModeToPrevious(void)
{
	setMode(_prev_mode);
}

void InputManager::setModeToDefault(void)
{
	setMode(Gaff::FNV1aHash32Const("Default"));
}

Gleam::IKeyboard* InputManager::getKeyboard(void)
{
	return _keyboard.get();
}

Gleam::IMouse* InputManager::getMouse(void)
{
	return _mouse.get();
}

void InputManager::handleKeyboardInput(Gleam::IInputDevice*, int32_t key_code, float value)
{
	GAFF_ASSERT(Gaff::Between(_km_player_id, 0, k_max_local_players /*static_cast<int32_t>(_binding_instances.size())*/));
	const Gleam::KeyCode code = static_cast<Gleam::KeyCode>(key_code);
	const int32_t num_bindings = static_cast<int32_t>(_bindings.size());

	auto& input_instance = _binding_instances[_km_player_id];
	auto& alias_values = _alias_values[_km_player_id];

	for (int32_t binding_index = 0; binding_index < num_bindings; ++binding_index) {
		auto& binding_instance = input_instance[binding_index];
		const auto& binding = _bindings[binding_index];

		if (Gaff::Find(binding.modes, _curr_mode) == binding.modes.end()) {
			continue;
		}

		const int8_t binding_size = static_cast<int8_t>(binding.key_codes.size() + binding.mouse_codes.size());
		const auto it = Gaff::Find(binding.key_codes, code);

		if (it != binding.key_codes.end()) {
			if (value > 0.0f) {
				++binding_instance.count;

				// All the bindings have been pressed.
				if (binding_instance.count == binding_size) {
					auto& alias = alias_values.at(binding.alias_index).second;

					if (binding.taps <= 0) {
						alias.value += binding.scale;
					}
				}

			} else {
				// All the bindings have been released.
				if (binding_instance.count == binding_size) {
					auto& alias = alias_values.at(binding.alias_index).second;

					// Check that we've reached the tap count.
					if (binding.taps > 0) {
						binding_instance.curr_tap_time = 0.0f;
						++binding_instance.curr_tap;

						// We've reached the tap count, set the alias.
						if (binding_instance.curr_tap == binding.taps) {
							alias.value += binding.scale;
							binding_instance.first_frame = true;
						}

						// We have no tap requirements.
					} else {
						alias.value -= binding.scale;
					}
				}

				--binding_instance.count;
			}
		}
	}
}

void InputManager::handleMouseInput(Gleam::IInputDevice*, int32_t mouse_code, float value)
{
	GAFF_ASSERT(Gaff::Between(_km_player_id, 0, k_max_local_players /*static_cast<int32_t>(_binding_instances.size())*/));

	const bool is_button = mouse_code < static_cast<int32_t>(Gleam::MouseCode::ButtonCount);
	const Gleam::MouseCode code = static_cast<Gleam::MouseCode>(mouse_code);
	const int32_t num_bindings = static_cast<int32_t>(_bindings.size());

	auto& input_instance = _binding_instances[_km_player_id];
	auto& alias_values = _alias_values[_km_player_id];

	for (int32_t binding_index = 0; binding_index < num_bindings; ++binding_index) {
		auto& binding_instance = input_instance[binding_index];
		const auto& binding = _bindings[binding_index];

		if (Gaff::Find(binding.modes, _curr_mode) == binding.modes.end()) {
			continue;
		}

		const int8_t binding_size = static_cast<int8_t>(binding.key_codes.size() + binding.mouse_codes.size());
		const auto it = Gaff::Find(binding.mouse_codes, code);

		if (it != binding.mouse_codes.end()) {
			if (binding_size == 1 && !is_button) {
				alias_values.at(binding.alias_index).second.value = binding.scale * value;

			} else {
				if (value != 0.0f) {
					++binding_instance.count;

					// All the bindings have been pressed.
					if (binding_instance.count == binding_size) {
						auto& alias = alias_values.at(binding.alias_index).second;

						if (binding.taps <= 0) {
							alias.value += binding.scale;
						}
					}

				} else {
					// All the bindings have been released.
					if (binding_instance.count == binding_size) {
						auto& alias = alias_values.at(binding.alias_index).second;

						// Check that we've reached the tap count.
						if (binding.taps > 0) {
							binding_instance.curr_tap_time = 0.0f;
							++binding_instance.curr_tap;

							// We've reached the tap count, set the alias.
							if (binding_instance.curr_tap == binding.taps) {
								alias.value += binding.scale;
								binding_instance.first_frame = true;
							}

						// We have no tap requirements.
						} else {
							alias.value -= binding.scale;
						}
					}

					--binding_instance.count;
				}
			}
		}
	}
}


bool InputSystem::init(void)
{
	_input_mgr = &GetApp().getManagerTFast<InputManager>();
	_input_mgr->resetTimer();
	return true;
}

void InputSystem::update(uintptr_t /*thread_id_int*/)
{
	_input_mgr->update();
}

NS_END
