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

#if defined(__linux__) || defined(__APPLE__)

#include "Gaff_StackTrace_Linux.h"
#include "Gaff_Assert.h"
#include <execinfo.h>
#include <cstring>

#ifdef __APPLE__
	#include <sys/malloc.h>
#else
	#include <malloc.h>
#endif

NS_GAFF

/*!
	\brief Global initialization of the stack trace system.
	\note Must be called before using a StackTrace instance. And called per execution context. (eg EXE, DLL)
*/
bool StackTrace::Init(void)
{
	return true;
}

/*!
	\brief Global destruction of the stack trace system.
*/
void StackTrace::Destroy(void)
{
}

StackTrace::StackTrace(const StackTrace& trace):
	_strings(nullptr), _file_name_size(-1), _total_frames(0)
{
	*this = trace;
}

StackTrace::StackTrace(void):
	_strings(nullptr), _file_name_size(-1), _total_frames(0)
{
	memset(_file_name_cache, 0, sizeof(char) * NAME_SIZE);
}

StackTrace::~StackTrace(void)
{
	if (_strings) {
		free(_strings);
	}
}

const StackTrace& StackTrace::operator=(const StackTrace& rhs)
{
	memcpy(_file_name_cache, rhs._file_name_cache, sizeof(char) * NAME_SIZE);
	memcpy(_stack, rhs._stack, sizeof(_stack));
	_file_name_size = rhs._file_name_size;
	_total_frames = rhs._total_frames;

	if (_strings) {
		free(_strings);
	}

	_strings = backtrace_symbols(_stack, _total_frames);

	return *this;
}

/*!
	\brief Captures the callstack \a frames_to_capture deep.
	\return The number of callstack frames captured.
*/
unsigned short StackTrace::captureStack(unsigned int frames_to_capture)
{
	_total_frames = backtrace(_stack, frames_to_capture);

	if (_strings) {
		free(_strings);
	}

	_strings = backtrace_symbols(_stack, _total_frames);
	return _total_frames;
}

unsigned short StackTrace::getNumCapturedFrames(void) const
{
	return _total_frames;
}

uint64_t StackTrace::getAddress(unsigned short frame) const
{
	return reinterpret_cast<unsigned long long>(_stack[frame]);
}

unsigned int StackTrace::getLineNumber(unsigned short frame) const
{
	getFileName(frame);

	if (_file_name_size < 0) {
		return 0;
	}

	return static_cast<unsigned int>(atoi(&_file_name_cache[_file_name_size + 1]));
}

const char* StackTrace::getSymbolName(unsigned short frame) const
{
	return _strings[frame];
}

const char* StackTrace::getFileName(unsigned short frame) const
{
	char command[64] = { 0 };
	sprintf(command, "addr2line %p -e App", _stack[frame]); // Make a generic way to solve for not hard-coding the "App" part.

	FILE* stream = popen(command, "r");

	if (!stream) {
		return nullptr;
	}

	fgets(_file_name_cache, NAME_SIZE, stream);
	pclose(stream);
	_file_name_cache[NAME_SIZE - 1] = 0;

	for (int i = NAME_SIZE - 2; i > -1; --i) {
		if (_file_name_cache[i] == ':') {
			_file_name_cache[i] = 0;
			break;
		}
	}

	return _file_name_cache;
}

NS_END

#endif
