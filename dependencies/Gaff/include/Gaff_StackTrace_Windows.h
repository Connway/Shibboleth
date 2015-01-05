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

/*! \file */

#pragma once

#include "Gaff_Defines.h"
#include "Gaff_IncludeWindows.h"
#include <DbgHelp.h>

NS_GAFF

#define MAX_FRAMES 128 //!< The maximum number of stack frames we can captures.
#define NAME_SIZE 256 //!< The maximum size a frame name can be.

/*!
	\brief Wrapper class for capturing stack traces.
*/
class StackTrace
{
public:
	static bool Init(void);
	static void Destroy(void);

	StackTrace(const StackTrace& trace);
	StackTrace(void);
	~StackTrace(void);

	const StackTrace& operator=(const StackTrace& rhs);

	INLINE unsigned short captureStack(unsigned int frames_to_capture = 64);
	INLINE unsigned short getTotalFrames(void) const;

	bool loadFrameInfo(unsigned short frame);
	const char* getFrameName(void) const;
	unsigned long long getFrameAddress(void) const;

private:
	static HANDLE _handle;

	char _symbol_info[sizeof(SYMBOL_INFO) + NAME_SIZE - 1];
	void* _stack[MAX_FRAMES];

	unsigned short _total_frames;
};

NS_END
