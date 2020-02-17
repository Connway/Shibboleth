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

#pragma once

#include "Shibboleth_InputReflection.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_IManager.h>
#include <Gleam_Keyboard_MessagePump.h>
#include <Gleam_Mouse_MessagePump.h>

NS_SHIBBOLETH

class InputManager final : public IManager
{
public:
	template <size_t size>
	float getAliasValue(const char (&alias_name)[size])
	{
		return getAliasValue(Gaff::FNV1aHash32Const(alias_name));
	}

	bool init(void) override;

	float getAliasValue(Gaff::Hash32 alias_name) const;
	float getAliasValue(const char* alias_name) const;

private:
	struct Binding final
	{
		Vector<Gleam::MouseCode> mouse_codes{ ProxyAllocator("Input") };
		Vector<Gleam::KeyCode> key_codes{ ProxyAllocator("Input") };
		Gaff::Hash32 alias;
		float scale = 1.0f;
	};

	VectorMap<Gaff::Hash32, float> _alias_values{ ProxyAllocator("Input") };
	Vector<Binding> _bindings{ ProxyAllocator("Input") };

	Gleam::KeyboardMP _keyboard;
	Gleam::MouseMP _mouse;

	void handleKeyboardInput(Gleam::IInputDevice*, int32_t key_code, float value);
	void handleMouseInput(Gleam::IInputDevice*, int32_t mouse_code, float value);

	SHIB_REFLECTION_CLASS_DECLARE(InputManager);
};

NS_END

SHIB_REFLECTION_DECLARE(InputManager)
