/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
#include "Gleam_Array.h"
#include <Gaff_Function.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <GL/glx.h>

NS_GLEAM

class Window : public IWindow
{
public:
	static void AddGlobalMessageHandler(const MessageHandler& callback);
	static bool RemoveGlobalMessageHandler(const MessageHandler& callback);

	static void HandleWindowMessages(void);

	Window(void);
	~Window(void);

	bool init(const char* app_name, MODE window_mode = FULLSCREEN,
				unsigned int width = 0, unsigned int height = 0,
				short refresh_rate = 60, int pos_x = 0, int pos_y = 0,
				const char* device_name = nullptr);
	void destroy(void);

	void addWindowMessageHandler(const MessageHandler& callback);
	bool removeWindowMessageHandler(const MessageHandler& callback);

	void showCursor(bool show);
	void containCursor(bool contain);

	bool isCursorVisible(void) const;
	bool isCursorContained(void) const;

	bool setWindowMode(MODE window_mode, int width = 0, int height = 0, short refresh_rate = 60);
	MODE getWindowMode(void) const;

	void getPos(int& x, int& y) const;
	void getDimensions(unsigned int& width, unsigned int& height) const;
	int getPosX(void) const;
	int getPosY(void) const;
	unsigned int getWidth(void) const;
	unsigned int getHeight(void) const;
	bool isFullScreen(void) const;

	bool setIcon(const char* icon);

	INLINE XVisualInfo* getVisualInfo(void) const;
	INLINE Display* getDisplay(void) const;
	INLINE ::Window getWindow(void) const;

	XRRScreenSize* getResolutions(int& num_sizes, int screen = -1) const;
	short* getRefreshRates(int& num_rates, int screen = -1) const;
	short* getRefreshRates(const XRRScreenSize& resolution, int& num_rates, int screen = -1) const;

private:
	int _pos_x, _pos_y;
	unsigned int _width, _height;
	GleamAString _application_name;
	short _refresh_rate;

	MODE _window_mode;
	bool _cursor_visible;
	bool _contain;

	Display* _display;
	XVisualInfo* _visual_info;
	::Window _window;

	Atom _delete_window;
	Atom _protocols;

	XRRScreenSize _original_size;
	Rotation _original_rotation;
	short _original_rate;

	GleamArray<MessageHandler> _window_callbacks;

	static void WindowProc(XEvent& event);

	int chooseClosestResolution(XRRScreenSize* sizes, int num_sizes,
		unsigned int width, unsigned int height);
	int chooseClosestRate(short* rates, int num_rates, short rate);
	void setToOriginalResolutionRate(void);
	INLINE void handleMessage(AnyMessage* message);

	GAFF_NO_COPY(Window);
	GAFF_NO_MOVE(Window);
};

NS_END
