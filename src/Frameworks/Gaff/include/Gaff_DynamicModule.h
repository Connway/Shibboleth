/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Gaff_Platform.h"

#ifdef PLATFORM_WINDOWS
	#include "Gaff_DynamicModule_Windows.h"
	#define DYNAMIC_MODULE_EXTENSION_U8 u8".dll"
	#define DYNAMIC_MODULE_EXTENSION ".dll"
#elif defined(PLATFORM_LINUX)
	#include "Gaff_DynamicModule_Linux.h"
	#define DYNAMIC_MODULE_EXTENSION_U8 u8".so"
	#define DYNAMIC_MODULE_EXTENSION ".so"
#elif defined(PLATFORM_MAC)
	#include "Gaff_DynamicModule_Linux.h"
	#define DYNAMIC_MODULE_EXTENSION_U8 u8".dylib"
	#define DYNAMIC_MODULE_EXTENSION ".dylib"
#else
	#error Platform not supported
#endif
