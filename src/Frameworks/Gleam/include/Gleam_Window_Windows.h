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
#include "Gleam_IWindow.h"
#include "Gleam_VectorMap.h"
#include "Gleam_Vector.h"
#include "Gleam_String.h"
#include <Gaff_IncludeWindows.h>

NS_GLEAM

class Window : public IWindow
{
public:
	static int32_t AddGlobalMessageHandler(const MessageHandler& callback);
	static int32_t AddGlobalMessageHandler(MessageHandler&& callback);
	static bool RemoveGlobalMessageHandler(int32_t id);

	static void HandleWindowMessages(void);
	static void Cleanup(void);

	Window(void);
	~Window(void);

	bool init(HWND hwnd);

	bool init(const char* window_name, WindowMode window_mode = WindowMode::Fullscreen,
				int32_t width = 0, int32_t height = 0,
				int32_t pos_x = 0, int32_t pos_y = 0, const char* compat = nullptr) override;
	void destroy(void) override;

	int32_t addWindowMessageHandler(const MessageHandler& callback) override;
	int32_t addWindowMessageHandler(MessageHandler&& callback) override;
	bool removeWindowMessageHandler(int32_t id) override;

	void showCursor(bool show) override;
	void containCursor(bool contain) override;

	bool isCursorVisible(void) const override;
	bool isCursorContained(void) const override;

	bool setWindowMode(WindowMode window_mode) override;
	WindowMode getWindowMode(void) const override;

	const glm::ivec2& getPos(void) const override;
	const glm::ivec2& getSize(void) const override;
	void setPos(const glm::ivec2& pos) override;
	void setSize(const glm::ivec2& size) override;

	bool isFullScreen(void) const override;

	bool setIcon(const char* icon) override;

	HINSTANCE getHInstance(void) const;
	HWND getHWnd(void) const;

private:
	glm::ivec2 _pos{ 0, 0 };
	glm::ivec2 _size{ 1, 1 };

	WindowMode _window_mode = WindowMode::Fullscreen;
	bool _cursor_visible = true;
	bool _contain = false;

	U8String _window_name;
	HINSTANCE _hinstance = nullptr;
	HWND _hwnd = nullptr;
	bool _owns_window = true;

	VectorMap<int32_t, MessageHandler> _window_callbacks;

	static VectorMap<uint16_t, KeyCode> g_right_keys;
	static VectorMap<uint16_t, KeyCode> g_left_keys;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);

	friend void WindowMoved(AnyMessage&, Window*, WPARAM, LPARAM);
	friend void WindowResized(AnyMessage&, Window*, WPARAM, LPARAM);
	friend void WindowInput(AnyMessage&, Window*, WPARAM, LPARAM);
	friend void WindowSetFocus(AnyMessage&, Window*, WPARAM, LPARAM);
	friend void WindowKillFocus(AnyMessage&, Window*, WPARAM, LPARAM);

	GAFF_NO_COPY(Window);
	GAFF_NO_MOVE(Window);
};

NS_END
