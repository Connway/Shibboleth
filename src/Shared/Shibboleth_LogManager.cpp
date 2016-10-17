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

#include "Shibboleth_LogManager.h"
#include <Gaff_File.h>

NS_SHIBBOLETH

Gaff::Thread::ReturnType THREAD_CALLTYPE LogManager::LogThread(void* thread_data)
{
	LogManager* lm = reinterpret_cast<LogManager*>(thread_data);

	while (!lm->_shutdown) {
		lm->_log_event.wait();
		lm->_log_queue_lock.lock();

		for (auto it = lm->_logs.begin(); it != lm->_logs.end(); ++it) {
			Gaff::File file;

			if (file.open(it->log_file.data(), Gaff::File::APPEND)) {
				file.writeString(it->message.data());
				file.close();
			}

			lm->notifyLogCallbacks(it->message.data(), it->type);
		}

		lm->_logs.clear();

		lm->_log_queue_lock.unlock();
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

bool LogManager::init(void)
{
	_shutdown = false;
	return _log_thread.create(LogThread, this);
}

void LogManager::destroy(void)
{
	_shutdown = true;
	_log_event.set();
	_log_thread.wait();
	_log_thread.close();
}

void LogManager::addLogCallback(const LogCallback& callback)
{
	_log_callbacks.emplace_back(callback);
}

void LogManager::removeLogCallback(const LogCallback& callback)
{
	auto it = eastl::find(_log_callbacks.begin(), _log_callbacks.end(), callback);

	if (it != _log_callbacks.end()) {
		_log_callbacks.erase_unsorted(it);
	}
}

void LogManager::notifyLogCallbacks(const char* message, LOG_TYPE type)
{
	for (auto it = _log_callbacks.begin(); it != _log_callbacks.end(); ++it) {
		(*it)(message, type);
	}
}

void LogManager::logMessage(LOG_TYPE type, const char* file, const char* format, ...)
{
	char temp[2048] = { 0 };
	va_list vl;

	va_start(vl, format);
	vsnprintf_s(temp, 2048, format, vl);
	va_end(vl);

	_log_queue_lock.lock();
	_logs.emplace_back(type, file, temp);
	_log_queue_lock.unlock();

	_log_event.set();
}

NS_END
