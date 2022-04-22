/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Gleam_Window.h"
#include <Gaff_Assert.h>
#include <GLFW/glfw3.h>

namespace
{
	static void OnError(int error, const char* description)
	{
		// $TODO: Log error.
	}

	static int32_t g_next_id = 0;

	template <class T>
	int32_t AddCallback(Gleam::VectorMap<int32_t, T>& callbacks, const T& cb)
	{
		const int32_t id = g_next_id++;
		_size_callbacks.emplace(id, cb);
		return id;
	}

	template <class T>
	int32_t AddCallback(Gleam::VectorMap<int32_t, T>& callbacks, T&& cb)
	{
		const int32_t id = g_next_id++;
		_size_callbacks.emplace(id, std::move(cb));
		return id;
	}
}

NS_GLEAM

bool Window::GlobalInit(void)
{
	glfwSetErrorCallback(OnError);
	
	if (!glfwInit()) {
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void Window::GlobalShutdown(void)
{
	glfwTerminate();
}

void Window::WaitEvents(double timeout_seconds)
{
	if (timeout_seconds < 0.0f) {
		glfwWaitEventsTimeout(timeout_seconds);
	} else {
		glfwWaitEvents();
	}
}

void Window::PollEvents(void)
{
	glfwPollEvents();
}

void Window::PostEmptyEvent(void)
{
	glfwPostEmptyEvent();
}

//using WindowProcHelper = void (*)(AnyMessage&, Window*, WPARAM, LPARAM);
//
//static const wchar_t* g_window_class_name = L"Gleam_Window_Class";
//
//static VectorMap<UINT, WindowProcHelper> g_window_helpers;
//static Vector<Window*> g_windows;
//static bool g_first_init = true;
//static MSG g_msg;
//
//static VectorMap<int32_t, MessageHandler> g_global_message_handlers;
//static int32_t g_global_next_id = 0;
//
//VectorMap<uint16_t, KeyCode> Window::g_right_keys;
//VectorMap<uint16_t, KeyCode> Window::g_left_keys;
//
//void SetCursor(CursorType cursor)
//{
//	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(static_cast<int32_t>(cursor))));
//}
//
////CursorType GetCursor(void)
////{
////	//const HCURSOR cursor = ::GetCursor();
////
////	//if (cursor == IDC_ARROW) {
////	//	return Cursor::Arrow;
////	//} else {
////	//}
////
////	return Cursor::None;
////}
//
//static void InitWindowProcHelpers(void)
//{
//	//WindowMode::MOUSELEAVE
//	//WindowMode::GETMINMAXINFO
//	//WindowMode::ENTERSIZEMOVE
//	//WindowMode::EXITSIZEMOVE
//	g_window_helpers.emplace(WM_CLOSE, WindowClosed);
//	g_window_helpers.emplace(WM_DESTROY, WindowDestroyed);
//	g_window_helpers.emplace(WM_MOVE, WindowMoved);
//	g_window_helpers.emplace(WM_SIZE, WindowResized);
//	g_window_helpers.emplace(WM_CHAR, WindowCharacter);
//	g_window_helpers.emplace(WM_INPUT, WindowInput);
//	g_window_helpers.emplace(WM_LBUTTONDOWN, WindowLeftButtonDown);
//	g_window_helpers.emplace(WM_RBUTTONDOWN, WindowRightButtonDown);
//	g_window_helpers.emplace(WM_MBUTTONDOWN, WindowMiddleButtonDown);
//	g_window_helpers.emplace(WM_XBUTTONDOWN, WindowXButtonDown);
//	g_window_helpers.emplace(WM_LBUTTONUP, WindowLeftButtonUp);
//	g_window_helpers.emplace(WM_RBUTTONUP, WindowRightButtonUp);
//	g_window_helpers.emplace(WM_MBUTTONUP, WindowMiddleButtonUp);
//	g_window_helpers.emplace(WM_XBUTTONUP, WindowXButtonUp);
//	g_window_helpers.emplace(WM_MOUSEHWHEEL, WindowMouseWheelHorizontal);
//	g_window_helpers.emplace(WM_MOUSEWHEEL, WindowMouseWheel);
//	g_window_helpers.emplace(WM_SETFOCUS, WindowSetFocus);
//	//g_window_helpers.emplace(WM_KILLFOCUS, WindowKillFocus);
//}
//
//static bool DoFirstInit(
//	WNDPROC window_proc,
//	HINSTANCE hinstance,
//	VectorMap<uint16_t, KeyCode>& left_keys,
//	VectorMap<uint16_t, KeyCode>& right_keys)
//{
//	if (g_first_init) {
//		left_keys[VK_CONTROL] = KeyCode::LeftControl;
//		left_keys[VK_MENU] = KeyCode::LeftAlt;
//		left_keys[VK_SHIFT] = KeyCode::LeftShift;
//
//		right_keys[VK_CONTROL] = KeyCode::RightControl;
//		right_keys[VK_MENU] = KeyCode::RightAlt;
//		right_keys[VK_SHIFT] = KeyCode::RightShift;
//
//		WNDCLASSEX wc;
//		wc.style = CS_HREDRAW | CS_VREDRAW;
//		wc.lpfnWndProc = window_proc;
//		wc.cbClsExtra = 0;
//		wc.cbWndExtra = 0;
//		wc.hInstance = hinstance;
//		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
//		wc.hIconSm = wc.hIcon;
//		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//		wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
//		wc.lpszMenuName = NULL;
//		wc.lpszClassName = g_window_class_name;
//		wc.cbSize = sizeof(WNDCLASSEX);
//
//		if (!RegisterClassEx(&wc)) {
//			return false;
//		}
//
//		InitWindowProcHelpers();
//
//		g_first_init = false;
//	}
//
//	return true;
//}
//
//static bool RemoveMessageHandler(VectorMap<int32_t, MessageHandler>& handlers, int32_t id)
//{
//	const auto it = handlers.find(id);
//
//	if (it != handlers.end()) {
//		handlers.erase(it);
//		return true;
//	}
//
//	return false;
//}
//
//int32_t Window::AddGlobalMessageHandler(const MessageHandler& callback)
//{
//	const int32_t id = g_global_next_id++;
//	g_global_message_handlers.emplace(id, callback);
//	return id;
//}
//
//int32_t Window::AddGlobalMessageHandler(MessageHandler&& callback)
//{
//	const int32_t id = g_global_next_id++;
//	g_global_message_handlers.emplace(id, std::move(callback));
//	return id;
//}
//
//bool Window::RemoveGlobalMessageHandler(int32_t id)
//{
//	return RemoveMessageHandler(g_global_message_handlers, id);
//}
//
//void Window::HandleWindowMessages(void)
//{
//	// Handle the windows messages.
//	while (PeekMessage(&g_msg, NULL, 0, 0, PM_REMOVE)) {
//		TranslateMessage(&g_msg);
//		DispatchMessage(&g_msg);
//	}
//}
//
//void Window::Cleanup(void)
//{
//	g_window_helpers.clear();
//	g_right_keys.clear();
//	g_left_keys.clear();
//	g_windows.clear();
//
//	g_window_helpers.shrink_to_fit();
//	g_right_keys.shrink_to_fit();
//	g_left_keys.shrink_to_fit();
//	g_windows.shrink_to_fit();
//}
//
//LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
//{
//	bool handled = false;
//	AnyMessage message;
//
//	auto it_wnd = Gaff::Find(g_windows, hwnd, [](const Window* lhs, const HWND rhs) -> bool
//	{
//		return lhs->getHWnd() == rhs;
//	});
//
//	if (it_wnd != g_windows.end())
//	{
//		Window* const window = *it_wnd;
//
//		if (!window->_window_callbacks.empty() || !g_global_message_handlers.empty()) {
//			// We are assuming doing a map lookup is as fast as the huge switch statement we had before.
//			WindowProcHelper helper_func = g_window_helpers[msg];
//
//			if (helper_func) {
//				helper_func(message, window, w, l);
//				message.base.window = window;
//
//				for (const auto& handler : window->_window_callbacks) {
//					handled = handler.second(message) || handled;
//				}
//
//				for (const auto& handler : g_global_message_handlers) {
//					handled = handler.second(message) || handled;
//				}
//
//				if (handled) {
//					return 0;
//				}
//			}
//		}
//	}
//
//	return DefWindowProc(hwnd, msg, w, l);
//}

Window::~Window(void)
{
	destroy();
}

bool Window::initFullscreen(
	const char8_t* window_name,
	GLFWmonitor& monitor,
	const GLFWvidmode& video_mode)
{
	glfwWindowHint(GLFW_RED_BITS, video_mode.redBits);
	glfwWindowHint(GLFW_GREEN_BITS, video_mode.greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, video_mode.blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, video_mode.refreshRate);

	_window = glfwCreateWindow(
		video_mode.width,
		video_mode.height,
		reinterpret_cast<const char*>(window_name),
		&monitor,
		nullptr);

	if (_window != nullptr) {
		_fullscreen = true;
		return true;
	}

	return false;
}

bool Window::initFullscreen(
	const char8_t* window_name,
	int32_t display_id,
	int32_t video_mode_id)
{
	const GLFWvidmode* video_mode = nullptr;
	GLFWmonitor* monitor = nullptr;

	if (display_id > -1) {
		int count = 0;
		GLFWmonitor* const* const monitors = glfwGetMonitors(&count);

		if (count <= display_id) {
			// $TODO: Log error.
			return false;
		}

		monitor = monitors[display_id];
	}

	if (video_mode_id > -1) {
		int count = 0;
		video_mode = glfwGetVideoModes(monitor, &count);

		if (count <= video_mode_id) {
			// $TODO: Log error.
			return false;
		}

		video_mode += video_mode_id;

	} else {
		video_mode = glfwGetVideoMode(monitor);
	}

	return initFullscreen(window_name, *monitor, *video_mode);
}

bool Window::initFullscreen(
	const char8_t* window_name,
	GLFWmonitor& monitor,
	const IVec2& size,
	int32_t refresh_rate)
{
	const GLFWvidmode* const video_mode = glfwGetVideoMode(&monitor);

	glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate);

	_window = glfwCreateWindow(
		size.x,
		size.y,
		reinterpret_cast<const char*>(window_name),
		&monitor,
		nullptr);

	if (_window != nullptr) {
		_fullscreen = true;
		return true;
	}

	return false;
}

bool Window::initWindowed(
	const char8_t* window_name,
	const IVec2& size)
{
	_window = glfwCreateWindow(
		size.x,
		size.y,
		reinterpret_cast<const char*>(window_name),
		nullptr,
		nullptr);

	return _window != nullptr;
}

void Window::destroy(void)
{
	if (_window) {
		glfwDestroyWindow(_window);
		_window = nullptr;
	}
}

GLFWwindow* Window::getGLFWWindow(void) const
{
	return _window;
}

void Window::setFullscreen(const GLFWvidmode& video_mode, GLFWmonitor* monitor)
{
	GAFF_ASSERT(_window);

	if (!monitor) {
		monitor = glfwGetWindowMonitor(_window);
	}

	GAFF_ASSERT(monitor);

	glfwSetWindowMonitor(_window, monitor, 0, 0, video_mode.width, video_mode.height, video_mode.refreshRate);
	_fullscreen = true;
}

void Window::setFullscreen(const IVec2& size, int32_t refresh_rate, GLFWmonitor* monitor)
{
	GAFF_ASSERT(_window);

	if (!monitor) {
		monitor = glfwGetWindowMonitor(_window);
	}

	GAFF_ASSERT(monitor);

	glfwSetWindowMonitor(_window, monitor, 0, 0, size.x, size.y, refresh_rate);
	_fullscreen = true;
}

void Window::setWindowed(const IVec2& size, const IVec2& pos)
{
	GAFF_ASSERT(_window);
	glfwSetWindowMonitor(_window, nullptr, pos.x, pos.y, size.x, size.y, GLFW_DONT_CARE);
	_fullscreen = false;
}

bool Window::isFullscreen(void) const
{
	return _fullscreen;
}

bool Window::isWindowed(void) const
{
	return !_fullscreen;
}

void Window::setSize(const IVec2& size)
{
	GAFF_ASSERT(_window);
	glfwSetWindowSize(_window, size.x, size.y);
}

IVec2 Window::getSize(void) const
{
	GAFF_ASSERT(_window);

	IVec2 size;
	glfwGetWindowSize(_window, &size.x, &size.y);

	return size;
}

void Window::setPos(const IVec2& pos)
{
	GAFF_ASSERT(_window);
	glfwSetWindowPos(_window, pos.x, pos.y);
}

IVec2 Window::getPos(void) const
{
	GAFF_ASSERT(_window);

	IVec2 pos;
	glfwGetWindowPos(_window, &pos.x, &pos.y);

	return pos;
}

void Window::setTitle(const char8_t* title)
{
	GAFF_ASSERT(_window);
	glfwSetWindowTitle(_window, reinterpret_cast<const char*>(title));
}

void Window::setIcon(const GLFWimage* icons, int32_t count)
{
	GAFF_ASSERT(_window);
	GAFF_ASSERT(count == 0 || (icons && count >= 0))
	glfwSetWindowIcon(_window, count, icons);
}

void Window::setVisible(bool visible)
{
	GAFF_ASSERT(_window);

	if (visible) {
		glfwShowWindow(_window);
	} else {
		glfwHideWindow(_window);
	}
}

bool Window::isVisible(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetWindowAttrib(_window, GLFW_VISIBLE) == GLFW_TRUE;
}

bool Window::isFocused(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetWindowAttrib(_window, GLFW_FOCUSED) == GLFW_TRUE;
}

void Window::focus(void)
{
	GAFF_ASSERT(_window);
	glfwFocusWindow(_window);
}

void Window::notify(void)
{
	GAFF_ASSERT(_window);
	glfwRequestWindowAttention(_window);
}

void Window::setOpacity(float opacity)
{
	GAFF_ASSERT(_window);
	glfwSetWindowOpacity(_window, opacity);
}

float Window::getOpacity(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetWindowOpacity(_window);
}

bool Window::isMaximized(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetWindowAttrib(_window, GLFW_MAXIMIZED) == GLFW_TRUE;
}

void Window::toggleMaximize(void)
{
	GAFF_ASSERT(_window);

	if (isMaximized()) {
		restore();
	} else {
		maximize();
	}
}

void Window::maximize(void)
{
	GAFF_ASSERT(_window);
	glfwMaximizeWindow(_window);
}

void Window::restore(void)
{
	GAFF_ASSERT(_window);
	glfwRestoreWindow(_window);
}

void Window::setAlwaysOnTop(bool always_on_top)
{
	GAFF_ASSERT(_window);
	glfwSetWindowAttrib(_window, GLFW_FLOATING, (always_on_top) ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::isAlwaysOnTop(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetWindowAttrib(_window, GLFW_FLOATING) == GLFW_TRUE;
}

void Window::setResizable(bool resizable)
{
	GAFF_ASSERT(_window);
	glfwSetWindowAttrib(_window, GLFW_RESIZABLE, (resizable) ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::isResizable(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetWindowAttrib(_window, GLFW_RESIZABLE) == GLFW_TRUE;
}

void Window::setHasDecorations(bool has_decorations)
{
	GAFF_ASSERT(_window);
	glfwSetWindowAttrib(_window, GLFW_DECORATED, (has_decorations) ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::hasDecorations(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetWindowAttrib(_window, GLFW_DECORATED) == GLFW_TRUE;
}

void* Window::getUserPointer(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetWindowUserPointer(_window);
}

void Window::setUserPointer(void* ptr)
{
	GAFF_ASSERT(_window);
	glfwSetWindowUserPointer(_window, ptr);
}

bool Window::shouldClose(void) const
{
	GAFF_ASSERT(_window);
	return glfwWindowShouldClose(_window);
}

void Window::forceClose(void)
{
	GAFF_ASSERT(_window);
	return glfwSetWindowShouldClose(_window, GLFW_TRUE);
}

int32_t Window::addSizeChangeCallback(const VecCallback& callback)
{
	return AddCallback(_size_callbacks, callback);
}

int32_t Window::addSizeChangeCallback(VecCallback&& callback)
{
	return AddCallback(_size_callbacks, std::move(callback));
}

int32_t Window::addPosChangeCallback(const VecCallback& callback)
{
	return AddCallback(_pos_callbacks, callback);
}

int32_t Window::addPosChangeCallback(VecCallback&& callback)
{
	return AddCallback(_pos_callbacks, std::move(callback));
}

int32_t Window::addCloseCallback(const WindowCallback& callback)
{
	return AddCallback(_close_callbacks, callback);
}

int32_t Window::addCloseCallback(WindowCallback&& callback)
{
	return AddCallback(_close_callbacks, std::move(callback));
}

int32_t Window::addMaximizeCallback(const BoolCallback& callback)
{
	return AddCallback(_maximize_callbacks, callback);
}

int32_t Window::addMaximizeCallback(BoolCallback&& callback)
{
	return AddCallback(_maximize_callbacks, std::move(callback));
}

int32_t Window::addFocusCallback(const BoolCallback& callback)
{
	return AddCallback(_focus_callbacks, callback);
}

int32_t Window::addFocusCallback(BoolCallback&& callback)
{
	return AddCallback(_focus_callbacks, std::move(callback));
}

//int32_t Window::addWindowMessageHandler(const MessageHandler& callback)
//{
//	const int32_t id = g_global_next_id++;
//	_window_callbacks.emplace(id, callback);
//	return id;
//}
//
//int32_t Window::addWindowMessageHandler(MessageHandler&& callback)
//{
//	const int32_t id = g_global_next_id++;
//	_window_callbacks.emplace(id, std::move(callback));
//	return id;
//}
//
//bool Window::removeWindowMessageHandler(int32_t id)
//{
//	return RemoveMessageHandler(_window_callbacks, id);
//}
//
//void Window::showCursor(bool show)
//{
//	_cursor_visible = show;
//
//	if (show) {
//		ShowCursor(show);
//
//	} else {
//		while (ShowCursor(show) > 0) {
//		}
//	}
//}
//
//void Window::containCursor(bool contain)
//{
//	_contain = contain;
//
//	if (contain) {
//		RECT rect = {
//			_pos.x,
//			_pos.y,
//			_pos.x + _size.x,
//			_pos.y + _size.y
//		};
//
//		ClipCursor(&rect);
//
//	} else {
//		ClipCursor(nullptr);
//	}
//}
//
//bool Window::isCursorVisible(void) const
//{
//	return _cursor_visible;
//}
//
//bool Window::isCursorContained(void) const
//{
//	return _contain;
//}
//
//bool Window::setWindowMode(WindowMode window_mode)
//{
//	if (_window_mode == window_mode) {
//		return true;
//	}
//
//
//	_window_mode = window_mode;
//	DWORD flags = 0;
//
//	switch (window_mode) {
//		case WindowMode::BorderlessWindowed:
//			_pos.x = _pos.y = 0;
//			_size.x = GetSystemMetrics(SM_CXSCREEN);
//			_size.y = GetSystemMetrics(SM_CYSCREEN);
//			break;
//
//		case WindowMode::Fullscreen:
//			_pos.x = _pos.y = 0;
//			flags = WS_POPUP;
//			break;
//
//		case WindowMode::Windowed:
//			flags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
//			break;
//	}
//
//	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
//	if (window_mode == WindowMode::Fullscreen) {
//		DEVMODE dm_screen_settings;
//		// If full screen set the screen to maximum size of the users desktop and 32bit.
//		memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
//		dm_screen_settings.dmSize = sizeof(dm_screen_settings);
//		dm_screen_settings.dmPelsWidth  = static_cast<DWORD>(_size.x);
//		dm_screen_settings.dmPelsHeight = static_cast<DWORD>(_size.y);
//		dm_screen_settings.dmBitsPerPel = 32;
//		dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
//
//		// Change the display settings to full screen.
//		if (ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN) == DISP_CHANGE_BADMODE) {
//			// If we try to change to a bad mode, default to desktop resolution
//			_size.x = GetSystemMetrics(SM_CXSCREEN);
//			_size.y = GetSystemMetrics(SM_CYSCREEN);
//			dm_screen_settings.dmPelsWidth = static_cast<DWORD>(_size.x);
//			dm_screen_settings.dmPelsHeight = static_cast<DWORD>(_size.y);
//
//			// try one last time using desktop resolution
//			if (ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
//				return false;
//			}
//		}
//	} else {
//		if (ChangeDisplaySettings(NULL, 0) != DISP_CHANGE_SUCCESSFUL) {
//			return false;
//		}
//	}
//
//	SetWindowLongPtr(_hwnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | flags);
//	return SetWindowPos(_hwnd, HWND_TOP, _pos.x, _pos.y, _size.x, _size.y, SWP_SHOWWINDOW | SWP_DRAWFRAME) != FALSE;
//}
//
//Window::WindowMode Window::getWindowMode(void) const
//{
//	return _window_mode;
//}
//
//const IVec2& Window::getPos(void) const
//{
//	return _pos;
//}
//
//const IVec2& Window::getSize(void) const
//{
//	return _size;
//}
//
//void Window::setPos(const IVec2& pos)
//{
//	_pos = pos;
//
//	if (_owns_window) {
//		MoveWindow(_hwnd, _pos.x, _pos.y, _size.x, _size.y, false);
//	}
//}
//
//void Window::setSize(const IVec2& size)
//{
//	_size = size;
//
//	if (_owns_window) {
//		MoveWindow(_hwnd, _pos.x, _pos.y, _size.x, _size.y, false);
//	}
//}
//
//bool Window::isFullScreen(void) const
//{
//	return _window_mode == WindowMode::Fullscreen;
//}
//
//bool Window::setIcon(const char8_t* icon)
//{
//	CONVERT_STRING(wchar_t, temp, icon);
//	HANDLE hIcon = LoadImageW(_hinstance, temp, IMAGE_ICON, 64, 64, LR_LOADFROMFILE);
//
//	if (!hIcon) {
//		return false;
//	}
//
//	SendMessage(_hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
//	return true;
//}
//
//bool Window::setIcon(const char* icon)
//{
//	CONVERT_STRING(wchar_t, temp, icon);
//	HANDLE hIcon = LoadImageW(_hinstance, temp, IMAGE_ICON, 64, 64, LR_LOADFROMFILE);
//
//	if (!hIcon) {
//		return false;
//	}
//
//	SendMessage(_hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
//	return true;
//}
//
//void* Window::getPlatformHandle(void) const
//{
//	return _hwnd;
//}
//
//HINSTANCE Window::getHInstance(void) const
//{
//	return _hinstance;
//}
//
//HWND Window::getHWnd(void) const
//{
//	return _hwnd;
//}

NS_END
