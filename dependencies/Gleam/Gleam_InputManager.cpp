/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_InputManager.h"
#include "Gleam_Keyboard_MessagePump.h"
#include "Gleam_Mouse_MessagePump.h"
#include "Gleam_IWindow.h"
#include "Gleam_Global.h"
#include "Gaff_IncludeAssert.h"

NS_GLEAM

InputManager::InputManager(void):
	_window(nullptr)
{
}

InputManager::~InputManager(void)
{
	destroy();
}
bool InputManager::init(const IWindow& window)
{
	_window = &window;
	return true;
}

void InputManager::destroy(void)
{
	for (unsigned int i = 0; i < _input_devices.size(); ++i) {
		GleamFree(_input_devices[i]);
	}

	_input_devices.clear();
	_window = nullptr;
}

void InputManager::update(void)
{
	for (unsigned int i = 0; i < _input_devices.size(); ++i) {
		_input_devices[i]->update();
	}
}

void InputManager::addInputDevice(IInputDevice* device)
{
	_input_devices.push(device);
}

bool InputManager::removeInputDevice(const IInputDevice* device)
{
	assert(device);
	GleamArray<IInputDevice*>::Iterator it = _input_devices.linearSearch(device);

	if (it != _input_devices.end()) {
		_input_devices.fastErase(it);
		return true;
	}

	return false;
}

IInputDevice* InputManager::getInputDevice(unsigned int i) const
{
	assert(i < _input_devices.size());
	return _input_devices[i];
}

IInputDevice* InputManager::createKeyboard(bool no_windows_key)
{
	assert(_window);

	IKeyboard* keyboard = GleamAllocateT(KeyboardMP);

	if (!keyboard || !keyboard->init(*_window, no_windows_key)) {
		return nullptr;
	}

	_input_devices.push(keyboard);
	return keyboard;
}

IInputDevice* InputManager::createMouse(void)
{
	assert(_window);

	IMouse* mouse = GleamAllocateT(MouseMP);

	if (!mouse || !mouse->init(*_window)) {
		return nullptr;
	}

	_input_devices.push(mouse);
	return mouse;
}

NS_END
