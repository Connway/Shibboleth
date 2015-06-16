/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_IMouse.h"

NS_GLEAM

class MouseMP : public IMouse
{
public:
	MouseMP(void);
	~MouseMP(void);

	bool init(const IWindow& window);
	void destroy(void);
	void update(void);

	const MouseData& getMouseData(void) const;
	void getPosition(int& x, int& y) const;
	void getDeltas(int& dx, int& dy) const;
	void getNormalizedPosition(float& nx, float& ny) const;
	void getNormalizedDeltas(float& ndx, float& ndy) const;
	short getWheelDelta(void) const;

	const char* getDeviceName(void) const;
	const char* getPlatformImplementationString(void) const;

	const IWindow* getAssociatedWindow(void) const;

private:
	MouseData _curr_data;
	MouseData _prev_data;
	IWindow* _window;

	int _dx, _dy;
	short _wheel;

	bool handleMessage(const AnyMessage& message);
};

NS_END
