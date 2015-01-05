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

/*! \file */

#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(_WIN32) || defined(_WIN64)
	#include "Gaff_IncludeWindows.h"
#endif

#include "Gaff_HashMap.h"
#include <dirent.h>
#include <cstring>
#include <ctime>

NS_GAFF

INLINE unsigned long GetNumberOfCores(void); //!< Returns the number of physical cores on the machine.
INLINE void GetCurrentTimeString(char* buff, size_t count, const char* format); //!< Gets the current time as a string using \a format. Result stored in \a buff.
INLINE bool CreateDir(const char* dirname, unsigned short mode); //<! Creates directory \a dirname with the given access \a mode.
void DebugPrintf(const char* format_string, ...); //!< Does a printf() to debug output on supported platforms. Unsupported platforms just call normal printf().
void DebugPrintf(const wchar_t* format_string, ...); //!< Does a printf() to debug output on supported platforms. Unsupported platforms just call normal printf().

//! Used to determine what type an entry in the filesystem is.
enum FileDataType
{
	FDT_Unknown = 0, //!< Unkown filesystem entry.
	FDT_RegularFile = 0x8000, //!< Entry is a regular file on disk.
	FDT_Directory = 0x4000, //!< Entry is a directory.
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
bool ForEachInDirectory(const char* directory, Callback&& callback);

template <FileDataType type, class Callback>
bool ForEachTypeInDirectory(const char* directory, Callback&& callback);

#ifdef _UNICODE
template <class Callback>
bool ForEachInDirectory(const wchar_t* directory, Callback&& callback);

template <FileDataType type, class Callback>
bool ForEachTypeInDirectory(const wchar_t* directory, Callback&& callback);

INLINE bool CreateDir(const wchar_t* dirname, unsigned short mode); //!< wchar_t version of CreateDir().
#endif

template <class Allocator>
HashMap<AHashString<Allocator>, AString<Allocator>, Allocator> ParseCommandLine(int argc, char** argv);

template <class Allocator>
void ParseCommandLine(int argc, char** argv, HashMap<AHashString<Allocator>, AString<Allocator>, Allocator>& out);

#include "Gaff_Utils_Common.inl"

NS_END
