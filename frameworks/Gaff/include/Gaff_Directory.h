/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gaff_Defines.h"
#include <dirent.h>

#ifdef PLATFORM_WINDOWS
	#include "Gaff_IncludeWindows.h"
#endif

#ifdef PLATFORM_MAC
	/* Return the exact length of d_namlen without zero terminator */
	#define _D_EXACT_NAMLEN(p) ((p)->d_namlen)
#endif

NS_GAFF

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

using FileDirTraversalFunc = bool (*)(const char* name, size_t name_len, FileDataType type);
using FileDirTraversalFuncSpecific = bool (*)(const char* name, size_t name_len);

template <class Callback>
bool ForEachInDirectory(const char* directory, Callback&& callback);

template <FileDataType type, class Callback>
bool ForEachTypeInDirectory(const char* directory, Callback&& callback);

NS_END

#include "Gaff_Directory.inl"
