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

#pragma once

#include "Gaff_HashString.h"
#include "Gaff_VectorMap.h"

#include <cstring>
#include <ctime>

#define STATIC_FILE_FUNC \
	static void static__file__func(void); \
	namespace { \
		struct static__file__func__helper \
		{ \
		public: \
			static__file__func__helper(void) { static__file__func(); } \
		}; \
	} \
	static const static__file__func__helper GAFF_CAT(g__static_file_global_, __LINE__); \
	void static__file__func(void)


NS_GAFF

INLINE unsigned long GetNumberOfCores(void); //!< Returns the number of physical cores on the machine.
INLINE void GetCurrentTimeString(char* buff, size_t count, const char* format); //!< Gets the current time as a string using \a format. Result stored in \a buff.
INLINE void GetCurrentTimeString(wchar_t* buff, size_t size, const wchar_t* format);
INLINE bool CreateDir(const char* dirname, unsigned short mode); //<! Creates directory \a dirname with the given access \a mode.
void DebugPrintf(const char* format_string, ...); //!< Does a printf() to debug output on supported platforms. Unsupported platforms just call normal printf().
void DebugPrintf(const wchar_t* format_string, ...); //!< Does a printf() to debug output on supported platforms. Unsupported platforms just call normal printf().
INLINE bool SetWorkingDir(const char* directory);

INLINE void* AlignedOffsetMalloc(size_t size, size_t alignment, size_t offset);
INLINE void* AlignedMalloc(size_t size, size_t alignment);
INLINE void AlignedFree(void* data);

bool IsDebuggerAttached(void);
void DebugBreak(void);

template <class Allocator>
VectorMap<HashString32<Allocator>, U8String<Allocator>, Allocator> ParseCommandLine(int argc, char** argv);

template <class Allocator>
void ParseCommandLine(int argc, char** argv, VectorMap<HashString32<Allocator>, U8String<Allocator>, Allocator>& out);

#ifdef PLATFORM_WINDOWS
template <class Allocator>
VectorMap<HashString32<Allocator>, U8String<Allocator>, Allocator> ParseCommandLine(int argc, wchar_t** argv);

template <class Allocator>
void ParseCommandLine(int argc, wchar_t** argv, VectorMap<HashString32<Allocator>, U8String<Allocator>, Allocator>& out);
#endif

template <class T>
void SetBitsToValue(T& value, T bits, bool set);

template <class T>
void SetBits(T& value, T bits);

template <class T>
void ClearBits(T& value, T bits);

template <class T>
bool IsAnyBitSet(const T& value, T bits);

template <class T>
bool AreAllBitsSet(const T& value, T bits);

template <class T, class R, R T::*M>
constexpr ptrdiff_t OffsetOfMember(void);

template <class T, class R>
ptrdiff_t OffsetOfMember(R T::*m);

template <class Derived, class Base>
ptrdiff_t OffsetOfClass(void);

template <typename T, typename M> M GetMemberType(M T::*);
template <typename T, typename M> T GetClassType(M T::*);

#define OFFSET_OF(x) Gaff::OffsetOfMember<decltype(Gaff::GetClassType(x)), decltype(Gaff::GetMemberType(x)), x>()


#include "Gaff_Utils_Common.inl"

NS_END
