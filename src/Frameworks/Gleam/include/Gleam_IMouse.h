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

#include "Gleam_Window_Defines.h"
#include "Gleam_IInputDevice.h"

NS_GLEAM

struct MouseData
{
	int32_t abs_x, abs_y;
	int32_t rel_x, rel_y;
	int32_t dx, dy;
	bool buttons[MOUSE_BUTTON_COUNT];
	int32_t wheel;
};

class IMouse : public IInputDevice
{
public:
	IMouse(void) {}
	virtual ~IMouse(void) {}

	virtual const MouseData& getMouseData(void) const = 0;
	virtual void getAbsolutePosition(int32_t& x, int32_t& y) const = 0;
	virtual void getRelativePosition(int32_t& x, int32_t& y) const = 0;
	virtual void getDeltas(int32_t& dx, int32_t& dy) const = 0;
	//virtual void getNormalizedAbsolutePosition(float& nx, float& ny) const = 0;
	virtual void getNormalizedRelativePosition(float& nx, float& ny) const = 0;
	virtual void getNormalizedDeltas(float& ndx, float& ndy) const = 0;
	virtual int32_t getWheelDelta(void) const = 0;

	bool isKeyboard(void) const override { return false; }
	bool isMouse(void) const override { return true; }
};

NS_END