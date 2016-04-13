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

SHIB_ENUM_REF_DEF_EMBEDDED(MouseCode, Gleam::MouseCode);
SHIB_ENUM_REF_IMPL_EMBEDDED(MouseCode, Gleam::MouseCode)
.addValue("Left Mouse", Gleam::MOUSE_LEFT)
.addValue("Right Mouse", Gleam::MOUSE_RIGHT)
.addValue("Middle Mouse", Gleam::MOUSE_MIDDLE)

.addValue("Mouse Back", Gleam::MOUSE_BACK)
.addValue("Mouse Forward", Gleam::MOUSE_FORWARD)
.addValue("Mouse Button 5", Gleam::MOUSE_BUTTON5)
.addValue("Mouse Button 6", Gleam::MOUSE_BUTTON6)
.addValue("Mouse Button 7", Gleam::MOUSE_BUTTON7)

.addValue("Mouse X-Axis", Gleam::MOUSE_DELTA_X)
.addValue("Mouse Y-Axis", Gleam::MOUSE_DELTA_Y)

.addValue("Mouse Wheel", Gleam::MOUSE_WHEEL)
;

SHIB_ENUM_REF_DEF_EMBEDDED(KeyCode, Gleam::KeyCode);
SHIB_ENUM_REF_IMPL_EMBEDDED(KeyCode, Gleam::KeyCode)
.addValue("Key Backspace", Gleam::KEY_BACKSPACE)
.addValue("Key Tab", Gleam::KEY_TAB)
.addValue("Key Enter", Gleam::KEY_ENTER)
.addValue("Key Pause", Gleam::KEY_PAUSE)
.addValue("Key Caps Lock", Gleam::KEY_CAPSLOCK)
.addValue("Key Escape", Gleam::KEY_ESCAPE)
.addValue("Key Space", Gleam::KEY_SPACE)
.addValue("Key Page Up", Gleam::KEY_PAGEUP)
.addValue("Key Page Down", Gleam::KEY_PAGEDOWN)
.addValue("Key End", Gleam::KEY_END)
.addValue("Key Home", Gleam::KEY_HOME)
.addValue("Key Left", Gleam::KEY_LEFT)
.addValue("Key Up", Gleam::KEY_UP)
.addValue("Key Right", Gleam::KEY_RIGHT)
.addValue("Key Down", Gleam::KEY_DOWN)
.addValue("Key Print Screen", Gleam::KEY_PRINTSCREEN)
.addValue("Key Insert", Gleam::KEY_INSERT)
.addValue("Key Delete", Gleam::KEY_DELETE)
.addValue("Key 0", Gleam::KEY_0)
.addValue("Key 1", Gleam::KEY_1)
.addValue("Key 2", Gleam::KEY_2)
.addValue("Key 3", Gleam::KEY_3)
.addValue("Key 4", Gleam::KEY_4)
.addValue("Key 5", Gleam::KEY_5)
.addValue("Key 6", Gleam::KEY_6)
.addValue("Key 7", Gleam::KEY_7)
.addValue("Key 8", Gleam::KEY_8)
.addValue("Key 9", Gleam::KEY_9)
.addValue("Key A", Gleam::KEY_A)
.addValue("Key B", Gleam::KEY_B)
.addValue("Key C", Gleam::KEY_C)
.addValue("Key D", Gleam::KEY_D)
.addValue("Key E", Gleam::KEY_E)
.addValue("Key F", Gleam::KEY_F)
.addValue("Key G", Gleam::KEY_G)
.addValue("Key H", Gleam::KEY_H)
.addValue("Key I", Gleam::KEY_I)
.addValue("Key J", Gleam::KEY_J)
.addValue("Key K", Gleam::KEY_K)
.addValue("Key L", Gleam::KEY_L)
.addValue("Key M", Gleam::KEY_M)
.addValue("Key N", Gleam::KEY_N)
.addValue("Key O", Gleam::KEY_O)
.addValue("Key P", Gleam::KEY_P)
.addValue("Key Q", Gleam::KEY_Q)
.addValue("Key R", Gleam::KEY_R)
.addValue("Key S", Gleam::KEY_S)
.addValue("Key T", Gleam::KEY_T)
.addValue("Key U", Gleam::KEY_U)
.addValue("Key V", Gleam::KEY_V)
.addValue("Key W", Gleam::KEY_W)
.addValue("Key X", Gleam::KEY_X)
.addValue("Key Y", Gleam::KEY_Y)
.addValue("Key Z", Gleam::KEY_Z)
//.addValue("Key Left Windows", Gleam::KEY_LEFTWINDOWS)
//.addValue("Key Right Windows", Gleam::KEY_RIGHTWINDOWS)
.addValue("Key Numpad 0", Gleam::KEY_NUMPAD0)
.addValue("Key Numpad 1", Gleam::KEY_NUMPAD1)
.addValue("Key Numpad 2", Gleam::KEY_NUMPAD2)
.addValue("Key Numpad 3", Gleam::KEY_NUMPAD3)
.addValue("Key Numpad 4", Gleam::KEY_NUMPAD4)
.addValue("Key Numpad 5", Gleam::KEY_NUMPAD5)
.addValue("Key Numpad 6", Gleam::KEY_NUMPAD6)
.addValue("Key Numpad 7", Gleam::KEY_NUMPAD7)
.addValue("Key Numpad 8", Gleam::KEY_NUMPAD8)
.addValue("Key Numpad 9", Gleam::KEY_NUMPAD9)
.addValue("Key Multiply", Gleam::KEY_MULTIPLY)
.addValue("Key Add", Gleam::KEY_ADD)
.addValue("Key Separator", Gleam::KEY_SEPARATOR)
.addValue("Key Subtract", Gleam::KEY_SUBTRACT)
.addValue("Key Decimal", Gleam::KEY_DECIMAL)
.addValue("Key Divide", Gleam::KEY_DIVIDE)
.addValue("Key F1", Gleam::KEY_F1)
.addValue("Key F2", Gleam::KEY_F2)
.addValue("Key F3", Gleam::KEY_F3)
.addValue("Key F4", Gleam::KEY_F4)
.addValue("Key F5", Gleam::KEY_F5)
.addValue("Key F6", Gleam::KEY_F6)
.addValue("Key F7", Gleam::KEY_F7)
.addValue("Key F8", Gleam::KEY_F8)
.addValue("Key F9", Gleam::KEY_F9)
.addValue("Key F10", Gleam::KEY_F10)
.addValue("Key F11", Gleam::KEY_F11)
.addValue("Key F12", Gleam::KEY_F12)
.addValue("Key Num Lock", Gleam::KEY_NUMLOCK)
.addValue("Key Scroll Lock", Gleam::KEY_SCROLLLOCK)
.addValue("Key Left Shift", Gleam::KEY_LEFTSHIFT)
.addValue("Key Right Shift", Gleam::KEY_RIGHTSHIFT)
.addValue("Key Left Control", Gleam::KEY_LEFTCONTROL)
.addValue("Key Right Control", Gleam::KEY_RIGHTCONTROL)
.addValue("Key Left Alt", Gleam::KEY_LEFTALT)
.addValue("Key Right Alt", Gleam::KEY_RIGHTALT)
.addValue("Key Semicolon", Gleam::KEY_SEMICOLON)
.addValue("Key Equals", Gleam::KEY_EQUALS)
.addValue("Key Comma", Gleam::KEY_COMMA)
.addValue("Key Minus", Gleam::KEY_MINUS)
.addValue("Key Period", Gleam::KEY_PERIOD)
.addValue("Key Forward Slash", Gleam::KEY_FORWARDSLASH)
.addValue("Key Tilde", Gleam::KEY_TILDE)
.addValue("Key Left Square Bracket", Gleam::KEY_LEFTSQUAREBRACKET)
.addValue("Key Back Slash", Gleam::KEY_BACKSLASH)
.addValue("Key Right Square Bracket", Gleam::KEY_RIGHTSQUAREBRACKET)
.addValue("Key Apostrophe", Gleam::KEY_APOSTROPHE)
.addValue("Key Numpad Enter", Gleam::KEY_NUMPADENTER)
;