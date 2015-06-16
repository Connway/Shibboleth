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

#if defined(_WIN32) || defined(_WIN64)

#include "Gleam_Window_Windows.h"
#include "Gleam_WindowHelpers_Windows.h"
#include "Gleam_String.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

typedef void (*WindowProcHelper)(AnyMessage*, Window*, WPARAM, LPARAM);

static GleamMap<unsigned int, WindowProcHelper> gWindowHelpers;
static GleamArray<Window*> gWindows;
static bool gFirstInit = true;
static MSG gMsg;

GleamMap<unsigned short, KeyCode> Window::gRightKeys;
GleamMap<unsigned short, KeyCode> Window::gLeftKeys;

static void InitWindowProcHelpers(void)
{
	gWindowHelpers.insert(WM_CLOSE, WindowClosed);
	gWindowHelpers.insert(WM_DESTROY, WindowDestroyed);
	gWindowHelpers.insert(WM_MOVE, WindowClosed);
	gWindowHelpers.insert(WM_SIZE, WindowClosed);
	gWindowHelpers.insert(WM_CHAR, WindowClosed);
	gWindowHelpers.insert(WM_INPUT, WindowClosed);
	gWindowHelpers.insert(WM_LBUTTONDOWN, WindowLeftButtonDown);
	gWindowHelpers.insert(WM_RBUTTONDOWN, WindowRightButtonDown);
	gWindowHelpers.insert(WM_MBUTTONDOWN, WindowMiddleButtonDown);
	gWindowHelpers.insert(WM_XBUTTONDOWN, WindowXButtonDown);
	gWindowHelpers.insert(WM_LBUTTONUP, WindowLeftButtonUp);
	gWindowHelpers.insert(WM_RBUTTONUP, WindowRightButtonUp);
	gWindowHelpers.insert(WM_MBUTTONUP, WindowMiddleButtonUp);
	gWindowHelpers.insert(WM_XBUTTONUP, WindowXButtonUp);
	gWindowHelpers.insert(WM_MOUSEWHEEL, WindowMouseWheel);
	gWindowHelpers.insert(WM_SETFOCUS, WindowSetFocus);
	gWindowHelpers.insert(WM_KILLFOCUS, WindowKillFocus);
}

void Window::HandleWindowMessages(void)
{
	// Handle the windows messages.
	while (PeekMessage(&gMsg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&gMsg);
		DispatchMessage(&gMsg);
	}
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	bool handled = false;
	char buffer[sizeof(AnyMessage)];

	auto it = gWindows.linearSearch(hwnd, [](const Window* lhs, const HWND rhs) -> bool
	{
		return lhs->getHWnd() == rhs;
	});

	if (it != gWindows.end())
	{
		Window* window = *it;

		if (!window->_window_callbacks.empty()) {
			// We are assuming doing a map lookup is as fast as the huge switch statement we had before.
			WindowProcHelper helper_func = gWindowHelpers[msg];

			if (helper_func) {
				helper_func(reinterpret_cast<AnyMessage*>(buffer), window, w, l);
				reinterpret_cast<AnyMessage*>(buffer)->base.window = window;

				for (unsigned int j = 0; j < window->_window_callbacks.size(); ++j) {
					handled = handled || window->_window_callbacks[j](*reinterpret_cast<AnyMessage*>(buffer));
				}

				if (handled) {
					return 0;
				}
			}
		}
	}

	return DefWindowProc(hwnd, msg, w, l);
}

Window::Window(void):
	_hinstance(nullptr), _hwnd(nullptr), _window_mode(FULLSCREEN),
	_cursor_visible(true), _no_repeats(false), _contain(false)
{
}

Window::~Window(void)
{
	destroy();
}

bool Window::init(const char* app_name, MODE window_mode,
					unsigned int width, unsigned int height,
					int pos_x, int pos_y, const char*)
{
	assert(app_name);

	if (gFirstInit) {
		gLeftKeys[VK_CONTROL] = KEY_LEFTCONTROL;
		gLeftKeys[VK_MENU] = KEY_LEFTALT;
		gLeftKeys[VK_SHIFT] = KEY_LEFTSHIFT;

		gRightKeys[VK_CONTROL] = KEY_RIGHTCONTROL;
		gRightKeys[VK_MENU] = KEY_RIGHTALT;
		gRightKeys[VK_SHIFT] = KEY_RIGHTSHIFT;

		InitWindowProcHelpers();

		gFirstInit = false;
	}

	WNDCLASSEX wc;

	_hinstance = GetModuleHandle(NULL);

	if (!_hinstance) {
		return false;
	}

	_application_name = app_name;
	_window_mode = window_mode;

#ifdef _UNICODE
	GleamWString an;
	an.convertToUTF16(app_name, strlen(app_name));
#endif

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
#ifdef _UNICODE
	wc.lpszClassName = an.getBuffer();
#else
	wc.lpszClassName = _application_name.getBuffer();
#endif
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc)) {
		return false;
	}

	DWORD flags = 0;

	_original_width = GetSystemMetrics(SM_CXSCREEN);
	_original_height = GetSystemMetrics(SM_CYSCREEN);

	switch (window_mode) {
		case FULLSCREEN_WINDOWED:
			pos_x = pos_y = 0;
			width = _original_width;
			height = _original_height;
			break;

		case FULLSCREEN:
			pos_x = pos_y = 0;

			if (!width || !height) {
				width = _original_width;
				height = _original_height;
			}

			flags = WS_POPUP;
			break;

		case WINDOWED:
			if (!width || !height) {
				width = 800;
				height = 600;
			}

			flags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
			break;
	}

	_pos_x = pos_x;
	_pos_y = pos_y;
	_width = width;
	_height = height;

	if (window_mode == FULLSCREEN) {
		// If full screen set the screen to maximum size of the users desktop and 32-bit.
		DEVMODE dm_screen_settings;
		memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
		dm_screen_settings.dmSize = sizeof(dm_screen_settings);
		dm_screen_settings.dmPelsWidth  = (unsigned long)width;
		dm_screen_settings.dmPelsHeight = (unsigned long)height;
		dm_screen_settings.dmBitsPerPel = 32;
		dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);
	}

	// Adjust window to make the drawable area to be the desired resolution.
	RECT window_rect = { _pos_x, pos_y, _pos_x + _width, _pos_y + _height };

	if (window_mode == WINDOWED) {
		AdjustWindowRectEx(&window_rect, flags, FALSE, WS_EX_APPWINDOW);

		if (_pos_x == 0 && _pos_y == 0) {
			window_rect.right += -window_rect.left;
			window_rect.left = 0;

			window_rect.bottom += -window_rect.top;
			window_rect.top = 0;
		}
	}

	_hwnd = CreateWindowEx(
#ifdef _UNICODE
		WS_EX_APPWINDOW, an.getBuffer(), an.getBuffer(),
#else
		WS_EX_APPWINDOW, _application_name.getBuffer(), _application_name.getBuffer(),
#endif
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | flags,
		window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
		NULL, NULL, _hinstance, NULL
	);

	if (!_hwnd) {
		return false;
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(_hwnd, (window_mode == FULLSCREEN) ? SW_MAXIMIZE : SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);

	ZeroMemory(&gMsg, sizeof(MSG));
	gWindows.push(this);

	return true;
}

void Window::destroy(void)
{
	containCursor(false);
	showCursor(true);

	if (_window_mode == FULLSCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	if (_hwnd) {
		DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}

	if (_hinstance) {
#ifdef _UNICODE
		GleamWString an;
		an.convertToUTF16(_application_name.getBuffer(), _application_name.size());

		UnregisterClass(an.getBuffer(), _hinstance);
#else
		UnregisterClass(_application_name, _hinstance);
#endif
		_application_name.clear();
		_hinstance = nullptr;
	}

	auto it = gWindows.linearSearch(this);
	assert(it != gWindows.end());
	gWindows.fastErase(it);
}

void Window::addWindowMessageHandler(Gaff::FunctionBinder<bool, const AnyMessage&>& callback)
{
	_window_callbacks.push(callback);
}

bool Window::removeWindowMessageHandler(Gaff::FunctionBinder<bool, const AnyMessage&>& callback)
{
	auto it = _window_callbacks.linearSearch(callback);

	if (it != _window_callbacks.end()) {
		_window_callbacks.fastErase(it);
		return true;
	}

	return false;
}

void Window::showCursor(bool show)
{
	_cursor_visible = show;

	if (show) {
		ShowCursor(show);

	} else {
		while (ShowCursor(show) > 0) {
		}
	}
}

void Window::containCursor(bool contain)
{
	_contain = contain;

	if (contain) {
		RECT rect = {
			_pos_x, _pos_y,
			_pos_x + _width, _pos_y + _height
		};

		ClipCursor(&rect);

	} else {
		ClipCursor(nullptr);
	}
}

bool Window::isCursorVisible(void) const
{
	return _cursor_visible;
}

bool Window::isCursorContained(void) const
{
	return _contain;
}

void Window::allowRepeats(bool allow)
{
	_no_repeats = !allow;
}

bool Window::areRepeatsAllowed(void) const
{
	return !_no_repeats;
}

bool Window::setWindowMode(MODE window_mode)
{
	_window_mode = window_mode;
	DWORD flags = 0;

	switch (window_mode) {
		case FULLSCREEN_WINDOWED:
			_pos_x = _pos_y = 0;
			_width = GetSystemMetrics(SM_CXSCREEN);
			_height = GetSystemMetrics(SM_CYSCREEN);
			break;

		case FULLSCREEN:
			_pos_x = _pos_y = 0;
			flags = WS_POPUP;
			break;

		case WINDOWED:
			flags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
			break;
	}

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (window_mode == FULLSCREEN) {
		DEVMODE dm_screen_settings;
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
		dm_screen_settings.dmSize = sizeof(dm_screen_settings);
		dm_screen_settings.dmPelsWidth  = (unsigned long)_width;
		dm_screen_settings.dmPelsHeight = (unsigned long)_height;
		dm_screen_settings.dmBitsPerPel = 32;
		dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		if (ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN) == DISP_CHANGE_BADMODE) {
			// If we try to change to a bad mode, default to desktop resolution
			_width = GetSystemMetrics(SM_CXSCREEN);
			_height = GetSystemMetrics(SM_CYSCREEN);
			dm_screen_settings.dmPelsWidth = (unsigned long)_width;
			dm_screen_settings.dmPelsHeight = (unsigned long)_height;

			// try one last time using desktop resolution
			if (ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
				return false;
			}
		}
	} else {
		if (ChangeDisplaySettings(NULL, 0) != DISP_CHANGE_SUCCESSFUL) {
			return false;
		}
	}

	SetWindowLongPtr(_hwnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | flags);
	return SetWindowPos(_hwnd, HWND_TOP, _pos_x, _pos_y, _width, _height, SWP_SHOWWINDOW | SWP_DRAWFRAME) != FALSE;
}

Window::MODE Window::getWindowMode(void) const
{
	return _window_mode;
}

void Window::getPos(int& x, int& y) const
{
	x = _pos_x;
	y = _pos_y;
}

void Window::getDimensions(unsigned int& width, unsigned int& height) const
{
	width = _width;
	height = _height;
}

int Window::getPosX(void) const
{
	return _pos_x;
}

int Window::getPosY(void) const
{
	return _pos_y;
}

unsigned int Window::getWidth(void) const
{
	return _width;
}

unsigned int Window::getHeight(void) const
{
	return _height;
}

bool Window::isFullScreen(void) const
{
	return _window_mode == FULLSCREEN;
}

bool Window::setIcon(const char* icon)
{
	HANDLE hIcon = LoadImageA(_hinstance, icon, IMAGE_ICON, 64, 64, LR_LOADFROMFILE);

	if (!hIcon) {
		return false;
	}

	SendMessage(_hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
	return true;
}

HINSTANCE Window::getHInstance(void) const
{
	return _hinstance;
}

HWND Window::getHWnd(void) const
{
	return _hwnd;
}

NS_END

#endif