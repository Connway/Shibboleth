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
#include <Gleam_Window.h>
#include <Gaff_JSON.h>

SHIB_REFLECTION_DEFINE_BEGIN(InputManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(InputManager)

NS_SHIBBOLETH

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
		// $TODO: Log error.
		return false;
	}

	Gaff::JSON aliases;

	if (!aliases.parse(reinterpret_cast<char*>(alias_file->getBuffer()))) {
		// $TODO: Log error.
		return false;
	}

	aliases.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		GAFF_ASSERT(value.isString());
		_alias_values[Gaff::FNV1aHash32String(value.getString())] = 0.0f;
		return false;
	});

	// Load bindings.
	IFile* const input_file = GetApp().getFileSystem().openFile("cfg/input_bindings.cfg");

	if (!input_file) {
		// $TODO: Log error.
		return false;
	}

	Gaff::JSON input_bindings;

	if (!input_bindings.parse(reinterpret_cast<char*>(input_file->getBuffer()))) {
		// $TODO: Log error.
		return false;
	}

	input_bindings.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		GAFF_ASSERT(value.isObject());
		GAFF_ASSERT(value["Alias"].isString());
		GAFF_ASSERT(value["Binding"].isString());
		GAFF_ASSERT(value["Scale"].isFloat());

		const auto& mouse_ref_def = Reflection<Gleam::MouseCode>::GetReflectionDefinition();
		const auto& key_ref_def = Reflection<Gleam::KeyCode>::GetReflectionDefinition();

		const Gaff::JSON binding = value["Binding"];
		const Gaff::JSON scale = value["Scale"];
		const Gaff::JSON alias = value["Alias"];

		const char* const binding_str = binding.getString();
		const char* const alias_str = alias.getString();
		const float scale_value = scale.getFloat();

		const Gaff::Hash32 alias_hash = Gaff::FNV1aHash32String(alias_str);

		if (const int32_t value_kb = key_ref_def.getEntryValue(binding_str); value_kb != std::numeric_limits<int32_t>::min()) {
			_kb_bindings.emplace_back(Binding<Gleam::KeyCode>{ alias_hash, scale_value, static_cast<Gleam::KeyCode>(value_kb) });
		} else if (const int32_t value_mouse = mouse_ref_def.getEntryValue(binding_str); value_mouse != std::numeric_limits<int32_t>::min()) {
			_mouse_bindings.emplace_back(Binding<Gleam::MouseCode>{ alias_hash, scale_value, static_cast<Gleam::MouseCode>(value_mouse) });
		} else {
			// $TODO: Log error.
		}

		binding.freeString(binding_str);
		alias.freeString(alias_str);

		return false;
	});

	return true;
}

NS_END
