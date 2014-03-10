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

#include "Gleam_Mouse_DirectInput.h"
#include "Gleam_Window.h"
#include "Gaff_Math.h"

NS_GLEAM

MouseDI::MouseDI(void):
#ifdef ONLY_INPUT_CHANGES
	_mouse(nullptr), _curr_state(&_mouse_state_a),
	_prev_state(&_mouse_state_b), _window(nullptr)
#else
	_mouse(nullptr), _window(nullptr)
#endif
{
}

MouseDI::~MouseDI(void)
{
	destroy();
}

bool MouseDI::init(const Window& window, void* direct_input, bool)
{
	memset(&_data, 0, sizeof(MouseData));

#ifdef ONLY_INPUT_CHANGES
	memset(&_mouse_state_a, 0, sizeof(DIMOUSESTATE2));
	memset(&_mouse_state_b, 0, sizeof(DIMOUSESTATE2));
#endif

	IDirectInput8* dinput = (IDirectInput8*)direct_input;

	// set our mouse data to the correct position
	POINT point;
	if (GetCursorPos(&point)) {
		_data.x = point.x - window.getPosX();
		_data.y = point.y - window.getPosY();
	}

	HRESULT result = dinput->CreateDevice(GUID_SysMouse, &_mouse, nullptr);
	RETURNIFFAILED(result)

	result = _mouse->SetDataFormat(&c_dfDIMouse2);
	RETURNIFFAILED(result)

	result = _mouse->SetCooperativeLevel(window.getHWnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	RETURNIFFAILED(result)

	_window = &window;

	result = _mouse->Acquire();
	return SUCCEEDED(result);
}

void MouseDI::destroy(void)
{
	if (_mouse) {
		_mouse->Unacquire();
		_mouse->Release();
		_mouse = nullptr;
	}

	_window = nullptr;
}

bool MouseDI::update(void)
{
#ifdef ONLY_INPUT_CHANGES
	DIMOUSESTATE2* temp = _curr_state;
	_curr_state = _prev_state;
	_prev_state = temp;

	HRESULT result = _mouse->GetDeviceState(sizeof(DIMOUSESTATE2), _curr_state);
#else
	HRESULT result = _mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &_mouse_state);
#endif

	if (FAILED(result)) {
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			_mouse->Acquire();
		} else {
			return false;
		}

	} else {
#ifdef ONLY_INPUT_CHANGES
		_data.dx = _curr_state->lX;
		_data.dy = _curr_state->lY;
		_data.x += _data.dx;
		_data.y += _data.dy;
		_data.wheel = (short)_curr_state->lZ / 120;

		_data.x = Gaff::Clamp(_data.x, 0, (int)_window->getWidth());
		_data.y = Gaff::Clamp(_data.y, 0, (int)_window->getHeight());

		for (unsigned int i = 0; i < _input_handlers.size(); ++i) {
			if (_curr_state->lX != _prev_state->lX) {
				_input_handlers[i](this, MOUSE_DELTA_X, (float)_data.dx);
				_input_handlers[i](this, MOUSE_POS_X, (float)_data.x);
			}

			if (_curr_state->lY != _prev_state->lY) {
				_input_handlers[i](this, MOUSE_DELTA_Y, (float)_data.dy);
				_input_handlers[i](this, MOUSE_POS_Y, (float)_data.y);
			}

			if (_curr_state->lZ != _prev_state->lZ) {
				_input_handlers[i](this, MOUSE_WHEEL, (float)_data.wheel);
			}
		}

		for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) {
			bool curr = (_curr_state->rgbButtons[i] & 0x80) != 0;
			bool prev = (_prev_state->rgbButtons[i] & 0x80) != 0;

			_data.buttons[i] = curr;

			if (curr != prev) {
				for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
					_input_handlers[j](this, i, (float)curr);
				}
			}
		}
#else
		_data.dx = _mouse_state.lX;
		_data.dy = _mouse_state.lY;
		_data.x += _data.dx;
		_data.y += _data.dy;
		_data.wheel = (short)_mouse_state.lZ / 120;

		_data.x = Gaff::Clamp(_data.x, 0, (int)_window->getWidth());
		_data.y = Gaff::Clamp(_data.y, 0, (int)_window->getHeight());

		for (unsigned int i = 0; i < _input_handlers.size(); ++i) {
			_input_handlers[i](this, MOUSE_DELTA_X, (float)_data.dx);
			_input_handlers[i](this, MOUSE_DELTA_Y, (float)_data.dy);
			_input_handlers[i](this, MOUSE_POS_X, (float)_data.x);
			_input_handlers[i](this, MOUSE_POS_Y, (float)_data.y);
			_input_handlers[i](this, MOUSE_WHEEL, (float)_data.wheel);
		}

		for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) {
			_data.buttons[i] = (_mouse_state.rgbButtons[i] & 0x80) != 0;

			for (unsigned int j = 0; j < _input_handlers.size(); ++j) {
				_input_handlers[j](this, i, (float)_data.buttons[i]);
			}
		}
#endif
	}

	return true;
}

const MouseData& MouseDI::getMouseData(void) const
{
	return _data;
}

void MouseDI::getPosition(int& x, int& y) const
{
	x = _data.x;
	y = _data.y;
}

void MouseDI::getDeltas(int& dx, int& dy) const
{
	dx = _data.dx;
	dy = _data.dy;
}

void MouseDI::getNormalizedPosition(float& nx, float& ny) const
{
	nx = (float)_data.x / (float)_window->getWidth();
	ny = (float)_data.y / (float)_window->getHeight();
}

void MouseDI::getNormalizedDeltas(float& ndx, float& ndy) const
{
	ndx = (float)_data.dx / (float)_window->getWidth();
	ndy = (float)_data.dy / (float)_window->getHeight();
}

short MouseDI::getWheelDelta(void) const
{
	return _data.wheel;
}

const GChar* MouseDI::getDeviceName(void) const
{
	return GC("Gleam:Mouse");
}

const GChar* MouseDI::getPlatformImplementationString(void) const
{
	return GC("Windows:DirectInput");
};

const Window* MouseDI::getAssociatedWindow(void) const
{
	return _window;
}

NS_END
