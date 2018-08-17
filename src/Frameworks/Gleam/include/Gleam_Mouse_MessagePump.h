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

#include "Gleam_IMouse.h"

NS_GLEAM

class MouseMP : public IMouse
{
public:
	MouseMP(void);
	~MouseMP(void);

	bool init(IWindow& window) override;
	bool init(void) override;
	void destroy(void) override;
	void update(void) override;

	const MouseData& getMouseData(void) const override;
	void getAbsolutePosition(int32_t& x, int32_t& y) const override;
	void getRelativePosition(int32_t& x, int32_t& y) const override;
	void getDeltas(int32_t& dx, int32_t& dy) const;
	//void getNormalizedAbsolutePosition(float& nx, float& ny) const override;
	void getNormalizedRelativePosition(float& nx, float& ny) const override;
	void getNormalizedDeltas(float& ndx, float& ndy) const override;
	int32_t getWheelDelta(void) const override;

	void allowRepeats(bool allow) override;
	bool areRepeatsAllowed(void) const override;

	const char* getDeviceName(void) const override;
	const char* getPlatformImplementationString(void) const override;

	const IWindow* getAssociatedWindow(void) const override;

private:
	MouseData _curr_data;
	MouseData _prev_data;

	int32_t _dx, _dy;
	int32_t _wheel;

	int32_t _flags;

	IWindow* _window;
	int32_t _id;

	bool handleMessage(const AnyMessage& message);
};

NS_END
