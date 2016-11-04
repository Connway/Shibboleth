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

#include "Gaff_Platform.h"

//#define ATTEMPT_INLINE

#define NUM_TRIES_UNTIL_YIELD 5

#define NS_GAFF namespace Gaff {
#ifndef NS_END
	#define NS_END }
#endif

#define GAFF_STRUCTORS_DEFAULT(x) \
	x(void) = default; \
	~x(void) = default

#define GAFF_NO_COPY(x) \
	x(const x&) = delete; \
	x& operator=(const x&) = delete

#define GAFF_NO_MOVE(x) \
	x(x&&) = delete; \
	x& operator=(x&&) = delete

#define GAFF_COPY_DEFAULT(x) \
	x(const x&) = default; \
	x& operator=(const x&) = default

#define GAFF_MOVE_DEFAULT(x) \
	x(x&&) = default; \
	x& operator=(x&&) = default

#ifdef ATTEMPT_INLINE
	#define INLINE inline
#else
	#define INLINE
#endif

#define SAFERELEASE(x) if (x) { x->Release(); x = nullptr; } // Safely releases Microsoft COM pointers.
#define SAFEADDREF(x) if (x) { x->AddRef(); } // Safely adds a reference to Microsoft COM pointers.

#define SAFEGAFFRELEASE(x) if (x) { x->release(); x = nullptr; } // Safely releases pointers that implement IRefCounted.
#define SAFEGAFFADDREF(x) if (x) { x->addRef(); } // Safely adds a reference to pointers that implement IRefCounted.

#define GAFF_REF(x) ((void)x)

#define SIZE_T_FAIL static_cast<size_t>(-1) // Returned from functions that use size_t's, but can potentially fail
#define UINT_FAIL static_cast<unsigned int>(-1)  // Returned from functions that use unsigned int's, but can potentially fail
#define DYNAMICEXPORT_C extern "C" DYNAMICEXPORT // Exports a function with C-style symbol names.

#define GAFF_STR_HELPER(x) #x
#define GAFF_STR(x) GAFF_STR_HELPER(x)
#define GAFF_CAT_HELPER(x, y) x##y
#define GAFF_CAT(x, y) GAFF_CAT_HELPER(x, y)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define FLAG_BIT(index) (1 << index)

#ifdef _DEBUG
	#define GAFF_ASSERT_ENABLED
#endif



#ifdef PLATFORM_WINDOWS
	// Disable nameless struct/union warning
	// Disable assignment operator could not be generated warning
	// Disable unrecognized character escape sequence warning
	#pragma warning(disable : 4201 4512 4129)

	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif

	#define DYNAMICEXPORT __declspec(dllexport) // Specifies a symbol for export.
	#define DYNAMICIMPORT __declspec(dllimport) // Specifies a symbol for import.
	#define THREAD_LOCAL __declspec(thread) // Specifies a static variable to use thread local storage.

	#define WARNING(msg) __pragma(message(__FILE__":(" GAFF_STR(__LINE__)") WARNING - " msg))
	#define YieldThread() Sleep(0) // Yields the thread to the scheduler.

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
	#define THREAD_LOCAL thread_local // Specifies a static variable to use thread local storage.

	#define WARNING(msg) _Pragma(message(__FILE__":(" GAFF_STR(__LINE__)") WARNING - " msg))
	#define YieldThread sched_yield // Yields the thread to the scheduler.
#endif

#ifdef PLATFORM_LINUX
	#define DYNAMICEXPORT __attribute__((visibility("default"))) // Specifies a symbol for export.
	#define DYNAMICIMPORT // Specifies a symbol for import.
#elif defined(PLATFORM_MAC)
	#define DYNAMICEXPORT // Specifies a symbol for export.
	#define DYNAMICIMPORT // Specifies a symbol for import.
#endif

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
	#include <cstdlib> // For size_t
	#include <sched.h> // For sched_yield
#endif

#include <utility> // For std::move() and std::forward()
#include <cstdint> // For (u)int*_t and size_t
