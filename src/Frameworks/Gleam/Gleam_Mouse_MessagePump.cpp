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

#include "Gleam_Mouse_MessagePump.h"
#include "Gleam_RawInputRegisterFunction.h"
#include "Gleam_Window.h"

NS_GLEAM

MouseMP::MouseMP(void)
{
}

MouseMP::~MouseMP(void)
{
	destroy();
}

bool MouseMP::init(IWindow& window)
{
	auto cb = Gaff::MemberFunc(this, &MouseMP::handleMessage);
	_window = &window;
	_id = _window->addWindowMessageHandler(cb);

	return RegisterForRawInput(RAW_INPUT_MOUSE, &window);
}

bool MouseMP::init(void)
{
	auto cb = Gaff::MemberFunc(this, &MouseMP::handleMessage);
	_id = Window::AddGlobalMessageHandler(cb);

	_flags.set(true, Flag::GlobalHandler);
	return RegisterForRawInput(RAW_INPUT_MOUSE, nullptr);
}

void MouseMP::destroy(void)
{
	auto cb = Gaff::MemberFunc(this, &MouseMP::handleMessage);

	if (_window) {
		_window->removeWindowMessageHandler(_id);
		_window = nullptr;

	} else {
		Window::RemoveGlobalMessageHandler(_id);
	}

	_flags.clear();
}

void MouseMP::update(void)
{
	if (areRepeatsAllowed()) {
		for (int32_t j = 0; j < static_cast<int32_t>(_input_handlers.size()); ++j) {
			_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_DELTA_X), static_cast<float>(_data.delta.x));
			_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_DELTA_Y), static_cast<float>(_data.delta.y));
			_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_POS_X), static_cast<float>(_data.rel_pos.x));
			_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_POS_Y), static_cast<float>(_data.rel_pos.y));

			for (int32_t i = 0; i < static_cast<int32_t>(MouseCode::MOUSE_BUTTON_COUNT); ++i) {
				_input_handlers[j](this, i, static_cast<float>(_data.buttons[i]));
			}
		}

	} else {
		for (int32_t j = 0; j < static_cast<int32_t>(_input_handlers.size()); ++j) {
			if (_data.rel_pos.x != _prev_data.rel_pos.x) {
				_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_POS_X), static_cast<float>(_data.rel_pos.x));
			}

			if (_data.rel_pos.y != _prev_data.rel_pos.y) {
				_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_POS_Y), static_cast<float>(_data.rel_pos.y));
			}

			if (_data.delta.x != _prev_data.delta.x) {
				_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_DELTA_X), static_cast<float>(_data.delta.x));
			}

			if (_data.delta.y != _prev_data.delta.y) {
				_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_DELTA_Y), static_cast<float>(_data.delta.y));
			}

			if (_data.wheel.x != _prev_data.wheel.x) {
				_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_WHEEL_HORIZ), static_cast<float>(_data.wheel.x));

				if (_data.wheel.x < 0) {
					_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_WHEEL_LEFT), fabsf(static_cast<float>(_data.wheel.x)));
				}
				else if (_data.wheel.x > 0) {
					_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_WHEEL_RIGHT), static_cast<float>(_data.wheel.x));
				}
			}

			if (_data.wheel.y != _prev_data.wheel.y) {
				_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_WHEEL_VERT), static_cast<float>(_data.wheel.y));

				if (_data.wheel.y < 0) {
					_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_WHEEL_DOWN), fabsf(static_cast<float>(_data.wheel.y)));
				} else if (_data.wheel.y > 0) {
					_input_handlers[j](this, static_cast<int32_t>(MouseCode::MOUSE_WHEEL_UP), static_cast<float>(_data.wheel.y));
				}
			}

			for (int32_t i = 0; i < static_cast<int32_t>(MouseCode::MOUSE_BUTTON_COUNT); ++i) {
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

glm::ivec2 MouseMP::getNormalizedAbsolutePosition(void) const
{
	return _data.abs_pos / _window->getSize();
}

glm::ivec2 MouseMP::getNormalizedRelativePosition(void) const
{
	return _data.rel_pos / _window->getSize();
}

glm::ivec2 MouseMP::getNormalizedDeltas(void) const
{
	return _data.delta / _window->getSize();
}

void MouseMP::allowRepeats(bool allow)
{
	_flags.set(allow, Flag::AllowRepeats);
}

bool MouseMP::areRepeatsAllowed(void) const
{
	return _flags.testAll(Flag::AllowRepeats);
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
		case EventType::IN_MOUSEMOVE:
			_data.abs_pos.x = message.mouse_move.abs_x;
			_data.abs_pos.y = message.mouse_move.abs_y;
			_data.rel_pos.x = message.mouse_move.rel_x;
			_data.rel_pos.y = message.mouse_move.rel_y;
			_data.delta.x = message.mouse_move.dx;
			_data.delta.y = message.mouse_move.dy;
			return true;

		case EventType::IN_MOUSEDOWN:
			_data.buttons[static_cast<uint8_t>(message.mouse_state.button)] = true;
			return true;

		case EventType::IN_MOUSEUP:
			_data.buttons[static_cast<uint8_t>(message.mouse_state.button)] = false;
			return true;

		case EventType::IN_MOUSEHWHEEL:
			_data.wheel.x = message.mouse_state.wheel;
			return true;

		case EventType::IN_MOUSEWHEEL:
			_data.wheel.y = message.mouse_state.wheel;
			return true;

		// To get rid of pesky "case not handled" warnings in GCC
		default:
			break;
	}

	return false;
}

NS_END
