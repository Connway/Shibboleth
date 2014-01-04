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

#pragma once

#include "Gleam_IWindowMessageHandler.h"
#include "Gleam_Keyboard_Defines.h"
#include "Gleam_IInputDevice.h"

NS_GLEAM

class Window;

class KeyboardMP : public IInputDevice, public IWindowMessageHandler
{
public:
	KeyboardMP(void);
	~KeyboardMP(void);

	bool init(const Window& window, void* compat = NULLPTR, bool no_windows_key = false);
	void destroy(void);
	INLINE bool update(void);

	INLINE bool isKeyDown(KeyboardCode key) const;
	INLINE bool isKeyUp(KeyboardCode key) const;

	INLINE const bool* getKeyboardData(void) const;

	INLINE const GChar* getDeviceName(void) const;
	INLINE const GChar* getPlatformImplementationString(void) const;

	INLINE const Window* getAssociatedWindow(void) const;

	INLINE bool isKeyboard(void) const;
	INLINE bool isMouse(void) const;

	bool handleMessage(const AnyMessage& message);

private:
#ifdef ONLY_INPUT_CHANGES
	bool _keyboard_state_a[256];
	bool _keyboard_state_b[256];
	bool* _curr_state;
	bool* _prev_state;
#else
	bool _keyboard_state[256];
#endif
	Window* _window;
};

NS_END
