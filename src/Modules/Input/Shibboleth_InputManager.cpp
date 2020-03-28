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

#include "Shibboleth_InputManager.h"
#include "Shibboleth_InputReflection.h"
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_GameTime.h>
#include <Gleam_Window.h>
#include <Gaff_JSON.h>
#include <EASTL/sort.h>

SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE_NO_INHERITANCE(InputManager, IManager)
SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE_NO_INHERITANCE(InputSystem, ISystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(InputManager)
SHIB_REFLECTION_CLASS_DEFINE(InputSystem)

constexpr char* const g_alias_cfg_schema =
R"({
	"type": "array",
	"items":
	{
		"type": "string"
	}
})";

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

	// Load aliases.
	IFile* const alias_file = GetApp().getFileSystem().openFile("cfg/input_aliases.cfg");

	if (!alias_file) {
		LogErrorDefault("InputManager: Failed to open file 'cfg/input_aliases.cfg'");
		return false;
	}

	Gaff::JSON aliases;

	if (!aliases.parse(reinterpret_cast<char*>(alias_file->getBuffer()), g_alias_cfg_schema)) {
		LogErrorDefault("InputManager: Failed to parse 'cfg/input_aliases.cfg' with error '%s'", aliases.getErrorText());
		return false;
	}

	VectorMap<Gaff::Hash32, Alias> alias_values(ProxyAllocator("Input"));

	aliases.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		GAFF_ASSERT(value.isString() || value.isObject());

		const char* const name_value = value.getString();
		alias_values[Gaff::FNV1aHash32String(name_value)] = Alias{};
		value.freeString(name_value);

		return false;
	});

	alias_values.shrink_to_fit();

	for (auto& av : _alias_values) {
		av = alias_values;
	}

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

	_keyboard->addInputHandler(Gaff::MemberFunc(this, &InputManager::handleKeyboardInput));
	_mouse->addInputHandler(Gaff::MemberFunc(this, &InputManager::handleMouseInput));

	Vector<Binding> bindings(ProxyAllocator("Input"));

	input_bindings.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		GAFF_ASSERT(value.isObject());

		const Gaff::JSON tap_interval = value["Tap Interval"];
		const Gaff::JSON binding = value["Binding"];
		const Gaff::JSON scale = value["Scale"];
		const Gaff::JSON alias = value["Alias"];
		const Gaff::JSON taps = value["Taps"];

		GAFF_ASSERT(tap_interval.isFloat() || tap_interval.isNull());
		GAFF_ASSERT(binding.isString() || binding.isArray());
		GAFF_ASSERT(scale.isFloat() || scale.isNull());
		GAFF_ASSERT(taps.isInt8() || taps.isNull());
		GAFF_ASSERT(alias.isString());

		const auto& mouse_ref_def = Reflection<Gleam::MouseCode>::GetReflectionDefinition();
		const auto& key_ref_def = Reflection<Gleam::KeyCode>::GetReflectionDefinition();

		const char* const alias_str = alias.getString();
		const float scale_value = scale.getFloat(1.0f);

		const Gaff::Hash32 alias_hash = Gaff::FNV1aHash32String(alias_str);
		const auto alias_it = alias_values.find(alias_hash);

		if (alias_values.find(alias_hash) == alias_values.end()) {
			LogErrorDefault("InputManager: Invalid input alias '%s'.", alias_str);
			alias.freeString(alias_str);

			return false;
		}

		const float tap_interval_value = tap_interval.getFloat(0.1f);
		const int8_t taps_value = taps.getInt8(0);

		Binding final_binding;
		final_binding.alias_index = static_cast<int32_t>(eastl::distance(alias_values.begin(), alias_it));
		final_binding.scale = scale_value;
		final_binding.tap_interval = tap_interval_value;
		final_binding.taps = taps_value;

		alias.freeString(alias_str);

		if (binding.isArray()) {
			const bool failed = binding.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
			{
				const char* const binding_str = value.getString();

				if (const int32_t value_kb = key_ref_def.getEntryValue(binding_str); value_kb != std::numeric_limits<int32_t>::min()) {
					final_binding.key_codes.emplace_back(static_cast<Gleam::KeyCode>(value_kb));

				} else if (const int32_t value_mouse = mouse_ref_def.getEntryValue(binding_str); value_mouse != std::numeric_limits<int32_t>::min()) {
					final_binding.mouse_codes.emplace_back(static_cast<Gleam::MouseCode>(value_mouse));

				} else {
					LogErrorDefault("InputManager: Invalid input binding '%s'.", binding_str);
					return true;
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

		eastl::sort(final_binding.mouse_codes.begin(), final_binding.mouse_codes.end());
		eastl::sort(final_binding.key_codes.begin(), final_binding.key_codes.end());

		bindings.emplace_back(final_binding);

		return false;
	});

	for (auto& b : _bindings) {
		b = bindings;
	}

	return true;
}

void InputManager::update()
{
	_keyboard->update();
	_mouse->update();

	const float dt = static_cast<float>(GetApp().getManagerTFast<GameTimeManager>().getRealTime().delta);

	for (int32_t i = 0; i < g_max_local_players; ++i) {
		for (auto& binding : _bindings[i]) {
			if (binding.taps > 0) {
				binding.curr_tap_time += dt;

				// We have been set for at least one frame or we have exceeded the tap time. Reset.
				if (!binding.first_frame && binding.curr_tap == binding.taps) {
					_alias_values[i].at(binding.alias_index).second.value = 0.0f;
					binding.curr_tap_time = 0.0f;
					binding.curr_tap = 0;

				} else if (binding.curr_tap_time > binding.tap_interval) {
					binding.curr_tap_time = 0.0f;
					binding.curr_tap = 0;
				}

				binding.first_frame = false;
			}
		}
	}
}

float InputManager::getAliasValue(Gaff::Hash32 alias_name, int32_t player_id) const
{
	GAFF_ASSERT(Gaff::Between(player_id, 0, g_max_local_players));
	const auto it = _alias_values[player_id].find(alias_name);
	return (it == _alias_values[player_id].end()) ? 0.0f : it->second.value;
}

float InputManager::getAliasValue(const char* alias_name, int32_t player_id) const
{
	return getAliasValue(Gaff::FNV1aHash32String(alias_name), player_id);
}

float InputManager::getAliasValue(int32_t index, int32_t player_id) const
{
	GAFF_ASSERT(Gaff::Between(player_id, 0, g_max_local_players));
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
	GAFF_ASSERT(Gaff::Between(player_id, 0, g_max_local_players));
	_km_player_id = player_id;
}

int32_t InputManager::getKeyboardMousePlayerID(void) const
{
	return _km_player_id;
}

void InputManager::handleKeyboardInput(Gleam::IInputDevice*, int32_t key_code, float value)
{
	GAFF_ASSERT(Gaff::Between(_km_player_id, 0, g_max_local_players));
	const Gleam::KeyCode code = static_cast<Gleam::KeyCode>(key_code);

	for (Binding& binding : _bindings[_km_player_id]) {
		const int8_t binding_size = static_cast<int8_t>(binding.key_codes.size() + binding.mouse_codes.size());
		const auto it = Gaff::Find(binding.key_codes, code);

		if (it != binding.key_codes.end()) {
			if (value > 0.0f) {
				++binding.count;

				// All the bindings have been pressed.
				if (binding.count == binding_size) {
					auto& alias = _alias_values[_km_player_id].at(binding.alias_index).second;

					if (binding.taps <= 0) {
						alias.value += binding.scale;
					}
				}

			} else {
				// All the bindings have been released.
				if (binding.count == binding_size) {
					auto& alias = _alias_values[_km_player_id].at(binding.alias_index).second;

					// Check that we've reached the tap count.
					if (binding.taps > 0) {
						binding.curr_tap_time = 0.0f;
						++binding.curr_tap;

						// We've reached the tap count, set the alias.
						if (binding.curr_tap == binding.taps) {
							alias.value += binding.scale;
							binding.first_frame = true;
						}

					// We have no tap requirements.
					} else {
						alias.value -= binding.scale;
					}
				}

				--binding.count;
			}
		}
	}
}

void InputManager::handleMouseInput(Gleam::IInputDevice*, int32_t mouse_code, float value)
{
	const bool is_button = mouse_code < static_cast<int32_t>(Gleam::MouseCode::MOUSE_BUTTON_COUNT);
	const Gleam::MouseCode code = static_cast<Gleam::MouseCode>(mouse_code);

	for (Binding& binding : _bindings[_km_player_id]) {
		const int8_t binding_size = static_cast<int8_t>(binding.key_codes.size() + binding.mouse_codes.size());
		const auto it = Gaff::Find(binding.mouse_codes, code);

		if (it != binding.mouse_codes.end()) {
			if (binding_size == 1 && !is_button) {
				_alias_values[_km_player_id].at(binding.alias_index).second.value = binding.scale * value;

			} else {
				if (value != 0.0f) {
					++binding.count;

					// All the bindings have been pressed.
					if (binding.count == binding_size) {
						auto& alias = _alias_values[_km_player_id].at(binding.alias_index).second;

						if (binding.taps <= 0) {
							alias.value += binding.scale;
						}
					}

				} else {
					// All the bindings have been released.
					if (binding.count == binding_size) {
						auto& alias = _alias_values[_km_player_id].at(binding.alias_index).second;

						// Check that we've reached the tap count.
						if (binding.taps > 0) {
							binding.curr_tap_time = 0.0f;
							++binding.curr_tap;

							// We've reached the tap count, set the alias.
							if (binding.curr_tap == binding.taps) {
								alias.value += binding.scale;
								binding.first_frame = true;
							}

						// We have no tap requirements.
						} else {
							alias.value -= binding.scale;
						}
					}

					--binding.count;
				}
			}
		}
	}
}


bool InputSystem::init(void)
{
	_input_mgr = &GetApp().getManagerTFast<InputManager>();
	return true;
}

void InputSystem::update(void)
{
	_input_mgr->update();
}

NS_END
