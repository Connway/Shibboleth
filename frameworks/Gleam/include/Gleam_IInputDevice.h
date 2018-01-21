/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gleam_Function.h"
#include "Gleam_Vector.h"

NS_GLEAM

class IInputDevice;
class IWindow;

using InputHandler = void (*)(IInputDevice*, int32_t, float);

class IInputDevice
{
public:
	using InputHandler = eastl::function<void (IInputDevice*, int32_t, float)>;

	IInputDevice(void) {}
	virtual ~IInputDevice(void) {}

	virtual bool init(IWindow& window) = 0; // Specific window init
	virtual bool init(void) = 0; // Global handler init
	virtual void destroy(void) = 0;

	virtual void update(void) = 0;

	virtual void allowRepeats(bool allow) = 0;
	virtual bool areRepeatsAllowed(void) const = 0;

	virtual const char* getDeviceName(void) const = 0;
	virtual const char* getPlatformImplementationString(void) const = 0;

	virtual const IWindow* getAssociatedWindow(void) const = 0;

	virtual bool isKeyboard(void) const = 0;
	virtual bool isMouse(void) const = 0;

	void addInputHandler(const InputHandler& cb)
	{
		_input_handlers.emplace_back(cb);
	}

	void addInputHandler(InputHandler&& cb)
	{
		_input_handlers.emplace_back(std::move(cb));
	}

	bool removeInputHandler(const InputHandler& cb)
	{
		auto it = eastl::find(_input_handlers.begin(), _input_handlers.end(), cb);

		if (it != _input_handlers.end()) {
			_input_handlers.erase_unsorted(it);
			return true;
		}

		return false;
	}

protected:
	Vector<InputHandler> _input_handlers;

	GAFF_NO_COPY(IInputDevice);
	GAFF_NO_MOVE(IInputDevice);
};

NS_END
