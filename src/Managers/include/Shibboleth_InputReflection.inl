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
// add enum values
;