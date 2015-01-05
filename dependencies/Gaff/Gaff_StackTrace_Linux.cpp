/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
#include "Gaff_IncludeAssert.h"
#include <execinfo.h>
#include <malloc.h>
#include <cstring>

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
	_strings(nullptr)
{
	*this = trace;
}

StackTrace::StackTrace(void):
	_total_frames(0), _strings(nullptr),
	_frame(nullptr)
{
}

StackTrace::~StackTrace(void)
{
	if (_strings) {
		::free(_strings);
	}
}

const StackTrace& StackTrace::operator=(const StackTrace& rhs)
{
	if (_strings) {
		::free(_strings);
	}

	memcpy(_stack, rhs._stack, sizeof(_stack));
	_frame = rhs._frame;

	_strings = backtrace_symbols(&_frame, 1);

	return *this;
}

/*!
	\brief Captures the callstack \a frames_to_capture deep.
	\return The number of callstack frames captured.
*/
unsigned short StackTrace::captureStack(unsigned int frames_to_capture)
{
	return (unsigned short)backtrace(_stack, frames_to_capture);
}

/*!
	\brief Returns the total number of currently captured callstack frames.
*/
unsigned short StackTrace::getTotalFrames(void) const
{
	return _total_frames;
}

/*!
	\brief Loads the frame information for the specified callstack \a frame.
	\param frame The callstack frame whose information to load.
	\return Whether the callstack frame information was successfully loaded.
*/
bool StackTrace::loadFrameInfo(unsigned short frame)
{
	if (_strings) {
		::free(_strings);
	}

	_strings = backtrace_symbols(_stack + frame, 1);
	_frame = _stack[frame];

	return _strings && _frame;
}

/*!
	\brief Returns the currently loaded callstack frame's name.
	\note Must call loadFrameInfo() first.
*/
const char* StackTrace::getFrameName(void) const
{
	return *_strings;
}

/*!
	\brief Gets the address of the currently loaded callstack frame.
	\note Must call loadFrameInfo() first.
*/
unsigned long long StackTrace::getFrameAddress(void) const
{
	// No idea if this is correct, but close enough I guess :/
	return (unsigned long long)_frame;
}

NS_END

#endif
