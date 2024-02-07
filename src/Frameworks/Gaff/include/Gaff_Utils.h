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

GCC_CLANG_DISABLE_WARNING_PUSH("-Wunused-parameter")
#include <EASTL/array.h>
GCC_CLANG_DISABLE_WARNING_POP()

#include <cstring>
#include <ctime>

#define GAFF_STATIC_FILE_FUNC \
	static void static__file__func(void); \
	namespace \
	{ \
		struct static__file__func__helper \
		{ \
		public: \
			static__file__func__helper(void) { static__file__func(); } \
		}; \
	} \
	static const static__file__func__helper GAFF_CAT(g__static_file_global_, __LINE__); \
	void static__file__func(void)


NS_GAFF

unsigned long GetNumberOfCores(void); //!< Returns the number of physical cores on the machine.
void GetCurrentTimeString(char8_t* buff, size_t count, const char8_t* format);
void GetCurrentTimeString(char* buff, size_t count, const char* format);
void GetCurrentTimeString(wchar_t* buff, size_t size, const wchar_t* format);
bool CreateDir(const char8_t* dir_name, unsigned short mode);
bool CreateDir(const char* dirname, unsigned short mode); //<! Creates directory \a dirname with the given access \a mode.
void DebugPrintf(const char8_t* format_string, ...); //!< Does a printf() to debug output on supported platforms. Unsupported platforms just call normal printf().
void DebugPrintf(const char* format_string, ...); //!< Does a printf() to debug output on supported platforms. Unsupported platforms just call normal printf().
bool GetWorkingDir(char8_t* buffer, size_t size);
bool SetWorkingDir(const char8_t* directory);

void* AlignedMalloc(size_t size, size_t alignment);
void AlignedFree(void* data);
void* AlignedRealloc(void* data, size_t size, size_t alignment);
void* AlignedCalloc(size_t num_members, size_t member_size, size_t alignment);
size_t GetUsableSize(void* ptr);

bool IsDebuggerAttached(void);
void DebugBreak(void);

template <class T, class R, R T::*M>
constexpr ptrdiff_t OffsetOfMember(void);

template <class T, class R>
constexpr ptrdiff_t OffsetOfMember(R T::*m);

template <class Base, class Derived>
constexpr ptrdiff_t OffsetOfClass(void);

template <typename T, typename M> M GetMemberType(M T::*);
template <typename T, typename M> T GetClassType(M T::*);

#define OFFSET_OF(x) Gaff::OffsetOfMember<decltype(Gaff::GetClassType(x)), decltype(Gaff::GetMemberType(x)), x>()

template <size_t SizeA, size_t SizeB>
constexpr eastl::array<char, SizeA + SizeB - 1> ConcatConst(const eastl::array<char, SizeA>& lhs, const char(&rhs)[SizeB]);

template <size_t SizeA, size_t SizeB>
constexpr eastl::array<char, SizeA + SizeB - 1> ConcatConst(const char(&lhs)[SizeA], const char(&rhs)[SizeB]);

NS_END

#include "Gaff_Utils_Common.inl"
