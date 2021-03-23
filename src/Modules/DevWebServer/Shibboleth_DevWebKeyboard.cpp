/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Shibboleth_DevWebKeyboard.h"
#include <Gaff_Assert.h>

NS_SHIBBOLETH

KeyboardWeb::KeyboardWeb(void)
{
}

KeyboardWeb::~KeyboardWeb(void)
{
}

bool KeyboardWeb::init(Gleam::IWindow& /*window*/, bool /*no_windows_key*/)
{
	GAFF_ASSERT(false);
	return false;
}

bool KeyboardWeb::init(Gleam::IWindow& /*window*/)
{
	GAFF_ASSERT(false);
	return false;
}

bool KeyboardWeb::init(bool /*no_windows_key*/)
{
	GAFF_ASSERT(false);
	return false;
}

bool KeyboardWeb::init(void)
{
	GAFF_ASSERT(false);
	return false;
}

void KeyboardWeb::destroy(void)
{
	_flags.clear();
}

void KeyboardWeb::update(void)
{
	if (areRepeatsAllowed()) {
		const int32_t size = static_cast<int32_t>(_input_handlers.size());

		for (int32_t i = 0; i < 256; ++i) {
			const bool  curr = _data[i];

			for (int32_t j = 0; j < size; ++j) {
				_input_handlers[j](this, i, static_cast<float>(curr));
			}
		}

	} else {
		for (int32_t i = 0; i < 256; ++i) {
			const bool curr = _data[i];
			const bool prev = _prev_state[i];

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

void KeyboardWeb::allowRepeats(bool allow)
{
	_flags.set(allow, Flag::AllowRepeats);
}

bool KeyboardWeb::areRepeatsAllowed(void) const
{
	return _flags.testAll(Flag::AllowRepeats);
}

const char* KeyboardWeb::getDeviceName(void) const
{
	return "Shibboleth:KeyboardWeb";
}

const char* KeyboardWeb::getPlatformImplementationString(void) const
{
	return "Agnostic:Web";
}

const Gleam::IWindow* KeyboardWeb::getAssociatedWindow(void) const
{
	return nullptr;
}

bool KeyboardWeb::handleMessage(const Gleam::AnyMessage& message)
{
	GAFF_REF(message);
	return false;
}

NS_END
