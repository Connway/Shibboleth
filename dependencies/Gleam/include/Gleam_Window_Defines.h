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

#pragma once

#include "Gleam_Defines.h"

NS_GLEAM

class Window;

enum EventType
{
	WND_CLOSED = 0,
	WND_DESTROYED,
	WND_MOVED,
	WND_RESIZED,
	IN_KEYDOWN,
	IN_KEYUP,
	IN_CHARACTER,
	IN_MOUSEMOVE,
	IN_MOUSEDOWN,
	IN_MOUSEUP,
	IN_MOUSEDOUBLECLICK,
	IN_MOUSEWHEEL,
	EVENT_COUNT
};

enum MouseCode
{
	MOUSE_LEFT = 0,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_BACK,
	MOUSE_FORWARD,
	MOUSE_BUTTON5,
	MOUSE_BUTTON6,
	MOUSE_BUTTON7,
	MOUSE_BUTTON_COUNT,
	MOUSE_DELTA_X,
	MOUSE_DELTA_Y,
	MOUSE_POS_X,
	MOUSE_POS_Y,
	MOUSE_WHEEL,
};

#if defined(_WIN32) || defined(_WIN64)
enum KeyCode
{
	KEY_CANCEL = 0x03,
	KEY_BACKSPACE = 0x08,
	KEY_TAB,
	KEY_CLEAR = 0x0C,
	KEY_ENTER,
	KEY_PAUSE = 0x12,
	KEY_CAPSLOCK,
	KEY_ESCAPE = 0x1B,
	KEY_SPACE = 0x20,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_END,
	KEY_HOME,
	KEY_LEFT,
	KEY_UP,
	KEY_RIGHT,
	KEY_DOWN,
	KEY_SELECT,
	KEY_PRINT,
	KEY_EXEC, // KEY_EXECUTE is apparently defined as something else already
	KEY_PRINTSCREEN,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HELP,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A = 0x41,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_LEFTWINDOWS,
	KEY_RIGHTWINDOWS,
	KEY_NUMPAD0,
	KEY_NUMPAD1,
	KEY_NUMPAD2,
	KEY_NUMPAD3,
	KEY_NUMPAD4,
	KEY_NUMPAD5,
	KEY_NUMPAD6,
	KEY_NUMPAD7,
	KEY_NUMPAD8,
	KEY_NUMPAD9,
	KEY_MULTIPLY,
	KEY_ADD,
	KEY_SEPARATOR,
	KEY_SUBTRACT,
	KEY_DECIMAL,
	KEY_DIVIDE,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_F13,
	KEY_F14,
	KEY_F15,
	KEY_F16,
	KEY_F17,
	KEY_F18,
	KEY_F19,
	KEY_F20,
	KEY_F21,
	KEY_F22,
	KEY_F23,
	KEY_F24,
	KEY_NUMLOCK = 0x90,
	KEY_SCROLLLOCK,
	KEY_LEFTSHIFT = 0xA0,
	KEY_RIGHTSHIFT,
	KEY_LEFTCONTROL,
	KEY_RIGHTCONTROL,
	KEY_LEFTALT,
	KEY_RIGHTALT,
	KEY_SEMICOLON = 0xBA,
	KEY_EQUALS,
	KEY_COMMA,
	KEY_MINUS,
	KEY_PERIOD,
	KEY_FORWARDSLASH,
	KEY_TILDE,
	KEY_LEFTSQUAREBRACKET = 0xDB,
	KEY_BACKSLASH,
	KEY_RIGHTSQUAREBRACKET,
	KEY_APOSTROPHE
};
#elif defined(__linux__)
enum KeyCode
{
	KEY_CANCEL = 0xff69,
	KEY_BACKSPACE = 0xff08,
	KEY_TAB,
	KEY_CLEAR = 0xff0b,
	KEY_ENTER = 0xff0d,
	KEY_PAUSE = 0xff13,
	KEY_CAPSLOCK = 0xffe5,
	KEY_ESCAPE = 0xff1b,
	KEY_SPACE = 0x0020,
	KEY_PAGEUP = 0xff55,
	KEY_PAGEDOWN,
	KEY_END,
	KEY_HOME = 0xff50,
	KEY_LEFT = 0xff51,
	KEY_UP,
	KEY_RIGHT,
	KEY_DOWN,
	KEY_SELECT = 0xff60,
	KEY_PRINT,
	KEY_EXEC, // KEY_EXECUTE is apparently defined as something else already
	KEY_PRINTSCREEN = 0xfd1d,
	KEY_INSERT = 0xff63,
	KEY_DELETE = 0xffff,
	KEY_HELP = 0xff6a,
	KEY_0 = 0x0030,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A = 0x0061,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_LEFTWINDOWS = 0xffeb,
	KEY_RIGHTWINDOWS,
	KEY_NUMPAD0 = 0xffb0,
	KEY_NUMPAD1,
	KEY_NUMPAD2,
	KEY_NUMPAD3,
	KEY_NUMPAD4,
	KEY_NUMPAD5,
	KEY_NUMPAD6,
	KEY_NUMPAD7,
	KEY_NUMPAD8,
	KEY_NUMPAD9,
	KEY_MULTIPLY = 0xffaa,
	KEY_ADD,
	KEY_SEPARATOR,
	KEY_SUBTRACT,
	KEY_DECIMAL,
	KEY_DIVIDE,
	KEY_F1 = 0xffbe,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_F13,
	KEY_F14,
	KEY_F15,
	KEY_F16,
	KEY_F17,
	KEY_F18,
	KEY_F19,
	KEY_F20,
	KEY_F21,
	KEY_F22,
	KEY_F23,
	KEY_F24,
	KEY_NUMLOCK = 0xff7f,
	KEY_SCROLLLOCK = 0xff14,
	KEY_LEFTSHIFT = 0xffe1,
	KEY_RIGHTSHIFT,
	KEY_LEFTCONTROL,
	KEY_RIGHTCONTROL,
	KEY_LEFTALT = 0xffe9,
	KEY_RIGHTALT,
	KEY_SEMICOLON = 0x003b,
	KEY_EQUALS = 0x003d,
	KEY_COMMA = 0x002c,
	KEY_MINUS,
	KEY_PERIOD,
	KEY_FORWARDSLASH,
	KEY_TILDE = 0x0060,
	KEY_LEFTSQUAREBRACKET = 0x005b,
	KEY_BACKSLASH,
	KEY_RIGHTSQUAREBRACKET,
	KEY_APOSTROPHE = 0x0027,
};
#endif

struct MessageBase
{
	EventType type;
	Window* window;
};

struct MouseMoveMessage
{
	EventType type;
	Window* window;
	int x;
	int y;
	int dx;
	int dy;
};

struct MouseStateMessage
{
	EventType type;
	Window* window;

	union
	{
		MouseCode button;
		short wheel;
	};
};

struct KeyCharMessage
{
	EventType type;
	Window* window;

	union
	{
		KeyCode key;
		unsigned int character;
	};
};

union AnyMessage
{
	MessageBase base;
	MouseMoveMessage mouse_move;
	MouseStateMessage mouse_state;
	KeyCharMessage key_char;
};

typedef bool (*WindowCallback)(const AnyMessage& message);

NS_END
