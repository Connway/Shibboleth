/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gleam_IInputHandler.h"
#include "Gleam_Array.h"

NS_GLEAM

typedef void (*InputHandler)(IInputDevice* device, unsigned int code, float value);

class Window;

class IInputDevice
{
public:
	IInputDevice(void) {}
	virtual ~IInputDevice(void) {}

	virtual bool init(const Window& window, void* compat = NULLPTR, bool no_windows_key = false) = 0;
	virtual bool update(void) = 0;

	void registerInputHandler(InputHandler handler)
	{
		_input_handlers_func.push(handler);
	}

	bool removeInputHandler(InputHandler handler)
	{
		int index = _input_handlers_func.linearFind(handler);

		if (index > -1) {
			_input_handlers_func.erase(index);
			return true;
		}

		return false;
	}

	void registerInputHandler(IInputHandler* handler)
	{
		_input_handlers_class.push(handler);
	}

	bool removeInputHandler(IInputHandler* handler)
	{
		int index = _input_handlers_class.linearFind(handler);

		if (index > -1) {
			_input_handlers_class.erase(index);
			return true;
		}

		return false;
	}

	virtual const GChar* getDeviceName(void) const = 0;
	virtual const GChar* getPlatformImplementationString(void) const = 0;

	virtual const Window* getAssociatedWindow(void) const = 0;

	virtual bool isKeyboard(void) const = 0;
	virtual bool isMouse(void) const = 0;

protected:
	GleamArray(IInputHandler*) _input_handlers_class;
	GleamArray(InputHandler) _input_handlers_func;

	GAFF_NO_COPY(IInputDevice);
};

NS_END
