/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
#include "Gleam_Window.h"

#define KMP_ALLOW_REPEATS (1 << 0)
#define KMP_NO_WINDOWS_KEY (1 << 1)
#define KMP_GLOBAL_HANDLER (1 << 2)

NS_GLEAM

KeyboardMP::KeyboardMP(void):
	_flags(0), _window(nullptr)
{
	memset(_curr_state, 0, sizeof(_curr_state));
	memset(_prev_state, 0, sizeof(_prev_state));
}

KeyboardMP::~KeyboardMP(void)
{
	destroy();
}

bool KeyboardMP::init(IWindow& window, bool no_windows_key)
{
	if (no_windows_key) {
		_flags |= KMP_NO_WINDOWS_KEY;
	}

	return init(window);
}

bool KeyboardMP::init(IWindow& window)
{
	auto cb = Gaff::MemberFunc(this, &KeyboardMP::handleMessage);
	_window = &window;
	_id = _window->addWindowMessageHandler(cb);

	return RegisterForRawInput(RAW_INPUT_KEYBOARD, window);
}

bool KeyboardMP::init(bool no_windows_key)
{
	if (no_windows_key) {
		_flags |= KMP_NO_WINDOWS_KEY;
	}

	return init();
}

bool KeyboardMP::init(void)
{
	auto cb = Gaff::MemberFunc(this, &KeyboardMP::handleMessage);
	_id = Window::AddGlobalMessageHandler(cb);

	_flags |= KMP_GLOBAL_HANDLER;
	return true;
}

void KeyboardMP::destroy(void)
{
	auto cb = Gaff::MemberFunc(this, &KeyboardMP::handleMessage);

	if (_window) {
		_window->removeWindowMessageHandler(_id);
		_window = nullptr;

	} else {
		Window::RemoveGlobalMessageHandler(_id);
	}

	_flags = 0;
}

void KeyboardMP::update(void)
{
	if (areRepeatsAllowed()) {
		for (int32_t i = 0; i < 256; ++i) {
			uint8_t curr = _curr_state[i];
			const int32_t size = static_cast<int32_t>(_input_handlers.size());

			for (int32_t j = 0; j < size; ++j) {
				_input_handlers[j](this, i, static_cast<float>(curr));
			}
		}

	} else {
		for (int32_t i = 0; i < 256; ++i) {
			uint8_t curr = _curr_state[i];
			uint8_t prev = _prev_state[i];

			if (curr != prev) {
				const int32_t size = static_cast<int32_t>(_input_handlers.size());

				for (int32_t j = 0; j < size; ++j) {
					_input_handlers[j](this, i, static_cast<float>(curr));
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

void KeyboardMP::allowRepeats(bool allow)
{
	if (allow) {
		_flags |= KMP_ALLOW_REPEATS;
	} else {
		_flags &= ~KMP_ALLOW_REPEATS;
	}
}

bool KeyboardMP::areRepeatsAllowed(void) const
{
	return _flags & KMP_ALLOW_REPEATS;
}

const uint8_t* KeyboardMP::getKeyboardData(void) const
{
	return _curr_state;
}

const char* KeyboardMP::getDeviceName(void) const
{
	return "Gleam:Keyboard";
}

const char* KeyboardMP::getPlatformImplementationString(void) const
{
	return "Agnostic:MessagePump";
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

		case IN_CHARACTER: {
			const int32_t size = static_cast<int32_t>(_character_handlers.size());

			for (int32_t i = 0; i < size; ++i) {
				_character_handlers[i](this, message.key_char.character);
			}

			return true;
		}

		// To get rid of pesky "case not handled" warnings in GCC
		default:
			break;
	}

	return false;
}

NS_END
