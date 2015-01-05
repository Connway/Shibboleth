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

#include "Gleam_Keyboard_MessagePump.h"
#include "Gleam_RawInputRegisterFunction.h"
#include "Gleam_IWindow.h"
#include "Gleam_HashMap.h"

NS_GLEAM

KeyboardMP::KeyboardMP(void):
	_window(nullptr)
{
	memset(_curr_state, 0, sizeof(_curr_state));
	memset(_prev_state, 0, sizeof(_prev_state));
}

KeyboardMP::~KeyboardMP(void)
{
	destroy();
}

bool KeyboardMP::init(const IWindow& window, bool no_windows_key)
{
	if (no_windows_key) {
	}

	return init(window);
}

bool KeyboardMP::init(const IWindow& window)
{
	auto cb = Gaff::Bind(this, &KeyboardMP::handleMessage);
	_window = (IWindow*)&window;
	_window->addWindowMessageHandler(cb);

	return RegisterForRawInput(RAW_INPUT_KEYBOARD, window);
}

void KeyboardMP::destroy(void)
{
	if (_window) {
		auto cb = Gaff::Bind(this, &KeyboardMP::handleMessage);
		_window->removeWindowMessageHandler(cb);
		_window = nullptr;
	}
}

void KeyboardMP::update(void)
{
	if (_window->areRepeatsAllowed()) {
		for (unsigned int i = 0; i < 256; ++i) {
			unsigned char curr = _curr_state[i];

			for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
				_input_handlers[j](this, i, (float)curr);
			}
		}

	} else {
		for (unsigned int i = 0; i < 256; ++i) {
			unsigned char curr = _curr_state[i];
			unsigned char prev = _prev_state[i];

			if (curr != prev) {
				for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
					_input_handlers[j](this, i, (float)curr);
				}
			}
		}

		memcpy(_prev_state, _curr_state, sizeof(_curr_state));
	}
}

bool KeyboardMP::isKeyDown(KeyCode key) const
{
	return _curr_state[key] != 0;
}

bool KeyboardMP::isKeyUp(KeyCode key) const
{
	return !_curr_state[key] == 0;
}

const unsigned char* KeyboardMP::getKeyboardData(void) const
{
	return _curr_state;
}

const GChar* KeyboardMP::getDeviceName(void) const
{
	return GC("Gleam:Keyboard");
}

const GChar* KeyboardMP::getPlatformImplementationString(void) const
{
	return GC("Agnostic:MessagePump");
};

const IWindow* KeyboardMP::getAssociatedWindow(void) const
{
	return _window;
}

bool KeyboardMP::handleMessage(const AnyMessage& message)
{
	switch (message.base.type) {
		case IN_KEYDOWN:
			_curr_state[message.key_char.key] = true;
			return true;

		case IN_KEYUP:
			_curr_state[message.key_char.key] = false;
			return true;

		// To get rid of pesky "case not handled" warnings in GCC
		default:
			break;
	}

	return false;
}

NS_END
