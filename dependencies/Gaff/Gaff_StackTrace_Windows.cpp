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

#if defined(_WIN32) || defined(_WIN64)

#include "Gaff_StackTrace_Windows.h"
#include "Gaff_IncludeAssert.h"

NS_GAFF

void* StackTrace::_handle = nullptr;

bool StackTrace::Init(void)
{
	assert(!_handle);
	_handle = GetCurrentProcess();
	return SymInitialize(_handle, nullptr, TRUE) == TRUE;
}

void StackTrace::Destroy(void)
{
	assert(_handle);
	SymCleanup(_handle);
	_handle = nullptr;
}

StackTrace::StackTrace(const StackTrace& trace):
	_total_frames(trace._total_frames)
{
	SYMBOL_INFO* sym = (SYMBOL_INFO*)_symbol_info;
	sym->SizeOfStruct = sizeof(_symbol_info);
	sym->MaxNameLen = NAME_SIZE - 1;

	memcpy(_stack, trace._stack, sizeof(_stack));
}

StackTrace::StackTrace(void):
	_total_frames(0)
{
	SYMBOL_INFO* sym = (SYMBOL_INFO*)_symbol_info;
	sym->SizeOfStruct = sizeof(SYMBOL_INFO);
	sym->MaxNameLen = NAME_SIZE - 1;
}

StackTrace::~StackTrace(void)
{
}

const StackTrace& StackTrace::operator=(const StackTrace& rhs)
{
	memcpy(_stack, rhs._stack, sizeof(_stack));
	return *this;
}

unsigned short StackTrace::captureStack(unsigned int frames_to_capture)
{
	assert(frames_to_capture <= MAX_FRAMES);
	_total_frames = CaptureStackBackTrace(0, frames_to_capture, _stack, nullptr);
	return _total_frames;
}

unsigned short StackTrace::getTotalFrames(void) const
{
	return _total_frames;
}

bool StackTrace::loadFrameInfo(unsigned short frame)
{
	SYMBOL_INFO* sym = (SYMBOL_INFO*)_symbol_info;
	return SymFromAddr(_handle, (DWORD64)_stack[frame], nullptr, sym) == TRUE;
}

const char* StackTrace::getFrameName(void) const
{
	SYMBOL_INFO* sym = (SYMBOL_INFO*)_symbol_info;
	return sym->Name;
}

unsigned long long StackTrace::getFrameAddress(void) const
{
	SYMBOL_INFO* sym = (SYMBOL_INFO*)_symbol_info;
	return sym->Address;
}

NS_END

#endif