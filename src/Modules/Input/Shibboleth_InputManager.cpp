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
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Gleam_Window.h>
#include <Gaff_JSON.h>
#include <EASTL/sort.h>

SHIB_REFLECTION_DEFINE_BEGIN(InputManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(InputManager)

NS_SHIBBOLETH

constexpr char* const g_alias_cfg_schema =
R"({
	"type": "array",
	"items": { "type": "string" }
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

			"required": ["Alias", "Binding"]
		}
	}
})";


SHIB_REFLECTION_CLASS_DEFINE(InputManager)

bool InputManager::init(void)
{
	if (!_keyboard.init()) {
		// $TODO: Log error.
		return false;
	}

	if (!_mouse.init()) {
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

	aliases.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		GAFF_ASSERT(value.isString());
		_alias_values[Gaff::FNV1aHash32String(value.getString())] = 0.0f;
		return false;
	});

	_alias_values.shrink_to_fit();

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

	_keyboard.addInputHandler(Gaff::MemberFunc(this, &InputManager::handleKeyboardInput));
	_mouse.addInputHandler(Gaff::MemberFunc(this, &InputManager::handleMouseInput));

	input_bindings.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		GAFF_ASSERT(value.isObject());

		const Gaff::JSON binding = value["Binding"];
		const Gaff::JSON scale = value["Scale"];
		const Gaff::JSON alias = value["Alias"];

		GAFF_ASSERT(binding.isString() || binding.isArray());
		GAFF_ASSERT(scale.isFloat() || scale.isNull());
		GAFF_ASSERT(alias.isString());

		const auto& mouse_ref_def = Reflection<Gleam::MouseCode>::GetReflectionDefinition();
		const auto& key_ref_def = Reflection<Gleam::KeyCode>::GetReflectionDefinition();

		const char* const alias_str = alias.getString();
		const float scale_value = scale.getFloat(1.0f);

		const Gaff::Hash32 alias_hash = Gaff::FNV1aHash32String(alias_str);

		if (_alias_values.find(alias_hash) == _alias_values.end()) {
			LogErrorDefault("InputManager: Invalid input alias '%s'.", alias_str);
			alias.freeString(alias_str);

			return false;
		}

		Binding final_binding;

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

		return false;
	});

	return true;
}

float InputManager::getAliasValue(Gaff::Hash32 alias_name) const
{
	const auto it = _alias_values.find(alias_name);
	return (it == _alias_values.end()) ? 0.0f : it->second;
}

float InputManager::getAliasValue(const char* alias_name) const
{
	return getAliasValue(Gaff::FNV1aHash32String(alias_name));
}

float InputManager::getAliasValue(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_alias_values.size()));
	return _alias_values.at(index).second;
}

int32_t InputManager::getAliasIndex(Gaff::Hash32 alias_name) const
{
	const auto it = _alias_values.find(alias_name);
	return (it != _alias_values.end()) ? static_cast<int32_t>(eastl::distance(_alias_values.begin(), it)) : -1;
}

int32_t InputManager::getAliasIndex(const char* alias_name) const
{
	return getAliasIndex(Gaff::FNV1aHash32String(alias_name));
}

void InputManager::handleKeyboardInput(Gleam::IInputDevice*, int32_t key_code, float value)
{
	const Gleam::KeyCode code = static_cast<Gleam::KeyCode>(key_code);

	for (Binding& binding : _bindings) {
		const int8_t binding_size = static_cast<int8_t>(binding.key_codes.size() + binding.mouse_codes.size());
		const auto it = Gaff::Find(binding.key_codes, code);

		if (it != binding.key_codes.end()) {
			if (value > 0.0f) {
				++binding.count;

				if (binding.count == binding_size) {
					_alias_values[binding.alias] += binding.scale;
				}

			} else {
				if (binding.count == binding_size) {
					_alias_values[binding.alias] -= binding.scale;
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

	for (Binding& binding : _bindings) {
		const int8_t binding_size = static_cast<int8_t>(binding.key_codes.size() + binding.mouse_codes.size());
		const auto it = Gaff::Find(binding.mouse_codes, code);

		if (it != binding.mouse_codes.end()) {
			if (binding_size == 1 && !is_button) {
				_alias_values[binding.alias] = binding.scale * value;

			} else {
				if (value != 0.0f) {
					++binding.count;

					if (binding.count == binding_size) {
						_alias_values[binding.alias] += binding.scale;
					}

				} else {
					if (binding.count == binding_size) {
						_alias_values[binding.alias] -= binding.scale;
					}

					--binding.count;
				}
			}
		}
	}
}

NS_END
