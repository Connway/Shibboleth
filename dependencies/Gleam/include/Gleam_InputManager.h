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

#include "Gleam_IInputDevice.h"
#include "Gleam_Array.h"

#ifdef USE_DI
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#include <dinput.h>
#else
	#define IDirectInput8 void
#endif

NS_GLEAM

class Window;

class InputManager
{
public:
	InputManager(void);
	~InputManager(void);

	bool init(const Window& window);
	void destroy(void);

	bool update(void);

	INLINE void addInputDevice(IInputDevice* device);
	bool removeInputDevice(const IInputDevice* device);
	INLINE IInputDevice* getInputDevice(unsigned int i) const;

	INLINE IDirectInput8* getDirectInput(void) const;

	IInputDevice* createKeyboard(bool no_windows_key = false);
	IInputDevice* createMouse(void);

private:
	GleamArray(IInputDevice*) _input_devices;
	const Window* _window;

#ifdef USE_DI
	IDirectInput8* _dinput;
#endif

	GAFF_NO_COPY(InputManager);
};

NS_END
