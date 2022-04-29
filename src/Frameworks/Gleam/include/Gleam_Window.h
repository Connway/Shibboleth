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

#pragma once

#include "Gleam_Window_Defines.h"
#include "Gleam_VectorMap.h"
#include "Gleam_Vec2.h"
#include <Gaff_Flags.h>
#include <EASTL/functional.h>

struct GLFWmonitor;
struct GLFWvidmode;
struct GLFWwindow;
struct GLFWcursor;
struct GLFWimage;

NS_GLEAM

class Window final
{
public:
	using KeyCallback = eastl::function<void (Window&, KeyCode, bool, Gaff::Flags<Modifier>, int32_t)>;
	using MouseButtonCallback = eastl::function<void (Window&, MouseButton, bool, Gaff::Flags<Modifier>)>;
	using CharCallback = eastl::function<void (Window&, uint32_t)>;
	using IVecCallback = eastl::function<void (Window&, const IVec2&)>;
	using VecCallback = eastl::function<void (Window&, const Vec2&)>;
	using BoolCallback = eastl::function<void (Window&, bool)>;
	using WindowCallback = eastl::function<void (Window&)>;
	using MouseCallback = eastl::function<void (Window&, MouseCode, float)>;

	static bool GlobalInit(void);
	static void GlobalShutdown(void);

	static void WaitEvents(double timeout_seconds = -1.0f);
	static void PollEvents(void);
	static void PostEmptyEvent(void);

	static int32_t AddGlobalSizeChangeCallback(const IVecCallback& callback);
	static int32_t AddGlobalSizeChangeCallback(IVecCallback&& callback);

	static int32_t AddGlobalPosChangeCallback(const IVecCallback& callback);
	static int32_t AddGlobalPosChangeCallback(IVecCallback&& callback);

	static int32_t AddGlobalCloseCallback(const WindowCallback& callback);
	static int32_t AddGlobalCloseCallback(WindowCallback&& callback);

	static int32_t AddGlobalMaximizeCallback(const BoolCallback& callback);
	static int32_t AddGlobalMaximizeCallback(BoolCallback&& callback);

	static int32_t AddGlobalFocusCallback(const BoolCallback& callback);
	static int32_t AddGlobalFocusCallback(BoolCallback&& callback);

	static int32_t AddGlobalMouseEnterLeaveCallback(const BoolCallback& callback);
	static int32_t AddGlobalMouseEnterLeaveCallback(BoolCallback&& callback);

	static int32_t AddGlobalMousePosCallback(const VecCallback& callback);
	static int32_t AddGlobalMousePosCallback(VecCallback&& callback);

	static int32_t AddGlobalMouseButtonCallback(const MouseButtonCallback& callback);
	static int32_t AddGlobalMouseButtonCallback(MouseButtonCallback&& callback);

	static int32_t AddGlobalMouseWheelCallback(const VecCallback& callback);
	static int32_t AddGlobalMouseWheelCallback(VecCallback&& callback);

	static int32_t AddGlobalCharacterCallback(const CharCallback& callback);
	static int32_t AddGlobalCharacterCallback(CharCallback&& callback);

	static int32_t AddGlobalKeyCallback(const KeyCallback& callback);
	static int32_t AddGlobalKeyCallback(KeyCallback&& callback);

	static int32_t AddGlobalMouseCallback(const MouseCallback& callback);
	static int32_t AddGlobalMouseCallback(MouseCallback&& callback);



	Window(void) = default;
	~Window(void);

	bool initFullscreen(
		const char8_t* window_name,
		GLFWmonitor& monitor,
		const GLFWvidmode& video_mode);

	bool initFullscreen(
		const char8_t* window_name,
		int32_t monitor_id,
		int32_t video_mode_id);

	bool initFullscreen(
		const char8_t* window_name,
		GLFWmonitor& monitor,
		const IVec2& size,
		int32_t refresh_rate = -1);

	bool initWindowed(
		const char8_t* window_name,
		const IVec2& size);

	void destroy(void);

	GLFWwindow* getGLFWWindow(void) const;

	void setFullscreen(const GLFWvidmode& video_mode, GLFWmonitor* monitor = nullptr);
	void setFullscreen(const IVec2& size, int32_t refresh_rate = -1, GLFWmonitor* monitor = nullptr);
	void setWindowed(const IVec2& size, const IVec2& pos = IVec2(0, 0));

	bool isFullscreen(void) const;
	bool isWindowed(void) const;

	void setSize(const IVec2& size);
	IVec2 getSize(void) const;

	void setPos(const IVec2& pos);
	IVec2 getPos(void) const;

	void setTitle(const char8_t* title);

	void setIcon(const GLFWimage* icons, int32_t count);

	void setVisible(bool visible);
	bool isVisible(void) const;

	bool isFocused(void) const;
	void focus(void);

	void notify(void);

	void setOpacity(float opacity);
	float getOpacity(void) const;

	bool isMaximized(void) const;
	void toggleMaximize(void);
	void maximize(void);
	void restore(void);

	void setAlwaysOnTop(bool always_on_top);
	bool isAlwaysOnTop(void) const;

	void setResizable(bool resizable);
	bool isResizable(void) const;

	void setHasDecorations(bool has_decorations);
	bool hasDecorations(void) const;

	bool shouldClose(void) const;
	void forceClose(void);

	bool isCursorDisabled(void) const;
	void disableCursor(void);

	bool isCursorHidden(void) const;
	void hideCursor(void);

	bool isCursorVisible(void) const;
	void showCursor(void);

	void setMousePos(const Vec2& pos);
	Vec2 getMousePos(void) const;

	void setCursor(GLFWcursor* cursor);

	bool isUsingRawMouseMotion(void) const;
	void useRawMouseMotion(bool enabled);

	const char* getClipboardText(void) const;
	void setClipboardText(const char* text);

	int32_t addSizeChangeCallback(const IVecCallback& callback);
	int32_t addSizeChangeCallback(IVecCallback&& callback);

	int32_t addPosChangeCallback(const IVecCallback& callback);
	int32_t addPosChangeCallback(IVecCallback&& callback);

	int32_t addCloseCallback(const WindowCallback & callback);
	int32_t addCloseCallback(WindowCallback&& callback);

	int32_t addMaximizeCallback(const BoolCallback& callback);
	int32_t addMaximizeCallback(BoolCallback&& callback);

	int32_t addFocusCallback(const BoolCallback& callback);
	int32_t addFocusCallback(BoolCallback&& callback);

	int32_t addMouseEnterLeaveCallback(const BoolCallback& callback);
	int32_t addMouseEnterLeaveCallback(BoolCallback&& callback);

	int32_t addMousePosCallback(const VecCallback& callback);
	int32_t addMousePosCallback(VecCallback&& callback);

	int32_t addMouseButtonCallback(const MouseButtonCallback& callback);
	int32_t addMouseButtonCallback(MouseButtonCallback&& callback);

	int32_t addMouseWheelCallback(const VecCallback& callback);
	int32_t addMouseWheelCallback(VecCallback&& callback);

	int32_t addCharacterCallback(const CharCallback& callback);
	int32_t addCharacterCallback(CharCallback&& callback);

	int32_t addKeyCallback(const KeyCallback& callback);
	int32_t addKeyCallback(KeyCallback&& callback);

	int32_t addMouseCallback(const MouseCallback& callback);
	int32_t addMouseCallback(MouseCallback&& callback);

#ifdef PLATFORM_WINDOWS
	void* getHWnd(void) const;
#endif

private:
	VectorMap<int32_t, IVecCallback> _size_callbacks;
	VectorMap<int32_t, IVecCallback> _pos_callbacks;
	VectorMap<int32_t, WindowCallback> _close_callbacks;
	VectorMap<int32_t, BoolCallback> _maximize_callbacks;
	VectorMap<int32_t, BoolCallback> _focus_callbacks;

	VectorMap<int32_t, BoolCallback> _mouse_enter_leave_callbacks;
	VectorMap<int32_t, VecCallback> _mouse_pos_callbacks;
	VectorMap<int32_t, MouseButtonCallback> _mouse_button_callbacks;
	VectorMap<int32_t, VecCallback> _mouse_wheel_callbacks;

	VectorMap<int32_t, CharCallback> _char_callbacks;
	VectorMap<int32_t, KeyCallback> _key_callbacks;

	VectorMap<int32_t, MouseCallback> _mouse_callbacks;

	// $TODO: Add joystick and gamepad support.

	Vec2 _prev_pos{ 0.0f, 0.0f };
	GLFWwindow* _window = nullptr;
	int32_t _next_id = 0;
	bool _fullscreen = false;

	static void OnWindowSize(GLFWwindow* glfw_window, int width, int height);
	static void OnWindowPos(GLFWwindow* glfw_window, int width, int height);
	static void OnWindowClose(GLFWwindow* glfw_window);
	static void OnWindowMaximize(GLFWwindow* glfw_window, int maximized);
	static void OnWindowFocus(GLFWwindow* glfw_window, int maximized);

	static void OnMouseEnterLeave(GLFWwindow* glfw_window, int entered);
	static void OnMousePos(GLFWwindow* glfw_window, double x, double y);
	static void OnMouseButton(GLFWwindow* glfw_window, int button, int action, int mods);
	static void OnMouseWheel(GLFWwindow* glfw_window, double x, double y);

	static void OnCharacter(GLFWwindow* glfw_window, unsigned int character);
	static void OnKey(GLFWwindow* glfw_window, int key, int scan_code, int action, int mods);

	void setWindowCallbacks(void);

	GAFF_NO_COPY(Window);
	GAFF_NO_MOVE(Window);
};

NS_END
