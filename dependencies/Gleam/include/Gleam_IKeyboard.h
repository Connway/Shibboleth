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

#pragma once

#include "Gleam_Window_Defines.h"
#include "Gleam_IInputDevice.h"

NS_GLEAM

class IKeyboard : public IInputDevice
{
public:
	IKeyboard(void) {}
	virtual ~IKeyboard(void) {}

	virtual bool init(const IWindow& window, bool no_windows_key) = 0;
	virtual bool init(const IWindow& window) = 0;

	virtual bool isKeyDown(KeyCode key) const = 0;
	virtual bool isKeyUp(KeyCode key) const = 0;

	virtual const unsigned char* getKeyboardData(void) const = 0;

	bool isKeyboard(void) const { return true; }
	bool isMouse(void) const { return false; }
};

NS_END
