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

#include "Shibboleth_Logger.h"

NS_SHIBBOLETH

LogManager::LogManager(Allocator& allocator):
	_files(ProxyAllocator(allocator)), _allocator(allocator)
{
	_files.reserve(10);
}

LogManager::~LogManager(void)
{
	destroy();
}

void LogManager::destroy(void)
{
	for (auto it = _files.begin(); it != _files.end(); ++it) {
		it->first.writeString("\nCLOSING LOG FILE\n");
		_allocator.freeT(it->second);
	}

	_files.clear();
}

bool LogManager::openLogFile(const AHashString& filename)
{
	assert(filename.size() && _files.indexOf(filename) == -1);
	Gaff::File file(filename.getBuffer(), Gaff::File::APPEND);

	if (!file.isOpen()) {
		return false;
	}

	Gaff::SpinLock* spin_lock = _allocator.template allocT<Gaff::SpinLock>();

	if (!spin_lock) {
		return false;
	}

	FileLockPair flp;
	flp.first = Move(file);
	flp.second = spin_lock;

	_files[filename] = Move(flp);

	return true;
}

bool LogManager::openLogFile(const AString& filename)
{
	assert(filename.size() && _files.indexOf(filename) == -1);
	return openLogFile(AHashString(filename));
}

bool LogManager::openLogFile(const char* filename)
{
	assert(filename && _files.indexOf(filename) == -1);
	return openLogFile(AHashString(filename));
}

void LogManager::closeLogFile(const AHashString& filename)
{
	assert(filename.size() && _files.indexOf(filename) != -1);
	_files.erase(filename);
}

void LogManager::closeLogFile(const AString& filename)
{
	assert(filename.size() && _files.indexOf(filename) != -1);
	_files.erase(filename);
}

void LogManager::closeLogFile(const char* filename)
{
	assert(filename && _files.indexOf(filename) != -1);
	_files.erase(filename);
}

LogManager::FileLockPair& LogManager::getLogFile(const AHashString& filename)
{
	assert(filename.size() && _files.indexOf(filename) != -1);
	return _files[filename];
}

LogManager::FileLockPair& LogManager::getLogFile(const AString& filename)
{
	assert(filename.size() && _files.indexOf(filename) != -1);
	return _files[filename];
}

LogManager::FileLockPair& LogManager::getLogFile(const char* filename)
{
	assert(filename && _files.indexOf(filename) != -1);
	return _files[filename];
}

NS_END
