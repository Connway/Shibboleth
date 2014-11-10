/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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
#include "Gaff_IncludeAssert.h"

NS_GLEAM

GleamHashMap<unsigned short, KeyCode> Window::g_Left_Keys;
GleamHashMap<unsigned short, KeyCode> Window::g_Right_Keys;
bool Window::g_First_Init = true;
MSG Window::gMsg;

GleamArray<Window*> Window::gWindows;

void Window::handleWindowMessages(void)
{
	// Handle the windows messages.
	while (PeekMessage(&gMsg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&gMsg);
		DispatchMessage(&gMsg);
	}
}

void Window::clear(void)
{
	g_Left_Keys.clear();
	g_Right_Keys.clear();

	assert(gWindows.empty());
	gWindows.clear();
}


LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	bool handled = false;
	char buffer[sizeof(AnyMessage)];
	LRESULT ret = 0;

	// if we make more than one window in our application
	for (unsigned int i = 0; i < gWindows.size(); ++i) {
		if (gWindows[i]->getHWnd() == hwnd) {
			AnyMessage* message = nullptr;
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

				case WM_CHAR:
					message = (AnyMessage*)buffer;
					message->base.type = IN_CHARACTER;
					message->key_char.character = (unsigned int)w;
					break;

				case WM_INPUT: {
					UINT dwSize = 64;
					BYTE lpb[64];

#ifdef _DEBUG
					GetRawInputData((HRAWINPUT)l, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
					assert(dwSize <= 64);
#endif

					GetRawInputData((HRAWINPUT)l, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
					RAWINPUT* raw = (RAWINPUT*)lpb;

					if (raw->header.dwType == RIM_TYPEMOUSE && raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {
						message = (AnyMessage*)buffer;
						message->base.type = IN_MOUSEMOVE;

						POINT pos;
						if (GetCursorPos(&pos)) {
							message->mouse_move.x = pos.x;
							message->mouse_move.y = pos.y;
						} else {
							message->mouse_move.x += raw->data.mouse.lLastX;
							message->mouse_move.y += raw->data.mouse.lLastY;
						}

						message->mouse_move.dx = raw->data.mouse.lLastX;
						message->mouse_move.dy = raw->data.mouse.lLastY;

					} else if (raw->header.dwType == RIM_TYPEKEYBOARD) {
						message = (AnyMessage*)buffer;
						message->base.type = (raw->data.keyboard.Flags & RI_KEY_BREAK) ? IN_KEYUP : IN_KEYDOWN;

						switch (raw->data.keyboard.VKey) {
							case VK_CONTROL:
							case VK_SHIFT:
								// For some reason, right shift isn't getting the RI_KEY_E0 flag set.
								// Special case it so that we send the correct key.
								if (raw->data.keyboard.MakeCode == 54) {
									message->key_char.key = KEY_RIGHTSHIFT;
									break;
								}

							case VK_MENU:
								if (raw->data.keyboard.Flags & RI_KEY_E0) {
									message->key_char.key = g_Right_Keys[raw->data.keyboard.VKey];
								} else {
									message->key_char.key = g_Left_Keys[raw->data.keyboard.VKey];
								}
								break;

							default:
								message->key_char.key = (KeyCode)raw->data.keyboard.VKey;
								break;
						}
					}
				} break;

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

				case WM_SETFOCUS:
					if (window->_window_mode == FULLSCREEN) {
						// If full screen set the screen to maximum size of the users desktop and 32-bit.
						DEVMODE dm_screen_settings;
						memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
						dm_screen_settings.dmSize = sizeof(dm_screen_settings);
						dm_screen_settings.dmPelsWidth  = window->_width;
						dm_screen_settings.dmPelsHeight = window->_height;
						dm_screen_settings.dmBitsPerPel = 32;
						dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

						ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);
					}

					message = (AnyMessage*)buffer;
					message->base.type = WND_GAINEDFOCUS;
					break;

				case WM_KILLFOCUS:
					if (window->_window_mode == FULLSCREEN) {
						DEVMODE dm_screen_settings;
						memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
						dm_screen_settings.dmSize = sizeof(dm_screen_settings);
						dm_screen_settings.dmPelsWidth  = window->_original_width;
						dm_screen_settings.dmPelsHeight = window->_original_height;
						dm_screen_settings.dmBitsPerPel = 32;
						dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

						ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);
					}

					message = (AnyMessage*)buffer;
					message->base.type = WND_LOSTFOCUS;
					break;
			}

			if (message) {
				message->base.window = window;

				for (unsigned int j = 0; j < window->_window_callbacks.size(); ++j) {
					handled = handled || window->_window_callbacks[j](*message);
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
	_application_name(nullptr), _hinstance(nullptr), _hwnd(nullptr),
	_window_mode(FULLSCREEN), _cursor_visible(true), _no_repeats(false),
	_contain(false)
{
}

Window::~Window(void)
{
	destroy();
}

bool Window::init(const GChar* app_name, MODE window_mode,
					unsigned int width, unsigned int height,
					int pos_x, int pos_y, const char*)
{
	assert(app_name);

	if (g_First_Init) {
		g_Left_Keys[VK_CONTROL] = KEY_LEFTCONTROL;
		g_Left_Keys[VK_MENU] = KEY_LEFTALT;
		g_Left_Keys[VK_SHIFT] = KEY_LEFTSHIFT;

		g_Right_Keys[VK_CONTROL] = KEY_RIGHTCONTROL;
		g_Right_Keys[VK_MENU] = KEY_RIGHTALT;
		g_Right_Keys[VK_SHIFT] = KEY_RIGHTSHIFT;

		g_First_Init = false;
	}

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

	_original_width = GetSystemMetrics(SM_CXSCREEN);
	_original_height = GetSystemMetrics(SM_CYSCREEN);

	switch (window_mode) {
		case FULLSCREEN_WINDOWED:
			//pos_x = pos_y = 0;
			width = _original_width;
			height = _original_height;
			break;

		case FULLSCREEN:
			//pos_x = pos_y = 0;

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
		WS_EX_APPWINDOW, _application_name, _application_name,
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
		UnregisterClass(_application_name, _hinstance);
		_application_name = nullptr;
		_hinstance = nullptr;
	}

	for (unsigned int i = 0; i < gWindows.size(); ++i) {
		if (gWindows[i] == this) {
			gWindows.fastErase(i);
			break;
		}
	}
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