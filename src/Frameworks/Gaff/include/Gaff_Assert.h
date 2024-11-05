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

#include "Gaff_Defines.h"
#include <cassert> // Until I find a decent cross-platform dialog box.

NS_GAFF

#ifdef _DEBUG
	#define GAFF_ASSERT_ENABLED
#endif

#ifdef GAFF_ASSERT_ENABLED
	#define GAFF_ASSERT(expr) GAFF_ASSERT_MSG(expr, nullptr)
	/*#define GAFF_ASSERT_MSG(expr, msg, ...) \
		{ \
			const bool __assert_result__ = expr; \
			if (!__assert_result__) { \
				Gaff::Assert(msg, #expr, __FILE__, __LINE__, __VA_ARGS__); \
				assert(__assert_result__); \
			} \
		}*/

	#ifdef PLATFORM_COMPILER_MSVC
		#define GAFF_ASSERT_MSG(expr, msg, ...) { (void)((expr) || (Gaff::Assert(msg, #expr, __FILE__, __LINE__, __VA_ARGS__), 0)); assert(expr); }
	#elif defined(PLATFORM_COMPILER_GCC) || defined(PLATFORM_COMPILER_CLANG)
		GCC_CLANG_DISABLE_WARNING_PUSH("-Wgnu-zero-variadic-macro-arguments")
		#define GAFF_ASSERT_MSG(expr, msg, ...) { (void)((expr) || (Gaff::Assert(msg, #expr, __FILE__, __LINE__, ##__VA_ARGS__), 0)); assert(expr); }
		GCC_CLANG_DISABLE_WARNING_POP()
	#endif

#else
	#define GAFF_ASSERT(expr)
	#define GAFF_ASSERT_MSG(expr, msg, ...)
#endif

#ifdef PLATFORM_COMPILER_MSVC
	// If we're on the MS compiler, just static_assert.
	#define GAFF_TEMPLATE_STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#else
	// Otherwise, just runtime assert.
	#define GAFF_TEMPLATE_STATIC_ASSERT(expr, msg) GAFF_ASSERT_MSG(expr, msg)
#endif

using AssertHandler = void (*)(const char* msg, const char* expr, const char* file, int line);
void DefaultAssertHandler(const char* msg, const char* expr, const char* file, int line);

void SetAssertHandler(AssertHandler handler);
void Assert(const char* msg, const char* expr, const char* file, int line, ...);

NS_END
