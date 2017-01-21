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

#include "Gaff_Platform.h"

#if defined(PLATFORM_LINUX) || defined(PLATform_MAC)

#include "Gaff_Event_Linux.h"
#include <sys/time.h>

#define EVENT_SET (1 << 0)
#define EVENT_MANUAL_RESET (1 << 1)

NS_GAFF

unsigned long Event::INF = static_cast<unsigned long>(-1);

Event::Event(bool manual_reset, bool initial_state):
	_condition(0), _mutex(0),
	_flags(0)
{
	pthread_cond_init(&_condition, nullptr);
	pthread_mutex_init(&_mutex, 0);

	if (initial_state) {
		_flags |= EVENT_SET;
	}

	if (manual_reset) {
		_flags |= EVENT_MANUAL_RESET;
	}
}

Event::~Event(void)
{
	if (_condition) {
		pthread_cond_destroy(&_condition);
	}

	if (_mutex) {
		pthread_mutex_destroy(&_mutex);
	}
}

bool Event::set(void)
{
	if (_flags & EVENT_MANUAL_RESET) {
		_flags |= EVENT_SET;
	}

	return !pthread_cond_broadcast(&_condition);
}

bool Event::reset(void)
{
	_flags &= ~EVENT_SET;
	return true;
}

Event::WaitCode Event::wait(unsigned long ms)
{
	int ret = 0;

	pthread_mutex_lock(&_mutex);

	if (ms == INF) {
		ret = pthread_cond_wait(&_condition, &_mutex);
	} else {
		timeval tv;
		gettimeofday(&tv, NULL);

		uint64_t ns = static_cast<uint64_t>(tv.tv_sec) * 1000 * 1000 * 1000 + ms * 1000 * 1000 + static_cast<uint64_t>(tv.tv_usec) * 1000;
		ts.tv_sec = ns / 1000 / 1000 / 1000;
		ts.tv_nsec = ns - static_cast<uint64_t>(ts.tv_sec) * 1000 * 1000 * 1000;

		ret = pthread_cond_timedwait(&_condition, &_mutex, &tv);
	}

	pthread_mutex_unlock(&_mutex);

	if (ret == ETIMEDOUT) {
		return EVENT_TIMEOUT;
	} else if (ret) {
		return EVENT_FAILED;
	}

	return EVENT_FINISHED;
}

NS_END

#endif
