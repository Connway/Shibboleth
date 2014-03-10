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

#include "Gleam_Keyboard_DirectInput.h"
#include "Gleam_Window.h"

NS_GLEAM

KeyboardDI::KeyboardDI(void):
#ifdef ONLY_INPUT_CHANGES
	_keyboard(nullptr), _curr_state(_keyboard_state_a),
	_prev_state(_keyboard_state_b),	_window(nullptr)
#else
	_keyboard(nullptr), _window(nullptr)
#endif
{
}

KeyboardDI::~KeyboardDI(void)
{
	destroy();
}

bool KeyboardDI::init(const Window& window, void* direct_input, bool no_windows_key)
{
#ifdef ONLY_INPUT_CHANGES
	memset(_keyboard_state_a, 0, sizeof(_keyboard_state_a));
	memset(_keyboard_state_b, 0, sizeof(_keyboard_state_b));
#endif

	IDirectInput8* dinput = (IDirectInput8*)direct_input;

	HRESULT result = dinput->CreateDevice(GUID_SysKeyboard, &_keyboard, nullptr);
	RETURNIFFAILED(result)

	result = _keyboard->SetDataFormat(&c_dfDIKeyboard);
	RETURNIFFAILED(result)

	result = _keyboard->SetCooperativeLevel(window.getHWnd(), DISCL_FOREGROUND | DISCL_EXCLUSIVE |
											((no_windows_key) ? DISCL_NOWINKEY : 0));
	RETURNIFFAILED(result)

	result = _keyboard->Acquire();
	return SUCCEEDED(result);
}

void KeyboardDI::destroy(void)
{
	if (_keyboard) {
		_keyboard->Unacquire();
		_keyboard->Release();
		_keyboard = nullptr;
	}

	_window = nullptr;
}

bool KeyboardDI::update(void)
{
#ifdef ONLY_INPUT_CHANGES
	unsigned char* temp = _curr_state;
	_curr_state = _prev_state;
	_prev_state = temp;

	HRESULT result = _keyboard->GetDeviceState(sizeof(_keyboard_state_a), _curr_state);
#else
	HRESULT result = _keyboard->GetDeviceState(sizeof(_keyboard_state), _keyboard_state);
#endif

	if (FAILED(result)) {
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			_keyboard->Acquire();
		} else {
			return false;
		}

	} else {
		for (unsigned int i = 0; i < 256; ++i) {
#ifdef ONLY_INPUT_CHANGES
			bool curr = (_curr_state[i] & 0x80) != 0;
			bool prev = (_prev_state[i] & 0x80) != 0;

			if (curr != prev) {
				for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
					_input_handlers[j](this, i, (float)curr);
				}
			}
#else
			bool curr = (_keyboard_state[i] & 0x80) != 0;

			for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
				_input_handlers[j](this, i, (float)curr);
			}
#endif
		}
	}

	return true;
}

bool KeyboardDI::isKeyDown(KeyboardCode key) const
{
#ifdef ONLY_INPUT_CHANGES
	return (_curr_state[key] & 0x80) != 0;
#else
	return (_keyboard_state[key] & 0x80) != 0;
#endif
}

bool KeyboardDI::isKeyUp(KeyboardCode key) const
{
#ifdef ONLY_INPUT_CHANGES
	return (_curr_state[key] & 0x80) == 0;
#else
	return (_keyboard_state[key] & 0x80) == 0;
#endif
}

const unsigned char* KeyboardDI::getKeyboardData(void) const
{
#ifdef ONLY_INPUT_CHANGES
	return _curr_state;
#else
	return _keyboard_state;
#endif
}

const GChar* KeyboardDI::getDeviceName(void) const
{
	return GC("Gleam:Keyboard");
}

const GChar* KeyboardDI::getPlatformImplementationString(void) const
{
	return GC("Windows:DirectInput");
};

const Window* KeyboardDI::getAssociatedWindow(void) const
{
	return _window;
}

NS_END
