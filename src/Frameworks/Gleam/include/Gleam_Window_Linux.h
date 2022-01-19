/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
#include "Gleam_String.h"
#include <Gaff_Function.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

NS_GLEAM

class Window : public IWindow
{
public:
	static int32_t AddGlobalMessageHandler(const MessageHandler& callback);
	static int32_t AddGlobalMessageHandler(MessageHandler&& callback);
	static bool RemoveGlobalMessageHandler(int32_t id);

	static void HandleWindowMessages(void);

	Window(void);
	~Window(void);

	bool init(
		const char* window_name,
		WindowMode window_mode = WindowMode::Fullscreen,
		int32_t width = 0,
		int32_t height = 0,
		int32_t pos_x = 0,
		int32_t pos_y = 0,
		const char* display_name = nullptr
	) override;

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

	const IVec2& getPos(void) const override;
	const IVec2& getSize(void) const override;
	void setPos(const IVec2& pos) override;
	void setSize(const IVec2& size) override;

	bool isFullScreen(void) const override;

	bool setIcon(const char* icon) override;

	void* getPlatformHandle(void) const override;

	XVisualInfo* getVisualInfo(void) const;
	Display* getDisplay(void) const;
	::Window getWindow(void) const;

	XRRScreenSize* getResolutions(int& num_sizes, int screen = -1) const;
	short* getRefreshRates(int& num_rates, int screen = -1) const;
	short* getRefreshRates(const XRRScreenSize& resolution, int& num_rates, int screen = -1) const;

private:
	IVec2 _pos{ 0, 0 };
	IVec2 _size{ 1, 1 };

	WindowMode _window_mode = WindowMode::Fullscreen;
	bool _cursor_visible = true;
	bool _contain = false;

	short _refresh_rate = 60;

	Display* _display = nullptr;
	XVisualInfo* _visual_info = nullptr;
	::Window _window = 0;

	Atom _delete_window = None;
	Atom _protocols = None;

	XRRScreenSize _original_size;
	Rotation _original_rotation;
	short _original_rate;

	VectorMap<int32_t, MessageHandler> _window_callbacks;

	static void WindowProc(XEvent& event);

	int chooseClosestResolution(XRRScreenSize* sizes, int num_sizes, unsigned int width, unsigned int height);
	int chooseClosestRate(short* rates, int num_rates, short rate);
	void setToOriginalResolutionRate(void);
	void handleMessage(AnyMessage* message);

	GAFF_NO_COPY(Window);
	GAFF_NO_MOVE(Window);
};

NS_END
