/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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
#include "Gleam_Window.h"
#include "Gleam_Global.h"
#include "Gaff_IncludeAssert.h"

#ifdef USE_DI
	#include "Gleam_Keyboard_DirectInput.h"
	#include "Gleam_Mouse_DirectInput.h"
#endif

NS_GLEAM

InputManager::InputManager(void):
#ifdef USE_DI
	_dinput(nullptr), _window(nullptr)
#else
	_window(nullptr)
#endif
{
}

InputManager::~InputManager(void)
{
	destroy();
}
bool InputManager::init(const Window& window)
{
	_window = &window;

#ifdef USE_DI
	HRESULT result = DirectInput8Create(window.getHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_dinput, nullptr);
	return SUCCEEDED(result);
#else
	return true;
#endif
}

void InputManager::destroy(void)
{
	for (unsigned int i = 0; i < _input_devices.size(); ++i) {
		GleamFree(_input_devices[i]);
	}

	_input_devices.clear();

#ifdef USE_DI
	_dinput->Release();
#endif

	_window = nullptr;
}

bool InputManager::update(void)
{
	bool fail = false;
	for (unsigned int i = 0; i < _input_devices.size(); ++i) {
		fail = fail || !_input_devices[i]->update();
	}

	return !fail;
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
		_input_devices.erase(it);
		return true;
	}

	return false;
}

IInputDevice* InputManager::getInputDevice(unsigned int i) const
{
	assert(i < _input_devices.size());
	return _input_devices[i];
}

IDirectInput8* InputManager::getDirectInput(void) const
{
#ifdef USE_DI
	return _dinput;
#else
	return nullptr;
#endif
}

IInputDevice* InputManager::createKeyboard(bool no_windows_key)
{
	assert(_window);

#ifdef USE_DI
	IInputDevice* keyboard = GleamAllocateT(KeyboardDI);
#else
	IInputDevice* keyboard = GleamAllocateT(KeyboardMP);
#endif

	if (!keyboard || !keyboard->init(*_window, getDirectInput(), no_windows_key)) {
		return nullptr;
	}

	_input_devices.push(keyboard);
	return keyboard;
}

IInputDevice* InputManager::createMouse(void)
{
	assert(_window);

#ifdef USE_DI
	IInputDevice* mouse = GleamAllocateT(MouseDI);
#else
	IInputDevice* mouse = GleamAllocateT(MouseMP);
#endif

	if (!mouse || !mouse->init(*_window, getDirectInput())) {
		return nullptr;
	}

	_input_devices.push(mouse);
	return mouse;
}

NS_END
