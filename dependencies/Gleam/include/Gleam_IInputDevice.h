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

#pragma once

#include "Gleam_Array.h"
#include <Gaff_Function.h>

NS_GLEAM

class IInputDevice;
class Window;

typedef void (*InputHandler)(IInputDevice*, unsigned int, float);

class IInputDevice
{
public:
	template <class T>
	INLINE void registerInputHandler(T* object, void (T::*cb)(IInputDevice*, unsigned int, float))
	{
		Gaff::FunctionBinder<void, IInputDevice*, unsigned int, float> function = Gaff::Bind(object, cb); 
		registerInputHandlerHelper(function);
	}

	template <class T>
	INLINE bool removeInputHandler(T* object, void (T::*cb)(IInputDevice*, unsigned int, float))
	{
		Gaff::FunctionBinder<void, IInputDevice*, unsigned int, float> function = Gaff::Bind(object, cb); 
		return removeInputHandlerHelper(function);
	}

	template <class T>
	INLINE void registerInputHandler(const T& functor)
	{
		Gaff::FunctionBinder<void, IInputDevice*, unsigned int, float> function = Gaff::Bind(functor); 
		registerInputHandlerHelper(function);
	}

	template <class T>
	INLINE bool removeInputHandler(const T& functor)
	{
		Gaff::FunctionBinder<void, IInputDevice*, unsigned int, float> function = Gaff::Bind(functor); 
		return removeInputHandlerHelper(function);
	}

	INLINE void registerInputHandler(InputHandler handler)
	{
		Gaff::FunctionBinder<void, IInputDevice*, unsigned int, float> function = Gaff::Bind(handler); 
		registerInputHandlerHelper(function);
	}

	INLINE bool removeInputHandler(InputHandler handler)
	{
		Gaff::FunctionBinder<void, IInputDevice*, unsigned int, float> function = Gaff::Bind(handler); 
		return removeInputHandlerHelper(function);
	}

	IInputDevice(void) {}
	virtual ~IInputDevice(void) {}

	virtual bool init(const Window& window) = 0;
	virtual void destroy(void) = 0;

	virtual void update(void) = 0;

	virtual const GChar* getDeviceName(void) const = 0;
	virtual const GChar* getPlatformImplementationString(void) const = 0;

	virtual const Window* getAssociatedWindow(void) const = 0;

	virtual bool isKeyboard(void) const = 0;
	virtual bool isMouse(void) const = 0;

protected:
	GleamArray< Gaff::FunctionBinder<void, IInputDevice*, unsigned int, float> > _input_handlers;

	INLINE void registerInputHandlerHelper(const Gaff::FunctionBinder<void, IInputDevice*, unsigned int, float>& cb)
	{
		_input_handlers.push(cb);
	}

	bool removeInputHandlerHelper(const Gaff::FunctionBinder<void, IInputDevice*, unsigned int, float>& cb)
	{
		auto it = _input_handlers.linearSearch(cb);

		if (it != _input_handlers.end()) {
			_input_handlers.erase(it);
			return true;
		}

		return false;
	}

	GAFF_NO_COPY(IInputDevice);
	GAFF_NO_MOVE(IInputDevice);
};

NS_END
