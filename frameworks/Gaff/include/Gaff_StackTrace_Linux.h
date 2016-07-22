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

#pragma once

#include "Gaff_Defines.h"

NS_GAFF

#define MAX_FRAMES 32 //!< The maximum number of stack frames we can captures.
#define NAME_SIZE 256 //!< The maximum size a frame name can be.

class StackTrace
{
public:
	INLINE static bool RefreshModuleList(void);
	INLINE static bool Init(void);
	INLINE static void Destroy(void);

	StackTrace(const StackTrace& trace);
	StackTrace(void);
	~StackTrace(void);

	const StackTrace& operator=(const StackTrace& rhs);

	INLINE unsigned short captureStack(const char* app_name, unsigned int frames_to_capture = MAX_FRAMES);
	INLINE unsigned short getNumCapturedFrames(void) const;

	INLINE uint64_t getAddress(unsigned short frame) const;
	INLINE unsigned int getLineNumber(unsigned short frame) const;
	INLINE const char* getSymbolName(unsigned short frame) const;
	INLINE const char* getFileName(unsigned short frame) const;

private:
	void* _stack[MAX_FRAMES];
	char** _strings;

	mutable char _file_name_cache[MAX_FRAMES][NAME_SIZE];
	mutable int _file_name_size[MAX_FRAMES];

	unsigned short _total_frames;

	bool loadFrameInfo(unsigned short frame);
};

NS_END
