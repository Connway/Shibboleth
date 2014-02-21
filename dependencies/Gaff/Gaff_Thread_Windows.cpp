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

#include "Gaff_Thread_Windows.h"

NS_GAFF

unsigned int Thread::INF = (unsigned int)-1;

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

bool Thread::create(Thread::ThreadFunc thread_func, void* thread_data)
{
	_thread = CreateThread(nullptr, 0, thread_func, thread_data, 0, nullptr);
	return (_thread) ? true : false;
}

bool Thread::terminate(void)
{
	return (TerminateThread(_thread, 0) == FALSE) ? false : true;
}

bool Thread::close(void)
{
	bool ret = (CloseHandle(_thread) == FALSE) ? false : true;
	_thread = nullptr;
	return ret;
}

Thread::WaitCode Thread::wait(unsigned int ms)
{
	DWORD time = (ms == INF) ? INFINITE : ms;
	DWORD ret = WaitForSingleObject(_thread, time);

	switch(ret) {
		case WAIT_TIMEOUT:
			return THREAD_TIMEOUT;
			break;

		case WAIT_FAILED:
			return THREAD_FAILED;
			break;

		default:
			return THREAD_FINISHED;
	}
}

NS_END

#endif
