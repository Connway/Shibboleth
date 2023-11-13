/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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
#include "Shibboleth_InputConfig.h"
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_GameTime.h>
#include <Shibboleth_AppUtils.h>
#include <Shibboleth_JobPool.h>
#include <Gaff_Function.h>
#include <Gaff_JSON.h>
#include <Gaff_Math.h>
#include <EASTL/sort.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::InputManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::InputManager)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(InputManager)

static constexpr int32_t k_keyboard_device = GLFW_JOYSTICK_LAST + 1;
static constexpr int32_t k_mouse_device = GLFW_JOYSTICK_LAST + 2;

static constexpr const char8_t* const g_binding_cfg_schema =
u8R"({
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

static void UpdateWindows(uintptr_t, void* data)
{
	reinterpret_cast<IRenderManager*>(data)->updateWindows();
}


bool InputManager::initAllModulesLoaded(void)
{
	const auto* const alias_config = GetConfig<InputAliasConfig>();

	for (const InputAlias& alias : alias_config->aliases) {
		GAFF_REF(alias);
	}

	// Load bindings.
	IFile* const input_file = GetApp().getFileSystem().openFile(u8"cfg/input_bindings.cfg");

	if (!input_file) {
		LogErrorDefault("InputManager: Failed to open file 'cfg/input_bindings.cfg'");
		return false;
	}

	Gaff::JSON input_bindings;

	if (!input_bindings.parse(reinterpret_cast<char8_t*>(input_file->getBuffer()), g_binding_cfg_schema)) {
		LogErrorDefault("InputManager: Failed to parse 'cfg/input_bindings.cfg' with error '%s'", input_bindings.getErrorText());
		return false;
	}


	// Aliases
	input_bindings.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		GAFF_ASSERT(value.isObject());

		const Gaff::JSON alias = value.getObject(u8"Alias");
		GAFF_ASSERT(alias.isString());

		const char8_t* const alias_string = alias.getString();
		_default_alias_values[Gaff::FNV1aHash32String(alias_string)] = Alias{};
		alias.freeString(alias_string);

		return false;
	});

	_default_alias_values.shrink_to_fit();


	// Bindings
	input_bindings.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		GAFF_ASSERT(value.isObject());

		const Gaff::JSON tap_interval = value.getObject(u8"Tap Interval");
		const Gaff::JSON binding = value.getObject(u8"Binding");
		const Gaff::JSON modes = value.getObject(u8"Modes");
		const Gaff::JSON scale = value.getObject(u8"Scale");
		const Gaff::JSON alias = value.getObject(u8"Alias");
		const Gaff::JSON taps = value.getObject(u8"Taps");

		GAFF_ASSERT(tap_interval.isFloat() || tap_interval.isNull());
		GAFF_ASSERT(binding.isString() || binding.isArray());
		GAFF_ASSERT(modes.isArray() || modes.isString() || modes.isNull());
		GAFF_ASSERT(scale.isFloat() || scale.isNull());
		GAFF_ASSERT(taps.isInt8() || taps.isNull());
		GAFF_ASSERT(alias.isString());

		const auto& mouse_ref_def = Refl::Reflection<Gleam::MouseCode>::GetReflectionDefinition();
		const auto& key_ref_def = Refl::Reflection<Gleam::KeyCode>::GetReflectionDefinition();

		const char8_t* const alias_str = alias.getString();
		const float scale_value = scale.getFloat(1.0f);

		const Gaff::Hash32 alias_hash = Gaff::FNV1aHash32String(alias_str);
		const auto alias_it = _default_alias_values.find(alias_hash);

		GAFF_ASSERT(_default_alias_values.find(alias_hash) != _default_alias_values.end());

		const float tap_interval_value = tap_interval.getFloat(0.1f);
		const int8_t taps_value = taps.getInt8(0);

		Binding final_binding;
		final_binding.alias_index = static_cast<int32_t>(eastl::distance(_default_alias_values.begin(), alias_it));
		final_binding.scale = scale_value;
		final_binding.tap_interval = tap_interval_value;
		final_binding.taps = taps_value;

		alias.freeString(alias_str);

		if (modes.isArray()) {
			/*const bool failed =*/ modes.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
			{
				GAFF_ASSERT(value.isString());
				const char8_t* const mode_string = value.getString();

				final_binding.modes.emplace_back(Gaff::FNV1aHash32String(mode_string));

				value.freeString(mode_string);
				return false;
			});

		} else {
			char8_t mode_name[256] = { 0 };
			const char8_t* const mode_string = modes.getString(mode_name, 256, u8"Default");

			final_binding.modes.emplace_back(Gaff::FNV1aHash32String(mode_string));
		}

		if (binding.isArray()) {
			const bool failed = binding.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
			{
				GAFF_ASSERT(value.isString());

				const char8_t* const binding_str = value.getString();

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
			const char8_t* const binding_str = binding.getString();

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

	// $TODO: Move this to a higher layer.
	// Always have a player 0.
	_km_player_id = addPlayer();

	_device_player_map[k_keyboard_device] = _km_player_id;
	_device_player_map[k_mouse_device] = _km_player_id;

	// Register for all the input callbacks.
	_render_mgr = &GETMANAGERT(Shibboleth::IRenderManager, Shibboleth::RenderManager);

	for (int32_t i = 0; i < _render_mgr->getNumWindows(); ++i) {
		Gleam::Window* const window = _render_mgr->getWindow(i);

		window->addKeyCallback(Gaff::MemberFunc(this, &InputManager::handleKeyboardInput));
		window->addMouseCallback(Gaff::MemberFunc(this, &InputManager::handleMouseInput));
		//window->addGamepadCallback();

		if (!window->isUsingRawMouseMotion()) {
			window->useRawMouseMotion(true);
		}
	}

	return true;
}

void InputManager::update(uintptr_t thread_id_int)
{
	const int32_t num_bindings = static_cast<int32_t>(_bindings.size());

	// Zero out any mouse axis bindings.
	for (int32_t binding_index = 0; binding_index < num_bindings; ++binding_index) {
		const Binding& binding = _bindings[binding_index];

		if (!binding.mouse_codes.empty()) {
			// Binding only uses mouse delta. Zero it out.
			if (binding.key_codes.empty() && binding.mouse_codes.size() == 1) {
				const bool is_delta_axis =	binding.mouse_codes.front() == Gleam::MouseCode::DeltaX ||
											binding.mouse_codes.front() == Gleam::MouseCode::DeltaY;

				if (is_delta_axis) {
					for (auto& instances : _alias_values) {
						instances.at(binding.alias_index).second.value = 0.0f;
					}
				}

			// It is a multiple input binding, reduce the count and handle accordingly.
			} else {
				int32_t change_count = 0;

				if (Gaff::Find(binding.mouse_codes, Gleam::MouseCode::DeltaX) != binding.mouse_codes.end()) {
					++change_count;
				}

				if (Gaff::Find(binding.mouse_codes, Gleam::MouseCode::DeltaY) != binding.mouse_codes.end()) {
					++change_count;
				}

				if (change_count <= 0) {
					continue;
				}

				for (auto it = _binding_instances.begin(); it != _binding_instances.end(); ++it) {
					auto& alias_values = _alias_values[it.getIndex()];

					for (int32_t i = 0; i < change_count; ++i) {
						handleInputChange(binding, it->at(binding_index), alias_values, false);
					}
				}
			}
		}
	}


	// Have main thread update the windows.
	Gaff::Counter counter = 0;
	const Gaff::JobData window_update_job =
	{
		UpdateWindows,
		_render_mgr
	};

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	JobPool& job_pool = GetApp().getJobPool();
	job_pool.addMainThreadJobs(&window_update_job, 1, counter);
	job_pool.helpWhileWaiting(thread_id, counter);


	// Update inputs.
	using DoubleSeconds = eastl::chrono::duration<double>;
	_end = eastl::chrono::high_resolution_clock::now();

	DoubleSeconds delta = _end - _start;
	_start = _end;

	// Delta time is real-time.
	const float dt = static_cast<float>(delta.count());

	for (auto it = _binding_instances.begin(); it != _binding_instances.end(); ++it) {
		const int32_t input_index = it.getIndex();
		auto& input_instance = *it;

		for (int32_t binding_index = 0; binding_index < num_bindings; ++binding_index) {
			auto& binding_instance = input_instance[binding_index];
			const auto& binding = _bindings[binding_index];

			// $TODO: Only do this once when we change modes.
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
	GAFF_ASSERT(_alias_values.validIndex(player_id));
	const auto it = _alias_values[player_id].find(alias_name);
	return (it == _alias_values[player_id].end()) ? 0.0f : it->second.value;
}

float InputManager::getAliasValue(const char* alias_name, int32_t player_id) const
{
	return getAliasValue(Gaff::FNV1aHash32String(alias_name), player_id);
}

float InputManager::getAliasValue(int32_t index, int32_t player_id) const
{
	GAFF_ASSERT(_alias_values.validIndex(player_id));
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
	GAFF_ASSERT(_alias_values.validIndex(player_id));
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

int32_t InputManager::addPlayer(void)
{
	const int32_t player_id = _binding_instances.emplace();
	const int32_t player_id_validate = _alias_values.emplace();

	GAFF_ASSERT(player_id == player_id_validate);
	GAFF_REF(player_id_validate);

	_binding_instances[player_id].resize(_bindings.size());
	_alias_values[player_id] = _default_alias_values;

	return player_id;
}

bool InputManager::removePlayer(int32_t player_id)
{
	if (!_binding_instances.validIndex(player_id)) {
		return false;
	}

	_binding_instances.remove(player_id);
	_alias_values.remove(player_id);

	for (int32_t i = 0; i < static_cast<int32_t>(_device_player_map.size()); ++i) {
		const auto it = _device_player_map.begin() + i;

		if (it->second == player_id) {
			_device_player_map.erase(it);
		}
	}

	return true;
}

bool InputManager::isValidPlayerID(int32_t player_id) const
{
	return _binding_instances.validIndex(player_id);
}

void InputManager::getPlayerIDs(Vector<int32_t>& out_player_ids) const
{
	for (const auto& entry : _device_player_map) {
		if (Gaff::Find(out_player_ids, entry.second) == out_player_ids.end()) {
			out_player_ids.emplace_back(entry.second);
		}
	}

	Gaff::Sort(out_player_ids);
}

bool InputManager::inputDevicePresent(int32_t device_id) const
{
	return _device_player_map.find(device_id) != _device_player_map.end();
}

void InputManager::addInputDevice(int32_t device_id, int32_t player_id)
{
	GAFF_ASSERT(!inputDevicePresent(device_id));
	_device_player_map[device_id] = player_id;
}

bool InputManager::removeInputDevice(int32_t device_id)
{
	return _device_player_map.erase(device_id) == 1;
}

void InputManager::handleKeyboardInput(
	Gleam::Window& /*window*/,
	Gleam::KeyCode key_code,
	bool pressed,
	Gaff::Flags<Gleam::Modifier> /*modifiers*/,
	int32_t /*scan_code*/)
{
	const auto dpm_it = _device_player_map.find(k_keyboard_device);

	GAFF_ASSERT(dpm_it != _device_player_map.end());
	GAFF_ASSERT(_alias_values.validIndex(dpm_it->second));

	const Gleam::KeyCode code = static_cast<Gleam::KeyCode>(key_code);
	const int32_t num_bindings = static_cast<int32_t>(_bindings.size());
	const int32_t player_id = dpm_it->second;

	auto& input_instance = _binding_instances[player_id];
	auto& alias_values = _alias_values[player_id];

	for (int32_t binding_index = 0; binding_index < num_bindings; ++binding_index) {
		auto& binding_instance = input_instance[binding_index];
		const auto& binding = _bindings[binding_index];

		if (Gaff::Find(binding.modes, _curr_mode) == binding.modes.end()) {
			continue;
		}

		if (Gaff::Find(binding.key_codes, code) != binding.key_codes.end()) {
			handleInputChange(binding, binding_instance, alias_values, pressed);
		}
	}
}

void InputManager::handleMouseInput(Gleam::Window& /*window*/, Gleam::MouseCode mouse_code, float value)
{
	const auto dpm_it = _device_player_map.find(k_mouse_device);

	GAFF_ASSERT(dpm_it != _device_player_map.end());
	GAFF_ASSERT(_alias_values.validIndex(dpm_it->second));

	const bool is_button = static_cast<int32_t>(mouse_code) < static_cast<int32_t>(Gleam::MouseCode::ButtonCount);
	const Gleam::MouseCode code = static_cast<Gleam::MouseCode>(mouse_code);
	const int32_t num_bindings = static_cast<int32_t>(_bindings.size());
	const int32_t player_id = dpm_it->second;

	auto& input_instance = _binding_instances[player_id];
	auto& alias_values = _alias_values[player_id];

	for (int32_t binding_index = 0; binding_index < num_bindings; ++binding_index) {
		auto& binding_instance = input_instance[binding_index];
		const auto& binding = _bindings[binding_index];

		if (Gaff::Find(binding.modes, _curr_mode) == binding.modes.end()) {
			continue;
		}

		if (Gaff::Find(binding.mouse_codes, code) != binding.mouse_codes.end()) {
			const int8_t binding_size = static_cast<int8_t>(binding.key_codes.size() + binding.mouse_codes.size());

			if (binding_size == 1 && !is_button) {
				alias_values.at(binding.alias_index).second.value = binding.scale * value;

			} else {
				handleInputChange(binding, binding_instance, alias_values, value != 0.0f);
			}
		}
	}
}

void InputManager::handleInputChange(
	const Binding& binding,
	BindingInstance& binding_instance,
	VectorMap<Gaff::Hash32, Alias>& alias_values,
	bool activated)
{
	const int8_t binding_size = static_cast<int8_t>(binding.key_codes.size() + binding.mouse_codes.size());

	if (activated) {
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

NS_END
