/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

// This file is only for defining the PLATFORM_* defines

#pragma once

#ifdef _MSC_VER
	#define PLATFORM_COMPILER_MSVC
#elif defined(__clang__)
	#define PLATFORM_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
	#define PLATFORM_COMPILER_GCC
#else
	#error "Unsupported compiler."
#endif

#if defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_WINDOWS
	#define PLATFORM_NAME "windows"
#elif defined(__linux__)
	#define PLATFORM_LINUX
	#define PLATFORM_NAME "linux"
//#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
//	#define PLATFORM_IOS
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
	#define PLATFORM_MAC
	#define PLATFORM_NAME "macosx"
//#elif defined(__ANDROID__)
//	#define PLATFORM_ANDROID
#else
	#error "Unsupported platform."
#endif

#ifdef PLATFORM_COMPILER_MSVC
	// Add endian detection for Microsoft compiler here.
	// Assuming little endian for now
	#define PLATFORM_LITTLE_ENDIAN

#elif defined(PLATFORM_COMPILER_GCC) || defined(PLATFORM_COMPILER_CLANG)
	#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
		#define PLATFORM_LITTLE_ENDIAN
	#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		#define PLATFORM_BIG_ENDIAN
	#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __LITTLE_ENDIAN__)
		#define PLATFORM_LITTLE_ENDIAN
	#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __BIG_ENDIAN__)
		#define PLATFORM_BIG_ENDIAN
	#elif defined (__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN)
		#define PLATFORM_LITTLE_ENDIAN
	#elif defined (__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN)
		#define PLATFORM_BIG_ENDIAN
	#elif defined (_BYTE_ORDER) && (_BYTE_ORDER == _LITTLE_ENDIAN)
		#define PLATFORM_LITTLE_ENDIAN
	#elif defined (_BYTE_ORDER) && (_BYTE_ORDER == _BIG_ENDIAN)
		#define PLATFORM_BIG_ENDIAN
	#endif
#endif

#if !defined(PLATFORM_LITTLE_ENDIAN) && !defined(PLATFORM_BIG_ENDIAN)
	#error "Could not determine platform endianness."
#endif

#if defined(__LP64__) || defined(_WIN64) || defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__)
	#define PLATFORM_64_BIT
#elif defined(__LP32__) || defined(_WIN32) || defined(_M_IX86) || defined(__i386__) || defined(__arm__)
	#define PLATFORM_32_BIT
#else
	#error "Cannot deduce platform bit-age."
#endif

#ifdef PLATFORM_COMPILER_MSVC
	#define MSVC_DISABLE_WARNING_PUSH(warnings) \
		__pragma(warning(push)) \
		__pragma(warning(disable : warnings))

	#define MSVC_DISABLE_WARNING_POP() __pragma(warning(pop))

#else
	#define MSVC_DISABLE_WARNING_PUSH(warnings)
	#define MSVC_DISABLE_WARNING_POP()
#endif

#ifdef PLATFORM_COMPILER_GCC
	#define GCC_PRAGMA(x) _Pragma(#x)
	#define GCC_DISABLE_WARNING_PUSH(warnings) \
		_Pragma("GCC diagnostic push") \
		GCC_PRAGMA(GCC diagnostic ignored warnings)
	
	#define GCC_DISABLE_WARNING_POP() _Pragma("GCC diagnostic pop")

#else
	#define GCC_DISABLE_WARNING_PUSH(warnings)
	#define GCC_DISABLE_WARNING_POP()
#endif
