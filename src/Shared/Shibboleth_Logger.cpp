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

static Logger gLogger;

Logger::Logger(void)
{
}

Logger::~Logger(void)
{
}

bool Logger::openLogFile(const AHashString& filename)
{
	assert(filename.size() && _files.indexOf(filename) == -1);
	Gaff::File file(filename.getBuffer(), Gaff::File::WRITE);

	if (!file.isOpen()) {
		return false;
	}

	_files[filename] = Move(file);

	return true;
}

bool Logger::openLogFile(const AString& filename)
{
	assert(filename.size() && _files.indexOf(filename) == -1);
	return openLogFile(AHashString(filename));
}

bool Logger::openLogFile(const char* filename)
{
	assert(filename && _files.indexOf(filename) == -1);
	return openLogFile(AHashString(filename));
}

void Logger::closeLogFile(const AHashString& filename)
{
	assert(filename.size() && _files.indexOf(filename) != -1);
	_files.erase(filename);
}

void Logger::closeLogFile(const AString& filename)
{
	assert(filename.size() && _files.indexOf(filename) != -1);
	_files.erase(filename);
}

void Logger::closeLogFile(const char* filename)
{
	assert(filename && _files.indexOf(filename) != -1);
	_files.erase(filename);
}

Gaff::File& Logger::getLogFile(const AHashString& filename)
{
	assert(filename.size() && _files.indexOf(filename) != -1);
	return _files[filename];
}

Gaff::File& Logger::getLogFile(const AString& filename)
{
	assert(filename.size() && _files.indexOf(filename) != -1);
	return _files[filename];
}

Gaff::File& Logger::getLogFile(const char* filename)
{
	assert(filename && _files.indexOf(filename) != -1);
	return _files[filename];
}

Logger& GetLogger(void)
{
	return gLogger;
}

NS_END
