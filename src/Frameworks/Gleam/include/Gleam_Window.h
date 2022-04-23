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

#include "Gleam_VectorMap.h"
#include "Gleam_Vec2.h"
#include <EASTL/functional.h>

struct GLFWmonitor;
struct GLFWvidmode;
struct GLFWwindow;
struct GLFWimage;

NS_GLEAM

class Window
{
public:
	using VecCallback = eastl::function<void (Window&, const IVec2&)>;
	using BoolCallback = eastl::function<void (Window&, bool)>;
	using WindowCallback = eastl::function<void (Window&)>;

	template <class T>
	T* getUserPointer(void) const
	{
		return reinterpret_cast<T*>(getUserPointer());
	}

	static bool GlobalInit(void);
	static void GlobalShutdown(void);

	static void WaitEvents(double timeout_seconds = -1.0f);
	static void PollEvents(void);
	static void PostEmptyEvent(void);

	//static int32_t AddGlobalMessageHandler(const MessageHandler& callback);
	//static int32_t AddGlobalMessageHandler(MessageHandler&& callback);
	//static bool RemoveGlobalMessageHandler(int32_t id);

	Window(void) = default;
	~Window(void);

	bool initFullscreen(
		const char8_t* window_name,
		GLFWmonitor& monitor,
		const GLFWvidmode& video_mode);

	bool initFullscreen(
		const char8_t* window_name,
		int32_t display_id,
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

	void* getUserPointer(void) const;
	void setUserPointer(void* ptr);

	bool shouldClose(void) const;
	void forceClose(void);

	int32_t addSizeChangeCallback(const VecCallback& callback);
	int32_t addSizeChangeCallback(VecCallback&& callback);

	int32_t addPosChangeCallback(const VecCallback& callback);
	int32_t addPosChangeCallback(VecCallback&& callback);

	int32_t addCloseCallback(const WindowCallback& callback);
	int32_t addCloseCallback(WindowCallback&& callback);

	int32_t addMaximizeCallback(const BoolCallback& callback);
	int32_t addMaximizeCallback(BoolCallback&& callback);

	int32_t addFocusCallback(const BoolCallback& callback);
	int32_t addFocusCallback(BoolCallback&& callback);

	bool isCursorDisabled(void) const;
	void disableCursor(void);

	bool isCursorHidden(void) const;
	void hideCursor(void);

	bool isCursorVisible(void) const;
	void showCursor(void);

	void setCursorPos(const IVec2 & pos);
	IVec2 getCursorPos(void) const;

	bool isUsingRawMouseMotion(void) const;
	void useRawMouseMotion(bool enabled);

	//void* getPlatformHandle(void) const override;

	//HINSTANCE getHInstance(void) const;
	//HWND getHWnd(void) const;

private:
	//bool _cursor_visible = true;
	//bool _contain = false;

	//VectorMap<int32_t, MessageHandler> _window_callbacks;

	//static VectorMap<uint16_t, KeyCode> g_right_keys;
	//static VectorMap<uint16_t, KeyCode> g_left_keys;

	//static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);

	//friend void WindowMoved(AnyMessage&, Window*, WPARAM, LPARAM);
	//friend void WindowResized(AnyMessage&, Window*, WPARAM, LPARAM);
	//friend void WindowInput(AnyMessage&, Window*, WPARAM, LPARAM);
	//friend void WindowSetFocus(AnyMessage&, Window*, WPARAM, LPARAM);
	//friend void WindowKillFocus(AnyMessage&, Window*, WPARAM, LPARAM);

	VectorMap<int32_t, VecCallback> _size_callbacks;
	VectorMap<int32_t, VecCallback> _pos_callbacks;
	VectorMap<int32_t, WindowCallback> _close_callbacks;
	VectorMap<int32_t, BoolCallback> _maximize_callbacks;
	VectorMap<int32_t, BoolCallback> _focus_callbacks;

	GLFWwindow* _window = nullptr;
	bool _fullscreen = false;

	GAFF_NO_COPY(Window);
	GAFF_NO_MOVE(Window);
};

NS_END
