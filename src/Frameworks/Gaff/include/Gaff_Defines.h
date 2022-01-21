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

#pragma once

#include "Gaff_Platform.h"
#include <utility> // For std::move() and std::forward()
#include <cstdint> // For (u)int*_t and size_t

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

#define SAFERELEASE(x) if (x) { x->Release(); x = nullptr; } // Safely releases Microsoft COM pointers.
#define SAFEADDREF(x) if (x) { x->AddRef(); } // Safely adds a reference to Microsoft COM pointers.

#define SAFEGAFFRELEASE(x) if (x) { x->release(); x = nullptr; } // Safely releases pointers that implement IRefCounted.
#define SAFEGAFFADDREF(x) if (x) { x->addRef(); } // Safely adds a reference to pointers that implement IRefCounted.

#define GAFF_REF_HELPER(x) ((void)x)
#define GAFF_REF(...) GAFF_FOR_EACH(GAFF_REF_HELPER, __VA_ARGS__)

#define UFAIL(type) static_cast<type>(-1)
#define SIZE_T_FAIL UFAIL(size_t) // Returned from functions that use size_t's, but can potentially fail
#define DYNAMICEXPORT_C extern "C" DYNAMICEXPORT // Exports a function with C-style symbol names.

#define GAFF_STR_HELPER(x) #x
#define GAFF_STR(x) GAFF_STR_HELPER(x)
#define GAFF_CAT_HELPER(x, y) x##y
#define GAFF_CAT(x, y) GAFF_CAT_HELPER(x, y)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define GAFF_FLAG_BIT(index) (1 << index)

#define GAFF_FAIL_RETURN(expr, return_value) if (!(expr)) { return return_value; }

#ifdef _DEBUG
	#define GAFF_ASSERT_ENABLED
#endif

#define GAFF_EXPAND(x) x
#define GAFF_FOR_EACH_NARG(...) GAFF_FOR_EACH_NARG_(__VA_ARGS__, GAFF_FOR_EACH_RSEQ_N())
#define GAFF_FOR_EACH_NARG_(...) GAFF_EXPAND(GAFF_FOR_EACH_ARG_N(__VA_ARGS__))
#define GAFF_FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define GAFF_FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0

#define GAFF_FOR_EACH_1(what, x, ...) what(x)
#define GAFF_FOR_EACH_2(what, x, ...) \
  what(x); \
  GAFF_EXPAND(GAFF_FOR_EACH_1(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_3(what, x, ...) \
  what(x); \
  GAFF_EXPAND(GAFF_FOR_EACH_2(what, __VA_ARGS__))
#define GAFF_FOR_EACH_4(what, x, ...) \
  what(x); \
  GAFF_EXPAND(GAFF_FOR_EACH_3(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_5(what, x, ...) \
  what(x); \
  GAFF_EXPAND(GAFF_FOR_EACH_4(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_6(what, x, ...) \
  what(x); \
  GAFF_EXPAND(GAFF_FOR_EACH_5(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_7(what, x, ...) \
  what(x); \
  GAFF_EXPAND(GAFF_FOR_EACH_6(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_8(what, x, ...) \
  what(x); \
  GAFF_EXPAND(GAFF_FOR_EACH_7(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_(N, what, ...) GAFF_EXPAND(GAFF_CAT_HELPER(GAFF_FOR_EACH_, N)(what, __VA_ARGS__))
#define GAFF_FOR_EACH(what, ...) GAFF_FOR_EACH_(GAFF_FOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)

#define GAFF_FOR_EACH_COMMA_1(what, x, ...) what(x)
#define GAFF_FOR_EACH_COMMA_2(what, x, ...) \
  what(x), \
  GAFF_EXPAND(GAFF_FOR_EACH_COMMA_1(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_COMMA_3(what, x, ...) \
  what(x), \
  GAFF_EXPAND(GAFF_FOR_EACH_COMMA_2(what, __VA_ARGS__))
#define GAFF_FOR_EACH_COMMA_4(what, x, ...) \
  what(x), \
  GAFF_EXPAND(GAFF_FOR_EACH_COMMA_3(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_COMMA_5(what, x, ...) \
  what(x), \
  GAFF_EXPAND(GAFF_FOR_EACH_COMMA_4(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_COMMA_6(what, x, ...) \
  what(x), \
  GAFF_EXPAND(GAFF_FOR_EACH_COMMA_5(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_COMMA_7(what, x, ...) \
  what(x), \
  GAFF_EXPAND(GAFF_FOR_EACH_COMMA_6(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_COMMA_8(what, x, ...) \
  what(x), \
  GAFF_EXPAND(GAFF_FOR_EACH_COMMA_7(what,  __VA_ARGS__))
#define GAFF_FOR_EACH_COMMA_(N, what, ...) GAFF_EXPAND(GAFF_CAT_HELPER(GAFF_FOR_EACH_COMMA_, N)(what, __VA_ARGS__))
#define GAFF_FOR_EACH_COMMA(what, ...) GAFF_FOR_EACH_COMMA_(GAFF_FOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)

#define GAFF_SINGLE_ARG(...) __VA_ARGS__
#define GAFF_STR_VA(...) GAFF_STR(GAFF_SINGLE_ARG(__VA_ARGS__))

#ifdef PLATFORM_WINDOWS
	// Disable nameless struct/union warning
	// Disable assignment operator could not be generated warning
	// Disable unrecognized character escape sequence warning
#ifdef _MSC_VER
	#pragma warning(disable : 4201 4512 4129)
#endif

	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif

	#define DYNAMICEXPORT __declspec(dllexport) // Specifies a symbol for export.
	#define DYNAMICIMPORT __declspec(dllimport) // Specifies a symbol for import.
	#define THREAD_LOCAL __declspec(thread) // Specifies a static variable to use thread local storage.

	#define WARNING(msg) __pragma(message(__FILE__":(" GAFF_STR(__LINE__)") WARNING - " msg))

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
	#define THREAD_LOCAL thread_local // Specifies a static variable to use thread local storage.

	#define WARNING(msg) _Pragma(message(__FILE__":(" GAFF_STR(__LINE__)") WARNING - " msg))
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

NS_GAFF
	template <class... Args>
	void VarArgRef(Args&&...)
	{
	}
NS_END
