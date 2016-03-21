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

#include "Gleam_Global.h"
#include <Gaff_DefaultAlignedAllocator.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_SpinLock.h>
#include <Gaff_File.h>

#define LOG_NAME_SIZE 128

NS_GLEAM

static Gaff::DefaultAlignedAllocator g_backup_allocator(16);
static Gaff::IAllocator* g_allocator = &g_backup_allocator;

static char g_log_file_name[LOG_NAME_SIZE] = { 0 };
static bool default_alloc = true;

static Gaff::SpinLock g_spin_lock;
static Gaff::File g_log_file;

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
	void* data = GAFF_ALLOC(size_bytes, *g_allocator);

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

void SetLogFileName(const char* log_file_name)
{
	strncpy(g_log_file_name, log_file_name, LOG_NAME_SIZE);
}

const char* GetLogFileName(void)
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
		g_log_file.flush();
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

		g_log_file.flush();
	}
}

NS_END
