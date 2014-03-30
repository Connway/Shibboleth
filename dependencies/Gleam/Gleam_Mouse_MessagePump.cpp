/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gleam_Mouse_MessagePump.h"
#include "Gleam_RawInputRegisterFunction.h"
#include "Gleam_Window.h"

NS_GLEAM

MouseMP::MouseMP(void):
	_window(nullptr)
{
}

MouseMP::~MouseMP(void)
{
	destroy();
}

bool MouseMP::init(const Window& window)
{
	memset(&_curr_data, 0, sizeof(MouseData));
	memset(&_prev_data, 0, sizeof(MouseData));

	_window = (Window*)&window;
	_window->addWindowMessageHandler(this, &MouseMP::handleMessage);

	return RegisterForRawInput(RAW_INPUT_MOUSE, window);
}

void MouseMP::destroy(void)
{
	if (_window) {
		_window->removeWindowMessageHandler(this, &MouseMP::handleMessage);
		_window = nullptr;
	}
}

void MouseMP::update(void)
{
	if (_window->areRepeatsAllowed()) {
		for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
			_input_handlers[j](this, MOUSE_DELTA_X, (float)_curr_data.dx);
			_input_handlers[j](this, MOUSE_DELTA_Y, (float)_curr_data.dy);
			_input_handlers[j](this, MOUSE_POS_X, (float)_curr_data.x);
			_input_handlers[j](this, MOUSE_POS_Y, (float)_curr_data.y);

			for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) {
				_input_handlers[j](this, i, (float)_curr_data.buttons[i]);
			}
		}

		_dx = _curr_data.dx;
		_dy = _curr_data.dy;
		_wheel = _curr_data.wheel;

		_curr_data.dx = _curr_data.dy = 0;
		_curr_data.wheel = 0;

	} else {
		for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
			if (_curr_data.x != _prev_data.x) {
				_input_handlers[j](this, MOUSE_POS_X, (float)_curr_data.x);
			}

			if (_curr_data.y != _prev_data.y) {
				_input_handlers[j](this, MOUSE_POS_Y, (float)_curr_data.y);
			}

			if (_curr_data.dx) {
				_input_handlers[j](this, MOUSE_DELTA_X, (float)_curr_data.dx);
			}

			if (_curr_data.dy) {
				_input_handlers[j](this, MOUSE_DELTA_Y, (float)_curr_data.dy);
			}

			if (_curr_data.wheel) {
				_input_handlers[j](this, MOUSE_WHEEL, (float)_curr_data.wheel);
			}

			for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) {
				if (_curr_data.buttons[i] != _prev_data.buttons[i]) {
					_input_handlers[j](this, i, (float)_curr_data.buttons[i]);
				}
			}
		}

		_dx = _curr_data.dx;
		_dy = _curr_data.dy;
		_wheel = _curr_data.wheel;

		_curr_data.dx = _curr_data.dy = 0;
		_curr_data.wheel = 0;

		memcpy(&_prev_data, &_curr_data, sizeof(MouseData));
	}
}

const MouseData& MouseMP::getMouseData(void) const
{
	return _curr_data;
}

void MouseMP::getPosition(int& x, int& y) const
{
	x = _curr_data.x;
	y = _curr_data.y;
}

void MouseMP::getDeltas(int& dx, int& dy) const
{
	dx = _dx;
	dy = _dy;
}

void MouseMP::getNormalizedPosition(float& nx, float& ny) const
{
	nx = (float)_curr_data.x / (float)_window->getWidth();
	ny = (float)_curr_data.y / (float)_window->getHeight();
}

void MouseMP::getNormalizedDeltas(float& ndx, float& ndy) const
{
	ndx = (float)_curr_data.dx / (float)_window->getWidth();
	ndy = (float)_curr_data.dy / (float)_window->getHeight();
}

short MouseMP::getWheelDelta(void) const
{
	return _wheel;
}

const GChar* MouseMP::getDeviceName(void) const
{
	return GC("Gleam:Mouse");
}

const GChar* MouseMP::getPlatformImplementationString(void) const
{
	return GC("Agnostic:MessagePump");
};

const Window* MouseMP::getAssociatedWindow(void) const
{
	return _window;
}

bool MouseMP::handleMessage(const AnyMessage& message)
{
	switch (message.base.type) {
		case IN_MOUSEMOVE:
			_curr_data.x = message.mouse_move.x;
			_curr_data.y = message.mouse_move.y;
			_dx = _curr_data.dx = message.mouse_move.dx;
			_dy = _curr_data.dy = message.mouse_move.dy;
			return true;

		case IN_MOUSEDOWN:
			_curr_data.buttons[message.mouse_state.button] = true;
			return true;

		case IN_MOUSEUP:
			_curr_data.buttons[message.mouse_state.button] = false;
			return true;

		case IN_MOUSEDOUBLECLICK:
			return true;

		case IN_MOUSEWHEEL:
			_wheel = _curr_data.wheel = message.mouse_state.wheel;
			return true;

		// To get rid of pesky "case not handled" warnings in GCC
		default:
			break;
	}

	return false;
}

NS_END
