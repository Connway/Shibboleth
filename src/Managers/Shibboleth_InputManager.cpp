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

#include "Shibboleth_InputManager.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_Window_Defines.h>

#define INPUT_ALIASES_CFG "cfg/input_aliases.cfg"

NS_SHIBBOLETH

REF_IMPL_REQ(InputManager);
SHIB_REF_IMPL(InputManager)
.addBaseClassInterfaceOnly<InputManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

InputManager::InputManager(void)
{
}

InputManager::~InputManager(void)
{
}

const char* InputManager::getName(void) const
{
	return GetName();
}

void InputManager::getUpdateEntries(Array<UpdateEntry>& entries) 
{
	entries.emplacePush(AString("Input Manager: Update"), Gaff::Bind(this, &InputManager::update));
}

bool InputManager::init(void)
{
	LogManager::FileLockPair& log = GetApp().getGameLogFile();

	if (!_keyboard.init()) {
		log.first.writeString("ERROR - Failed to initialize keyboard.\n");
		return false;
	}

	_keyboard.addInputHandler(Gaff::Bind(this, &InputManager::KeyboardHandler));
	_keyboard.allowRepeats(false);

	if (!_mouse.init()) {
		log.first.writeString("ERROR - Failed to initialize mouse.\n");
		return false;
	}

	_mouse.addInputHandler(Gaff::Bind(this, &InputManager::MouseHandler));
	_mouse.allowRepeats(false);

	IFile* input_aliases_file = GetApp().getFileSystem()->openFile(INPUT_ALIASES_CFG);

	if (!input_aliases_file) {
		log.first.writeString("ERROR - Could not open file '" INPUT_ALIASES_CFG "'.\n");
		return false;
	}

	Gaff::JSON aliases;

	if (!aliases.parse(input_aliases_file->getBuffer())) {
		log.first.writeString("ERROR - Could not parse file '" INPUT_ALIASES_CFG "'.\n");
		log.first.printf("%s\n", aliases.getErrorText());

		GetApp().getFileSystem()->closeFile(input_aliases_file);
		return false;
	}

	GetApp().getFileSystem()->closeFile(input_aliases_file);

	GAFF_ASSERT(aliases.isArray());
	_values.reserve(aliases.size());

	bool succeeded = !aliases.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
	{
		if (!value.isString()) {
			log.first.printf("ERROR - Element '%zu' in '%s' is not a string.\n", index, INPUT_ALIASES_CFG);
			return true;
		}

		uint32_t hash = Gaff::FNV1aHash32(value.getString(), strlen(value.getString()));
		_values.emplace(hash, 0.0f);

		return false;
	});

	return succeeded;
}

void InputManager::update(double, void*)
{
	_keyboard.update();
	_mouse.update();
}

void InputManager::addKeyBinding(const char* alias, Gleam::KeyCode key_code, bool negative)
{
	uint32_t hash = Gaff::FNV1aHash32(alias, strlen(alias));
	float sign_scale = (negative) ? -1.0f : 1.0f;

	auto it = _values.findElementWithKey(hash);
	GAFF_ASSERT_MSG(it != _values.end(), "No input alias with name '%s' found!", alias);

	_key_bindings[key_code] = Gaff::MakePair(sign_scale, &it->second);
}

void InputManager::removeKeyBinding(Gleam::KeyCode key_code)
{
	auto it = _key_bindings.findElementWithKey(key_code);

	if (it != _key_bindings.end()) {
		_key_bindings.erase(it);
	}
}

void InputManager::addMouseBinding(const char* alias, Gleam::MouseCode mouse_code, bool negative)
{
	uint32_t hash = Gaff::FNV1aHash32(alias, strlen(alias));
	float sign_scale = (negative) ? -1.0f : 1.0f;

	auto it = _values.findElementWithKey(hash);
	GAFF_ASSERT_MSG(it != _values.end(), "No input alias with name '%s' found!", alias);

	_mouse_bindings[mouse_code] = Gaff::MakePair(sign_scale, &it->second);
}

void InputManager::removeMouseBinding(Gleam::MouseCode mouse_code)
{
	auto it = _mouse_bindings.findElementWithKey(mouse_code);

	if (it != _mouse_bindings.end()) {
		_mouse_bindings.erase(it);
	}
}

void InputManager::KeyboardHandler(Gleam::IInputDevice*, unsigned int key, float value)
{
	Gleam::KeyCode key_code = static_cast<Gleam::KeyCode>(key);
	auto it = _key_bindings.findElementWithKey(key_code);

	if (it != _key_bindings.end()) {
		*it->second.second = it->second.first * value;
	}
}

void InputManager::MouseHandler(Gleam::IInputDevice*, unsigned int event, float value)
{
	Gleam::MouseCode mouse_code = static_cast<Gleam::MouseCode>(event);
	auto it = _mouse_bindings.findElementWithKey(mouse_code);

	if (it != _mouse_bindings.end()) {
		*it->second.second = it->second.first * value;
	}
}

NS_END
