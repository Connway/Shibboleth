/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_HashString.h"
#include "Shibboleth_VectorMap.h"
#include "Shibboleth_Vector.h"
#include "Shibboleth_String.h"
#include "Shibboleth_Queue.h"
#include <Gaff_File.h>
#include <EAThread/eathread_condition.h>
#include <EAThread/eathread_thread.h>
#include <EAThread/eathread_mutex.h>
#include <EASTL/functional.h>

NS_SHIBBOLETH

enum class LogType
{
	Normal = 0,
	Warning,
	Error
};

class LogManager final
{
public:
	using LogCallback = eastl::function<void (const char*, LogType)>;

	LogManager(void);
	~LogManager(void);

	bool init(const char* log_dir);
	void destroy(void);

	int32_t addLogCallback(const LogCallback& callback);
	int32_t addLogCallback(LogCallback&& callback);
	bool removeLogCallback(int32_t id);

	void addChannel(HashStringView32<> channel, const char* file);
	void logMessage(LogType type, Gaff::Hash32 channel, const char* format, ...);

private:
	struct LogTask
	{
		LogTask(Gaff::File& f, U8String&& m, LogType t):
			file(f.getFile()), message(std::move(m)), type(t)
		{
		}

		FILE* const file;
		U8String message;
		LogType type;
	};

	bool _shutdown;
	EA::Thread::Condition _log_event;

	VectorMap<HashString32<>, Gaff::File> _channels{ ProxyAllocator("Log") };
	VectorMap<int32_t, LogCallback> _log_callbacks{ ProxyAllocator("Log") };
	Queue<LogTask> _logs{ ProxyAllocator("Log") };

	int32_t _next_id = 0;

	EA::Thread::Mutex _log_condition_lock;
	EA::Thread::Mutex _log_callback_lock;
	EA::Thread::Mutex _log_queue_lock;

	EA::Thread::Thread _log_thread;

	U8String _log_dir = "./logs";


	bool logMessageHelper(LogType type, Gaff::Hash32 channel, const char* format, va_list& vl);
	void notifyLogCallbacks(const char* message, LogType type);

	static intptr_t LogThread(void* args);

	GAFF_NO_COPY(LogManager);
	GAFF_NO_MOVE(LogManager);
};

static constexpr Gaff::Hash32 k_log_channel_default = Gaff::FNV1aHash32Const("Default");

NS_END

#define LogWithApp(app, type, channel, message, ...) app.getLogManager().logMessage(type, channel, message, ##__VA_ARGS__)

#define LogType(type, channel, message, ...) LogWithApp(Shibboleth::GetApp(), type, channel, message, ##__VA_ARGS__)
#define LogError(channel, message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Error, channel, "[ERROR] " message, ##__VA_ARGS__)
#define LogWarning(channel, message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Warning, channel, "[WARNING] " message, ##__VA_ARGS__)
#define LogInfo(channel, message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Normal, channel, message, ##__VA_ARGS__)
#define LogErrorDefault(message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Error, Shibboleth::k_log_channel_default, "[ERROR] " message, ##__VA_ARGS__)
#define LogWarningDefault(message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Warning, Shibboleth::k_log_channel_default, "[WARNING] " message, ##__VA_ARGS__)
#define LogInfoDefault(message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Normal, Shibboleth::k_log_channel_default, message, ##__VA_ARGS__)
#define LogDefault(type, message, ...) LogWithApp(Shibboleth::GetApp(), type, Shibboleth::k_log_channel_default, message, ##__VA_ARGS__)

#define LogAndReturn(return_value, type, channel, message, ...) LogWithApp(Shibboleth::GetApp(), type, channel, message, ##__VA_ARGS__); return return_value
#define LogErrorAndReturn(return_value, channel, message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Error, channel, message, ##__VA_ARGS__); return return_value
#define LogWarningAndReturn(return_value, channel, message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Warning, channel, message, ##__VA_ARGS__); return return_value
#define LogInfoAndReturn(return_value, channel, message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Normal, channel, message, ##__VA_ARGS__); return return_value
#define LogErrorAndReturnDefault(return_value, message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Error, Shibboleth::k_log_channel_default, message, ##__VA_ARGS__); return return_value
#define LogWarningAndReturnDefault(return_value, message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Warning, Shibboleth::k_log_channel_default, message, ##__VA_ARGS__); return return_value
#define LogInfoAndReturnDefault(return_value, message, ...) LogWithApp(Shibboleth::GetApp(), Shibboleth::LogType::Normal, Shibboleth::k_log_channel_default, message, ##__VA_ARGS__); return return_value
