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

/*! \file */

#pragma once

#include "Gaff_Defines.h"
#include <pthread.h>
#include <sched.h>

#define THREAD_CALLTYPE //!< The calling convention used for a thread function.

NS_GAFF

/*!
	\brief A simple thread wrapper class.
*/
class Thread
{
public:
	static unsigned int GetCurrentThreadID(void);
	static unsigned int INF; //!< Value for infinite waiting period.

	typedef void* ReturnType; //!< The return type used by thread functions.
	typedef ReturnType (THREAD_CALLTYPE *ThreadFunc)(void*);

	//! The return values used by wait().
	enum WaitCode
	{
		THREAD_FINISHED = 0,	//!< Thread has finished executing
		THREAD_TIMEOUT,			//!< Thread is still running and has timed out
		THREAD_FAILED			//!< Something went wrong!
	};

	Thread(Thread&& thread);
	Thread(void);
	~Thread(void);

	bool create(ThreadFunc thread_func, void* thread_data = 0);
	bool terminate(void);
	bool close(void);
	WaitCode wait(unsigned int ms = INF);
	unsigned int getID(void) const;

	const Thread& operator=(Thread&& thread);

private:
	pthread_t _thread;
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;

	GAFF_NO_COPY(Thread);
};

NS_END
