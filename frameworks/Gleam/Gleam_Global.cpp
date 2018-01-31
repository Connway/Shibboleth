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

#include "Gleam_Global.h"
#include <Gaff_DefaultAlignedAllocator.h>

NS_GLEAM

static Gaff::DefaultAllocator g_backup_allocator;
static Gaff::IAllocator* g_allocator = &g_backup_allocator;
static LogFunc g_log_func = nullptr;

void SetAllocator(Gaff::IAllocator* allocator)
{
	g_allocator = allocator;
}

Gaff::IAllocator* GetAllocator(void)
{
	return g_allocator;
}

void* GleamAlloc(size_t size_bytes, const char* filename, unsigned int line_number)
{
	void* data = g_allocator->alloc(size_bytes, filename, line_number);

	if (!data) {
		PrintfToLog("Failed to allocate %i bytes in \'%s\':%i", LOG_ERROR, filename, line_number);
		return nullptr;
	}

	return data;
}

void GleamFree(void* data)
{
	GAFF_FREE(data, *g_allocator);
}

void SetLogFunc(LogFunc log_func)
{
	g_log_func = log_func;
}

void PrintfToLog(const char* format_string, LogMsgType type, ...)
{
	if (g_log_func) {
		char temp[2048] = { 0 };
		va_list vl;

		va_start(vl, type);
		vsnprintf(temp, 2048, format_string, vl);
		va_end(vl);

		g_log_func(temp, type);
	}
}

NS_END
