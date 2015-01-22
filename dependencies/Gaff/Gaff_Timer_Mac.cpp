/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#ifdef __APPLE__

#include "Gaff_Timer_Mac.h"
#include <mach/clock.h>

NS_GAFF

Timer::Timer(void):
	_deltaTime(0), _totalTime(0.0)
{
	// if it fails, oh well
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &_clock_serv);
}

bool Timer::start(void)
{
	return clock_get_time(_clock_serv, &_start) == KERN_SUCCESS;
}

bool Timer::stop(void)
{
	if (clock_get_time(_clock_serv, &_stop) != KERN_SUCCESS) {
		return false;
	}

	_deltaTime = (_stop.tv_sec - _start.tv_sec) * 1000000 + (long long)((double)(_stop.tv_nsec - _start.tv_nsec) * 0.001);
	_totalTime += getDeltaSec();

	return true;
}

/*!
	\brief Returns the delta time between start()/stop() calls in seconds.
	\note Delta time is cached, so calling in between start()/stop() calls will not have adverse effects.
*/
double Timer::getDeltaSec(void) const
{
	return (double)getDeltaMicro() * 0.000001;
}

/*!
	\brief Returns the delta time between start()/stop() calls in milliseconds.
	\note Delta time is cached, so calling in between start()/stop() calls will not have adverse effects.
*/
long long Timer::getDeltaMilli(void) const
{
	return getDeltaMicro() / 1000;
}

/*!
	\brief Returns the delta time between start()/stop() calls in microseconds.
	\note Delta time is cached, so calling in between start()/stop() calls will not have adverse effects.
*/
long long Timer::getDeltaMicro(void) const
{
	return _deltaTime;
}

/*!
	\brief Gets the current time from the epoch measured in seconds.
*/
double Timer::getCurrSec(void) const
{
	return (double)getCurrMicro() * 0.000001;
}

/*!
	\brief Gets the current time from the epoch measured in milliseconds.
*/
long long Timer::getCurrMilli(void) const
{
	return getCurrMicro() / 1000;
}

/*!
	\brief Gets the current time from the epoch measured in microseconds.
*/
long long Timer::getCurrMicro(void) const
{
	mach_timespec_t temp;

	if (clock_get_time(_clock_serv, &temp) != KERN_SUCCESS) {
		return -1;
	}

	return temp.tv_sec * 1000000 + (long long)((double)temp.tv_nsec * 0.001);
}

/*!
	\brief Gets the total amount of time measured across all start()/stop() calls measured in seconds.
*/
double Timer::getTotalTime(void) const
{
	return _totalTime;
}

/*!
	\brief Resets the total time to zero.
*/
void Timer::resetTotalTime(void)
{
	_totalTime = 0.0f;
}

NS_END

#endif
