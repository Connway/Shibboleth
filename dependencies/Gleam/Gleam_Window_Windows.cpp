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

#include "Gleam_Window_Windows.h"
#include "Gaff_IncludeAssert.h"

NS_GLEAM

GleamArray(Window*) Window::gWindows;

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	bool handled = false;
	char buffer[sizeof(AnyMessage)];
	LRESULT ret = 0;

	// if we make more than one window in our application
	for (unsigned int i = 0; i < gWindows.size(); ++i) {
		if (gWindows[i]->getHWnd() == hwnd) {
			AnyMessage* message = NULLPTR;
			Window* window = gWindows[i];

			switch (msg) {
				case WM_CLOSE:
					message = (AnyMessage*)buffer;
					message->base.type = WND_CLOSED;
					break;

				case WM_DESTROY:
					message = (AnyMessage*)buffer;
					message->base.type = WND_DESTROYED;
					break;

				case WM_MOVE:
					window->_pos_x = (int)(short)LOWORD(l);
					window->_pos_y = (int)(short)HIWORD(l);

					message = (AnyMessage*)buffer;
					message->base.type = WND_MOVED;
					break;

				case WM_SIZE:
					window->_width = LOWORD(l);
					window->_height = HIWORD(l);

					message = (AnyMessage*)buffer;
					message->base.type = WND_RESIZED;
					break;

				case WM_KEYDOWN:
					if (window->_no_repeats && l & 0x0000000040000000) {
						break;
					}

					message = (AnyMessage*)buffer;
					message->base.type = IN_KEYDOWN;
					message->key_char.key = (KeyCode)w;
					break;

				case WM_KEYUP:
					message = (AnyMessage*)buffer;
					message->base.type = IN_KEYUP;
					message->key_char.key = (KeyCode)w;
					break;

				case WM_CHAR:
					message = (AnyMessage*)buffer;
					message->base.type = IN_CHARACTER;
					message->key_char.character = (unsigned int)w;
					break;

				case WM_MOUSEMOVE:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEMOVE;
					message->mouse_move.x = (int)(short)LOWORD(l);
					message->mouse_move.y = (int)(short)HIWORD(l);

					if (window->_first_mouse) {
						window->_first_mouse = false;
						message->mouse_move.dx = 0;
						message->mouse_move.dy = 0;
					} else {
						message->mouse_move.dx = message->mouse_move.x - window->_mouse_prev_x;
						message->mouse_move.dy = message->mouse_move.y - window->_mouse_prev_y;
					}

					window->_mouse_prev_x = message->mouse_move.x;
					window->_mouse_prev_y = message->mouse_move.y;
					break;

				case WM_LBUTTONDOWN:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEDOWN;
					message->mouse_state.button = MOUSE_LEFT;
					break;
				case WM_RBUTTONDOWN:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEDOWN;
					message->mouse_state.button = MOUSE_RIGHT;
					break;
				case WM_MBUTTONDOWN:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEDOWN;
					message->mouse_state.button = MOUSE_MIDDLE;
					break;
				case WM_XBUTTONDOWN:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEDOWN;
					message->mouse_state.button = (HIWORD(w) == XBUTTON1) ? MOUSE_BACK : MOUSE_FORWARD;
					break;

				case WM_LBUTTONUP:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEUP;
					message->mouse_state.button = MOUSE_LEFT;
					break;
				case WM_RBUTTONUP:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEUP;
					message->mouse_state.button = MOUSE_RIGHT;
					break;
				case WM_MBUTTONUP:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEUP;
					message->mouse_state.button = MOUSE_MIDDLE;
					break;
				case WM_XBUTTONUP:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEUP;
					message->mouse_state.button = (HIWORD(w) == XBUTTON1) ? MOUSE_BACK : MOUSE_FORWARD;
					break;

				case WM_MOUSEWHEEL:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEWHEEL;
					message->mouse_state.wheel = (short)HIWORD(w) / WHEEL_DELTA;
					break;
			}

			if (message) {
				message->base.window = window;

				for (unsigned int j = 0; j < window->_window_callbacks.size(); ++j) {
					handled = handled || window->_window_callbacks[j](*message);
				}

				for (unsigned int j = 0; j < window->_window_message_handlers.size(); ++j) {
					handled = handled || window->_window_message_handlers[j]->handleMessage(*message);
				}
			}
			break;
		}
	}

	if (!handled) {
		return DefWindowProc(hwnd, msg, w, l);
	}

	return ret;
}

Window::Window(void):
	_application_name(NULLPTR), _hinstance(NULLPTR), _hwnd(NULLPTR),
	_window_mode(FULLSCREEN), _no_repeats(false),
	_mouse_prev_x(0), _mouse_prev_y(0),
	_first_mouse(true)
{
}

Window::~Window(void)
{
	destroy();
}

bool Window::init(const GChar* app_name, MODE window_mode,
					unsigned int width, unsigned int height,
					int pos_x, int pos_y)
{
	assert(app_name);

	WNDCLASSEX wc;

	_hinstance = GetModuleHandle(NULL);

	if (!_hinstance) {
		return false;
	}

	_application_name = app_name;
	_window_mode = window_mode;

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
	wc.lpszClassName = _application_name;
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc)) {
		return false;
	}

	DWORD flags = 0;

	switch (window_mode) {
		case FULLSCREEN_WINDOWED:
			pos_x = pos_y = 0;
			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);
			break;

		case FULLSCREEN:
			pos_x = pos_y = 0;

			if (!width || !height) {
				width = GetSystemMetrics(SM_CXSCREEN);
				height = GetSystemMetrics(SM_CYSCREEN);
			}

			flags = WS_POPUP;
			break;

		case WINDOWED:
			if (!width || !height) {
				width = 800;
				height = 600;
			}

			// set the window to be centered on the screen
			if (pos_x < 0 || pos_y < 0) {
				pos_x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
				pos_y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
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
	AdjustWindowRectEx(&window_rect, flags, FALSE, WS_EX_APPWINDOW);

	_hwnd = CreateWindowEx(WS_EX_APPWINDOW, _application_name, _application_name,
							WS_CLIPSIBLINGS | WS_CLIPCHILDREN | flags,
							window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
							NULL, NULL, _hinstance, NULL);

	if (!_hwnd) {
		return false;
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);

	ShowCursor(false);

	ZeroMemory(&_msg_temp, sizeof(MSG));

	gWindows.push(this);

	return true;
}

void Window::destroy(void)
{
	ShowCursor(true);

	if (_window_mode == FULLSCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	if (_hwnd) {
		DestroyWindow(_hwnd);
		_hwnd = NULLPTR;
	}

	if (_hinstance) {
		UnregisterClass(_application_name, _hinstance);
		_application_name = NULLPTR;
		_hinstance = NULLPTR;
	}

	for (unsigned int i = 0; i < gWindows.size(); ++i) {
		if (gWindows[i] == this) {
			gWindows.erase(i);
			break;
		}
	}

	_first_mouse = true;
}

void Window::handleWindowMessages(void)
{
	// Handle the windows messages.
	while (PeekMessage(&_msg_temp, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&_msg_temp);
		DispatchMessage(&_msg_temp);
	}
}

void Window::addWindowMessageHandler(WindowCallback callback)
{
	_window_callbacks.push(callback);
}

bool Window::removeWindowMessageHandler(WindowCallback callback)
{
	for (unsigned int i = 0; i < _window_callbacks.size(); ++i) {
		if (_window_callbacks[i] == callback) {
			_window_callbacks.erase(i);
			return true;
		}
	}

	return false;
}

void Window::addWindowMessageHandler(IWindowMessageHandler* handler)
{
	_window_message_handlers.push(handler);
}

bool Window::removeWindowMessageHandler(IWindowMessageHandler* handler)
{
	for (unsigned int i = 0; i < _window_message_handlers.size(); ++i) {
		if (_window_message_handlers[i] == handler) {
			_window_message_handlers.erase(i);
			return true;
		}
	}

	return false;
}

void Window::showCursor(bool show)
{
	ShowCursor(show);
}

void Window::allowRepeats(bool allow)
{
	_no_repeats = !allow;
}

void Window::containCursor(bool contain)
{
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

HINSTANCE Window::getHInstance(void) const
{
	return _hinstance;
}

HWND Window::getHWnd(void) const
{
	return _hwnd;
}

NS_END
