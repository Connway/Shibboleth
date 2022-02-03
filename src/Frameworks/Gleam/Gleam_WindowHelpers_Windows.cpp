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

#if defined(_WIN32) || defined(_WIN64)

#include "Gleam_WindowHelpers_Windows.h"
#include "Gleam_Window_Windows.h"
#include <Gaff_Assert.h>

NS_GLEAM

void WindowClosed(AnyMessage& message, Window*, WPARAM, LPARAM)
{
	message.base.type = EventType::WindowClosed;
}

void WindowDestroyed(AnyMessage& message, Window*, WPARAM, LPARAM)
{
	message.base.type = EventType::WindowDestroyed;
}

void WindowMoved(AnyMessage& message, Window* window, WPARAM, LPARAM l)
{
	window->_pos = IVec2{
		(int)(short)LOWORD(l),
		(int)(short)HIWORD(l)
	};

	message.base.type = EventType::WindowMoved;
}

void WindowResized(AnyMessage& message, Window* window, WPARAM, LPARAM l)
{
	window->_size = IVec2{
		LOWORD(l),
		HIWORD(l)
	};

	message.base.type = EventType::WindowResized;
}

void WindowCharacter(AnyMessage& message, Window*, WPARAM w, LPARAM)
{
	message.base.type = EventType::InputCharacter;
	message.key_char.character = static_cast<uint32_t>(w);

	//if (m_keyRepeatEnabled || ((lParam & (1 << 30)) == 0))
	//{
	//	// Get the code of the typed character
	//	Uint32 character = static_cast<Uint32>(wParam);

	//	// Check if it is the first part of a surrogate pair, or a regular character
	//	if ((character >= 0xD800) && (character <= 0xDBFF))
	//	{
	//		// First part of a surrogate pair: store it and wait for the second one
	//		m_surrogate = static_cast<Uint16>(character);
	//	}
	//	else
	//	{
	//		// Check if it is the second part of a surrogate pair, or a regular character
	//		if ((character >= 0xDC00) && (character <= 0xDFFF))
	//		{
	//			// Convert the UTF-16 surrogate pair to a single UTF-32 value
	//			Uint16 utf16[] = { m_surrogate, static_cast<Uint16>(character) };
	//			sf::Utf16::toUtf32(utf16, utf16 + 2, &character);
	//			m_surrogate = 0;
	//		}

	//		// Send a TextEntered event
	//		Event event;
	//		event.type = Event::TextEntered;
	//		event.text.unicode = character;
	//		pushEvent(event);
	//	}
	//}
}

void WindowInput(AnyMessage& message, Window* window, WPARAM, LPARAM l)
{
	UINT dwSize = 64;
	BYTE lpb[64];

#ifdef _DEBUG
	GetRawInputData((HRAWINPUT)l, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
	GAFF_ASSERT(dwSize <= 64);
#endif

	GetRawInputData((HRAWINPUT)l, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
	RAWINPUT* raw = (RAWINPUT*)lpb;

	if (raw->header.dwType == RIM_TYPEMOUSE && raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {

		message.base.type = EventType::InputMouseMove;

		POINT pos;

		if (GetCursorPos(&pos)) {
			message.mouse_move.abs_x = pos.x;
			message.mouse_move.abs_y = pos.y;
		} else {
			// This shouldn't happen, but just in case.
			message.mouse_move.abs_x += raw->data.mouse.lLastX;
			message.mouse_move.abs_y += raw->data.mouse.lLastY;
		}

		if (MapWindowPoints(NULL, window->getHWnd(), &pos, 1)) {
			message.mouse_move.rel_x = pos.x;
			message.mouse_move.rel_y = pos.y;
		} else {
			// This shouldn't happen, but just in case.
			message.mouse_move.rel_x += raw->data.mouse.lLastX;
			message.mouse_move.rel_y += raw->data.mouse.lLastY;
		}

		message.mouse_move.dx = raw->data.mouse.lLastX;
		message.mouse_move.dy = raw->data.mouse.lLastY;

	} else if (raw->header.dwType == RIM_TYPEKEYBOARD) {

		message.base.type = (raw->data.keyboard.Flags & RI_KEY_BREAK) ? EventType::InputKeyUp : EventType::InputKeyDown;

		switch (raw->data.keyboard.VKey) {
			case VK_CONTROL:
			case VK_SHIFT:
				// For some reason, right shift isn't getting the RI_KEY_E0 flag set.
				// Special case it so that we send the correct key.
				if (raw->data.keyboard.MakeCode == 54) {
					message.key_char.key = KeyCode::RightShift;
					break;
				}

			case VK_MENU:
				if (raw->data.keyboard.Flags & RI_KEY_E0) {
					message.key_char.key = Window::g_right_keys[raw->data.keyboard.VKey];
				} else {
					message.key_char.key = Window::g_left_keys[raw->data.keyboard.VKey];
				}
				break;

			default:
				message.key_char.key = static_cast<KeyCode>(raw->data.keyboard.VKey);
				break;
		}
	}
}

void WindowLeftButtonDown(AnyMessage& message, Window*, WPARAM, LPARAM)
{
	message.base.type = EventType::InputMouseDown;
	message.mouse_state.button = MouseCode::Left;
}

void WindowRightButtonDown(AnyMessage& message, Window*, WPARAM, LPARAM)
{
	message.base.type = EventType::InputMouseDown;
	message.mouse_state.button = MouseCode::Right;
}

void WindowMiddleButtonDown(AnyMessage& message, Window*, WPARAM, LPARAM)
{
	message.base.type = EventType::InputMouseDown;
	message.mouse_state.button = MouseCode::Middle;
}

void WindowXButtonDown(AnyMessage& message, Window*, WPARAM w, LPARAM)
{
	message.base.type = EventType::InputMouseDown;
	message.mouse_state.button = (HIWORD(w) == XBUTTON1) ? MouseCode::Back : MouseCode::Forward;
}

void WindowLeftButtonUp(AnyMessage& message, Window*, WPARAM, LPARAM)
{
	message.base.type = EventType::InputMouseUp;
	message.mouse_state.button = MouseCode::Left;
}

void WindowRightButtonUp(AnyMessage& message, Window*, WPARAM, LPARAM)
{
	message.base.type = EventType::InputMouseUp;
	message.mouse_state.button = MouseCode::Right;
}

void WindowMiddleButtonUp(AnyMessage& message, Window*, WPARAM, LPARAM)
{
	message.base.type = EventType::InputMouseUp;
	message.mouse_state.button = MouseCode::Middle;
}

void WindowXButtonUp(AnyMessage& message, Window*, WPARAM w, LPARAM)
{
	message.base.type = EventType::InputMouseUp;
	message.mouse_state.button = (HIWORD(w) == XBUTTON1) ? MouseCode::Back : MouseCode::Forward;
}

void WindowMouseWheelHorizontal(AnyMessage& message, Window*, WPARAM w, LPARAM)
{
	message.base.type = EventType::InputMouseWheelHorizontal;
	message.mouse_state.wheel = static_cast<short>(HIWORD(w) / WHEEL_DELTA);
}

void WindowMouseWheel(AnyMessage& message, Window*, WPARAM w, LPARAM)
{
	message.base.type = EventType::InputMouseWheelVertical;
	message.mouse_state.wheel = static_cast<short>(HIWORD(w) / WHEEL_DELTA);
}

void WindowSetFocus(AnyMessage& message, Window* window, WPARAM, LPARAM)
{
	if (window->_window_mode == IWindow::WindowMode::Fullscreen) {
		const IVec2& size = window->getSize();

		// If full screen set the screen to maximum size of the users desktop and 32-bit.
		DEVMODE dm_screen_settings;
		memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
		dm_screen_settings.dmSize = sizeof(dm_screen_settings);
		dm_screen_settings.dmPelsWidth  = size.x;
		dm_screen_settings.dmPelsHeight = size.y;
		dm_screen_settings.dmBitsPerPel = 32;
		dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);
	}

	message.base.type = EventType::WindowGainedFocus;
}

void WindowKillFocus(AnyMessage& message, Window* window, WPARAM, LPARAM)
{
	GAFF_REF(message, window);

	//if (window->_window_mode == IWindow::WM_FULLSCREEN) {
	//	DEVMODE dm_screen_settings;
	//	memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
	//	dm_screen_settings.dmSize = sizeof(dm_screen_settings);
	//	dm_screen_settings.dmPelsWidth  = window->_original_width;
	//	dm_screen_settings.dmPelsHeight = window->_original_height;
	//	dm_screen_settings.dmBitsPerPel = 32;
	//	dm_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	//	ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);
	//}

	//message.base.type = EventType::WindowLostFocus;
}

NS_END

#endif
