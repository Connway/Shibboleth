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

#include "Gaff_StackTrace_Linux.h"

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)

#include "Gaff_Assert.h"
#include <execinfo.h>
#include <cstring>

#ifdef __APPLE__
	#include <sys/malloc.h>
#else
	#include <malloc.h>
#endif

NS_GAFF

bool StackTrace::Init(void)
{
	return true;
}

void StackTrace::Destroy(void)
{
}

StackTrace::StackTrace(const StackTrace& trace):
	_strings(nullptr), _total_frames(0)
{
	*this = trace;
}

StackTrace::StackTrace(void):
	_strings(nullptr), _total_frames(0)
{
	memset(_stack, 0, sizeof(void*) * MAX_FRAMES);
	memset(_file_name_cache, 0, sizeof(char) * NAME_SIZE * MAX_FRAMES);
	memset(_file_name_size, 0, sizeof(int) * MAX_FRAMES);
}

StackTrace::~StackTrace(void)
{
	if (_strings) {
		free(_strings);
	}
}

const StackTrace& StackTrace::operator=(const StackTrace& rhs)
{
	memcpy(_stack, rhs._stack, sizeof(_stack));
	memcpy(_file_name_cache, rhs._file_name_cache, sizeof(char) * NAME_SIZE * MAX_FRAMES);
	memcpy(_file_name_size, rhs._file_name_size, sizeof(int) * MAX_FRAMES);
	_total_frames = rhs._total_frames;

	if (_strings) {
		free(_strings);
	}

	_strings = backtrace_symbols(_stack, _total_frames);

	return *this;
}

int32_t StackTrace::captureStack(const char* app_name, uint32_t frames_to_capture, uint32_t /*frames_to_skip*/)
{
	_total_frames = backtrace(_stack, frames_to_capture);

	if (_strings) {
		free(_strings);
	}

	_strings = backtrace_symbols(_stack, _total_frames);

	for (int32_t i = 0; i < _total_frames; ++i) {
		char command[256] = { 0 };

#ifdef PLATFORM_MAC
		snprintf(command, sizeof(command), "atos -o %s %p", app_name, _stack[i]); // Make a generic way to solve for not hard-coding the "App" part.
#else
		sprintf(command, "addr2line -e %s %p", app_name, _stack[i]); // Make a generic way to solve for not hard-coding the "App" part.
#endif

		FILE* stream = popen(command, "r");

		if (!stream) {
			continue;
		}

		char* const result = fgets(_file_name_cache[i], NAME_SIZE, stream);
		pclose(stream);

		_file_name_cache[i][NAME_SIZE - 1] = 0;

		if (result) {
			for (int32_t j = NAME_SIZE - 2; j > -1; --j) {
				if (_file_name_cache[i][j] == ':') {
					_file_name_size[i] = j;
					_file_name_cache[i][j] = 0;
					break;
				}
			}
		}
	}

	return _total_frames;
}

int32_t StackTrace::getNumCapturedFrames(void) const
{
	return _total_frames;
}

uint64_t StackTrace::getAddress(int32_t frame) const
{
	return reinterpret_cast<uint64_t>(_stack[frame]);
}

uint32_t StackTrace::getLineNumber(int32_t frame) const
{
	GAFF_ASSERT(frame < _total_frames);
	return static_cast<uint32_t>(atoi(&_file_name_cache[frame][_file_name_size[frame] + 1]));
}

const char* StackTrace::getSymbolName(int32_t frame) const
{
	GAFF_ASSERT(frame < _total_frames);
	return _strings[frame];
}

const char* StackTrace::getFileName(int32_t frame) const
{
	GAFF_ASSERT(frame < _total_frames);
	return _file_name_cache[frame];
}

NS_END

#endif
