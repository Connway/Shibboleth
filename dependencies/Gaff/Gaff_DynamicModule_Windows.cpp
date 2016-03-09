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

#include "Gaff_Platform.h"

#ifdef PLATFORM_WINDOWS

#include "Gaff_DynamicModule_Windows.h"
#include "Gaff_String.h"

#define MAX_ERR_LEN 1024

NS_GAFF

DynamicModule::DynamicModule(void):
	_module(nullptr)
{
}

DynamicModule::~DynamicModule(void)
{
	destroy();
}

bool DynamicModule::load(const char* filename)
{
#ifdef _UNICODE
	CONVERT_TO_UTF16(temp, filename);
	_module = LoadLibraryEx(temp, NULL, 0);
#else
	_module = LoadLibraryEx(filename, NULL, 0);
#endif

	return _module != nullptr;
}

bool DynamicModule::destroy(void)
{
	if (_module) {
		return FreeLibrary(_module) != 0;
	}

	return false;
}

void* DynamicModule::getAddress(const char* name) const
{
	return reinterpret_cast<void*>(GetProcAddress(_module, name));
}

const char* DynamicModule::GetErrorString(void)
{
	static char error[MAX_ERR_LEN] = { 0 };

	LPTSTR msg = nullptr;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&msg),
		0,
		NULL
	);

#ifdef _UNICODE
	ConvertToUTF8(error, msg, wcslen(msg));
#else
	memcpy(_error, msg, strlen(msg));
#endif

	LocalFree(msg);

	// Strip out the \r\n at the end of the string.
	for (size_t i = 0; i < MAX_ERR_LEN; ++i) {
		if (error[i] == '\r') {
			error[i] = 0;
			break;
		}
	}

	return error;
}


NS_END

#endif
