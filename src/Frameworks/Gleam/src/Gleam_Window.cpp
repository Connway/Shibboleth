/************************************************************************************
Copyright (C) by Nicholas LaCroix

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
#include "Gleam_IncludeGLFWNative.h"
#include <Gaff_Assert.h>
#include <Gaff_Utils.h>

namespace
{
	static Gleam::ProxyAllocator s_allocator;

	static void* Allocate(size_t size, void* /*user*/)
	{
		return GAFF_ALLOC(size, s_allocator);
	}

	static void* Reallocate(void* block, size_t size, void* /*user*/)
	{
		return GAFF_REALLOC(block, size, s_allocator);
	}

	static void Deallocate(void* block, void* /*user*/)
	{
		GAFF_FREE(block, s_allocator);
	}

	static void OnError(int error, const char* description)
	{
		GAFF_REF(error, description);
		// $TODO: Log error.
	}

	static int32_t g_next_id = 0;

	template <class T>
	static int32_t AddCallback(int32_t id, Gleam::VectorMap<int32_t, T>& callbacks, const T& cb)
	{
		callbacks.emplace(id, cb);
		return id;
	}

	template <class T>
	static int32_t AddCallback(int32_t id, Gleam::VectorMap<int32_t, T>& callbacks, T&& cb)
	{
		callbacks.emplace(id, std::move(cb));
		return id;
	}

	template <class T>
	static int32_t AddCallback(Gleam::VectorMap<int32_t, T>& callbacks, const T& cb)
	{
		return AddCallback(g_next_id++, callbacks, cb);
	}

	template <class T>
	static int32_t AddCallback(Gleam::VectorMap<int32_t, T>& callbacks, T&& cb)
	{
		return AddCallback(g_next_id++, callbacks, std::move(cb));
	}


	static Gleam::VectorMap<int32_t, Gleam::Window::IVecCallback> g_size_callbacks;
	static Gleam::VectorMap<int32_t, Gleam::Window::IVecCallback> g_pos_callbacks;
	static Gleam::VectorMap<int32_t, Gleam::Window::WindowCallback> g_close_callbacks;
	static Gleam::VectorMap<int32_t, Gleam::Window::BoolCallback> g_maximize_callbacks;
	static Gleam::VectorMap<int32_t, Gleam::Window::BoolCallback> g_focus_callbacks;

	static Gleam::VectorMap<int32_t, Gleam::Window::BoolCallback> g_mouse_enter_leave_callbacks;
	static Gleam::VectorMap<int32_t, Gleam::Window::VecCallback> g_mouse_pos_callbacks;
	static Gleam::VectorMap<int32_t, Gleam::Window::MouseButtonCallback> g_mouse_button_callbacks;
	static Gleam::VectorMap<int32_t, Gleam::Window::VecCallback> g_mouse_wheel_callbacks;

	static Gleam::VectorMap<int32_t, Gleam::Window::CharCallback> g_char_callbacks;
	static Gleam::VectorMap<int32_t, Gleam::Window::KeyCallback> g_key_callbacks;

	static Gleam::VectorMap<int32_t, Gleam::Window::MouseCallback> g_mouse_callbacks;
}

NS_GLEAM

bool Window::GlobalInit(void)
{
	const GLFWallocator allocator_data =
	{
		Allocate, Reallocate, Deallocate, nullptr
	};

	glfwSetErrorCallback(OnError);
	glfwInitAllocator(&allocator_data);

	if (!glfwInit()) {
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	return true;
}

void Window::GlobalShutdown(void)
{
	// Clear global message handlers.
	g_size_callbacks.clear();
	g_pos_callbacks.clear();
	g_close_callbacks.clear();
	g_maximize_callbacks.clear();
	g_focus_callbacks.clear();

	g_mouse_enter_leave_callbacks.clear();
	g_mouse_pos_callbacks.clear();
	g_mouse_button_callbacks.clear();
	g_mouse_wheel_callbacks.clear();

	g_char_callbacks.clear();
	g_key_callbacks.clear();

	g_mouse_callbacks.clear();


	glfwTerminate();
}

void Window::WaitEvents(double timeout_seconds)
{
	glfwWaitEventsTimeout(timeout_seconds);
}

void Window::PollEvents(void)
{
	glfwPollEvents();
}

void Window::PostEmptyEvent(void)
{
	glfwPostEmptyEvent();
}

int32_t Window::AddGlobalSizeChangeCallback(const IVecCallback& callback)
{
	return AddCallback(g_size_callbacks, callback);
}

int32_t Window::AddGlobalSizeChangeCallback(IVecCallback&& callback)
{
	return AddCallback(g_size_callbacks, std::move(callback));
}

int32_t Window::AddGlobalPosChangeCallback(const IVecCallback& callback)
{
	return AddCallback(g_pos_callbacks, callback);
}

int32_t Window::AddGlobalPosChangeCallback(IVecCallback&& callback)
{
	return AddCallback(g_pos_callbacks, std::move(callback));
}

int32_t Window::AddGlobalCloseCallback(const WindowCallback& callback)
{
	return AddCallback(g_close_callbacks, callback);
}

int32_t Window::AddGlobalCloseCallback(WindowCallback&& callback)
{
	return AddCallback(g_close_callbacks, std::move(callback));
}

int32_t Window::AddGlobalMaximizeCallback(const BoolCallback& callback)
{
	return AddCallback(g_maximize_callbacks, callback);
}

int32_t Window::AddGlobalMaximizeCallback(BoolCallback&& callback)
{
	return AddCallback(g_maximize_callbacks, std::move(callback));
}

int32_t Window::AddGlobalFocusCallback(const BoolCallback& callback)
{
	return AddCallback(g_focus_callbacks, callback);
}

int32_t Window::AddGlobalFocusCallback(BoolCallback&& callback)
{
	return AddCallback(g_focus_callbacks, std::move(callback));
}

int32_t Window::AddGlobalMouseEnterLeaveCallback(const BoolCallback& callback)
{
	return AddCallback(g_mouse_enter_leave_callbacks, callback);
}

int32_t Window::AddGlobalMouseEnterLeaveCallback(BoolCallback&& callback)
{
	return AddCallback(g_mouse_enter_leave_callbacks, std::move(callback));
}

int32_t Window::AddGlobalMousePosCallback(const VecCallback& callback)
{
	return AddCallback(g_mouse_pos_callbacks, callback);
}

int32_t Window::AddGlobalMousePosCallback(VecCallback&& callback)
{
	return AddCallback(g_mouse_pos_callbacks, std::move(callback));
}

int32_t Window::AddGlobalMouseButtonCallback(const MouseButtonCallback& callback)
{
	return AddCallback(g_mouse_button_callbacks, callback);
}

int32_t Window::AddGlobalMouseButtonCallback(MouseButtonCallback&& callback)
{
	return AddCallback(g_mouse_button_callbacks, std::move(callback));
}

int32_t Window::AddGlobalMouseWheelCallback(const VecCallback& callback)
{
	return AddCallback(g_mouse_wheel_callbacks, callback);
}

int32_t Window::AddGlobalMouseWheelCallback(VecCallback&& callback)
{
	return AddCallback(g_mouse_wheel_callbacks, std::move(callback));
}

int32_t Window::AddGlobalCharacterCallback(const CharCallback& callback)
{
	return AddCallback(g_char_callbacks, callback);
}

int32_t Window::AddGlobalCharacterCallback(CharCallback&& callback)
{
	return AddCallback(g_char_callbacks, std::move(callback));
}

int32_t Window::AddGlobalKeyCallback(const KeyCallback& callback)
{
	return AddCallback(g_key_callbacks, callback);
}

int32_t Window::AddGlobalKeyCallback(KeyCallback&& callback)
{
	return AddCallback(g_key_callbacks, std::move(callback));
}

int32_t Window::AddGlobalMouseCallback(const MouseCallback& callback)
{
	return AddCallback(g_mouse_callbacks, callback);
}

int32_t Window::AddGlobalMouseCallback(MouseCallback&& callback)
{
	return AddCallback(g_mouse_callbacks, std::move(callback));
}



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

	if (!_window) {
		return false;
	}

	double x, y;
	glfwGetCursorPos(_window, &x, &y);

	_prev_pos.x = static_cast<float>(x);
	_prev_pos.y = static_cast<float>(y);

	setWindowCallbacks();

	_fullscreen = true;
	return true;
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

	if (!_window) {
		return false;
	}

	double x, y;
	glfwGetCursorPos(_window, &x, &y);

	_prev_pos.x = static_cast<float>(x);
	_prev_pos.y = static_cast<float>(y);

	setWindowCallbacks();
	return true;
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

const char* Window::getTitle(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetWindowTitle(_window);
}

void Window::setIcon(const GLFWimage* icons, int32_t count)
{
	GAFF_ASSERT(_window);
	GAFF_ASSERT(count == 0 || (icons && count >= 0))
	glfwSetWindowIcon(_window, count, icons);
}

void Window::setIcon(const GLFWimage& icon)
{
	setIcon(&icon, 1);
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

bool Window::isCursorDisabled(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetInputMode(_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void Window::disableCursor(void)
{
	GAFF_ASSERT(_window);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool Window::isCursorHidden(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetInputMode(_window, GLFW_CURSOR) == GLFW_CURSOR_HIDDEN;
}

void Window::hideCursor(void)
{
	GAFF_ASSERT(_window);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

bool Window::isCursorVisible(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetInputMode(_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
}

void Window::showCursor(void)
{
	GAFF_ASSERT(_window);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::setMousePos(const Vec2& pos)
{
	GAFF_ASSERT(_window);
	glfwSetCursorPos(_window, pos.x, pos.y);
}

Vec2 Window::getMousePos(void) const
{
	GAFF_ASSERT(_window);

	double x, y;
	glfwGetCursorPos(_window, &x, &y);

	return Vec2(static_cast<float>(x), static_cast<float>(y));
}

void Window::setCursor(GLFWcursor* cursor)
{
	GAFF_ASSERT(_window);
	glfwSetCursor(_window, cursor);
}

bool Window::isUsingRawMouseMotion(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetInputMode(_window, GLFW_RAW_MOUSE_MOTION) == GLFW_TRUE;
}

void Window::useRawMouseMotion(bool enabled)
{
	GAFF_ASSERT(_window);
	glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, (enabled) ? GLFW_TRUE : GLFW_FALSE);
}

const char* Window::getClipboardText(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetClipboardString(_window);
}

void Window::setClipboardText(const char* text)
{
	GAFF_ASSERT(_window);
	glfwSetClipboardString(_window, text);
}

int32_t Window::addSizeChangeCallback(const IVecCallback& callback)
{
	return AddCallback(_next_id++, _size_callbacks, callback);
}

int32_t Window::addSizeChangeCallback(IVecCallback&& callback)
{
	return AddCallback(_next_id++, _size_callbacks, std::move(callback));
}

int32_t Window::addPosChangeCallback(const IVecCallback& callback)
{
	return AddCallback(_next_id++, _pos_callbacks, callback);
}

int32_t Window::addPosChangeCallback(IVecCallback&& callback)
{
	return AddCallback(_next_id++, _pos_callbacks, std::move(callback));
}

int32_t Window::addCloseCallback(const WindowCallback& callback)
{
	return AddCallback(_next_id++, _close_callbacks, callback);
}

int32_t Window::addCloseCallback(WindowCallback&& callback)
{
	return AddCallback(_next_id++, _close_callbacks, std::move(callback));
}

int32_t Window::addMaximizeCallback(const BoolCallback& callback)
{
	return AddCallback(_next_id++, _maximize_callbacks, callback);
}

int32_t Window::addMaximizeCallback(BoolCallback&& callback)
{
	return AddCallback(_next_id++, _maximize_callbacks, std::move(callback));
}

int32_t Window::addFocusCallback(const BoolCallback& callback)
{
	return AddCallback(_next_id++, _focus_callbacks, callback);
}

int32_t Window::addFocusCallback(BoolCallback&& callback)
{
	return AddCallback(_next_id++, _focus_callbacks, std::move(callback));
}

int32_t Window::addMouseEnterLeaveCallback(const BoolCallback& callback)
{
	return AddCallback(_next_id++, _mouse_enter_leave_callbacks, callback);
}

int32_t Window::addMouseEnterLeaveCallback(BoolCallback&& callback)
{
	return AddCallback(_next_id++, _mouse_enter_leave_callbacks, std::move(callback));
}

int32_t Window::addMousePosCallback(const VecCallback& callback)
{
	return AddCallback(_next_id++, _mouse_pos_callbacks, callback);
}

int32_t Window::addMousePosCallback(VecCallback&& callback)
{
	return AddCallback(_next_id++, _mouse_pos_callbacks, std::move(callback));
}

int32_t Window::addMouseButtonCallback(const MouseButtonCallback& callback)
{
	return AddCallback(_next_id++, _mouse_button_callbacks, callback);
}

int32_t Window::addMouseButtonCallback(MouseButtonCallback&& callback)
{
	return AddCallback(_next_id++, _mouse_button_callbacks, std::move(callback));
}

int32_t Window::addMouseWheelCallback(const VecCallback& callback)
{
	return AddCallback(_next_id++, _mouse_wheel_callbacks, callback);
}

int32_t Window::addMouseWheelCallback(VecCallback&& callback)
{
	return AddCallback(_next_id++, _mouse_wheel_callbacks, std::move(callback));
}

int32_t Window::addCharacterCallback(const CharCallback& callback)
{
	return AddCallback(_next_id++, _char_callbacks, callback);
}

int32_t Window::addCharacterCallback(CharCallback&& callback)
{
	return AddCallback(_next_id++, _char_callbacks, std::move(callback));
}

int32_t Window::addKeyCallback(const KeyCallback& callback)
{
	return AddCallback(_next_id++, _key_callbacks, callback);
}

int32_t Window::addKeyCallback(KeyCallback&& callback)
{
	return AddCallback(_next_id++, _key_callbacks, std::move(callback));
}

int32_t Window::addMouseCallback(const MouseCallback& callback)
{
	return AddCallback(_next_id++, _mouse_callbacks, callback);
}

int32_t Window::addMouseCallback(MouseCallback&& callback)
{
	return AddCallback(_next_id++, _mouse_callbacks, std::move(callback));
}

#ifdef PLATFORM_WINDOWS
void* Window::getHWnd(void) const
{
	GAFF_ASSERT(_window);
	return glfwGetWin32Window(_window);
}
#endif

void Window::OnWindowSize(GLFWwindow* glfw_window, int width, int height)
{
	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
	const IVec2 size(width, height);

	for (const auto& cb : window->_size_callbacks) {
		cb.second(*window, size);
	}

	for (const auto& cb : g_size_callbacks) {
		cb.second(*window, size);
	}
}

void Window::OnWindowPos(GLFWwindow* glfw_window, int x, int y)
{
	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
	const IVec2 pos(x, y);

	for (const auto& cb : window->_size_callbacks) {
		cb.second(*window, pos);
	}

	for (const auto& cb : g_size_callbacks) {
		cb.second(*window, pos);
	}
}

void Window::OnWindowClose(GLFWwindow* glfw_window)
{
	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

	for (const auto& cb : window->_close_callbacks) {
		cb.second(*window);
	}

	for (const auto& cb : g_close_callbacks) {
		cb.second(*window);
	}
}

void Window::OnWindowMaximize(GLFWwindow* glfw_window, int maximized)
{
	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

	for (const auto& cb : window->_maximize_callbacks) {
		cb.second(*window, maximized == GLFW_TRUE);
	}

	for (const auto& cb : g_maximize_callbacks) {
		cb.second(*window, maximized == GLFW_TRUE);
	}
}

void Window::OnWindowFocus(GLFWwindow* glfw_window, int maximized)
{
	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

	for (const auto& cb : window->_focus_callbacks) {
		cb.second(*window, maximized == GLFW_TRUE);
	}

	for (const auto& cb : g_focus_callbacks) {
		cb.second(*window, maximized == GLFW_TRUE);
	}
}

void Window::OnMouseEnterLeave(GLFWwindow* glfw_window, int entered)
{
	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

	for (const auto& cb : window->_mouse_enter_leave_callbacks) {
		cb.second(*window, entered == GLFW_TRUE);
	}

	for (const auto& cb : g_mouse_enter_leave_callbacks) {
		cb.second(*window, entered == GLFW_TRUE);
	}
}

void Window::OnMousePos(GLFWwindow* glfw_window, double x, double y)
{
	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
	const Vec2 pos(static_cast<float>(x), static_cast<float>(y));
	const Vec2 delta = pos - window->_prev_pos;

	for (const auto& cb : window->_mouse_pos_callbacks) {
		cb.second(*window, pos);
	}

	for (const auto& cb : window->_mouse_callbacks) {
		cb.second(*window, MouseCode::DeltaX, delta.x);
		cb.second(*window, MouseCode::DeltaY, delta.y);
		cb.second(*window, MouseCode::PosX, pos.x);
		cb.second(*window, MouseCode::PosY, pos.y);
	}

	for (const auto& cb : g_mouse_pos_callbacks) {
		cb.second(*window, pos);
	}

	for (const auto& cb : g_mouse_callbacks) {
		cb.second(*window, MouseCode::DeltaX, delta.x);
		cb.second(*window, MouseCode::DeltaY, delta.y);
		cb.second(*window, MouseCode::PosX, pos.x);
		cb.second(*window, MouseCode::PosY, pos.y);
	}

	window->_prev_pos = pos;
}

void Window::OnMouseButton(GLFWwindow* glfw_window, int button, int action, int mods)
{
	// We don't process mouse button repeats.
	if (action == GLFW_REPEAT) {
		return;
	}

	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

	// No flags on the negative bit, this should be fine.
	const Gaff::Flags<Modifier> modifiers(static_cast<Gaff::Flags<Modifier>::StorageType>(mods));

	for (const auto& cb : window->_mouse_button_callbacks) {
		cb.second(*window, static_cast<MouseButton>(button), action == GLFW_PRESS, modifiers);
	}

	for (const auto& cb : window->_mouse_callbacks) {
		cb.second(*window, static_cast<MouseCode>(button), (action) == GLFW_PRESS ? 1.0f : 0.0f);
	}

	for (const auto& cb : g_mouse_button_callbacks) {
		cb.second(*window, static_cast<MouseButton>(button), action == GLFW_PRESS, modifiers);
	}

	for (const auto& cb : g_mouse_callbacks) {
		cb.second(*window, static_cast<MouseCode>(button), (action) == GLFW_PRESS ? 1.0f : 0.0f);
	}
}

void Window::OnMouseWheel(GLFWwindow* glfw_window, double x, double y)
{
	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
	const Vec2 wheel(static_cast<float>(x), static_cast<float>(y));

	for (const auto& cb : window->_mouse_wheel_callbacks) {
		cb.second(*window, wheel);
	}

	for (const auto& cb : window->_mouse_callbacks) {
		cb.second(*window, MouseCode::WheelHorizontal, wheel.x);
		cb.second(*window, MouseCode::WheelVertical, wheel.y);
	}

	for (const auto& cb : g_mouse_wheel_callbacks) {
		cb.second(*window, wheel);
	}

	for (const auto& cb : g_mouse_callbacks) {
		cb.second(*window, MouseCode::WheelHorizontal, wheel.x);
		cb.second(*window, MouseCode::WheelVertical, wheel.y);
	}
}

void Window::OnCharacter(GLFWwindow* glfw_window, unsigned int character)
{
	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

	for (const auto& cb : window->_char_callbacks) {
		cb.second(*window, character);
	}

	for (const auto& cb : g_char_callbacks) {
		cb.second(*window, character);
	}
}

void Window::OnKey(GLFWwindow* glfw_window, int key, int scan_code, int action, int mods)
{
	// We don't process key repeats.
	// We don't process unknown keys.
	if (action == GLFW_REPEAT || key == GLFW_KEY_UNKNOWN) {
		return;
	}

	Window* const window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

	// No flags on the negative bit, this should be fine.
	const Gaff::Flags<Modifier> modifiers(static_cast<Gaff::Flags<Modifier>::StorageType>(mods));

	for (const auto& cb : window->_key_callbacks) {
		cb.second(*window, static_cast<KeyCode>(key), action == GLFW_PRESS, modifiers, scan_code);
	}

	for (const auto& cb : g_key_callbacks) {
		cb.second(*window, static_cast<KeyCode>(key), action == GLFW_PRESS, modifiers, scan_code);
	}
}

void Window::setWindowCallbacks(void)
{
	glfwSetWindowUserPointer(_window, this);

	glfwSetWindowSizeCallback(_window, &Window::OnWindowSize);
	glfwSetWindowPosCallback(_window, &Window::OnWindowPos);
	glfwSetWindowCloseCallback(_window, &Window::OnWindowClose);
	glfwSetWindowMaximizeCallback(_window, &Window::OnWindowMaximize);
	glfwSetWindowFocusCallback(_window, &Window::OnWindowFocus);

	glfwSetCursorEnterCallback(_window, &Window::OnMouseEnterLeave);
	glfwSetCursorPosCallback(_window, &Window::OnMousePos);
	glfwSetMouseButtonCallback(_window, &Window::OnMouseButton);
	glfwSetScrollCallback(_window, &Window::OnMouseWheel);

	glfwSetCharCallback(_window, &Window::OnCharacter);
	glfwSetKeyCallback(_window, &Window::OnKey);

	//glfwSetJoystickCallback(_window, &Window::OnJoystickConnectedOrDisconnected);
}

NS_END
