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

#pragma once

#if defined(_WIN32) || defined(_WIN64)
	#include "Gaff_IncludeWindows.h"
#endif

#include "Gaff_Defines.h"
#include <dirent.h>
#include <cstring>
#include <ctime>

NS_GAFF

INLINE unsigned long GetNumberOfCores(void);
INLINE void GetCurrentTimeString(char* buff, size_t count, const char* format);
INLINE bool CreateDir(const char* dirname, unsigned short mode);

enum FileDataType
{
	FDT_Unknown = 0,
	FDT_RegularFile = 0x8000,
	FDT_Directory = 0x4000,
	FDT_Pipe = 0x1000,
	//DT_Socket
	FDT_Character = 0x2000
	//DT_Block
};

typedef bool (*FileDirTraversalFunc)(const char* name, size_t name_len, FileDataType type);
typedef bool (*FileDirTraversalFuncUnicode)(const wchar_t* name, size_t name_len, FileDataType type);

typedef bool (*FileDirTraversalFuncSpecific)(const char* name, size_t name_len);
typedef bool (*FileDirTraversalFuncSpecificUnicode)(const wchar_t* name, size_t name_len);

template <class Callback>
void ForEachInDirectory(const char* directory, Callback&& callback);

template <FileDataType type, class Callback>
void ForEachTypeInDirectory(const char* directory, Callback&& callback);

#ifdef _UNICODE
template <class Callback>
void ForEachInDirectory(const wchar_t* directory, Callback&& callback);

template <FileDataType type, class Callback>
void ForEachTypeInDirectory(const wchar_t* directory, Callback&& callback);

INLINE bool CreateDir(const wchar_t* dirname, unsigned short mode);
#endif

#include "Gaff_Utils_Common.inl"

NS_END
