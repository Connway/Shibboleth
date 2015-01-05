/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
#include "Gleam_HashMap.h"
#include "Gleam_Array.h"
#include <Gaff_IncludeWindows.h>
#include <Gaff_Function.h>

NS_GLEAM

class Window : public IWindow
{
public:
	static void handleWindowMessages(void);
	static void clear(void);

	Window(void);
	~Window(void);

	bool init(const GChar* app_name, MODE window_mode = FULLSCREEN,
				unsigned int width = 0, unsigned int height = 0,
				int pos_x = 0, int pos_y = 0,
				const char* compat = nullptr);
	void destroy(void);

	void addWindowMessageHandler(Gaff::FunctionBinder<bool, const AnyMessage&>& callback);
	bool removeWindowMessageHandler(Gaff::FunctionBinder<bool, const AnyMessage&>& callback);

	void showCursor(bool show);
	void containCursor(bool contain);

	bool isCursorVisible(void) const;
	bool isCursorContained(void) const;

	void allowRepeats(bool allow);
	bool areRepeatsAllowed(void) const;

	bool setWindowMode(MODE window_mode);
	MODE getWindowMode(void) const;

	void getPos(int& x, int& y) const;
	void getDimensions(unsigned int& width, unsigned int& height) const;
	int getPosX(void) const;
	int getPosY(void) const;
	unsigned int getWidth(void) const;
	unsigned int getHeight(void) const;
	bool isFullScreen(void) const;

	INLINE HINSTANCE getHInstance(void) const;
	INLINE HWND getHWnd(void) const;

private:
	int _pos_x, _pos_y;
	unsigned int _width, _height;
	const GChar* _application_name;
	HINSTANCE _hinstance;
	HWND _hwnd;

	MODE _window_mode;
	unsigned int _original_width;
	unsigned int _original_height;
	bool _cursor_visible;
	bool _no_repeats;
	bool _contain;

	GleamArray< Gaff::FunctionBinder<bool, const AnyMessage&> > _window_callbacks;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
	static GleamArray<Window*> gWindows;
	static MSG gMsg;

	static GleamHashMap<unsigned short, KeyCode> g_Left_Keys;
	static GleamHashMap<unsigned short, KeyCode> g_Right_Keys;
	static bool g_First_Init;

	GAFF_NO_COPY(Window);
	GAFF_NO_MOVE(Window);
};

NS_END
