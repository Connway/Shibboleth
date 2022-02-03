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

#include "Shibboleth_Mouse_Web.h"
#include <Gaff_Assert.h>

NS_SHIBBOLETH

MouseWeb::MouseWeb(void)
{
}

MouseWeb::~MouseWeb(void)
{
}

bool MouseWeb::init(Gleam::IWindow& /*window*/)
{
	GAFF_ASSERT(false);
	return false;
}

bool MouseWeb::init(void)
{
	return true;
}

void MouseWeb::destroy(void)
{
	Gleam::IMouse::destroy();

	memset(&_prev_data, 0, sizeof(_prev_data));
	_flags.clear();
}

void MouseWeb::update(void)
{
	if (areRepeatsAllowed()) {
		for (int32_t j = 0; j < static_cast<int32_t>(_input_handlers.size()); ++j) {
			_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::DeltaX), static_cast<float>(_data.delta.x));
			_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::DeltaY), static_cast<float>(_data.delta.y));
			_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::PosX), static_cast<float>(_data.rel_pos.x));
			_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::PosY), static_cast<float>(_data.rel_pos.y));

			for (int32_t i = 0; i < static_cast<int32_t>(Gleam::MouseCode::ButtonCount); ++i) {
				_input_handlers[j](this, i, static_cast<float>(_data.buttons[i]));
			}
		}

	} else {
		for (int32_t j = 0; j < static_cast<int32_t>(_input_handlers.size()); ++j) {
			if (_data.rel_pos.x != _prev_data.rel_pos.x) {
				_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::PosX), static_cast<float>(_data.rel_pos.x));
			}

			if (_data.rel_pos.y != _prev_data.rel_pos.y) {
				_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::PosY), static_cast<float>(_data.rel_pos.y));
			}

			if (_data.delta.x != _prev_data.delta.x) {
				_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::DeltaX), static_cast<float>(_data.delta.x));
			}

			if (_data.delta.y != _prev_data.delta.y) {
				_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::DeltaY), static_cast<float>(_data.delta.y));
			}

			if (_data.wheel.x != _prev_data.wheel.x) {
				_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::WheelHorizontal), static_cast<float>(_data.wheel.x));

				if (_data.wheel.x < 0) {
					_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::WheelLeft), fabsf(static_cast<float>(_data.wheel.x)));
				} else if (_data.wheel.x > 0) {
					_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::WheelRight), static_cast<float>(_data.wheel.x));
				}
			}

			if (_data.wheel.y != _prev_data.wheel.y) {
				_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::WheelVertical), static_cast<float>(_data.wheel.y));

				if (_data.wheel.y < 0) {
					_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::WheelDown), fabsf(static_cast<float>(_data.wheel.y)));
				} else if (_data.wheel.y > 0) {
					_input_handlers[j](this, static_cast<int32_t>(Gleam::MouseCode::WheelUp), static_cast<float>(_data.wheel.y));
				}
			}

			for (int32_t i = 0; i < static_cast<int32_t>(Gleam::MouseCode::ButtonCount); ++i) {
				if (_data.buttons[i] != _prev_data.buttons[i]) {
					_input_handlers[j](this, i, static_cast<float>(_data.buttons[i]));
				}
			}
		}

		_prev_data = _data;
		_data.delta.x = 0;
		_data.delta.y = 0;
	}
}

Gleam::IVec2 MouseWeb::getNormalizedAbsolutePosition(void) const
{
	GAFF_ASSERT(false);
	return Gleam::IVec2(0.0f, 0.0f);
}

Gleam::IVec2 MouseWeb::getNormalizedRelativePosition(void) const
{
	GAFF_ASSERT(false);
	return Gleam::IVec2(0.0f, 0.0f);
}

Gleam::IVec2 MouseWeb::getNormalizedDeltas(void) const
{
	GAFF_ASSERT(false);
	return Gleam::IVec2(0.0f, 0.0f);
}

void MouseWeb::allowRepeats(bool allow)
{
	_flags.set(allow, Flag::AllowRepeats);
}

bool MouseWeb::areRepeatsAllowed(void) const
{
	return _flags.testAll(Flag::AllowRepeats);
}

const char* MouseWeb::getDeviceName(void) const
{
	return "Shibboleth:MouseWeb";
}

const char* MouseWeb::getPlatformImplementationString(void) const
{
	return "Agnostic:Web";
}

const Gleam::IWindow* MouseWeb::getAssociatedWindow(void) const
{
	return nullptr;
}

bool MouseWeb::handleMessage(const Gleam::AnyMessage& message)
{
	switch (message.base.type) {
		case Gleam::EventType::InputMouseMove:
			_data.abs_pos.x = message.mouse_move.abs_x;
			_data.abs_pos.y = message.mouse_move.abs_y;
			_data.rel_pos.x = message.mouse_move.rel_x;
			_data.rel_pos.y = message.mouse_move.rel_y;
			_data.delta.x = message.mouse_move.dx;
			_data.delta.y = message.mouse_move.dy;
			return true;

		case Gleam::EventType::InputMouseDown:
			_data.buttons[static_cast<uint8_t>(message.mouse_state.button)] = true;
			return true;

		case Gleam::EventType::InputMouseUp:
			_data.buttons[static_cast<uint8_t>(message.mouse_state.button)] = false;
			return true;

		case Gleam::EventType::InputMouseWheelHorizontal:
			_data.wheel.x = message.mouse_state.wheel;
			return true;

		case Gleam::EventType::InputMouseWheelVertical:
			_data.wheel.y = message.mouse_state.wheel;
			return true;

		default:
			break;
	}

	return false;
}

NS_END
