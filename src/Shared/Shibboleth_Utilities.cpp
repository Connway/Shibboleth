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

#include "Shibboleth_Utilities.h"
#include <Shibboleth_RefCounted.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_String.h>
#include <Shibboleth_IApp.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_Atomic.h>

NS_SHIBBOLETH

//static volatile unsigned int g_Current_Flush_Count = 0;
//static unsigned int g_Flush_Count = 10;
static IApp* gApp = nullptr;

struct LogData
{
	AString string;
	LogManager::FileLockPair* flp;
	LogManager::LOG_TYPE log_type;
	//bool flush;
};

static void LogJob(void* data)
{
	LogData* log_data = reinterpret_cast<LogData*>(data);

	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(*log_data->flp->second);

	log_data->flp->first.writeString(log_data->string.getBuffer());

	//if (flush) {
		log_data->flp->first.flush();
	//}

	gApp->notifyLogCallbacks(log_data->string.getBuffer(), log_data->log_type);

	GetAllocator()->freeT(log_data);
}

void LogMessage(LogManager::FileLockPair& flp, unsigned int job_pool, LogManager::LOG_TYPE log_type, const char* format, ...)
{
	GAFF_ASSERT(gApp && format && strlen(format));

	char temp[512] = { 0 };

	va_list vl;
	va_start(vl, format);
	vsprintf(temp, format, vl);
	va_end(vl);

	//unsigned int curr_flush_count = AtomicUAddFetchOrig(&g_Current_Flush_Count, 1);
	LogData* log_data = GetAllocator()->template allocT<LogData>();

	if (log_data) {
		log_data->string = temp;
		log_data->flp = &flp;
		log_data->log_type = log_type;
		//log_data->flush = !(curr_flush_count % g_Flush_Count);

		Gaff::JobData job_data(&LogJob, log_data);
		gApp->getJobPool().addJobs(&job_data, 1, nullptr, job_pool);
	}
}

//void PrintToLogFlushCount(unsigned int flush_count)
//{
//	g_Flush_Count = flush_count;
//}

void SetApp(IApp& app)
{
	gApp = &app;
}

IApp& GetApp(void)
{
	GAFF_ASSERT(gApp);
	return *gApp;
}

NS_END
