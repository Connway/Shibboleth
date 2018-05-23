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

#pragma once

#include "Gaff_Defines.h"

NS_GAFF

#define MAX_FRAMES 32 //!< The maximum number of stack frames we can captures.
#define NAME_SIZE 256 //!< The maximum size a frame name can be.

class StackTrace
{
public:
	static bool RefreshModuleList(void);
	static bool Init(void);
	static void Destroy(void);

	StackTrace(const StackTrace& trace);
	StackTrace(void);
	~StackTrace(void);

	const StackTrace& operator=(const StackTrace& rhs);

	int32_t captureStack(const char* app_name, uint32_t frames_to_capture = MAX_FRAMES, uint32_t frames_to_skip = 0);
	int32_t getNumCapturedFrames(void) const;

	uint64_t getAddress(int32_t frame) const;
	uint32_t getLineNumber(int32_t frame) const;
	const char* getSymbolName(int32_t frame) const;
	const char* getFileName(int32_t frame) const;

private:
	static void* _handle;

	struct SymbolInfo
	{
		char file_name[NAME_SIZE];
		char symbol_name[NAME_SIZE];
		uint64_t address;
		uint32_t line_number;
	};

	SymbolInfo _symbol_info[MAX_FRAMES];
	void* _stack[MAX_FRAMES];

	int32_t _frames;
};

NS_END
