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

#pragma once

#include "Gleam_Defines.h"
#include <Gaff_IAllocator.h>

#ifdef __linux__
	#include <stddef.h>
#endif

NS_GLEAM

enum LOG_MSG_TYPE
{
	LOG_ERROR = 0,
	LOG_WARNING,
	LOG_NORMAL
};

void SetAllocator(Gaff::IAllocator* allocator);
INLINE Gaff::IAllocator* GetAllocator(void);
void* GleamAlloc(unsigned int size_bytes, const char* filename, unsigned int line_number);
INLINE void GleamFree(void* data);

INLINE void SetLogFileName(const GChar* log_file_name);
INLINE const GChar* GetLogFileName(void);
void WriteMessageToLog(const char* msg, size_t size, LOG_MSG_TYPE type = LOG_NORMAL);
void PrintfToLog(const char* format_string, LOG_MSG_TYPE type, ...);

template <class T, class... Args>
T* GleamAllocT(const char* filename, unsigned int line_number, Args... args)
{
	T* data = (T*)GleamAlloc(sizeof(T), filename, line_number);

	if (data) {
		Gaff::construct(data, args...);
	}

	return data;
}

NS_END

#define GleamAllocateT(T, ...) Gleam::GleamAllocT<T>(__FILE__, __LINE__, ##__VA_ARGS__)
#define GleamAllocate(size_bytes) Gleam::GleamAlloc(size_bytes, __FILE__, __LINE__)
