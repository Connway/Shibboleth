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

#if defined(__linux__) || defined(__APPLE__)

#include "Gaff_Thread_Linux.h"
#include <errno.h>
#include <sys/time.h>

NS_GAFF

unsigned int Thread::INF = (unsigned int)-1;

// to make GCC stop complaining
static pthread_mutex_t MUTEX_NULL = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t COND_NULL = PTHREAD_COND_INITIALIZER;

Thread::Thread(Thread&& thread):
	_thread(thread._thread), _mutex(thread._mutex), _cond(thread._cond)
{
	thread._thread = 0;
	thread._mutex = MUTEX_NULL;
	thread._cond = COND_NULL;
}

Thread::Thread(void):
	_thread(0), _mutex(MUTEX_NULL), _cond(COND_NULL)
{
}

Thread::~Thread(void)
{
	if (_thread) {
		close();
	}
}

bool Thread::create(ThreadFunc thread_func, void* thread_data)
{
	return pthread_create(&_thread, 0, thread_func, thread_data) == 0;
}

bool Thread::terminate(void)
{
	return pthread_cancel(_thread) == 0;
}

bool Thread::close(void)
{
	bool ret = pthread_detach(_thread) == 0;
	_thread = 0;
	return ret;
}

Thread::WaitCode Thread::wait(unsigned int ms)
{
	WaitCode ret = THREAD_FINISHED;

	if (ms == INF) {
		ret = (pthread_join(_thread, 0)) ? THREAD_FAILED : THREAD_FINISHED;

	} else {
		// Might want to find a better way of getting the final timespec struct
		// calculate time to wait for
		timeval time;
		gettimeofday(&time, 0);

		time.tv_sec += ms / 1000;
		time.tv_usec += (ms % 1000) * 1000;

		timespec temp = { time.tv_sec, time.tv_usec * 1000 };

		pthread_mutex_lock(&_mutex);
		int r = pthread_cond_timedwait(&_cond, &_mutex, &temp);
		pthread_mutex_unlock(&_mutex);

		switch (r) {
			case ETIMEDOUT:
				ret = THREAD_TIMEOUT;
				break;

			case EINVAL:
				ret = THREAD_FAILED;
				break;

			default:
				ret = THREAD_FINISHED;
		}
	}

	return ret;
}

const Thread& Thread::operator=(Thread&& rhs)
{
	if (_thread) {
		close();
	}

	_thread = rhs._thread;
	_mutex = rhs._mutex;
	_cond = rhs._cond;

	rhs._thread = 0;
	rhs._mutex = MUTEX_NULL;
	rhs._cond = COND_NULL;

	return *this;
}

NS_END

#endif
