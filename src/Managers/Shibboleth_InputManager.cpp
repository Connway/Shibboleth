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
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IKeyboard.h>
#include <Gleam_IMouse.h>

#define INPUT_ALIASES_CFG "cfg/input_aliases.cfg"
#define KEY_BINDINGS_CFG "cfg/key_bindings.cfg"

NS_SHIBBOLETH

#include "Shibboleth_InputReflection.inl"

REF_IMPL_REQ(InputManager);
SHIB_REF_IMPL(InputManager)
.addBaseClassInterfaceOnly<InputManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

InputManager::InputManager(void):
	_keyboard(nullptr), _mouse(nullptr)
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
	RenderManager& rm = GetApp().getManagerT<RenderManager>();

	_keyboard = rm.createKeyboard();

	if (!_keyboard->init()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Failed to initialize keyboard.\n");
		GetApp().quit();
	}

	_keyboard->addInputHandler(Gaff::Bind(this, &InputManager::keyboardHandler));
	_keyboard->allowRepeats(false);

	_mouse = rm.createMouse();

	if (!_mouse->init()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Failed to initialize mouse.\n");
		GetApp().quit();
	}

	_mouse->addInputHandler(Gaff::Bind(this, &InputManager::mouseHandler));
	_mouse->allowRepeats(false);

	if (!loadAliasFile()) {
		return false;
	}

	loadKeyBindings();
	return true;
}

void InputManager::update(double, void*)
{
	_keyboard->update();
	_mouse->update();
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

size_t InputManager::getAliasIndex(const char* alias) const
{
	uint32_t hash = Gaff::FNV1aHash32(alias, strlen(alias));
	auto it = _values.findElementWithKey(hash);

	if (it != _values.end()) {
		return static_cast<size_t>(it - _values.begin());
	}

	return SIZE_T_FAIL;
}

float InputManager::getAliasValue(size_t index) const
{
	GAFF_ASSERT(index < _values.size());
	return _values.atIndex(index)->second;
}

void InputManager::keyboardHandler(Gleam::IInputDevice*, unsigned int key, float value)
{
	Gleam::KeyCode key_code = static_cast<Gleam::KeyCode>(key);
	auto it = _key_bindings.findElementWithKey(key_code);

	if (it != _key_bindings.end()) {
		*it->second.second = it->second.first * value;
	}
}

void InputManager::mouseHandler(Gleam::IInputDevice*, unsigned int event, float value)
{
	Gleam::MouseCode mouse_code = static_cast<Gleam::MouseCode>(event);
	auto it = _mouse_bindings.findElementWithKey(mouse_code);

	if (it != _mouse_bindings.end()) {
		*it->second.second = it->second.first * value;
	}
}

bool InputManager::loadAliasFile(void)
{
	IFile* input_aliases_file = GetApp().getFileSystem()->openFile(INPUT_ALIASES_CFG);

	if (!input_aliases_file) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Could not open file '" INPUT_ALIASES_CFG "'.\n");
		return false;
	}

	Gaff::JSON aliases;

	if (!aliases.parse(input_aliases_file->getBuffer())) {
		GetApp().getLogManager().logMessage(
			LogManager::LOG_ERROR, GetApp().getLogFileName(),
			"Could not parse file '" INPUT_ALIASES_CFG "' with error: '%s'\n",
			aliases.getErrorText()
		);

		GetApp().getFileSystem()->closeFile(input_aliases_file);
		return false;
	}

	GetApp().getFileSystem()->closeFile(input_aliases_file);

	if (!aliases.isArray()) {
		GetApp().getLogManager().logMessage(
			LogManager::LOG_ERROR, GetApp().getLogFileName(),
			"Root element of '" INPUT_ALIASES_CFG "' is not an array.\n"
		);

		return false;
	}

	_values.reserve(aliases.size());

	bool succeeded = !aliases.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
	{
		if (!value.isString()) {
			GetApp().getLogManager().logMessage(
				LogManager::LOG_ERROR, GetApp().getLogFileName(),
				"Element '%zu' in '" INPUT_ALIASES_CFG "' is not a string.\n",
				index
			);
			return true;
		}

		uint32_t hash = Gaff::FNV1aHash32(value.getString(), strlen(value.getString()));
		_values.emplace(hash, 0.0f);

		return false;
	});

	return succeeded;
}

bool InputManager::loadKeyBindings(void)
{
	IFile* key_bidings_file = GetApp().getFileSystem()->openFile(KEY_BINDINGS_CFG);

	if (!key_bidings_file) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Could not open file '" KEY_BINDINGS_CFG "'.\n");
		return false;
	}

	Gaff::JSON key_bindings;

	if (!key_bindings.parse(key_bidings_file->getBuffer())) {
		GetApp().getLogManager().logMessage(
			LogManager::LOG_ERROR, GetApp().getLogFileName(),
			"Could not parse file '" KEY_BINDINGS_CFG "' with error: '%s'\n",
			key_bindings.getErrorText()
		);

		GetApp().getFileSystem()->closeFile(key_bidings_file);
		return false;
	}

	GetApp().getFileSystem()->closeFile(key_bidings_file);

	if (!key_bindings.isArray()) {
		GetApp().getLogManager().logMessage(
			LogManager::LOG_ERROR, GetApp().getLogFileName(),
			"Root element of '" KEY_BINDINGS_CFG "' is not an array.\n"
		);

		return false;
	}

	const auto& mkeys = GetEnumRefDef<Gleam::MouseCode>();
	const auto& keys = GetEnumRefDef<Gleam::KeyCode>();

	bool succeeded = true;
	
	key_bindings.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
	{
		if (!value.isObject()) {
			GetApp().getLogManager().logMessage(
				LogManager::LOG_ERROR, GetApp().getLogFileName(),
				"Element '%zu' in '" KEY_BINDINGS_CFG "' is not an object.\n",
				index
			);

			succeeded = false;
			return false;
		}

		Gaff::JSON negative = value["Negative"];
		Gaff::JSON binding = value["Binding"];
		Gaff::JSON alias = value["Alias"];

		if (!binding.isString()) {
			GetApp().getLogManager().logMessage(
				LogManager::LOG_ERROR, GetApp().getLogFileName(),
				"Element '%zu' in '" KEY_BINDINGS_CFG "' does not have key 'Binding' or value is not a string.\n",
				index
			);

			succeeded = false;
			return false;
		}

		if (!alias.isString()) {
			GetApp().getLogManager().logMessage(
				LogManager::LOG_ERROR, GetApp().getLogFileName(),
				"Element '%zu' in '" KEY_BINDINGS_CFG "' does not have key 'Alias' or value is not a string.\n",
				index
			);

			succeeded = false;
			return false;
		}

		const char* binding_str = binding.getString();
		Gleam::MouseCode mouse_code;
		Gleam::KeyCode key_code;

		if (mkeys.getValue(binding_str, mouse_code)) {
			addMouseBinding(alias.getString(), mouse_code, negative.isTrue());
			return false;
		}

		if (keys.getValue(binding_str, key_code)) {
			addKeyBinding(alias.getString(), key_code, negative.isTrue());
			return false;
		}

		GetApp().getLogManager().logMessage(
			LogManager::LOG_WARNING, GetApp().getLogFileName(),
			"Element '%zu' in '" KEY_BINDINGS_CFG "' unknown input value '%s' for 'Binding'.\n",
			index, binding_str
		);

		succeeded = false;
		return false;
	});

	return succeeded;
}

NS_END
