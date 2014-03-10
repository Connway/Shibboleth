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

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include "Gleam_IMouse.h"
#include <dinput.h>

NS_GLEAM

class MouseDI : public IMouse
{
public:
	MouseDI(void);
	~MouseDI(void);

	bool init(const Window& window, void* dinput, bool compat);
	void destroy(void);
	bool update(void);

	const MouseData& getMouseData(void) const;
	void getPosition(int& x, int& y) const;
	void getDeltas(int& dx, int& dy) const;
	void getNormalizedPosition(float& nx, float& ny) const;
	void getNormalizedDeltas(float& ndx, float& ndy) const;
	short getWheelDelta(void) const;

	const GChar* getDeviceName(void) const;
	const GChar* getPlatformImplementationString(void) const;

	const Window* getAssociatedWindow(void) const;

private:
	IDirectInputDevice8* _mouse;
	MouseData _data;

#ifdef ONLY_INPUT_CHANGES
	DIMOUSESTATE2 _mouse_state_a;
	DIMOUSESTATE2 _mouse_state_b;
	DIMOUSESTATE2* _curr_state;
	DIMOUSESTATE2* _prev_state;
#else
	DIMOUSESTATE2 _mouse_state;
#endif

	const Window* _window;
};

NS_END
