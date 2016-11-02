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

#pragma once

#include "Gleam_Defines.h"

NS_GLEAM

class IWindow;

enum EventType
{
	WND_CLOSED = 0,
	WND_DESTROYED,
	WND_MOVED,
	WND_RESIZED,
	WND_LOSTFOCUS,
	WND_GAINEDFOCUS,
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
	MOUSE_WHEEL
};

#ifdef PLATFORM_WINDOWS
enum KeyCode
{
	// KEY_CANCEL = 0x03,
	KEY_BACKSPACE = 0x08,
	KEY_TAB,
	// KEY_CLEAR = 0x0C,
	KEY_ENTER = 0x0D,
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
	// KEY_SELECT,
	// KEY_PRINT,
	// KEY_EXEC, // KEY_EXECUTE is apparently defined as something else already
	KEY_PRINTSCREEN = 0x2C,
	KEY_INSERT,
	KEY_DELETE,
	// KEY_HELP,
	KEY_0 = 0x30,
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
	// KEY_F13,
	// KEY_F14,
	// KEY_F15,
	// KEY_F16,
	// KEY_F17,
	// KEY_F18,
	// KEY_F19,
	// KEY_F20,
	// KEY_F21,
	// KEY_F22,
	// KEY_F23,
	// KEY_F24,
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
	KEY_APOSTROPHE,
	KEY_NUMPADENTER = 0x0D
};
#elif defined(PLATFORM_LINUX)
enum KeyCode
{
	// KEY_CANCEL = 0xff69,
	KEY_BACKSPACE = 22,
	KEY_TAB = 23,
	// KEY_CLEAR = 0xff0b,
	KEY_ENTER = 36,
	KEY_PAUSE = 127,
	KEY_CAPSLOCK = 66,
	KEY_ESCAPE = 9,
	KEY_SPACE = 65,
	KEY_PAGEUP = 112,
	KEY_PAGEDOWN = 117,
	KEY_END = 115,
	KEY_HOME = 110,
	KEY_LEFT = 113,
	KEY_UP = 111,
	KEY_RIGHT = 114,
	KEY_DOWN = 116,
	// KEY_SELECT = 0xff60,
	// KEY_PRINT,
	// KEY_EXEC, // KEY_EXECUTE is apparently defined as something else already
	KEY_PRINTSCREEN = 107,
	KEY_INSERT = 118,
	KEY_DELETE = 119,
	// KEY_HELP = 0xff6a,
	KEY_0 = 19,
	KEY_1 = 10,
	KEY_2 = 11,
	KEY_3 = 12,
	KEY_4 = 13,
	KEY_5 = 14,
	KEY_6 = 15,
	KEY_7 = 16,
	KEY_8 = 17,
	KEY_9 = 18,
	KEY_A = 38,
	KEY_B = 56,
	KEY_C = 54,
	KEY_D = 40,
	KEY_E = 26,
	KEY_F = 41,
	KEY_G = 42,
	KEY_H = 43,
	KEY_I = 31,
	KEY_J = 44,
	KEY_K = 45,
	KEY_L = 46,
	KEY_M = 58,
	KEY_N = 57,
	KEY_O = 32,
	KEY_P = 33,
	KEY_Q = 24,
	KEY_R = 27,
	KEY_S = 39,
	KEY_T = 28,
	KEY_U = 30,
	KEY_V = 55,
	KEY_W = 25,
	KEY_X = 53,
	KEY_Y = 29,
	KEY_Z = 52,
	KEY_LEFTWINDOWS = 133,
	KEY_RIGHTWINDOWS = 134,
	KEY_NUMPAD0 = 90,
	KEY_NUMPAD1 = 86,
	KEY_NUMPAD2 = 87,
	KEY_NUMPAD3 = 88,
	KEY_NUMPAD4 = 83,
	KEY_NUMPAD5 = 84,
	KEY_NUMPAD6 = 85,
	KEY_NUMPAD7 = 79,
	KEY_NUMPAD8 = 80,
	KEY_NUMPAD9 = 81,
	KEY_MULTIPLY = 63,
	KEY_ADD = 86,
	KEY_SEPARATOR,
	KEY_SUBTRACT = 82,
	KEY_DECIMAL = 91,
	KEY_DIVIDE = 106,
	KEY_F1 = 67,
	KEY_F2 = 68,
	KEY_F3 = 69,
	KEY_F4 = 70,
	KEY_F5 = 71,
	KEY_F6 = 72,
	KEY_F7 = 73,
	KEY_F8 = 74,
	KEY_F9 = 75,
	KEY_F10 = 76,
	KEY_F11 = 95,
	KEY_F12 = 96,
	// These are guesses and are almost positively wrong!
	// KEY_F13 = 97,
	// KEY_F14 = 98,
	// KEY_F15 = 99,
	// KEY_F16 = 100,
	// KEY_F17 = 101,
	// KEY_F18 = 102,
	// KEY_F19 = 103,
	// KEY_F20 = 104,
	// KEY_F21 = 105,
	// KEY_F22 = 106,
	// KEY_F23 = 107,
	// KEY_F24 = 108,
	KEY_NUMLOCK = 77,
	KEY_SCROLLLOCK = 78,
	KEY_LEFTSHIFT = 50,
	KEY_RIGHTSHIFT = 62,
	KEY_LEFTCONTROL = 37,
	KEY_RIGHTCONTROL = 105,
	KEY_LEFTALT = 64,
	KEY_RIGHTALT = 108,
	KEY_SEMICOLON = 46,
	KEY_EQUALS = 21,
	KEY_COMMA = 59,
	KEY_MINUS = 20,
	KEY_PERIOD = 60,
	KEY_FORWARDSLASH = 61,
	KEY_TILDE = 49,
	KEY_LEFTSQUAREBRACKET = 34,
	KEY_BACKSLASH = 51,
	KEY_RIGHTSQUAREBRACKET = 35,
	KEY_APOSTROPHE = 48,
	KEY_NUMPADENTER = 104
};
#endif

struct MessageBase
{	
	EventType type;
	IWindow* window;
};

struct MouseMoveMessage
{
	EventType type;
	IWindow* window;
	int32_t abs_x;
	int32_t abs_y;
	int32_t rel_x;
	int32_t rel_y;
	int32_t dx;
	int32_t dy;
};

struct MouseStateMessage
{
	EventType type;
	IWindow* window;

	union
	{
		MouseCode button;
		int16_t wheel;
	};
};

struct KeyCharMessage
{
	EventType type;
	IWindow* window;

	union
	{
		KeyCode key;
		uint32_t character;
	};
};

union AnyMessage
{
	MessageBase base;
	MouseMoveMessage mouse_move;
	MouseStateMessage mouse_state;
	KeyCharMessage key_char;
};

NS_END
