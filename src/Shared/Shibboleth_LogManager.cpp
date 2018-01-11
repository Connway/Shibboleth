/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
#include <EASTL/algorithm.h>
#include <Gaff_Utils.h>
#include <mutex>

NS_SHIBBOLETH

void LogManager::LogThread(LogManager& lm)
{
	AllocatorThreadInit();

	while (!lm._shutdown) {
		lm._log_event.wait();

		lm._log_queue_lock.lock();

		if (lm._logs.empty()) {
			lm._log_queue_lock.unlock();

		} else {
			LogTask task = std::move(lm._logs.front());
			lm._logs.pop();

			lm._log_queue_lock.unlock();

			task.file.writeString(task.message.data());
			task.file.writeChar('\n');
			task.file.flush();

			std::lock_guard<Gaff::SpinLock> lock(lm._log_callback_lock);
			lm.notifyLogCallbacks(task.message.data(), task.type);
		}
	}
}

LogManager::LogManager(void):
	_shutdown(false)
{
}

LogManager::~LogManager(void)
{
	destroy();
}

bool LogManager::init(void)
{
	addChannel("Default", "Log");

	std::thread log_thread(LogThread, std::ref(*this));
	_log_thread.swap(log_thread);

	return _log_thread.joinable();
}

void LogManager::destroy(void)
{
	_shutdown = true;

	_log_event.set();

	if (_log_thread.joinable()) {
		_log_thread.join();
	}

	_shutdown = false;

	_log_callbacks.clear();
	_channels.clear();
}

void LogManager::addLogCallback(const LogCallback& callback)
{
	std::lock_guard<Gaff::SpinLock> lock(_log_callback_lock);
	_log_callbacks.emplace_back(callback);
}

void LogManager::removeLogCallback(const LogCallback& callback)
{
	auto it = eastl::find(_log_callbacks.begin(), _log_callbacks.end(), callback);

	if (it != _log_callbacks.end()) {
		_log_callbacks.erase_unsorted(it);
	}
}

void LogManager::notifyLogCallbacks(const char* message, LogType type)
{
	for (auto it = _log_callbacks.begin(); it != _log_callbacks.end(); ++it) {
		(*it)(message, type);
	}
}

void LogManager::addChannel(Gaff::HashStringTemp32 channel, const char* file)
{
	auto it = Gaff::Find(_channels, channel);

	if (it == _channels.end()) {
		char8_t time_string[64] = { 0 };
		char8_t file_name[256] = { 0 };

		Gaff::GetCurrentTimeString(time_string, ARRAY_SIZE(time_string), "%Y-%m-%d %H-%M-%S");

		snprintf(
			file_name,
			ARRAY_SIZE(file_name),
			"Logs/%s %s.txt",
			file,
			time_string
		);

		auto pair = eastl::make_pair<HashString32, Gaff::File>(HashString32(channel), Gaff::File());

		if (pair.second.open(file_name, Gaff::File::WRITE) && channel.getHash() != LOG_CHANNEL_DEFAULT) {
			_channels.insert(std::move(pair));

		} else {
			// If this is not the channel added in the constructor, then log the error in that channel.
			if (channel.getHash() != LOG_CHANNEL_DEFAULT) {
				logMessage(
					LOG_ERROR,
					LOG_CHANNEL_DEFAULT,
					"Failed to create channel '%s'! Failed to open file '%s'!",
					channel.getBuffer(),
					file_name
				);
			}
		}
	}
}

void LogManager::logMessage(LogType type, Gaff::HashStringTemp32 channel, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	if(!logMessageHelper(type, channel.getHash(), format, vl) && channel.getHash() != LOG_CHANNEL_DEFAULT) {
		logMessage(
			LOG_ERROR,
			LOG_CHANNEL_DEFAULT,
			"Failed to find channel '%s'!",
			channel.getBuffer()
		);

		// Log the message to the default channel.
		logMessageHelper(type, LOG_CHANNEL_DEFAULT, format, vl);
	}

	va_end(vl);
}

void LogManager::logMessage(LogType type, Gaff::Hash32 channel, const char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	if (!logMessageHelper(type, channel, format, vl) && channel != LOG_CHANNEL_DEFAULT) {
		logMessage(
			LOG_ERROR,
			LOG_CHANNEL_DEFAULT,
			"Failed to find channel with hash '%u'!",
			channel
		);

		// Log the message to the default channel.
		logMessageHelper(type, LOG_CHANNEL_DEFAULT, format, vl);
	}

	va_end(vl);
}

bool LogManager::logMessageHelper(LogType type, Gaff::Hash32 channel, const char* format, va_list& vl)
{
	char temp[2048] = { 0 };
	vsnprintf(temp, ARRAY_SIZE(temp), format, vl);

	auto it = Gaff::Find(_channels, channel);

	if (it == _channels.end()) {
		return false;
	}

	std::lock_guard<Gaff::SpinLock> lock(_log_queue_lock);
	_logs.emplace_back(it->second, temp, type);
	_log_event.set();

	return true;
}

NS_END
