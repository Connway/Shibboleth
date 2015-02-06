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

#include "Shibboleth_Utilities.h"
#include <Shibboleth_RefCounted.h>
#include <Shibboleth_String.h>
#include <Shibboleth_ITask.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Atomic.h>

NS_SHIBBOLETH

//static volatile unsigned int g_Current_Flush_Count = 0;
//static unsigned int g_Flush_Count = 10;
static IApp* g_App = nullptr;

class LogTask : public ITask
{
public:
	LogTask(LogManager::FileLockPair& flp, const char* string/*, bool flush*/):
		_string(string), _flp(flp)/*, _flush(flush)*/
	{
	}

	~LogTask(void) {}

	void doTask(void)
	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(*_flp.second);

		_flp.first.writeString(_string.getBuffer());

		//if (_flush) {
			_flp.first.flush();
		//}
	}

private:
	AString _string;
	LogManager::FileLockPair& _flp;
	//bool _flush;

	SHIB_REF_COUNTED(LogTask);
};

void PrintToLogTask(LogManager::FileLockPair& flp, unsigned int task_pool, const char* format, ...)
{
	assert(g_App && format && strlen(format));

	char temp[512] = { 0 };

	va_list vl;
	va_start(vl, format);
	vsprintf(temp, format, vl);
	va_end(vl);

	//unsigned int curr_flush_count = AtomicUAddFetchOrig(&g_Current_Flush_Count, 1);
	LogTask* log_task = GetAllocator()->allocT<LogTask>(flp, temp/*, !(curr_flush_count % g_Flush_Count)*/);

	if (log_task) {
		TaskPtr task(log_task);
		g_App->addTask(task, task_pool);
	}
}

//void PrintToLogFlushCount(unsigned int flush_count)
//{
//	g_Flush_Count = flush_count;
//}

void SetApp(IApp& app)
{
	g_App = &app;
}

IApp& GetApp(void)
{
	assert(g_App);
	return *g_App;
}

NS_END
