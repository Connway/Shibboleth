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

#include "Gaff_Platform.h"

#ifdef PLATFORM_WINDOWS

#include "Gaff_Event_Windows.h"
#include "Gaff_Assert.h"

#define EVENT_SET (1 << 0)
#define EVENT_MANUAL_RESET (1 << 1)

NS_GAFF

unsigned long Event::INF = static_cast<unsigned long>(-1);

Event::Event(bool manual_reset, bool initial_state):
	_event(nullptr), _flags(0)
{
	if (initial_state) {
		_flags |= EVENT_SET;
	}

	if (manual_reset) {
		_flags |= EVENT_MANUAL_RESET;
	}

	_event = CreateEvent(NULL, manual_reset, initial_state, NULL);
}

Event::~Event(void)
{
	if (_event) {
		CloseHandle(_event);
	}
}

bool Event::set(void)
{
	if (_flags & EVENT_MANUAL_RESET) {
		_flags |= EVENT_SET;
	}

	return SetEvent(_event) == TRUE;
}

bool Event::reset(void)
{
	_flags &= ~EVENT_SET;
	return ResetEvent(_event) == TRUE;
}

Event::WaitCode Event::wait(unsigned long ms)
{
	DWORD ret = WaitForSingleObject(_event, ms);

	switch (ret) {
		case WAIT_TIMEOUT:
			return EVENT_TIMEOUT;

		case WAIT_ABANDONED:
		case WAIT_FAILED:
			return EVENT_FAILED;

		default:
			return EVENT_FINISHED;
	}
}

NS_END

#endif
