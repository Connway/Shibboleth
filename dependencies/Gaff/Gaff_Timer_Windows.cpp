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

#if defined(_WIN32) || defined(_WIN64)

#include "Gaff_Timer_Windows.h"

NS_GAFF

static long long CalculateMicroSec(const LARGE_INTEGER& time, const LARGE_INTEGER& frequency)
{
	return (time.QuadPart * 1000000) / frequency.QuadPart;
}

Timer::Timer(void):
	_deltaTime(0), _totalTime(0.0)
{
	QueryPerformanceFrequency(&_frequency);
}

bool Timer::start(void)
{
	return QueryPerformanceCounter(&_start) == TRUE;
}

bool Timer::stop(void)
{
	if (QueryPerformanceCounter(&_stop) == TRUE) {
		_deltaTime = (_stop.QuadPart - _start.QuadPart) * 1000000;
		_deltaTime /= _frequency.QuadPart;

		_totalTime += getDeltaSec();

		return true;
	}

	return false;
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
	LARGE_INTEGER temp;
	QueryPerformanceCounter(&temp);
	return CalculateMicroSec(temp, _frequency);
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
