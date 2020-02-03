/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

NS_GLEAM

KeyboardMP::KeyboardMP(void)
{
}

KeyboardMP::~KeyboardMP(void)
{
	destroy();
}

bool KeyboardMP::init(IWindow& window, bool no_windows_key)
{
	_flags.set(no_windows_key, Flag::NoWindowsKey);
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
	_flags.set(no_windows_key, Flag::NoWindowsKey);
	return init();
}

bool KeyboardMP::init(void)
{
	auto cb = Gaff::MemberFunc(this, &KeyboardMP::handleMessage);
	_id = Window::AddGlobalMessageHandler(cb);

	_flags.set(true, Flag::GlobalHandler);
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
			uint8_t curr = _data[i];
			const int32_t size = static_cast<int32_t>(_input_handlers.size());

			for (int32_t j = 0; j < size; ++j) {
				_input_handlers[j](this, i, static_cast<float>(curr));
			}
		}

	} else {
		for (int32_t i = 0; i < 256; ++i) {
			uint8_t curr = _data[i];
			uint8_t prev = _prev_state[i];

			if (curr != prev) {
				const int32_t size = static_cast<int32_t>(_input_handlers.size());

				for (int32_t j = 0; j < size; ++j) {
					_input_handlers[j](this, i, static_cast<float>(curr));
				}
			}
		}

		memcpy(_prev_state, _data, sizeof(_data));
	}
}

void KeyboardMP::allowRepeats(bool allow)
{
	_flags.set(allow, Flag::AllowRepeats);
}

bool KeyboardMP::areRepeatsAllowed(void) const
{
	return _flags.testAll(Flag::AllowRepeats);
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
		case EventType::IN_KEYDOWN:
			_data[static_cast<int32_t>(message.key_char.key)] = true;
			return true;

		case EventType::IN_KEYUP:
			_data[static_cast<int32_t>(message.key_char.key)] = false;
			return true;

		case EventType::IN_CHARACTER: {
			const int32_t size = static_cast<int32_t>(_character_handlers.size());

			for (int32_t i = 0; i < size; ++i) {
				_character_handlers[i](this, message.key_char.character);
			}

			return true;
		}

		// To get rid of pesky "case not handled" warnings.
		default:
			break;
	}

	return false;
}

NS_END
