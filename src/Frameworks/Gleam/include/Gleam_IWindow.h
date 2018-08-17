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

#include <Gleam_Window_Defines.h>
#include <EASTL/functional.h>

NS_GLEAM

using MessageHandler = eastl::function<bool (const AnyMessage&)>;

class IWindow
{
public:
	enum WindowMode { WM_FULLSCREEN = 0, WM_WINDOWED, WM_BORDERLESS_WINDOWED };

	IWindow(void) {}
	virtual ~IWindow(void) {}

	virtual bool init(
		const char* app_name, WindowMode window_mode = WM_FULLSCREEN,
		int32_t width = 0, int32_t height = 0,
		int32_t pos_x = 0, int32_t pos_y = 0,
		const char* compat = nullptr
	) = 0;

	virtual void destroy(void) = 0;

	virtual int32_t addWindowMessageHandler(const MessageHandler& callback) = 0;
	virtual int32_t addWindowMessageHandler(MessageHandler&& callback) = 0;
	virtual bool removeWindowMessageHandler(int32_t id) = 0;

	virtual void showCursor(bool show) = 0;
	virtual void containCursor(bool contain) = 0;

	virtual bool isCursorVisible(void) const = 0;
	virtual bool isCursorContained(void) const = 0;

	virtual bool setWindowMode(WindowMode window_mode) = 0;
	virtual WindowMode getWindowMode(void) const = 0;

	virtual void getPos(int& x, int& y) const = 0;
	virtual void getDimensions(int32_t& width, int32_t& height) const = 0;
	virtual int32_t getPosX(void) const = 0;
	virtual int32_t getPosY(void) const = 0;
	virtual int32_t getWidth(void) const = 0;
	virtual int32_t getHeight(void) const = 0;
	virtual bool isFullScreen(void) const = 0;

	virtual bool setIcon(const char* icon) = 0;

	virtual void setPos(int32_t x, int32_t y) = 0;
	virtual void setDimensions(int32_t width, int32_t height) = 0;
};

NS_END
