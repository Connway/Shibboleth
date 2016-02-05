/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gleam_WindowHelpers_Windows.h"
#include "Gleam_Window_Windows.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

void WindowClosed(AnyMessage* message, Window*, WPARAM, LPARAM)
{
	message->base.type = WND_CLOSED;
}

void WindowDestroyed(AnyMessage* message, Window*, WPARAM, LPARAM)
{
	message->base.type = WND_DESTROYED;
}

void WindowMoved(AnyMessage* message, Window* window, WPARAM, LPARAM l)
{
	window->_pos_x = (int)(short)LOWORD(l);
	window->_pos_y = (int)(short)HIWORD(l);

	message->base.type = WND_MOVED;
}

void WindowResized(AnyMessage* message, Window* window, WPARAM, LPARAM l)
{
	window->_width = LOWORD(l);
	window->_height = HIWORD(l);

	message->base.type = WND_RESIZED;
}

void WindowCharacter(AnyMessage* message, Window*, WPARAM w, LPARAM)
{
	message->base.type = IN_CHARACTER;
	message->key_char.character = static_cast<unsigned int>(w);
}

void WindowInput(AnyMessage* message, Window*, WPARAM, LPARAM l)
{
	UINT dwSize = 64;
	BYTE lpb[64];

#ifdef _DEBUG
	GetRawInputData((HRAWINPUT)l, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
	assert(dwSize <= 64);
#endif

	GetRawInputData((HRAWINPUT)l, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
	RAWINPUT* raw = (RAWINPUT*)lpb;

	if (raw->header.dwType == RIM_TYPEMOUSE && raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {

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
					message->key_char.key = Window::gRightKeys[raw->data.keyboard.VKey];
				} else {
					message->key_char.key = Window::gLeftKeys[raw->data.keyboard.VKey];
				}
				break;

			default:
				message->key_char.key = static_cast<KeyCode>(raw->data.keyboard.VKey);
				break;
		}
	}
}

void WindowLeftButtonDown(AnyMessage* message, Window*, WPARAM, LPARAM)
{
	message->base.type = IN_MOUSEDOWN;
	message->mouse_state.button = MOUSE_LEFT;
}

void WindowRightButtonDown(AnyMessage* message, Window*, WPARAM, LPARAM)
{
	message->base.type = IN_MOUSEDOWN;
	message->mouse_state.button = MOUSE_RIGHT;
}

void WindowMiddleButtonDown(AnyMessage* message, Window*, WPARAM, LPARAM)
{
	message->base.type = IN_MOUSEDOWN;
	message->mouse_state.button = MOUSE_MIDDLE;
}

void WindowXButtonDown(AnyMessage* message, Window*, WPARAM w, LPARAM)
{
	message->base.type = IN_MOUSEDOWN;
	message->mouse_state.button = (HIWORD(w) == XBUTTON1) ? MOUSE_BACK : MOUSE_FORWARD;
}

void WindowLeftButtonUp(AnyMessage* message, Window*, WPARAM, LPARAM)
{
	message->base.type = IN_MOUSEUP;
	message->mouse_state.button = MOUSE_LEFT;
}

void WindowRightButtonUp(AnyMessage* message, Window*, WPARAM, LPARAM)
{
	message->base.type = IN_MOUSEUP;
	message->mouse_state.button = MOUSE_RIGHT;
}

void WindowMiddleButtonUp(AnyMessage* message, Window*, WPARAM, LPARAM)
{
	message->base.type = IN_MOUSEUP;
	message->mouse_state.button = MOUSE_MIDDLE;
}

void WindowXButtonUp(AnyMessage* message, Window*, WPARAM w, LPARAM)
{
	message->base.type = IN_MOUSEUP;
	message->mouse_state.button = (HIWORD(w) == XBUTTON1) ? MOUSE_BACK : MOUSE_FORWARD;
}

void WindowMouseWheel(AnyMessage* message, Window*, WPARAM w, LPARAM)
{
	message->base.type = IN_MOUSEWHEEL;
	message->mouse_state.wheel = (short)HIWORD(w) / WHEEL_DELTA;
}

void WindowSetFocus(AnyMessage* message, Window* window, WPARAM, LPARAM)
{
	if (window->_window_mode == IWindow::FULLSCREEN) {
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

	message->base.type = WND_GAINEDFOCUS;
}

void WindowKillFocus(AnyMessage* message, Window* window, WPARAM, LPARAM)
{
	if (window->_window_mode == IWindow::FULLSCREEN) {
		DEVMODE dm_screen_settings;
		memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
		dm_screen_settings.dmSize = sizeof(dm_screen_settings);
		dm_screen_settings.dmPelsWidth  = window->_original_width;
		dm_screen_settings.dmPelsHeight = window->_original_height;
		dm_screen_settings.dmBitsPerPel = 32;
		dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);
	}

	message->base.type = WND_LOSTFOCUS;
}

NS_END
