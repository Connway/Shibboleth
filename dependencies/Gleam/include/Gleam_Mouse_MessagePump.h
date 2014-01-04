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
#include "Gleam_IInputDevice.h"
#include "Gleam_MouseData.h"

NS_GLEAM

class Window;

class MouseMP : public IInputDevice, public IWindowMessageHandler
{
public:
	MouseMP(void);
	~MouseMP(void);

	bool init(const Window& window, void* compat1 = NULLPTR, bool compat2 = false);
	void destroy(void);
	INLINE bool update(void);

	INLINE const MouseData& getMouseData(void) const;
	INLINE void getPosition(int& x, int& y) const;
	INLINE void getDeltas(int& dx, int& dy) const;
	INLINE void getNormalizedPosition(float& nx, float& ny) const;
	INLINE void getNormalizedDeltas(float& ndx, float& ndy) const;
	INLINE short getWheelDelta(void) const;

	INLINE const GChar* getDeviceName(void) const;
	INLINE const GChar* getPlatformImplementationString(void) const;

	INLINE const Window* getAssociatedWindow(void) const;

	INLINE bool isKeyboard(void) const;
	INLINE bool isMouse(void) const;

	bool handleMessage(const AnyMessage& message);

private:
	MouseData _data;
	Window* _window;
};

NS_END
