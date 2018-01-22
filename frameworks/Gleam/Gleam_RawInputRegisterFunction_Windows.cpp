/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#if defined(_WIN32) || defined(_WIN64)

#include "Gleam_RawInputRegisterFunction.h"
#include "Gleam_Window_Windows.h"
#include <Gaff_Assert.h>

NS_GLEAM

bool RegisterForRawInput(unsigned short device, const IWindow& window)
{
	GAFF_ASSERT(device == RAW_INPUT_MOUSE || (device >= RAW_INPUT_JOYSTICK && device <= RAW_INPUT_KEYBOARD));

	RAWINPUTDEVICE rid;
	rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid.usUsage = device;
	rid.dwFlags = 0;
	rid.hwndTarget = reinterpret_cast<const Window&>(window).getHWnd();

	return RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE)) == TRUE;
}

NS_END

#endif
