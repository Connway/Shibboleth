/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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
#include <Gaff_ScopedLock.h>
#include <Gaff_SpinLock.h>
#include <Gaff_File.h>
#include <iostream>
#include <cstdarg>

#ifdef ALIGN_SIMD
#include "Gaff_DefaultAlignedAllocator.h"
#else
#include "Gaff_DefaultAllocator.h"
#endif

NS_GLEAM

#ifdef ALIGN_SIMD
static Gaff::IAllocator* g_allocator = new Gaff::DefaultAlignedAllocator(16);
#else
static Gaff::IAllocator* g_allocator = new Gaff::DefaultAllocator;
#endif

static const GChar* g_log_file_name = GC("gleam.log");
static bool default_alloc = true;

static Gaff::SpinLock g_spin_lock;
static Gaff::File g_log_file;

void SetAllocator(Gaff::IAllocator* allocator)
{
	if (default_alloc) {
		default_alloc = false;
		delete g_allocator;
	}

	g_allocator = allocator;
}

Gaff::IAllocator* GetAllocator(void)
{
	return g_allocator;
}

void* GleamAlloc(unsigned int size_bytes, const char* filename, unsigned int line_number)
{
	void* data = g_allocator->alloc(size_bytes);

	if (!data) {
		// log error or something

		// attempt to write a log about what happened
		PrintfToLog("Failed to allocate %i bytes in \'%s\':%i", LOG_ERROR, filename, line_number);
		return nullptr;
	}

	return data;
}

void GleamFree(void* data)
{
	g_allocator->free(data);
}

void SetLogFileName(const GChar* log_file_name)
{
	g_log_file_name = log_file_name;
}

const GChar* GetLogFileName(void)
{
	return g_log_file_name;
}

void WriteMessageToLog(const char* msg, size_t size, LOG_MSG_TYPE type)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(g_spin_lock);

	if (!g_log_file.isOpen()) {
		g_log_file.open(g_log_file_name, Gaff::File::APPEND);
	}

	if (g_log_file.isOpen()) {
		switch (type) {
			case LOG_ERROR:
				g_log_file.writeString("ERROR:\n");
				break;

			case LOG_WARNING:
				g_log_file.writeString("WARNING:\n");
				break;

			case LOG_NORMAL:
			default:
				break;
		}

		g_log_file.write((void*)msg, sizeof(char), size);
		g_log_file.writeChar('\n');
	}
}

void PrintfToLog(const char* format_string, LOG_MSG_TYPE type, ...)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(g_spin_lock);

	if (!g_log_file.isOpen()) {
		g_log_file.open(g_log_file_name, Gaff::File::APPEND);
	}

	if (g_log_file.isOpen()) {
		switch (type) {
			case LOG_ERROR:
				g_log_file.writeString("ERROR:\n");
				break;

			case LOG_WARNING:
				g_log_file.writeString("WARNING:\n");
				break;

			case LOG_NORMAL:
			default:
				break;
		}

		va_list vl;
		va_start(vl, type);

		g_log_file.printfVA(format_string, vl);
		g_log_file.writeChar('\n');

		va_end(vl);
	}
}

NS_END
