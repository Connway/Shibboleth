/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Gaff_CrashHandler.h"

#ifdef PLATFORM_WINDOWS

#include "Gaff_Utils.h"
#include "Gaff_IncludeWindows.h"
#include <EASTL/algorithm.h>

// Silence MS warnings
#if _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4091)
#endif

#include <DbgHelp.h>

#if _MSC_VER
	#pragma warning(pop)
#endif

#include <Psapi.h>

NS_GAFF

extern CrashHandler g_crash_handler;

static void WriteMiniDump(EXCEPTION_POINTERS* _exception_info)
{
	// Generate dump file name
	TCHAR process_name[MAX_PATH] = { 0 };
	GetProcessImageFileName(GetCurrentProcess(), process_name, MAX_PATH);
	size_t name_begin = FindLastOf(process_name, MAX_PATH - 1, L'\\') + 1;


	TCHAR dump_format[128] = { 0 };
	GetCurrentTimeString(dump_format, ARRAY_SIZE(dump_format) - 1, L"dumps/0s_%Y-%m-%d %H-%M-%S.dmp");
	dump_format[6] = L'%';

	TCHAR dump_file_name[1024] = { 0 };

	_snwprintf(dump_file_name, ARRAY_SIZE(dump_file_name) - 1, dump_format, process_name + name_begin);

	// Make sure our output folder exists.
	CreateDir("dumps", 0777);

	HANDLE hFile = CreateFile(
		dump_file_name, GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
	);

	if (hFile != NULL && hFile != INVALID_HANDLE_VALUE) {
		MINIDUMP_EXCEPTION_INFORMATION mdei;

		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = _exception_info;
		mdei.ClientPointers = FALSE;

		MINIDUMP_TYPE mdt = static_cast<MINIDUMP_TYPE>(
			MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithProcessThreadData |
			MiniDumpWithThreadInfo
		);

		MiniDumpWriteDump(
			GetCurrentProcess(), GetCurrentProcessId(),
			hFile, mdt, (_exception_info != nullptr) ? &mdei : 0,
			0, 0
		);

		CloseHandle(hFile);
	}
}

static LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* _exception_info)
{
	// Convert to our data structure

	if (g_crash_handler) {
		g_crash_handler(_exception_info);
	}

	if (IsDebuggerPresent()) {
		return EXCEPTION_CONTINUE_SEARCH;
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void DefaultCrashHandler(void* crash_data)
{
	WriteMiniDump(reinterpret_cast<EXCEPTION_POINTERS*>(crash_data));
}

void InitializeCrashHandler(void)
{
	SetUnhandledExceptionFilter(ExceptionHandler);
}

void UninitializeCrashHandler(void)
{
}

NS_END

#endif
