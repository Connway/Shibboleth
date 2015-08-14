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

#if defined(_WIN32) || defined(_WIN64)

#include "Gaff_Utils.h"
#include "Gaff_IncludeAssert.h"
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
	assert(dirname);
	return !_mkdir(dirname) || errno == EEXIST;
}

void DebugPrintf(const char* format_string, ...)
{
	assert(format_string);

	va_list vl;
	va_start(vl, format_string);

	char buf[256] = { 0 };
	vsnprintf(buf, 256, format_string, vl);
	OutputDebugStringA(buf);

	va_end(vl);
}

void DebugPrintf(const wchar_t* format_string, ...)
{
	assert(format_string);

	va_list vl;
	va_start(vl, format_string);

	wchar_t buf[256] = { 0 };
	_vsnwprintf(buf, 256, format_string, vl);
	OutputDebugStringW(buf);

	va_end(vl);
}

bool SetWorkingDir(const char* directory)
{
#ifdef _UNICODE
	wchar_t buffer[256];
	mbstowcs(buffer, directory, 256);
	return SetCurrentDirectoryW(buffer) != 0;
#else
	return SetCurrentDirectoryW(directory) != 0;
#endif
}

void* AlignedMalloc(size_t size, size_t alignment)
{
	return _aligned_malloc(size, alignment);
}

void AlignedFree(void* data)
{
	_aligned_free(data);
}

NS_END

#endif
