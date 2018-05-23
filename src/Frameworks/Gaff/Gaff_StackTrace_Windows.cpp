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

#include "Gaff_StackTrace_Windows.h"

#ifdef PLATFORM_WINDOWS

#include "Gaff_Assert.h"
#include "Gaff_IncludeWindows.h"

// Silence MS warnings
#pragma warning(push)
#pragma warning(disable: 4091)
#include <DbgHelp.h>
#pragma warning(pop)

NS_GAFF

void* StackTrace::_handle = GetCurrentProcess();

bool StackTrace::RefreshModuleList(void)
{
	if (!_handle) {
		_handle = GetCurrentProcess();
	}

	return SymRefreshModuleList(_handle) != FALSE;
}

bool StackTrace::Init(void)
{
	if (!_handle) {
		_handle = GetCurrentProcess();
	}

	return SymInitialize(_handle, nullptr, TRUE) == TRUE;
}

void StackTrace::Destroy(void)
{
	GAFF_ASSERT(_handle);
	SymCleanup(_handle);
	_handle = nullptr;
}

StackTrace::StackTrace(const StackTrace& trace):
	_frames(trace._frames)
{
	_handle = GetCurrentProcess();
	*this = trace;
}

StackTrace::StackTrace(void):
	_frames(0)
{
	_handle = GetCurrentProcess();
}

StackTrace::~StackTrace(void)
{
}

const StackTrace& StackTrace::operator=(const StackTrace& rhs)
{
	memcpy(_symbol_info, rhs._symbol_info, sizeof(_symbol_info));
	memcpy(_stack, rhs._stack, sizeof(_stack));
	_frames = rhs._frames;
	return *this;
}

int32_t StackTrace::captureStack(const char*, uint32_t frames_to_capture, uint32_t frames_to_skip)
{
	GAFF_ASSERT(frames_to_capture <= MAX_FRAMES);
	_frames = CaptureStackBackTrace(frames_to_skip, frames_to_capture, _stack, nullptr);

	char data[sizeof(SYMBOL_INFO) + NAME_SIZE - 1];
	SYMBOL_INFO* sym = reinterpret_cast<SYMBOL_INFO*>(data);
	sym->SizeOfStruct = sizeof(SYMBOL_INFO);
	sym->MaxNameLen = NAME_SIZE - 1;

	IMAGEHLP_LINE64 image_help = { sizeof(IMAGEHLP_LINE) };
	DWORD displacement = 0;

	for (int32_t i = 0; i < _frames; ++i) {
		_symbol_info[i].symbol_name[0] = 0;
		_symbol_info[i].file_name[0] = 0;
		_symbol_info[i].line_number = 0;
		_symbol_info[i].address = 0;

		if (SymFromAddr(_handle, reinterpret_cast<DWORD64>(_stack[i]), nullptr, sym)) {
			strncpy(_symbol_info[i].symbol_name, sym->Name, NAME_SIZE);
			_symbol_info[i].address = sym->Address;
		}

		if (SymGetLineFromAddr64(_handle, reinterpret_cast<DWORD64>(_stack[i]), &displacement, &image_help)) {
			strncpy(_symbol_info[i].file_name, image_help.FileName, NAME_SIZE);
			_symbol_info[i].line_number = image_help.LineNumber;
		}
	}

	return _frames;
}

int32_t StackTrace::getNumCapturedFrames(void) const
{
	return _frames;
}

uint64_t StackTrace::getAddress(int32_t frame) const
{
	GAFF_ASSERT(frame < _frames);
	return _symbol_info[frame].address;
}

uint32_t StackTrace::getLineNumber(int32_t frame) const
{
	GAFF_ASSERT(frame < _frames);
	return _symbol_info[frame].line_number;
}

const char* StackTrace::getSymbolName(int32_t frame) const
{
	GAFF_ASSERT(frame < _frames);
	return _symbol_info[frame].symbol_name;
}

const char* StackTrace::getFileName(int32_t frame) const
{
	GAFF_ASSERT(frame < _frames);
	return _symbol_info[frame].file_name;
}

NS_END

#endif
