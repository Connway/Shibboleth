/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Gaff_Utils.h"

#ifdef PLATFORM_WINDOWS
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

bool CreateDir(const char8_t* dir_name, unsigned short mode)
{
	return CreateDir(reinterpret_cast<const char*>(dir_name), mode);
}

bool CreateDir(const char* dir_name, unsigned short)
{
	GAFF_ASSERT(dir_name);

	CONVERT_STRING(wchar_t, temp, dir_name);
	const BOOL result = CreateDirectory(temp, nullptr);

	return result || GetLastError() == ERROR_ALREADY_EXISTS;
}

void DebugPrintf(const char8_t* format_string, ...)
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

bool GetWorkingDir(char8_t* buffer, size_t size)
{
	wchar_t temp_buffer[1024] = { 0 };
	const DWORD ret = GetCurrentDirectoryW(static_cast<DWORD>(std::size(temp_buffer)), temp_buffer);

	if (ret > 0) {
		const wchar_t* temp_start = temp_buffer;
		eastl::DecodePart(temp_start, temp_start + ret + 1, buffer, buffer + size);

		return true;
	}

	return false;
}

bool SetWorkingDir(const char8_t* directory)
{
	CONVERT_STRING(wchar_t, temp, directory);
	return SetCurrentDirectoryW(temp) != 0;
}

void* AlignedMalloc(size_t size, size_t alignment)
{
	return _aligned_malloc(size, alignment);
}

void AlignedFree(void* data)
{
	_aligned_free(data);
}

void* AlignedRealloc(void* data, size_t size, size_t alignment)
{
	return _aligned_realloc(data, size, alignment);
}

void* AlignedCalloc(size_t num_members, size_t member_size, size_t alignment)
{
	void* const ptr = AlignedMalloc(num_members * member_size, alignment);

	if (ptr) {
		memset(ptr, 0, num_members * member_size);
	}

	return ptr;
}

size_t GetUsableSize(void* ptr)
{
	return _msize(ptr);
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
