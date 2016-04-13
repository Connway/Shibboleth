/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IUpdateQuery.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Map.h>
#include <Gleam_Window_Defines.h>

namespace Gleam
{
	class IInputDevice;
	class IKeyboard;
	class IMouse;
	enum KeyCode;
}

NS_SHIBBOLETH

class InputManager : public IManager, public IUpdateQuery
{
public:
	INLINE static const char* GetName(void)
	{
		return "Input Manager";
	}

	InputManager(void);
	~InputManager(void);

	const char* getName(void) const override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	bool init(void);
	void update(double dt, void*);

	void addKeyBinding(const char* alias, Gleam::KeyCode key_code, bool negative = false);
	void removeKeyBinding(Gleam::KeyCode key_code);

	void addMouseBinding(const char* alias, Gleam::MouseCode mouse_code, bool negative = false);
	void removeMouseBinding(Gleam::MouseCode mouse_code);

	size_t getAliasIndex(const char* alias) const;
	float getAliasValue(size_t index) const;

private:
	Gleam::IKeyboard* _keyboard;
	Gleam::IMouse* _mouse;

	Map<Gleam::KeyCode, Gaff::Pair<float, float*> > _key_bindings;
	Map<Gleam::MouseCode, Gaff::Pair<float, float*> > _mouse_bindings;
	Map<uint32_t, float> _values;

	void keyboardHandler(Gleam::IInputDevice*, unsigned int key, float value);
	void mouseHandler(Gleam::IInputDevice*, unsigned int event, float value);

	bool loadAliasFile(void);
	bool loadKeyBindings(void);

	SHIB_REF_DEF(InputManager);
	REF_DEF_REQ;
};

NS_END
