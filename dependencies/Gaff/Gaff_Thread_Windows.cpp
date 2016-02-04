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

#if defined(_WIN32) || defined(_WIN64)

#include "Gaff_Thread_Windows.h"

NS_GAFF

/*!
	\brief Returns the thread ID of the callee.
*/
unsigned int Thread::GetCurrentThreadID(void)
{
	return GetCurrentThreadId();
}

unsigned int Thread::INF = UINT_FAIL;

/*!
	\note Move semantics supported to allow pushing Thread instances into containers.
*/
Thread::Thread(Thread&& thread):
	_thread(thread._thread)
{
	thread._thread = nullptr;
}

Thread::Thread(void):
	_thread(nullptr)
{
}

Thread::~Thread(void)
{
	if (_thread) {
		close();
	}
}

/*!
	\brief Creates a thread and starts running \a thread_func immediately.
	\param thread_func The function to run on the thread.
	\param thread_data The data to pass to the thread.
	\return Whether the thread was successfully created.
*/
bool Thread::create(Thread::ThreadFunc thread_func, void* thread_data)
{
	_thread = CreateThread(nullptr, 0, thread_func, thread_data, 0, nullptr);
	return (_thread) ? true : false;
}

/*!
	\brief Forcefully terminates the thread.
	\return Whether the thread was successfully terminated.
*/
bool Thread::terminate(void)
{
	return (TerminateThread(_thread, 0) == FALSE) ? false : true;
}

/*!
	\brief Closes and cleans up a finished thread and its resources.
	\return Whether the thread was successfully closed and cleaned up.
*/
bool Thread::close(void)
{
	bool ret = (CloseHandle(_thread) == FALSE) ? false : true;
	_thread = nullptr;
	return ret;
}

/*!
	\brief Waits for a thread for \a ms milliseconds or until it finishes executing, whichever comes first.
	\param ms The time to wait on the thread in milliseconds.
	\return The result of waiting on the thread.
*/
Thread::WaitCode Thread::wait(unsigned int ms)
{
	DWORD time = (ms == INF) ? INFINITE : ms;
	DWORD ret = WaitForSingleObject(_thread, time);

	switch(ret) {
		case WAIT_TIMEOUT:
			return THREAD_TIMEOUT;

		case WAIT_FAILED:
			return THREAD_FAILED;

		default:
			return THREAD_FINISHED;
	}
}

/*!
	\brief Returns the ID of the created thread.
*/
unsigned int Thread::getID(void) const
{
	return GetThreadId(_thread);
}

/*!
	\note Move semantics supported to allow pushing Thread instances into containers.
*/
const Thread& Thread::operator=(Thread&& rhs)
{
	_thread = rhs._thread;
	rhs._thread = nullptr;

	return *this;
}

NS_END

#endif
