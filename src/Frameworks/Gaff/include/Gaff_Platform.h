/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#if defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_WINDOWS
#elif defined(__linux__)
	#define PLATFORM_LINUX
//#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
//	#define PLATFORM_IOS
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
	#define PLATFORM_MAC
//#elif defined(__ANDROID__)
//	#define PLATFORM_ANDROID
#else
	#error "Unsupported platform."
#endif

#ifdef _MSC_VER
	// Add endian detection for Microsoft compiler here.
	// Assuming little endian for now
	#define PLATFORM_LITTLE_ENDIAN

#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
	#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __LITTLE_ENDIAN__)
		#define PLATFORM_LITTLE_ENDIAN
	#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __BIG_ENDIAN__)
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