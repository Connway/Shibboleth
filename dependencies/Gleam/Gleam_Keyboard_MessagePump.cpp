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

#include "Gleam_Keyboard_MessagePump.h"
#include "Gleam_HashMap.h"
#include "Gleam_Window.h"

NS_GLEAM

static GleamHashMap<KeyCode, KeyboardCode> _key_map;
static bool _first_init = true;

KeyboardMP::KeyboardMP(void):
#ifdef ONLY_INPUT_CHANGES
	_curr_state(_keyboard_state_a),
	_prev_state(_keyboard_state_b),
#endif
	_window(nullptr)
{
#ifdef ONLY_INPUT_CHANGES
	memset(_keyboard_state_a, 0, sizeof(_keyboard_state_a));
	memset(_keyboard_state_b, 0, sizeof(_keyboard_state_b));
#else
	memset(_keyboard_state, 0, sizeof(_keyboard_state));
#endif

	if (_first_init) {
		_key_map[KEY_ESCAPE] = K_ESCAPE;
		_key_map[KEY_1] = K_1;
		_key_map[KEY_2] = K_2;
		_key_map[KEY_3] = K_3;
		_key_map[KEY_4] = K_4;
		_key_map[KEY_5] = K_5;
		_key_map[KEY_6] = K_6;
		_key_map[KEY_7] = K_7;
		_key_map[KEY_8] = K_8;
		_key_map[KEY_9] = K_9;
		_key_map[KEY_0] = K_0;
		_key_map[KEY_MINUS] = K_MINUS;
		_key_map[KEY_EQUALS] = K_EQUALS;
		_key_map[KEY_BACKSPACE] = K_BACK;
		_key_map[KEY_TAB] = K_TAB;
		_key_map[KEY_Q] = K_Q;
		_key_map[KEY_W] = K_W;
		_key_map[KEY_E] = K_E;
		_key_map[KEY_R] = K_R;
		_key_map[KEY_T] = K_T;
		_key_map[KEY_Y] = K_Y;
		_key_map[KEY_U] = K_U;
		_key_map[KEY_O] = K_O;
		_key_map[KEY_P] = K_P;
		_key_map[KEY_LEFTSQUAREBRACKET] = K_LEFTBRACKET;
		_key_map[KEY_RIGHTSQUAREBRACKET] = K_RIGHTBRACKET;
		_key_map[KEY_ENTER] = K_ENTER;
		_key_map[KEY_LEFTCONTROL] = K_LEFTCONTROL;
		_key_map[KEY_A] = K_A;
		_key_map[KEY_S] = K_S;
		_key_map[KEY_D] = K_D;
		_key_map[KEY_F] = K_F;
		_key_map[KEY_G] = K_G;
		_key_map[KEY_H] = K_H;
		_key_map[KEY_J] = K_J;
		_key_map[KEY_K] = K_K;
		_key_map[KEY_L] = K_L;
		_key_map[KEY_SEMICOLON] = K_SEMICOLON;
		_key_map[KEY_APOSTROPHE] = K_APOSTROPHE;
		_key_map[KEY_TILDE] = K_GRAVE;
		_key_map[KEY_LEFTSHIFT] = K_LEFTSHIFT;
		_key_map[KEY_BACKSLASH] = K_BACKSLASH;
		_key_map[KEY_Z] = K_Z;
		_key_map[KEY_X] = K_X;
		_key_map[KEY_C] = K_C;
		_key_map[KEY_V] = K_V;
		_key_map[KEY_B] = K_B;
		_key_map[KEY_N] = K_N;
		_key_map[KEY_M] = K_M;
		_key_map[KEY_COMMA] = K_COMMA;
		_key_map[KEY_PERIOD] = K_PERIOD;
		_key_map[KEY_FORWARDSLASH] = K_SLASH;
		_key_map[KEY_RIGHTSHIFT] = K_RIGHTSHIFT;
		_key_map[KEY_MULTIPLY] = K_MULTIPLY;
		_key_map[KEY_LEFTALT] = K_LEFTALT;
		_key_map[KEY_SPACE] = K_SPACE;
		_key_map[KEY_CAPSLOCK] = K_CAPSLOCK;
		_key_map[KEY_F1] = K_F1;
		_key_map[KEY_F2] = K_F2;
		_key_map[KEY_F3] = K_F3;
		_key_map[KEY_F4] = K_F4;
		_key_map[KEY_F5] = K_F5;
		_key_map[KEY_F6] = K_F6;
		_key_map[KEY_F7] = K_F7;
		_key_map[KEY_F8] = K_F8;
		_key_map[KEY_F9] = K_F9;
		_key_map[KEY_F10] = K_F10;
		_key_map[KEY_NUMLOCK] = K_NUMLOCK;
		_key_map[KEY_SCROLLLOCK] = K_SCROLLLOCK;
		_key_map[KEY_NUMPAD7] = K_NUMPAD7;
		_key_map[KEY_NUMPAD8] = K_NUMPAD8;
		_key_map[KEY_NUMPAD9] = K_NUMPAD9;
		_key_map[KEY_SUBTRACT] = K_SUBTRACT;
		_key_map[KEY_NUMPAD4] = K_NUMPAD4;
		_key_map[KEY_NUMPAD5] = K_NUMPAD5;
		_key_map[KEY_NUMPAD6] = K_NUMPAD6;
		_key_map[KEY_ADD] = K_ADD;
		_key_map[KEY_NUMPAD1] = K_NUMPAD1;
		_key_map[KEY_NUMPAD2] = K_NUMPAD2;
		_key_map[KEY_NUMPAD2] = K_NUMPAD3;
		_key_map[KEY_NUMPAD0] = K_NUMPAD0;
		_key_map[KEY_DECIMAL] = K_DECIMAL;
		//_key_map[KEY_] = K_OEM_102;
		_key_map[KEY_F11] = K_F11;
		_key_map[KEY_F12] = K_F12;
		_key_map[KEY_F13] = K_F13;
		_key_map[KEY_F14] = K_F14;
		_key_map[KEY_F15] = K_F15;
		//_key_map[KEY_] = K_NUMPADEQUALS;
		//_key_map[KEY_] = K_NUMPADCENTER;
		_key_map[KEY_RIGHTCONTROL] = K_RIGHTCONTROL;
		//_key_map[KEY_] = K_MUTE;
		//_key_map[KEY_] = K_CALCULATOR;
		//_key_map[KEY_] = K_PLAYPAUSE;
		//_key_map[KEY_] = K_MEDIASTOP;
		//_key_map[KEY_] = K_VOLUMEDOWN;
		//_key_map[KEY_] = K_VOLUMEUP;
		//_key_map[KEY_] = K_WEBHOME;
		//_key_map[KEY_] = K_NUMPADCOMMA;
		_key_map[KEY_DIVIDE] = K_DIVIDE;
		//_key_map[KEY_] = K_SYSRQ;
		_key_map[KEY_RIGHTALT] = K_RIGHTALT;
		_key_map[KEY_PAUSE] = K_PAUSE;
		_key_map[KEY_HOME] = K_HOME;
		_key_map[KEY_UP] = K_UP;
		_key_map[KEY_PAGEUP] = K_PAGEUP;
		_key_map[KEY_LEFT] = K_LEFT;
		_key_map[KEY_RIGHT] = K_RIGHT;
		_key_map[KEY_END] = K_END;
		_key_map[KEY_DOWN] = K_DOWN;
		_key_map[KEY_PAGEDOWN] = K_PAGEDOWN;
		_key_map[KEY_INSERT] = K_INSERT;
		_key_map[KEY_DELETE] = K_DELETE;
		_key_map[KEY_LEFTWINDOWS] = K_LEFTWIN;
		_key_map[KEY_RIGHTWINDOWS] = K_RIGHTWIN;

		_first_init = false;
	}
}

KeyboardMP::~KeyboardMP(void)
{
	destroy();
}

bool KeyboardMP::init(const Window& window, void*, bool)
{
	_window = (Window*)&window;
	_window->addWindowMessageHandler(this, &KeyboardMP::handleMessage);

#ifdef ONLY_INPUT_CHANGES
	_window->allowRepeats(false);
#endif

	return true;
}

void KeyboardMP::destroy(void)
{
	if (_window) {
		_window->removeWindowMessageHandler(this, &KeyboardMP::handleMessage);
		_window = nullptr;
	}
}

bool KeyboardMP::update(void)
{
#ifdef ONLY_INPUT_CHANGES
	unsigned char* temp = _curr_state;
	_curr_state = _prev_state;
	_prev_state = temp;
#endif

	for (unsigned int i = 0; i < 256; ++i) {
#ifdef ONLY_INPUT_CHANGES
		unsigned char curr = _curr_state[i];
		unsigned char prev = _prev_state[i];

		if (curr != prev) {
			for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
				_input_handlers[j](this, i, (float)curr);
			}
		}
#else
		unsigned char curr = _keyboard_state[i];

		for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
			_input_handlers[j](this, i, (float)curr);
		}
#endif
	}

	return true;
}

bool KeyboardMP::isKeyDown(KeyboardCode key) const
{
#ifdef ONLY_INPUT_CHANGES
	return _curr_state[key] != 0;
#else
	return _keyboard_state[key] != 0;
#endif
}

bool KeyboardMP::isKeyUp(KeyboardCode key) const
{
#ifdef ONLY_INPUT_CHANGES
	return !_curr_state[key] == 0;
#else
	return !_keyboard_state[key] == 0;
#endif
}

const unsigned char* KeyboardMP::getKeyboardData(void) const
{
#ifdef ONLY_INPUT_CHANGES
	return _curr_state;
#else
	return _keyboard_state;
#endif
}

const GChar* KeyboardMP::getDeviceName(void) const
{
	return GC("Gleam:Keyboard");
}

const GChar* KeyboardMP::getPlatformImplementationString(void) const
{
	return GC("Agnostic:MessagePump");
};

const Window* KeyboardMP::getAssociatedWindow(void) const
{
	return _window;
}

bool KeyboardMP::handleMessage(const AnyMessage& message)
{
	switch (message.base.type) {
		case IN_KEYDOWN:
#ifdef ONLY_INPUT_CHANGES
			_curr_state[_key_map[message.key_char.key]] = true;
#else
			_keyboard_state[_key_map[message.key_char.key]] = true;
#endif
			return true;

		case IN_KEYUP:
#ifdef ONLY_INPUT_CHANGES
			_curr_state[_key_map[message.key_char.key]] = false;
#else
			_keyboard_state[_key_map[message.key_char.key]] = false;
#endif
			return true;

		// To get rid of pesky "case not handled" warnings in GCC
		default:
			break;
	}

	return false;
}

NS_END
