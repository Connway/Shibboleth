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
#include "Gaff_IncludeWindows.h"
#include "Gleam_Array.h"

NS_GLEAM

class Window
{
public:
	enum MODE { FULLSCREEN = 0, WINDOWED, FULLSCREEN_WINDOWED };

	Window(void);
	~Window(void);

	bool init(const GChar* app_name, MODE window_mode = FULLSCREEN,
					unsigned int width = 0, unsigned int height = 0,
					int pos_x = -1, int pos_y = -1);
	void destroy(void);

	INLINE void handleWindowMessages(void);
	INLINE void addWindowMessageHandler(WindowCallback callback);
	bool removeWindowMessageHandler(WindowCallback callback);
	INLINE void addWindowMessageHandler(IWindowMessageHandler* handler);
	bool removeWindowMessageHandler(IWindowMessageHandler* handler);

	INLINE void showCursor(bool show);
	INLINE void allowRepeats(bool allow);
	void containCursor(bool contain);

	bool setWindowMode(MODE window_mode);
	INLINE MODE getWindowMode(void) const;

	INLINE void getPos(int& x, int& y) const;
	INLINE void getDimensions(unsigned int& width, unsigned int& height) const;
	INLINE int getPosX(void) const;
	INLINE int getPosY(void) const;
	INLINE unsigned int getWidth(void) const;
	INLINE unsigned int getHeight(void) const;
	INLINE bool isFullScreen(void) const;

	INLINE HINSTANCE getHInstance(void) const;
	INLINE HWND getHWnd(void) const;

private:
	int _pos_x, _pos_y;
	unsigned int _width, _height;
	const GChar* _application_name;
	HINSTANCE _hinstance;
	HWND _hwnd;

	MODE _window_mode;
	MSG _msg_temp;
	bool _no_repeats;

	GleamArray(IWindowMessageHandler*) _window_message_handlers;
	GleamArray(WindowCallback) _window_callbacks;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
	static GleamArray(Window*) gWindows;

	unsigned int _mouse_prev_x;
	unsigned int _mouse_prev_y;
	bool _first_mouse;
};

NS_END
