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

unsigned int Thread::INF = UINT_FAIL;

// to make GCC stop complaining
static pthread_mutex_t MUTEX_NULL = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t COND_NULL = PTHREAD_COND_INITIALIZER;

/*!
	\note Move semantics supported to allow pushing Thread instances into containers.
*/
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
	close();
}

/*!
	\brief Creates a thread and starts running \a thread_func immediately.
	\param thread_func The function to run on the thread.
	\param thread_data The data to pass to the thread.
	\return Whether the thread was successfully created.
*/
bool Thread::create(ThreadFunc thread_func, void* thread_data)
{
	return pthread_create(&_thread, 0, thread_func, thread_data) == 0;
}

/*!
	\brief Forcefully terminates the thread.
	\return Whether the thread was successfully terminated.
*/
bool Thread::terminate(void)
{
	return pthread_cancel(_thread) == 0;
}

/*!
	\brief Closes and cleans up a finished thread and its resources.
	\return Whether the thread was successfully closed and cleaned up.
*/
bool Thread::close(void)
{
	if (_thread) {
		bool ret = pthread_detach(_thread) == 0;
		_thread = 0;
		_mutex = MUTEX_NULL;
		_cond = COND_NULL;
		return ret;
	}

	return true;
}

/*!
	\brief Waits for a thread for \a ms milliseconds or until it finishes executing, whichever comes first.
	\param ms The time to wait on the thread in milliseconds.
	\return The result of waiting on the thread.
*/
Thread::WaitCode Thread::wait(unsigned int ms)
{
	if (ms == INF) {
		WaitCode ret = (pthread_join(_thread, 0)) ? THREAD_FAILED : THREAD_FINISHED;

		// Reset thread data, since pthread_join() cleaned up the thread for us.
		_thread = 0;
		_mutex = MUTEX_NULL;
		_cond = COND_NULL;

		return ret;

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
				return THREAD_TIMEOUT;

			case EINVAL:
				return THREAD_FAILED;

			default:
				return THREAD_FINISHED;
		}
	}
}

/*!
	\note Move semantics supported to allow pushing Thread instances into containers.
*/
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
