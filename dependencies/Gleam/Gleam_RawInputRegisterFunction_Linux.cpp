/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#ifdef __linux__

#include "Gleam_RawInputRegisterFunction.h"
#include "Gleam_Window_Linux.h"
#include <X11/extensions/XInput2.h>

NS_GLEAM

bool gAlreadyQueried;
int gOpCode;
int gEvent;
int gError;

bool RegisterForRawInput(unsigned int device, const IWindow& window)
{
	assert(device >= RAW_INPUT_MOUSE && device <= RAW_INPUT_KEYBOARD);

	const Window& wnd = (const Window&)window;

	if (!gAlreadyQueried) {
		if (!XQueryExtension(wnd.getDisplay(), "XInputExtension", &gOpCode, &gEvent, &gError)) {
			return false;
		}

		int major = 2;
		int minor = 0;
		if (XIQueryVersion(wnd.getDisplay(), &major, &minor) != Success) {
			return false;
		}
	}

	unsigned char masks[3] = { 0 };
	XIEventMask event_mask;
	event_mask.mask_len = sizeof(masks);
	event_mask.mask = masks;

	int mouse = -1;
	int keyboard = -1;
	// int gamepad = -1;
	// int joystick = -1;

	int num_devices;
	XIDeviceInfo* devices;

	devices = XIQueryDevice(wnd.getDisplay(), XIAllMasterDevices, &num_devices);

	for (int i = 0; i < num_devices; ++i) {
		switch(devices[i].use) {
			case XIMasterPointer: 
				mouse = devices[i].deviceid;
				break;
	
			case XIMasterKeyboard:
				keyboard = devices[i].deviceid;
				break;
		}
	}

	XIFreeDeviceInfo(devices);

	switch (device) {
		case RAW_INPUT_MOUSE:
			event_mask.deviceid = mouse;
			XISetMask(masks, XI_ButtonRelease);
			XISetMask(masks, XI_ButtonPress);
			break;

		case RAW_INPUT_JOYSTICK:
			return false;
			break;

		case RAW_INPUT_GAMEPAD:
			return false;
			break;

		case RAW_INPUT_KEYBOARD:
			event_mask.deviceid = keyboard;
			XISetMask(masks, XI_KeyRelease);
			XISetMask(masks, XI_KeyPress);
			break;
	}

	if (XISelectEvents(wnd.getDisplay(), wnd.getWindow(), &event_mask, 1) != Success) {
		return false;
	}

	// We need to use the root window to get raw motion events
	if (device == RAW_INPUT_MOUSE) {
		XIClearMask(masks, XI_ButtonRelease);
		XIClearMask(masks, XI_ButtonPress);
		XISetMask(masks, XI_RawMotion);

		if (XISelectEvents(wnd.getDisplay(), DefaultRootWindow(wnd.getDisplay()), &event_mask, 1) != Success) {
			return false;
		}
	}

	return true;
}

NS_END

#endif
