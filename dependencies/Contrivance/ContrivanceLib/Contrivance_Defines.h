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

#pragma once

#include <QtGlobal>

#define NS_CONTRIVANCE namespace Contrivance {

#ifndef NS_END
	#define NS_END }
#endif

#if defined(Q_OS_WIN)
	#define SHARED_LIBRARY_EXTENSION ".dll"
#elif defined(Q_OS_LINUX)
	#define SHARED_LIBRARY_EXTENSION ".so"
#elif defined(Q_OS_MAC)
	#define SHARED_LIBRARY_EXTENSION ".dylib"
#else
	#error "Platform not supported"
#endif

#if defined(Q_OS_WIN32) && !defined(Q_OS_WIN64)
	#ifdef QT_DEBUG
		#define SHARED_LIBRARY_SUFFIX "32d"
	#else
		#define SHARED_LIBRARY_SUFFIX "32"
	#endif
#else
	#ifdef QT_DEBUG
		#define SHARED_LIBRARY_SUFFIX "64d"
	#else
		#define SHARED_LIBRARY_SUFFIX "64"
	#endif
#endif

#define Q_DECL_EXPORT_C extern "C" Q_DECL_EXPORT

enum ConsoleMessageType
{
	CMT_NORMAL = 0,
	CMT_WARNING,
	CMT_ERROR//,
	//CMT_USR_TYPE_START
};
