/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_Window_Defines.h"
#include <Gaff_IncludeWindows.h>

NS_GLEAM

class Window;

void WindowClosed(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowDestroyed(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowMoved(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowResized(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowCharacter(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowInput(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowLeftButtonDown(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowRightButtonDown(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowMiddleButtonDown(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowXButtonDown(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowLeftButtonUp(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowRightButtonUp(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowMiddleButtonUp(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowXButtonUp(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowMouseWheel(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowSetFocus(AnyMessage* message, Window* window, WPARAM w, LPARAM l);
void WindowKillFocus(AnyMessage* message, Window* window, WPARAM w, LPARAM l);

NS_END
