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

#pragma once

#include "Gleam_Window_Defines.h"
#include "Gleam_IInputDevice.h"
#include "Gleam_Vec2.h"
#include <EASTL/bitset.h>

NS_GLEAM

struct MouseData final
{
	IVec2 abs_pos{ 0, 0 };
	IVec2 rel_pos{ 0, 0 };
	IVec2 delta{ 0, 0 };
	IVec2 wheel{ 0, 0 };

	eastl::bitset<static_cast<size_t>(MouseCode::ButtonCount), uint8_t> buttons;
};

class IMouse : public IInputDevice
{
public:
	IMouse(void) {}
	virtual ~IMouse(void) {}

	void destroy(void) override
	{
		IInputDevice::destroy();
		_data = MouseData();
	}

	const MouseData& getMouseData(void) const { return _data; }

	virtual IVec2 getNormalizedAbsolutePosition(void) const = 0;
	virtual IVec2 getNormalizedRelativePosition(void) const = 0;
	virtual IVec2 getNormalizedDeltas(void) const = 0;

	bool isKeyboard(void) const override { return false; }
	bool isMouse(void) const override { return true; }

protected:
	MouseData _data;
};

NS_END
