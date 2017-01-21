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

#pragma once

#include "Gleam_Defines.h"

NS_GLEAM
	class IWindow;
NS_END

#if defined(_WIN32) || defined(_WIN64)
	#ifndef HID_USAGE_PAGE_GENERIC
		#define HID_USAGE_PAGE_GENERIC ((unsigned short) 0x01)
	#endif

	#define RAW_INPUT_MOUSE ((unsigned short)0x02)
	#define RAW_INPUT_JOYSTICK ((unsigned short)0x04)
	#define RAW_INPUT_GAMEPAD ((unsigned short)0x05)
	#define RAW_INPUT_KEYBOARD ((unsigned short)0x06)

NS_GLEAM

	INLINE bool RegisterForRawInput(unsigned short device, const IWindow& window);

NS_END

#elif defined(__linux__)
	#define RAW_INPUT_MOUSE 1U
	#define RAW_INPUT_JOYSTICK 2U
	#define RAW_INPUT_GAMEPAD 3U
	#define RAW_INPUT_KEYBOARD 4U

NS_GLEAM

	extern bool gAlreadyQueried;
	extern int gOpCode;
	extern int gEvent;
	extern int gError;

	bool RegisterForRawInput(unsigned int device, const IWindow& window);

NS_END

#elif defined (__APPLE__)
#else
	#error "Platform not supported!"
#endif
