/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
#include "Gleam_Vec2.h"
#include <EASTL/functional.h>

NS_GLEAM

void SetCursor(CursorType cursor);
//CursorType GetCursor(void);

using MessageHandler = eastl::function<bool (const AnyMessage&)>;

class IWindow
{
public:
	enum class WindowMode : uint8_t { Fullscreen = 0, Windowed, BorderlessWindowed };

	IWindow(void) {}
	virtual ~IWindow(void) {}

	virtual bool init(
		const char8_t* window_name,
		WindowMode window_mode = WindowMode::Fullscreen,
		int32_t width = 0,
		int32_t height = 0,
		int32_t pos_x = 0,
		int32_t pos_y = 0,
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

	virtual const IVec2& getPos(void) const = 0;
	virtual const IVec2& getSize(void) const = 0;
	virtual void setPos(const IVec2& pos) = 0;
	virtual void setSize(const IVec2& size) = 0;

	virtual bool isFullScreen(void) const = 0;

	virtual bool setIcon(const char8_t* icon) = 0;
	virtual bool setIcon(const char* icon) = 0;

	virtual void* getPlatformHandle(void) const = 0;
};

NS_END
