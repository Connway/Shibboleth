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

#include "Gleam_Defines.h"

NS_GLEAM

class IWindow;

enum class EventType : uint8_t
{
	WindowClosed = 0,
	WindowDestroyed,
	WindowMoved,
	WindowResized,
	WindowLostFocus,
	WindowGainedFocus,
	InputKeyDown,
	InputKeyUp,
	InputCharacter,
	InputMouseMove,
	InputMouseDown,
	InputMouseUp,
	InputMouseWheelHorizontal,
	InputMouseWheelVertical
};

enum class MouseCode : uint8_t
{
	Left = 0,
	Right,
	Middle,
	Back,
	Forward,
	Button5,
	Button6,
	Button7,
	ButtonCount,

	DeltaX = ButtonCount,
	DeltaY,
	PosX,
	PosY,

	WheelVertical,
	WheelUp,
	WheelDown,

	WheelHorizontal,
	WheelLeft,
	WheelRight
};

#ifdef PLATFORM_WINDOWS
enum class KeyCode
{
	// Cancel = 0x03,
	Backspace = 0x08,
	Tab,
	// Clear = 0x0C,
	Enter = 0x0D,
	Pause = 0x12,
	CapsLock,
	Escape = 0x1B,
	Space = 0x20,
	PageUp,
	PageDown,
	End,
	Home,
	Left,
	Up,
	Right,
	Down,
	// Select,
	// Print,
	// Execute,
	PrintScreen = 0x2C,
	Insert,
	Delete,
	// Help,
	Num0 = 0x30,
	Num1,
	Num2,
	Num3,
	Num4,
	Num5,
	Num6,
	Num7,
	Num8,
	Num9,
	A = 0x41,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	LeftSuper,
	RightSuper,
	Numpad0 = 0x60,
	Numpad1,
	Numpad2,
	Numpad3,
	Numpad4,
	Numpad5,
	Numpad6,
	Numpad7,
	Numpad8,
	Numpad9,
	Multiply,
	Add,
	Separator,
	Subtract,
	Decimal,
	Divide,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	// F13,
	// F14,
	// F15,
	// F16,
	// F17,
	// F18,
	// F19,
	// F20,
	// F21,
	// F22,
	// F23,
	// F24,
	NumLock = 0x90,
	ScrollLock,
	LeftShift = 0xA0,
	RightShift,
	LeftControl,
	RightControl,
	LeftAlt,
	RightAlt,
	Semicolon = 0xBA,
	Equals,
	Comma,
	Minus,
	Period,
	ForwardSlash,
	Tilde,
	LeftSquareBracket = 0xDB,
	Backslash,
	RightSquareBracket,
	Apostrophe,
	NumpadEnter = 0x0D
};

enum class CursorType
{
	None = -1,

	Arrow = 32512,
	IBeam = 32513,
	Wait = 32514,
	Cross = 32515,
	UpArrow = 32516,
	Size = 32640,
	Icon = 32641,
	SizeNWSE = 32642,
	SizeNESW = 32643,
	SizeWE = 32644,
	SizeNS = 32645,
	SizeAll = 32646,
	No = 32648,
	Hand = 32649,
	AppStarting = 32650,
	Help = 32651,
	Pin = 32671,
	Person = 32672
};

#elif defined(PLATFORM_LINUX)
enum class KeyCode
{
	// Cancel = 0xff69,
	Backspace = 22,
	Tab = 23,
	// Clear = 0xff0b,
	Enter = 36,
	Pause = 127,
	CapsLock = 66,
	Escape = 9,
	Space = 65,
	PageUp = 112,
	PageDown = 117,
	End = 115,
	Home = 110,
	Left = 113,
	Up = 111,
	Right = 114,
	Down = 116,
	// Select = 0xff60,
	// Print,
	// Execute,
	PrintScreen = 107,
	Insert = 118,
	Delete = 119,
	// Help = 0xff6a,
	Num0 = 19,
	Num1 = 10,
	Num2 = 11,
	Num3 = 12,
	Num4 = 13,
	Num5 = 14,
	Num6 = 15,
	Num7 = 16,
	Num8 = 17,
	Num9 = 18,
	A = 38,
	B = 56,
	C = 54,
	D = 40,
	E = 26,
	F = 41,
	G = 42,
	H = 43,
	I = 31,
	J = 44,
	K = 45,
	L = 46,
	M = 58,
	N = 57,
	O = 32,
	P = 33,
	Q = 24,
	R = 27,
	S = 39,
	T = 28,
	U = 30,
	V = 55,
	W = 25,
	X = 53,
	Y = 29,
	Z = 52,
	LeftSuper = 133,
	RightSuper = 134,
	Numpad0 = 90,
	Numpad1 = 86,
	Numpad2 = 87,
	Numpad3 = 88,
	Numpad4 = 83,
	Numpad5 = 84,
	Numpad6 = 85,
	Numpad7 = 79,
	Numpad8 = 80,
	Numpad9 = 81,
	Multiply = 63,
	Add = 86,
	Separator,
	Subtract = 82,
	Decimal = 91,
	Divide = 106,
	F1 = 67,
	F2 = 68,
	F3 = 69,
	F4 = 70,
	F5 = 71,
	F6 = 72,
	F7 = 73,
	F8 = 74,
	F9 = 75,
	F10 = 76,
	F11 = 95,
	F12 = 96,
	// These are guesses and are almost positively wrong!
	// F13 = 97,
	// F14 = 98,
	// F15 = 99,
	// F16 = 100,
	// F17 = 101,
	// F18 = 102,
	// F19 = 103,
	// F20 = 104,
	// F21 = 105,
	// F22 = 106,
	// F23 = 107,
	// F24 = 108,
	NumLock = 77,
	ScrollLock = 78,
	LeftShift = 50,
	RightShift = 62,
	LeftControl = 37,
	RightControl = 105,
	LeftAlt = 64,
	RightAlt = 108,
	Semicolon = 46,
	Equals = 21,
	Comma = 59,
	Minus = 20,
	Period = 60,
	ForwardSlash = 61,
	Tilde = 49,
	LeftSquareBracket = 34,
	Backslash = 51,
	RightSquareBracket = 35,
	Apostrophe = 48,
	NumpadEnter = 104
};

enum class CursorType
{
	None = -1,

	Arrow = 32512,
	IBeam = 32513,
	Wait = 32514,
	Cross = 32515,
	UpArrow = 32516,
	Size = 32640,
	Icon = 32641,
	SizeNWSE = 32642,
	SizeNESW = 32643,
	SizeWE = 32644,
	SizeNS = 32645,
	SizeAll = 32646,
	No = 32648,
	Hand = 32649,
	AppStarting = 32650,
	Help = 32651,
	Pin = 32671,
	Person = 32672
};
#endif

struct MessageBase final
{
	EventType type;
	IWindow* window;
};

struct MouseMoveMessage final
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

struct MouseStateMessage final
{
	EventType type;
	IWindow* window;

	union
	{
		MouseCode button;
		int16_t wheel;
	};
};

struct KeyCharMessage final
{
	EventType type;
	IWindow* window;

	union
	{
		KeyCode key;
		uint32_t character;
	};
};

union AnyMessage final
{
	MessageBase base;
	MouseMoveMessage mouse_move;
	MouseStateMessage mouse_state;
	KeyCharMessage key_char;
};

NS_END
