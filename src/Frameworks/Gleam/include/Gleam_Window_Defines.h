/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include <GLFW/glfw3.h>

NS_GLEAM

enum class Modifier : uint8_t
{
	Shift = 0,
	Control,
	Alt,
	Super,
	CapsLock,
	NumLock,

	Count
};

enum class MouseButton : uint8_t
{
	Left = 0,
	Right,
	Middle,
	Back,
	Forward,
	Button5,
	Button6,
	Button7,

	Count,
};

enum class MouseCode : uint8_t
{
	LeftButton = 0,
	RightButton,
	MiddleButton,
	BackButton,
	ForwardButton,
	Button5,
	Button6,
	Button7,
	ButtonCount,

	DeltaX = ButtonCount,
	DeltaY,
	PosX,
	PosY,

	WheelVertical,
	WheelHorizontal,

	Count
};

enum class KeyCode : uint16_t
{
	// Cancel = 0xff69,
	Backspace = GLFW_KEY_BACKSPACE,
	Tab = GLFW_KEY_TAB,
	// Clear = 0xff0b,
	Enter = GLFW_KEY_ENTER,
	Pause = GLFW_KEY_PAUSE,
	CapsLock = GLFW_KEY_CAPS_LOCK,
	Escape = GLFW_KEY_ESCAPE,
	Space = GLFW_KEY_SPACE,
	PageUp = GLFW_KEY_PAGE_UP,
	PageDown = GLFW_KEY_PAGE_DOWN,
	End = GLFW_KEY_END,
	Home = GLFW_KEY_HOME,
	Left = GLFW_KEY_LEFT,
	Up = GLFW_KEY_UP,
	Right = GLFW_KEY_RIGHT,
	Down = GLFW_KEY_DOWN,
	// Select = 0xff60,
	// Print,
	// Execute,
	PrintScreen = GLFW_KEY_PRINT_SCREEN,
	Insert = GLFW_KEY_INSERT,
	Delete = GLFW_KEY_DELETE,
	// Help = 0xff6a,
	Num0 = GLFW_KEY_0,
	Num1 = GLFW_KEY_1,
	Num2 = GLFW_KEY_2,
	Num3 = GLFW_KEY_3,
	Num4 = GLFW_KEY_4,
	Num5 = GLFW_KEY_5,
	Num6 = GLFW_KEY_6,
	Num7 = GLFW_KEY_7,
	Num8 = GLFW_KEY_8,
	Num9 = GLFW_KEY_9,
	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_M,
	N = GLFW_KEY_N,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z,
	LeftSuper = GLFW_KEY_LEFT_SUPER,
	RightSuper = GLFW_KEY_RIGHT_SUPER,
	Numpad0 = GLFW_KEY_KP_0,
	Numpad1 = GLFW_KEY_KP_1,
	Numpad2 = GLFW_KEY_KP_2,
	Numpad3 = GLFW_KEY_KP_3,
	Numpad4 = GLFW_KEY_KP_4,
	Numpad5 = GLFW_KEY_KP_5,
	Numpad6 = GLFW_KEY_KP_6,
	Numpad7 = GLFW_KEY_KP_7,
	Numpad8 = GLFW_KEY_KP_8,
	Numpad9 = GLFW_KEY_KP_9,
	Multiply = GLFW_KEY_KP_MULTIPLY,
	Add = GLFW_KEY_KP_ADD,
	//Separator = GLFW_KEY_,
	Subtract = GLFW_KEY_KP_SUBTRACT,
	Decimal = GLFW_KEY_KP_DECIMAL,
	Divide = GLFW_KEY_KP_DIVIDE,
	F1 = GLFW_KEY_F1,
	F2 = GLFW_KEY_F2,
	F3 = GLFW_KEY_F3,
	F4 = GLFW_KEY_F4,
	F5 = GLFW_KEY_F5,
	F6 = GLFW_KEY_F6,
	F7 = GLFW_KEY_F7,
	F8 = GLFW_KEY_F8,
	F9 = GLFW_KEY_F9,
	F10 = GLFW_KEY_F10,
	F11 = GLFW_KEY_F11,
	F12 = GLFW_KEY_F12,
	F13 = GLFW_KEY_F13,
	F14 = GLFW_KEY_F14,
	F15 = GLFW_KEY_F15,
	F16 = GLFW_KEY_F16,
	F17 = GLFW_KEY_F17,
	F18 = GLFW_KEY_F18,
	F19 = GLFW_KEY_F19,
	F20 = GLFW_KEY_F20,
	F21 = GLFW_KEY_F21,
	F22 = GLFW_KEY_F22,
	F23 = GLFW_KEY_F23,
	F24 = GLFW_KEY_F24,
	F25 = GLFW_KEY_F25,
	NumLock = GLFW_KEY_NUM_LOCK,
	ScrollLock = GLFW_KEY_SCROLL_LOCK,
	LeftShift = GLFW_KEY_LEFT_SHIFT,
	RightShift = GLFW_KEY_RIGHT_SHIFT,
	LeftControl = GLFW_KEY_LEFT_CONTROL,
	RightControl = GLFW_KEY_RIGHT_CONTROL,
	LeftAlt = GLFW_KEY_LEFT_ALT,
	RightAlt = GLFW_KEY_RIGHT_ALT,
	Semicolon = GLFW_KEY_SEMICOLON,
	Equals = GLFW_KEY_EQUAL,
	Comma = GLFW_KEY_COMMA,
	Minus = GLFW_KEY_MINUS,
	Period = GLFW_KEY_PERIOD,
	ForwardSlash = GLFW_KEY_SLASH,
	Grave = GLFW_KEY_GRAVE_ACCENT,
	LeftBracket = GLFW_KEY_LEFT_BRACKET,
	Backslash = GLFW_KEY_BACKSLASH,
	RightBracket = GLFW_KEY_RIGHT_BRACKET,
	Apostrophe = GLFW_KEY_APOSTROPHE,
	NumpadEnter = GLFW_KEY_KP_ENTER
};

NS_END

#ifdef APIENTRY
	#undef APIENTRY
#endif
