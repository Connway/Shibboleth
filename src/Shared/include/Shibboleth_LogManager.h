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

#pragma once

#include "Shibboleth_String.h"
#include "Shibboleth_Vector.h"
#include <Gaff_Function.h>
#include <Gaff_SpinLock.h>
#include <Gaff_Thread.h>
#include <Gaff_Event.h>

NS_SHIBBOLETH

class LogManager
{
public:
	enum LOG_TYPE
	{
		LOG_NORMAL = 0,
		LOG_WARNING,
		LOG_ERROR
	};

	typedef Gaff::FunctionBinder<void, const char*, LOG_TYPE> LogCallback;

	LogManager(void);
	~LogManager(void);

	bool init(void);
	void destroy(void);

	INLINE void addLogCallback(const LogCallback& callback);
	INLINE void removeLogCallback(const LogCallback& callback);
	void notifyLogCallbacks(const char* message, LOG_TYPE type);

	void logMessage(LOG_TYPE type, const char* file, const char* format, ...);

private:
	struct LogTasks
	{
		LogTasks(LOG_TYPE t, const char* lf, const char* msg):
			log_file(lf), message(msg), type(t)
		{
		}

		U8String log_file;
		U8String message;
		LOG_TYPE type;
	};

	bool _shutdown;

	Vector<LogCallback> _log_callbacks;
	Vector<LogTasks> _logs;
	Gaff::SpinLock _log_queue_lock;

	Gaff::Event _log_event;
	Gaff::Thread _log_thread;

	static Gaff::Thread::ReturnType THREAD_CALLTYPE LogThread(void* thread_data);

	GAFF_NO_COPY(LogManager);
	GAFF_NO_MOVE(LogManager);
};

NS_END
