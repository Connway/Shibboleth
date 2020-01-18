/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
#include <vec2.hpp>

NS_GLEAM

struct MouseData final
{
	glm::ivec2 abs_pos{ 0, 0 };
	glm::ivec2 rel_pos{ 0, 0 };
	glm::ivec2 delta{ 0, 0 };

	bool buttons[static_cast<uint8_t>(MouseCode::MOUSE_BUTTON_COUNT)] = { false };
	int32_t wheel = 0;
};

class IMouse : public IInputDevice
{
public:
	IMouse(void) {}
	virtual ~IMouse(void) {}

	const MouseData& getMouseData(void) const { return _data; }

	virtual glm::ivec2 getNormalizedAbsolutePosition(void) const = 0;
	virtual glm::ivec2 getNormalizedRelativePosition(void) const = 0;
	virtual glm::ivec2 getNormalizedDeltas(void) const = 0;

	bool isKeyboard(void) const override { return false; }
	bool isMouse(void) const override { return true; }

protected:
	MouseData _data;
};

NS_END
