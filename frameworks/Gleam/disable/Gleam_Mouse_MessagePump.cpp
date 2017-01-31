/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#define MMP_ALLOW_REPEATS (1 << 0)
#define MMP_GLOBAL_HANDLER (1 << 1)

NS_GLEAM

MouseMP::MouseMP(void):
	_flags(0), _window(nullptr)
{
}

MouseMP::~MouseMP(void)
{
	destroy();
}

bool MouseMP::init(IWindow& window)
{
	memset(&_curr_data, 0, sizeof(MouseData));
	memset(&_prev_data, 0, sizeof(MouseData));

	auto cb = Gaff::Bind(this, &MouseMP::handleMessage);
	_window = &window;
	_window->addWindowMessageHandler(cb);

	return RegisterForRawInput(RAW_INPUT_MOUSE, window);
}

bool MouseMP::init(void)
{
	auto cb = Gaff::Bind(this, &MouseMP::handleMessage);
	Window::AddGlobalMessageHandler(cb);

	_flags |= MMP_GLOBAL_HANDLER;
	return true;
}

void MouseMP::destroy(void)
{
	auto cb = Gaff::Bind(this, &MouseMP::handleMessage);

	if (_window) {
		_window->removeWindowMessageHandler(cb);
		_window = nullptr;

	} else {
		Window::RemoveGlobalMessageHandler(cb);
	}

	_flags = 0;
}

void MouseMP::update(void)
{
	if (areRepeatsAllowed()) {
		for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
			_input_handlers[j](this, MOUSE_DELTA_X, static_cast<float>(_curr_data.dx));
			_input_handlers[j](this, MOUSE_DELTA_Y, static_cast<float>(_curr_data.dy));
			_input_handlers[j](this, MOUSE_POS_X, static_cast<float>(_curr_data.rel_x));
			_input_handlers[j](this, MOUSE_POS_Y, static_cast<float>(_curr_data.rel_y));

			for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) {
				_input_handlers[j](this, i, static_cast<float>(_curr_data.buttons[i]));
			}
		}

		_dx = _curr_data.dx;
		_dy = _curr_data.dy;
		_wheel = _curr_data.wheel;

		_curr_data.dx = _curr_data.dy = 0;
		_curr_data.wheel = 0;

	} else {
		for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
			if (_curr_data.rel_x != _prev_data.rel_x) {
				_input_handlers[j](this, MOUSE_POS_X, static_cast<float>(_curr_data.rel_x));
			}

			if (_curr_data.rel_y != _prev_data.rel_y) {
				_input_handlers[j](this, MOUSE_POS_Y, static_cast<float>(_curr_data.rel_y));
			}

			if (_curr_data.dx != _prev_data.dx) {
				_input_handlers[j](this, MOUSE_DELTA_X, static_cast<float>(_curr_data.dx));
			}

			if (_curr_data.dy != _prev_data.dy) {
				_input_handlers[j](this, MOUSE_DELTA_Y, static_cast<float>(_curr_data.dy));
			}

			if (_curr_data.wheel != _prev_data.wheel) {
				_input_handlers[j](this, MOUSE_WHEEL, static_cast<float>(_curr_data.wheel));
			}

			for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) {
				if (_curr_data.buttons[i] != _prev_data.buttons[i]) {
					_input_handlers[j](this, i, static_cast<float>(_curr_data.buttons[i]));
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

void MouseMP::getAbsolutePosition(int& x, int& y) const
{
	x = _curr_data.abs_x;
	y = _curr_data.abs_y;
}

void MouseMP::getRelativePosition(int& x, int& y) const
{
	x = _curr_data.rel_x;
	y = _curr_data.rel_y;
}

void MouseMP::getDeltas(int& dx, int& dy) const
{
	dx = _dx;
	dy = _dy;
}

//void MouseMP::getNormalizedAbsolutePosition(float& nx, float& ny) const
//{
//	nx = static_cast<float>(_curr_data.abs_x) / static_cast<float>(_window->getWidth());
//	ny = static_cast<float>(_curr_data.abs_y) / static_cast<float>(_window->getHeight());
//}

void MouseMP::getNormalizedRelativePosition(float& nx, float& ny) const
{
	nx = static_cast<float>(_curr_data.rel_x) / static_cast<float>(_window->getWidth());
	ny = static_cast<float>(_curr_data.rel_y) / static_cast<float>(_window->getHeight());
}

void MouseMP::getNormalizedDeltas(float& ndx, float& ndy) const
{
	ndx = static_cast<float>(_curr_data.dx) / static_cast<float>(_window->getWidth());
	ndy = static_cast<float>(_curr_data.dy) / static_cast<float>(_window->getHeight());
}

short MouseMP::getWheelDelta(void) const
{
	return _wheel;
}

void MouseMP::allowRepeats(bool allow)
{
	if (allow) {
		_flags |= MMP_ALLOW_REPEATS;
	} else {
		_flags &= ~MMP_ALLOW_REPEATS;
	}
}

bool MouseMP::areRepeatsAllowed(void) const
{
	return _flags & MMP_ALLOW_REPEATS;
}

const char* MouseMP::getDeviceName(void) const
{
	return "Gleam:Mouse";
}

const char* MouseMP::getPlatformImplementationString(void) const
{
	return "Agnostic:MessagePump";
};

const IWindow* MouseMP::getAssociatedWindow(void) const
{
	return _window;
}

bool MouseMP::handleMessage(const AnyMessage& message)
{
	switch (message.base.type) {
		case IN_MOUSEMOVE:
			_curr_data.abs_x = message.mouse_move.abs_x;
			_curr_data.abs_y = message.mouse_move.abs_y;
			_curr_data.rel_x = message.mouse_move.rel_x;
			_curr_data.rel_y = message.mouse_move.rel_y;
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
