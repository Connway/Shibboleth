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

#include <Gleam_Window_Defines.h>
#include <Gaff_Function.h>

NS_GLEAM

using MessageHandler = Gaff::FunctionBinder<bool, const AnyMessage&>;

class IWindow
{
public:
	enum MODE { FULLSCREEN = 0, WINDOWED, FULLSCREEN_WINDOWED };

	IWindow(void) {}
	virtual ~IWindow(void) {}

	virtual bool init(
		const char* app_name, MODE window_mode = FULLSCREEN,
		unsigned int width = 0, unsigned int height = 0,
		int pos_x = 0, int pos_y = 0,
		const char* compat = nullptr
	) = 0;

	virtual void destroy(void) = 0;

	virtual void addWindowMessageHandler(const MessageHandler& callback) = 0;
	virtual bool removeWindowMessageHandler(const MessageHandler& callback) = 0;

	virtual void showCursor(bool show) = 0;
	virtual void containCursor(bool contain) = 0;

	virtual bool isCursorVisible(void) const = 0;
	virtual bool isCursorContained(void) const = 0;

	virtual bool setWindowMode(MODE window_mode) = 0;
	virtual MODE getWindowMode(void) const = 0;

	virtual void getPos(int& x, int& y) const = 0;
	virtual void getDimensions(unsigned int& width, unsigned int& height) const = 0;
	virtual int getPosX(void) const = 0;
	virtual int getPosY(void) const = 0;
	virtual unsigned int getWidth(void) const = 0;
	virtual unsigned int getHeight(void) const = 0;
	virtual bool isFullScreen(void) const = 0;

	virtual bool setIcon(const char* icon) = 0;
};

NS_END
