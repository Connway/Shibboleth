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

#pragma once

#include "Gleam_IKeyboard.h"

NS_GLEAM

class KeyboardMP : public IKeyboard
{
public:
	KeyboardMP(void);
	~KeyboardMP(void);

	bool init(IWindow& window, bool no_windows_key);
	bool init(IWindow& window);
	bool init(bool no_windows_key);
	bool init(void);
	void destroy(void);
	void update(void);

	bool isKeyDown(KeyCode key) const;
	bool isKeyUp(KeyCode key) const;

	void allowRepeats(bool allow);
	bool areRepeatsAllowed(void) const;

	const uint8_t* getKeyboardData(void) const;

	const char* getDeviceName(void) const;
	const char* getPlatformImplementationString(void) const;

	const IWindow* getAssociatedWindow(void) const;

	bool handleMessage(const AnyMessage& message);

private:
	uint8_t _curr_state[256];
	uint8_t _prev_state[256];
	char _flags;

	IWindow* _window;
};

NS_END