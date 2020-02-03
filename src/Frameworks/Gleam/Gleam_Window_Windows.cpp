/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include <Gaff_Assert.h>

NS_GLEAM

using WindowProcHelper = void (*)(AnyMessage&, Window*, WPARAM, LPARAM);

static const wchar_t* g_window_class_name = L"Gleam_Window_Class";

static VectorMap<UINT, WindowProcHelper> g_window_helpers;
static Vector<Window*> g_windows;
static bool g_first_init = true;
static MSG g_msg;

static VectorMap<int32_t, MessageHandler> g_global_message_handlers;
static int32_t g_global_next_id = 0;

VectorMap<uint16_t, KeyCode> Window::g_right_keys;
VectorMap<uint16_t, KeyCode> Window::g_left_keys;

static void InitWindowProcHelpers(void)
{
	//WindowMode::MOUSELEAVE
	//WindowMode::GETMINMAXINFO
	//WindowMode::ENTERSIZEMOVE
	//WindowMode::EXITSIZEMOVE
	g_window_helpers.emplace(WM_CLOSE, WindowClosed);
	g_window_helpers.emplace(WM_DESTROY, WindowDestroyed);
	g_window_helpers.emplace(WM_MOVE, WindowMoved);
	g_window_helpers.emplace(WM_SIZE, WindowResized);
	g_window_helpers.emplace(WM_CHAR, WindowCharacter);
	g_window_helpers.emplace(WM_INPUT, WindowInput);
	g_window_helpers.emplace(WM_LBUTTONDOWN, WindowLeftButtonDown);
	g_window_helpers.emplace(WM_RBUTTONDOWN, WindowRightButtonDown);
	g_window_helpers.emplace(WM_MBUTTONDOWN, WindowMiddleButtonDown);
	g_window_helpers.emplace(WM_XBUTTONDOWN, WindowXButtonDown);
	g_window_helpers.emplace(WM_LBUTTONUP, WindowLeftButtonUp);
	g_window_helpers.emplace(WM_RBUTTONUP, WindowRightButtonUp);
	g_window_helpers.emplace(WM_MBUTTONUP, WindowMiddleButtonUp);
	g_window_helpers.emplace(WM_XBUTTONUP, WindowXButtonUp);
	g_window_helpers.emplace(WM_MOUSEWHEEL, WindowMouseWheel);
	g_window_helpers.emplace(WM_SETFOCUS, WindowSetFocus);
	g_window_helpers.emplace(WM_KILLFOCUS, WindowKillFocus);
}

static bool DoFirstInit(
	WNDPROC window_proc,
	HINSTANCE hinstance,
	VectorMap<uint16_t, KeyCode>& left_keys,
	VectorMap<uint16_t, KeyCode>& right_keys)
{
	if (g_first_init) {
		left_keys[VK_CONTROL] = KeyCode::KEY_LEFTCONTROL;
		left_keys[VK_MENU] = KeyCode::KEY_LEFTALT;
		left_keys[VK_SHIFT] = KeyCode::KEY_LEFTSHIFT;

		right_keys[VK_CONTROL] = KeyCode::KEY_RIGHTCONTROL;
		right_keys[VK_MENU] = KeyCode::KEY_RIGHTALT;
		right_keys[VK_SHIFT] = KeyCode::KEY_RIGHTSHIFT;

		WNDCLASSEX wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = window_proc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hinstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName = NULL;
		wc.lpszClassName = g_window_class_name;
		wc.cbSize = sizeof(WNDCLASSEX);

		if (!RegisterClassEx(&wc)) {
			return false;
		}

		InitWindowProcHelpers();

		g_first_init = false;
	}

	return true;
}

static bool RemoveMessageHandler(VectorMap<int32_t, MessageHandler>& handlers, int32_t id)
{
	const auto it = handlers.find(id);

	if (it != handlers.end()) {
		handlers.erase(it);
		return true;
	}

	return false;
}

int32_t Window::AddGlobalMessageHandler(const MessageHandler& callback)
{
	const int32_t id = g_global_next_id++;
	g_global_message_handlers.emplace(id, callback);
	return id;
}

int32_t Window::AddGlobalMessageHandler(MessageHandler&& callback)
{
	const int32_t id = g_global_next_id++;
	g_global_message_handlers.emplace(id, std::move(callback));
	return id;
}

bool Window::RemoveGlobalMessageHandler(int32_t id)
{
	return RemoveMessageHandler(g_global_message_handlers, id);
}

void Window::HandleWindowMessages(void)
{
	// Handle the windows messages.
	while (PeekMessage(&g_msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&g_msg);
		DispatchMessage(&g_msg);
	}
}

void Window::Cleanup(void)
{
	g_window_helpers.clear();
	g_right_keys.clear();
	g_left_keys.clear();
	g_windows.clear();

	g_window_helpers.shrink_to_fit();
	g_right_keys.shrink_to_fit();
	g_left_keys.shrink_to_fit();
	g_windows.shrink_to_fit();
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	bool handled = false;
	AnyMessage message;

	auto it_wnd = Gaff::Find(g_windows, hwnd, [](const Window* lhs, const HWND rhs) -> bool
	{
		return lhs->getHWnd() == rhs;
	});

	if (it_wnd != g_windows.end())
	{
		Window* window = *it_wnd;

		if (!window->_window_callbacks.empty()) {
			// We are assuming doing a map lookup is as fast as the huge switch statement we had before.
			WindowProcHelper helper_func = g_window_helpers[msg];

			if (helper_func) {
				helper_func(message, window, w, l);
				message.base.window = window;

				const int32_t size = static_cast<int32_t>(window->_window_callbacks.size());

				for (int32_t i = 0; i < size; ++i) {
					handled = handled || window->_window_callbacks[i](message);
				}

				for (auto it_hnd = g_global_message_handlers.begin(); it_hnd != g_global_message_handlers.end(); ++it_hnd) {
					handled = handled || it_hnd->second(message);
				}

				if (handled) {
					return 0;
				}
			}
		}
	}

	return DefWindowProc(hwnd, msg, w, l);
}

Window::Window(void)
{
}

Window::~Window(void)
{
	destroy();
}

bool Window::init(HWND hwnd)
{
	_hinstance = GetModuleHandle(NULL);

	if (!_hinstance) {
		return false;
	}

	DoFirstInit(WindowProc, _hinstance, g_left_keys, g_right_keys);

	_window_mode = WindowMode::Windowed;
	_owns_window = false;
	_hwnd = hwnd;

	RECT rect;
	GetWindowRect(_hwnd, &rect);

	_pos.x = rect.left;
	_pos.y = rect.top;

	_size.x = rect.right - rect.left;
	_size.y = rect.bottom - rect.top;

	return true;
}

bool Window::init(const char* window_name, WindowMode window_mode,
					int32_t width, int32_t height,
					int32_t pos_x, int32_t pos_y, const char*)
{
	GAFF_ASSERT(window_name);

	_hinstance = GetModuleHandle(NULL);

	if (!_hinstance) {
		return false;
	}

	DoFirstInit(WindowProc, _hinstance, g_left_keys, g_right_keys);

	_window_name = window_name;
	_window_mode = window_mode;

	CONVERT_STRING(wchar_t, temp, window_name);

	DWORD flags = 0;

	switch (window_mode) {
		case WindowMode::BorderlessWindowed:
		case WindowMode::Fullscreen:
			flags = WS_POPUP;
			break;

		case WindowMode::Windowed:
			flags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
			break;
	}

	_pos.x = pos_x;
	_pos.y = pos_y;
	_size.x = width;
	_size.y = height;

	if (window_mode == WindowMode::Fullscreen) {
		// If full screen set the screen to maximum size of the users desktop and 32-bit.
		DEVMODE dm_screen_settings;
		memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
		dm_screen_settings.dmSize = sizeof(dm_screen_settings);
		dm_screen_settings.dmPelsWidth  = static_cast<DWORD>(width);
		dm_screen_settings.dmPelsHeight = static_cast<DWORD>(height);
		dm_screen_settings.dmBitsPerPel = 32;
		dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);
	}

	// Adjust window to make the drawable area to be the desired resolution.
	RECT window_rect = { _pos.x, _pos.y, _pos.x + _size.x, _pos.y + _size.y };

	if (window_mode == WindowMode::Windowed) {
		AdjustWindowRectEx(&window_rect, flags, FALSE, WS_EX_APPWINDOW);

		if (_pos.x == 0 && _pos.y == 0) {
			window_rect.right += -window_rect.left;
			window_rect.left = 0;

			window_rect.bottom += -window_rect.top;
			window_rect.top = 0;
		}
	}

	_hwnd = CreateWindowEx(
		WS_EX_APPWINDOW, g_window_class_name, temp,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | flags,
		window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
		NULL, NULL, _hinstance, NULL
	);

	if (!_hwnd) {
		return false;
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(_hwnd, (window_mode == WindowMode::Fullscreen) ? SW_MAXIMIZE : SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);

	ZeroMemory(&g_msg, sizeof(MSG));
	g_windows.emplace_back(this);

	return true;
}

void Window::destroy(void)
{
	if (!_owns_window) {
		_hwnd = nullptr;
		return;
	}

	auto it = Gaff::Find(g_windows, this);
	GAFF_ASSERT(it != g_windows.end());
	g_windows.erase_unsorted(it);

	containCursor(false);
	showCursor(true);

	if (_window_mode == WindowMode::Fullscreen) {
		ChangeDisplaySettings(NULL, 0);
	}

	if (_hwnd) {
		DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}

	if (_hinstance) {
		const char8_t* window_name = _window_name.data();
		CONVERT_STRING(wchar_t, temp, window_name);
		UnregisterClass(temp, _hinstance);

		_window_name.clear();
		_hinstance = nullptr;
	}
}

int32_t Window::addWindowMessageHandler(const MessageHandler& callback)
{
	const int32_t id = g_global_next_id++;
	_window_callbacks.emplace(id, callback);
	return id;
}

int32_t Window::addWindowMessageHandler(MessageHandler&& callback)
{
	const int32_t id = g_global_next_id++;
	_window_callbacks.emplace(id, std::move(callback));
	return id;
}

bool Window::removeWindowMessageHandler(int32_t id)
{
	return RemoveMessageHandler(_window_callbacks, id);
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
			_pos.x,
			_pos.y,
			_pos.x + _size.x,
			_pos.y + _size.y
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

bool Window::setWindowMode(WindowMode window_mode)
{
	_window_mode = window_mode;
	DWORD flags = 0;

	switch (window_mode) {
		case WindowMode::BorderlessWindowed:
			_pos.x = _pos.y = 0;
			_size.x = GetSystemMetrics(SM_CXSCREEN);
			_size.y = GetSystemMetrics(SM_CYSCREEN);
			break;

		case WindowMode::Fullscreen:
			_pos.x = _pos.y = 0;
			flags = WS_POPUP;
			break;

		case WindowMode::Windowed:
			flags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
			break;
	}

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (window_mode == WindowMode::Fullscreen) {
		DEVMODE dm_screen_settings;
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
		dm_screen_settings.dmSize = sizeof(dm_screen_settings);
		dm_screen_settings.dmPelsWidth  = static_cast<DWORD>(_size.x);
		dm_screen_settings.dmPelsHeight = static_cast<DWORD>(_size.y);
		dm_screen_settings.dmBitsPerPel = 32;
		dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		if (ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN) == DISP_CHANGE_BADMODE) {
			// If we try to change to a bad mode, default to desktop resolution
			_size.x = GetSystemMetrics(SM_CXSCREEN);
			_size.y = GetSystemMetrics(SM_CYSCREEN);
			dm_screen_settings.dmPelsWidth = static_cast<DWORD>(_size.x);
			dm_screen_settings.dmPelsHeight = static_cast<DWORD>(_size.y);

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
	return SetWindowPos(_hwnd, HWND_TOP, _pos.x, _pos.y, _size.x, _size.y, SWP_SHOWWINDOW | SWP_DRAWFRAME) != FALSE;
}

Window::WindowMode Window::getWindowMode(void) const
{
	return _window_mode;
}

const glm::ivec2& Window::getPos(void) const
{
	return _pos;
}

const glm::ivec2& Window::getSize(void) const
{
	return _size;
}

void Window::setPos(const glm::ivec2& pos)
{
	_pos = pos;

	if (_owns_window) {
		MoveWindow(_hwnd, _pos.x, _pos.y, _size.x, _size.y, false);
	}
}

void Window::setSize(const glm::ivec2& size)
{
	_size = size;

	if (_owns_window) {
		MoveWindow(_hwnd, _pos.x, _pos.y, _size.x, _size.y, false);
	}
}

bool Window::isFullScreen(void) const
{
	return _window_mode == WindowMode::Fullscreen;
}

bool Window::setIcon(const char* icon)
{
	CONVERT_STRING(wchar_t, temp, icon);
	HANDLE hIcon = LoadImageW(_hinstance, temp, IMAGE_ICON, 64, 64, LR_LOADFROMFILE);

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