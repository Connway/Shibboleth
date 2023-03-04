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

#include "Gaff_Utils.h"

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
#include "Gaff_String.h"
#include "Gaff_Math.h"
#include <EASTL/allocator_malloc.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <cstdarg>
#include <csignal>
#include <cstdio>

NS_GAFF

unsigned long GetNumberOfCores(void)
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}

bool CreateDir(const char8_t* dir_name, unsigned short mode)
{
	return CreateDir(reinterpret_cast<const char*>(dir_name), mode);
}

bool CreateDir(const char* dirname, unsigned short mode)
{
	GAFF_ASSERT(dirname);
	return !mkdir(dirname, mode) || errno == EEXIST;
}

void DebugPrintf(const char8_t* format_string, ...)
{
	GAFF_ASSERT(format_string);

	va_list vl;
	va_start(vl, format_string);

	CONVERT_STRING(char, temp_format_string, format_string);
	vprintf(temp_format_string, vl);

	va_end(vl);
}

void DebugPrintf(const char* format_string, ...)
{
	GAFF_ASSERT(format_string);

	va_list vl;
	va_start(vl, format_string);

	vprintf(format_string, vl);

	va_end(vl);
}

bool SetWorkingDir(const char8_t* directory)
{
	return !chdir(reinterpret_cast<const char*>(directory));
}

void* AlignedMalloc(size_t size, size_t alignment)
{
	return aligned_alloc(alignment, size);
}

void AlignedFree(void* data)
{
	free(data);
}

void* AlignedRealloc(void* data, size_t size, size_t alignment)
{
	const size_t old_size = GetUsableSize(data);

	if (old_size >= size) {
		return data;
	}

	void* const new_ptr = AlignedMalloc(size, alignment);
	memcpy(new_ptr, data, Gaff::Min(old_size, size));
	AlignedFree(data);

	return new_ptr;
}

void* AlignedCalloc(size_t num_members, size_t member_size, size_t alignment)
{
	void* const ptr = AlignedMalloc(num_members * member_size, alignment);

	if (ptr) {
		memset(ptr, 0, num_members * member_size);
	}

	return ptr;
}

void DebugBreak(void)
{
	raise(SIGTRAP);
}

NS_END

#endif

#ifdef PLATFORM_LINUX

NS_GAFF

bool IsDebuggerAttached(void)
{
	FILE* file = fopen("/proc/self/status", "r");

	if (!file) {
		return false;
	}

	// Skip the first 5 lines.
	char buffer[1024] = { 0 };
	for (unsigned int i = 0; i < 5; ++i) {
		if (!fgets(buffer, 1024, file)) {
			return false;
		}
	}

	int pid = 0;
	const int result = fscanf(file, "TracerPid: %d", &pid);

	fclose(file);

	return result != EOF && pid != 0;
}

size_t GetUsableSize(void* ptr)
{
	return malloc_usable_size(ptr);
}

NS_END

#endif
