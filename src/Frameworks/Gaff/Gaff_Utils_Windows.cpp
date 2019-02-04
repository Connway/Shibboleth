/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Gaff_Utils.h"
#include "Gaff_Assert.h"
#include "Gaff_String.h"
#include "Gaff_IncludeWindows.h"
#include <direct.h>
#include <errno.h>

NS_GAFF

unsigned long GetNumberOfCores(void)
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}

bool CreateDir(const char* dirname, unsigned short)
{
	GAFF_ASSERT(dirname);
	return !_mkdir(dirname) || errno == EEXIST;
}

void DebugPrintf(const char* format_string, ...)
{
	GAFF_ASSERT(format_string);

	va_list vl;
	va_start(vl, format_string);

	CONVERT_STRING(wchar_t, temp, format_string);
	wchar_t buf[256] = { 0 };
	_vsnwprintf(buf, 256, temp, vl);
	OutputDebugStringW(buf);

	va_end(vl);
}

bool SetWorkingDir(const char* directory)
{
	CONVERT_STRING(wchar_t, temp, directory);
	return SetCurrentDirectoryW(temp) != 0;
}

void* AlignedOffsetMalloc(size_t size, size_t alignment, size_t offset)
{
	return _aligned_offset_malloc(size, alignment, offset);
}

void* AlignedMalloc(size_t size, size_t alignment)
{
	return _aligned_malloc(size, alignment);
}

void AlignedFree(void* data)
{
	_aligned_free(data);
}

bool IsDebuggerAttached(void)
{
	return IsDebuggerPresent() != 0;
}

void DebugBreak(void)
{
	__debugbreak();
}

NS_END

#endif
