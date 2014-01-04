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
#include "Gleam_Array.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <GL/glx.h>

NS_GLEAM

class Window
{
public:
	enum MODE { FULLSCREEN = 0, WINDOWED, FULLSCREEN_WINDOWED };

	Window(void);
	~Window(void);

	bool init(const GChar* app_name, MODE window_mode = FULLSCREEN,
					unsigned int width = 0, unsigned int height = 0,
					short refresh_rate = 60, int pos_x = -1, int pos_y = -1);
	void destroy(void);

	INLINE void handleWindowMessages(void);
	INLINE void addWindowMessageHandler(WindowCallback callback);
	bool removeWindowMessageHandler(WindowCallback callback);
	INLINE void addWindowMessageHandler(IWindowMessageHandler* handler);
	bool removeWindowMessageHandler(IWindowMessageHandler* handler);

	void showCursor(bool show);
	INLINE void allowRepeats(bool allow);
	void containCursor(bool contain);

	bool setWindowMode(MODE window_mode, int width = -1, int height = -1);
	INLINE MODE getWindowMode(void) const;

	INLINE void getPos(int& x, int& y) const;
	INLINE void getDimensions(unsigned int& width, unsigned int& height) const;
	INLINE int getPosX(void) const;
	INLINE int getPosY(void) const;
	INLINE unsigned int getWidth(void) const;
	INLINE unsigned int getHeight(void) const;
	INLINE bool isFullScreen(void) const;

	INLINE XVisualInfo* getVisualInfo(void) const;
	INLINE Display* getDisplay(void) const;
	INLINE ::Window getWindow(void) const;

	XRRScreenSize* getResolutions(int& num_sizes, int screen = -1) const;
	short* getRefreshRates(int& num_rates, int screen = -1) const;
	short* getRefreshRates(const XRRScreenSize& resolution, int& num_rates, int screen = -1) const;

private:
	int _pos_x, _pos_y;
	unsigned int _width, _height;
	const GChar* _application_name;
	short _refresh_rate;

	MODE _window_mode;
	bool _no_repeats;
	bool _contain;

	Display* _display;
	XVisualInfo* _visual_info;
	::Window _window;
	XEvent _event;

	Atom _delete_window;
	Atom _protocols;

	XRRScreenSize _original_size;
	short _original_rate;

	int chooseClosestResolution(XRRScreenSize* sizes, int num_sizes,
								unsigned int width, unsigned int height);
	int chooseClosestRate(short* rates, int num_rates, short rate);
	void setToOriginalResolutionRate(void);

	GleamArray(IWindowMessageHandler*) _window_message_handlers;
	GleamArray(WindowCallback) _window_callbacks;

	static void WindowProc(const XEvent& event);
	static GleamArray(Window*) gWindows;

	unsigned int _prev_keycode;

	unsigned int _mouse_prev_x;
	unsigned int _mouse_prev_y;
	bool _first_mouse;
};

NS_END
