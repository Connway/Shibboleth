/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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
	_dinput(NULLPTR), _window(NULLPTR)
#else
	_window(NULLPTR)
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
	HRESULT result = DirectInput8Create(window.getHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_dinput, NULLPTR);
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

	_window = NULLPTR;
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
	int index = _input_devices.linearFind((IInputDevice* const)device);

	if (index > -1) {
		_input_devices.erase(index);
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
	return NULLPTR;
#endif
}

IInputDevice* InputManager::createKeyboard(bool no_windows_key)
{
	assert(_window);

#ifdef USE_DI
	IInputDevice* keyboard = GleamClassAllocate(KeyboardDI);
#else
	IInputDevice* keyboard = GleamClassAllocate(KeyboardMP);
#endif

	if (!keyboard || !keyboard->init(*_window, getDirectInput(), no_windows_key)) {
		return NULLPTR;
	}

	_input_devices.push(keyboard);
	return keyboard;
}

IInputDevice* InputManager::createMouse(void)
{
	assert(_window);

#ifdef USE_DI
	IInputDevice* mouse = GleamClassAllocate(MouseDI);
#else
	IInputDevice* mouse = GleamClassAllocate(MouseMP);
#endif

	if (!mouse || !mouse->init(*_window, getDirectInput())) {
		return NULLPTR;
	}

	_input_devices.push(mouse);
	return mouse;
}

NS_END
