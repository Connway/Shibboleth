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
#include "Gleam_IWindow.h"
#include "Gleam_VectorMap.h"
#include "Gleam_Vector.h"
#include "Gleam_String.h"
#include <Gaff_IncludeWindows.h>

NS_GLEAM

class Window : public IWindow
{
public:
	static void AddGlobalMessageHandler(const MessageHandler& callback);
	static void AddGlobalMessageHandler(MessageHandler&& callback);
	static bool RemoveGlobalMessageHandler(const MessageHandler& callback);

	static void HandleWindowMessages(void);

	Window(void);
	~Window(void);

	bool init(HWND hwnd);

	bool init(const char* app_name, MODE window_mode = FULLSCREEN,
				int32_t width = 0, int32_t height = 0,
				int32_t pos_x = 0, int32_t pos_y = 0, const char* compat = nullptr) override;
	void destroy(void) override;

	void addWindowMessageHandler(const MessageHandler& callback) override;
	void addWindowMessageHandler(MessageHandler&& callback) override;
	bool removeWindowMessageHandler(const MessageHandler& callback) override;

	void showCursor(bool show) override;
	void containCursor(bool contain) override;

	bool isCursorVisible(void) const override;
	bool isCursorContained(void) const override;

	bool setWindowMode(MODE window_mode) override;
	MODE getWindowMode(void) const override;

	void getPos(int32_t& x, int32_t& y) const override;
	void getDimensions(int32_t& width, int32_t& height) const override;
	int32_t getPosX(void) const override;
	int32_t getPosY(void) const override;
	int32_t getWidth(void) const override;
	int32_t getHeight(void) const override;
	bool isFullScreen(void) const override;

	bool setIcon(const char* icon) override;

	void setPos(int32_t x, int32_t y) override;
	void setDimensions(int32_t width, int32_t height) override;

	HINSTANCE getHInstance(void) const;
	HWND getHWnd(void) const;

private:
	int32_t _pos_x, _pos_y;
	int32_t _width, _height;

	MODE _window_mode = FULLSCREEN;
	int32_t _original_width = 0;
	int32_t _original_height = 0;
	bool _cursor_visible = true;
	bool _contain = false;

	U8String _application_name;
	HINSTANCE _hinstance = nullptr;
	HWND _hwnd = nullptr;
	bool _owns_window = true;

	Vector<MessageHandler> _window_callbacks;

	static VectorMap<uint16_t, KeyCode> g_right_keys;
	static VectorMap<uint16_t, KeyCode> g_left_keys;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);

	friend void WindowMoved(AnyMessage*, Window*, WPARAM, LPARAM);
	friend void WindowResized(AnyMessage*, Window*, WPARAM, LPARAM);
	friend void WindowInput(AnyMessage*, Window*, WPARAM, LPARAM);
	friend void WindowSetFocus(AnyMessage*, Window*, WPARAM, LPARAM);
	friend void WindowKillFocus(AnyMessage*, Window*, WPARAM, LPARAM);

	GAFF_NO_COPY(Window);
	GAFF_NO_MOVE(Window);
};

NS_END
