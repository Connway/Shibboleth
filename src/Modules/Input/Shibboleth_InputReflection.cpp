/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_InputReflection.h"

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::MouseCode)
	.entry("Mouse Button Left", Gleam::MouseCode::MOUSE_LEFT)
	.entry("Mouse Button Right", Gleam::MouseCode::MOUSE_RIGHT)
	.entry("Mouse Button Middle", Gleam::MouseCode::MOUSE_MIDDLE)
	.entry("Mouse Button Back", Gleam::MouseCode::MOUSE_BACK)
	.entry("Mouse Button Forward", Gleam::MouseCode::MOUSE_FORWARD)
	.entry("Mouse Button 5", Gleam::MouseCode::MOUSE_BUTTON5)
	.entry("Mouse Button 6", Gleam::MouseCode::MOUSE_BUTTON6)
	.entry("Mouse Button 7", Gleam::MouseCode::MOUSE_BUTTON7)
	.entry("Mouse Wheel Up", Gleam::MouseCode::MOUSE_WHEEL_UP)
	.entry("Mouse Wheel Down", Gleam::MouseCode::MOUSE_WHEEL_DOWN)

	.entry("Mouse Delta X", Gleam::MouseCode::MOUSE_DELTA_X)
	.entry("Mouse Delta Y", Gleam::MouseCode::MOUSE_DELTA_Y)
	SHIB_REFLECTION_DEFINE_END(Gleam::MouseCode)

SHIB_REFLECTION_DEFINE_BEGIN(Gleam::KeyCode)
	.entry("Backspace", Gleam::KeyCode::KEY_BACKSPACE)
	.entry("Tab", Gleam::KeyCode::KEY_TAB)
	.entry("Enter", Gleam::KeyCode::KEY_ENTER)
	.entry("Pause", Gleam::KeyCode::KEY_PAUSE)
	.entry("Capslock", Gleam::KeyCode::KEY_CAPSLOCK)
	.entry("Escape", Gleam::KeyCode::KEY_ESCAPE)
	.entry("Space", Gleam::KeyCode::KEY_SPACE)
	.entry("Page Up", Gleam::KeyCode::KEY_PAGEUP)
	.entry("Page Down", Gleam::KeyCode::KEY_PAGEDOWN)
	.entry("End", Gleam::KeyCode::KEY_END)
	.entry("Home", Gleam::KeyCode::KEY_HOME)
	.entry("Left", Gleam::KeyCode::KEY_LEFT)
	.entry("Right", Gleam::KeyCode::KEY_RIGHT)
	.entry("Up", Gleam::KeyCode::KEY_UP)
	.entry("Down", Gleam::KeyCode::KEY_DOWN)
	.entry("Print Screen", Gleam::KeyCode::KEY_PRINTSCREEN)
	.entry("Insert", Gleam::KeyCode::KEY_INSERT)
	.entry("Delete", Gleam::KeyCode::KEY_DELETE)
	.entry("0", Gleam::KeyCode::KEY_0)
	.entry("1", Gleam::KeyCode::KEY_1)
	.entry("2", Gleam::KeyCode::KEY_2)
	.entry("3", Gleam::KeyCode::KEY_3)
	.entry("4", Gleam::KeyCode::KEY_4)
	.entry("5", Gleam::KeyCode::KEY_5)
	.entry("6", Gleam::KeyCode::KEY_6)
	.entry("7", Gleam::KeyCode::KEY_7)
	.entry("8", Gleam::KeyCode::KEY_8)
	.entry("9", Gleam::KeyCode::KEY_9)
	.entry("A", Gleam::KeyCode::KEY_A)
	.entry("B", Gleam::KeyCode::KEY_B)
	.entry("C", Gleam::KeyCode::KEY_C)
	.entry("D", Gleam::KeyCode::KEY_D)
	.entry("E", Gleam::KeyCode::KEY_E)
	.entry("F", Gleam::KeyCode::KEY_F)
	.entry("G", Gleam::KeyCode::KEY_G)
	.entry("H", Gleam::KeyCode::KEY_H)
	.entry("I", Gleam::KeyCode::KEY_I)
	.entry("J", Gleam::KeyCode::KEY_J)
	.entry("K", Gleam::KeyCode::KEY_K)
	.entry("L", Gleam::KeyCode::KEY_L)
	.entry("M", Gleam::KeyCode::KEY_M)
	.entry("N", Gleam::KeyCode::KEY_N)
	.entry("O", Gleam::KeyCode::KEY_O)
	.entry("P", Gleam::KeyCode::KEY_P)
	.entry("Q", Gleam::KeyCode::KEY_Q)
	.entry("R", Gleam::KeyCode::KEY_R)
	.entry("S", Gleam::KeyCode::KEY_S)
	.entry("T", Gleam::KeyCode::KEY_T)
	.entry("U", Gleam::KeyCode::KEY_U)
	.entry("V", Gleam::KeyCode::KEY_V)
	.entry("W", Gleam::KeyCode::KEY_W)
	.entry("X", Gleam::KeyCode::KEY_X)
	.entry("y", Gleam::KeyCode::KEY_Y)
	.entry("Z", Gleam::KeyCode::KEY_Z)
	.entry("Left Windows", Gleam::KeyCode::KEY_LEFTWINDOWS)
	.entry("Right Windows", Gleam::KeyCode::KEY_RIGHTWINDOWS)
	.entry("Numpad 0", Gleam::KeyCode::KEY_NUMPAD0)
	.entry("Numpad 1", Gleam::KeyCode::KEY_NUMPAD1)
	.entry("Numpad 2", Gleam::KeyCode::KEY_NUMPAD2)
	.entry("Numpad 3", Gleam::KeyCode::KEY_NUMPAD3)
	.entry("Numpad 4", Gleam::KeyCode::KEY_NUMPAD4)
	.entry("Numpad 5", Gleam::KeyCode::KEY_NUMPAD5)
	.entry("Numpad 6", Gleam::KeyCode::KEY_NUMPAD6)
	.entry("Numpad 7", Gleam::KeyCode::KEY_NUMPAD7)
	.entry("Numpad 8", Gleam::KeyCode::KEY_NUMPAD8)
	.entry("Numpad 9", Gleam::KeyCode::KEY_NUMPAD9)
	.entry("Numpad *", Gleam::KeyCode::KEY_MULTIPLY)
	.entry("Numpad +", Gleam::KeyCode::KEY_ADD)
	.entry("Numpad -", Gleam::KeyCode::KEY_SUBTRACT)
	.entry("Numpad .", Gleam::KeyCode::KEY_DECIMAL)
	.entry("Numpad /", Gleam::KeyCode::KEY_DIVIDE)
	.entry("Numpad Enter", Gleam::KeyCode::KEY_NUMPADENTER)
	.entry("F1", Gleam::KeyCode::KEY_F1)
	.entry("F2", Gleam::KeyCode::KEY_F2)
	.entry("F3", Gleam::KeyCode::KEY_F3)
	.entry("F4", Gleam::KeyCode::KEY_F4)
	.entry("F5", Gleam::KeyCode::KEY_F5)
	.entry("F6", Gleam::KeyCode::KEY_F6)
	.entry("F7", Gleam::KeyCode::KEY_F7)
	.entry("F8", Gleam::KeyCode::KEY_F8)
	.entry("F9", Gleam::KeyCode::KEY_F9)
	.entry("F10", Gleam::KeyCode::KEY_F10)
	.entry("F11", Gleam::KeyCode::KEY_F11)
	.entry("F12", Gleam::KeyCode::KEY_F12)
	.entry("Numlock", Gleam::KeyCode::KEY_NUMLOCK)
	.entry("Scrolllock", Gleam::KeyCode::KEY_SCROLLLOCK)
	.entry("Left Shift", Gleam::KeyCode::KEY_LEFTSHIFT)
	.entry("Right Shift", Gleam::KeyCode::KEY_RIGHTSHIFT)
	.entry("Left Control", Gleam::KeyCode::KEY_LEFTCONTROL)
	.entry("Right Control", Gleam::KeyCode::KEY_RIGHTCONTROL)
	.entry("Left Alt", Gleam::KeyCode::KEY_LEFTALT)
	.entry("Right Alt", Gleam::KeyCode::KEY_RIGHTALT)
	.entry(";", Gleam::KeyCode::KEY_SEMICOLON)
	.entry("=", Gleam::KeyCode::KEY_EQUALS)
	.entry(",", Gleam::KeyCode::KEY_COMMA)
	.entry(".", Gleam::KeyCode::KEY_PERIOD)
	.entry("/", Gleam::KeyCode::KEY_FORWARDSLASH)
	.entry("\\", Gleam::KeyCode::KEY_BACKSLASH)
	.entry("`", Gleam::KeyCode::KEY_TILDE)
	.entry("[", Gleam::KeyCode::KEY_LEFTSQUAREBRACKET)
	.entry("]", Gleam::KeyCode::KEY_RIGHTSQUAREBRACKET)
	.entry("'", Gleam::KeyCode::KEY_APOSTROPHE)
SHIB_REFLECTION_DEFINE_END(Gleam::KeyCode)