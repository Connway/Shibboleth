/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_LogManager.h"
#include <Shibboleth_Utilities.h>
#include "Shibboleth_IApp.h"
#include <EASTL/algorithm.h>
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

intptr_t LogManager::LogThread(void* args)
{
	LogManager& lm = *reinterpret_cast<LogManager*>(args);

	while (!lm._shutdown) {
		lm._log_lock.Wait();

		lm._log_queue_lock.Lock();

		if (!lm._logs.empty()) {
			LogTask task = std::move(lm._logs.front());
			lm._logs.pop();

			lm._log_queue_lock.Unlock();

			Gaff::File file(task.file);
			file.writeString(task.message.data());
			file.writeChar(u8'\n');
			file.flush();
			file.release(); // We don't want to close the file.

			lm.notifyLogCallbacks(task.message.data(), task.type);

		} else {
			lm._log_queue_lock.Unlock();
		}
	}

	return 0;
}

LogManager::LogManager(void):
	_shutdown(false)
{
}

LogManager::~LogManager(void)
{
	destroy();
}

bool LogManager::init(const char8_t* log_dir)
{
	_log_dir = log_dir;

	addChannel(HashStringView32<>(k_log_channel_name_default));

	EA::Thread::ThreadParameters thread_params;
	thread_params.mbDisablePriorityBoost = false;
	thread_params.mnAffinityMask = EA::Thread::kThreadAffinityMaskAny;
	thread_params.mnPriority = EA::Thread::kThreadPriorityDefault;
	thread_params.mnProcessor = EA::Thread::kProcessorDefault;
	thread_params.mnStackSize = 0;
	thread_params.mpName = "Log Thread";
	thread_params.mpStack = nullptr;

	return _log_thread.Begin(LogThread, this, &thread_params) != EA::Thread::kThreadIdInvalid;
}

void LogManager::destroy(void)
{
	_shutdown = true;

	if (_log_thread.GetId() != EA::Thread::kThreadIdInvalid) {
		_log_lock.Post();
		_log_thread.WaitForEnd();
	}

	_shutdown = false;

	_log_callbacks.clear();
	_channels.clear();
}

int32_t LogManager::addLogCallback(const LogCallback& callback)
{
	const EA::Thread::AutoMutex lock(_log_callback_lock);
	const int32_t id = _next_id++;
	_log_callbacks.emplace(id, callback);

	return id;
}

int32_t LogManager::addLogCallback(LogCallback&& callback)
{
	const EA::Thread::AutoMutex lock(_log_callback_lock);
	const int32_t id = _next_id++;
	_log_callbacks.emplace(id, std::move(callback));

	return id;
}

bool LogManager::removeLogCallback(int32_t id)
{
	const EA::Thread::AutoMutex lock(_log_callback_lock);
	const auto it = _log_callbacks.find(id);

	if (it != _log_callbacks.end()) {
		_log_callbacks.erase(it);
		return true;
	}
	
	return false;
}

void LogManager::addChannel(HashStringView32<> channel)
{
	auto it = Gaff::Find(_channels, channel);

	if (it == _channels.end()) {
		const U8String file_name(U8String::CtorSprintf(), u8"%s/%sLog.txt", _log_dir.data(), channel.getBuffer());
		auto pair = eastl::make_pair<HashString32<>, Gaff::File>(HashString32<>(channel), Gaff::File());

		if (pair.second.open(file_name.data(), Gaff::File::OpenMode::/*Write*/Append)) {
			_channels.insert(std::move(pair));

		} else {
			// If this is not the channel added in the constructor, then log the error in that channel.
			if (channel.getHash() != k_log_channel_default) {
				logMessage(
					LogType::Error,
					k_log_channel_default,
					u8"Failed to create channel '%s'! Failed to open file '%s'!",
					channel.getBuffer(),
					file_name.data()
				);
			}
		}
	}
}

void LogManager::logMessage(LogType type, Gaff::Hash32 channel, const char8_t* format, ...)
{
	va_list vl;
	va_start(vl, format);

	if (!logMessageHelper(type, channel, format, vl) && channel != k_log_channel_default) {
		logMessage(
			LogType::Error,
			k_log_channel_default,
			u8"Failed to find channel with hash '%u'!",
			channel
		);

		// Log the message to the default channel.
		logMessageHelper(type, k_log_channel_default, format, vl);
	}

	va_end(vl);
}

bool LogManager::logMessageHelper(LogType type, Gaff::Hash32 channel, const char8_t* format, va_list& vl)
{
	char8_t time_string[64] = { 0 };
	Gaff::GetCurrentTimeString(time_string, ARRAY_SIZE(time_string), u8"[%H-%M-%S] ");

	U8String message;
	message.sprintf_va_list(format, vl);

	auto it = Gaff::Find(_channels, channel);

	if (it == _channels.end()) {
		return false;
	}

	{
		const EA::Thread::AutoMutex lock(_log_queue_lock);
		_logs.emplace(LogTask{ it->second.getFile(), U8String(time_string) + message, type});
	}

	_log_lock.Post();

	const U8String debug_msg(U8String::CtorSprintf(), u8"[%s] %s\n", it->first.getBuffer(), message.data());
	Gaff::DebugPrintf(debug_msg.data());

	return true;
}

void LogManager::notifyLogCallbacks(const char8_t* message, LogType type)
{
	const EA::Thread::AutoMutex lock(_log_callback_lock);

	for (auto it = _log_callbacks.begin(); it != _log_callbacks.end(); ++it) {
		it->second(message, type);
	}
}

NS_END
