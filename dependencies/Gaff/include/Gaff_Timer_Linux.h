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

/*! \file */

#pragma once

#include "Gaff_Defines.h"
#include <time.h>

NS_GAFF

/*!
	\brief High-performance timer class.
*/
class Timer
{
public:
	Timer(void);

	INLINE bool start(void);
	bool stop(void);

	INLINE double getDeltaSec(void) const;
	INLINE long long getDeltaMilli(void) const;
	INLINE long long getDeltaMicro(void) const;

	INLINE double getCurrSec(void) const;
	INLINE long long getCurrMilli(void) const;
	long long getCurrMicro(void) const;

	INLINE double getTotalTime(void) const;

private:
	timespec _start;
	timespec _stop;
	long long _deltaTime;

	double _totalTime;
};

NS_END
