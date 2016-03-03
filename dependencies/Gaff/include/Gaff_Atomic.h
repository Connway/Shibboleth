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

/*!
	\file

	Preprocessor defines for platform specific versions of atomic functions.
	Functions that do not end in FetchOrig will return the modified values.

	For more information, visit this
	<a href="http://msdn.microsoft.com/en-us/library/windows/desktop/ms686360%28v=vs.85%29.aspx">MSDN page</a>
	for more details.
*/

#pragma once

#if defined(_WIN32) || defined(_WIN64)
	#include "Gaff_IncludeWindows.h"

	// Supports unsigned long, unsigned long long, unsigned int, and long
	#define AtomicCompareExchange InterlockedCompareExchange
	#define AtomicIncrement InterlockedIncrement
	#define AtomicDecrement InterlockedDecrement
	#define AtomicAcquire(ptr) InterlockedExchangeAcquire(ptr, 1)
	#define AtomicRelease InterlockedDecrementRelease

	// Only supports long
	#define AtomicAdd32 InterlockedAdd
	#define AtomicSub32(ptr, val) InterlockedAdd(ptr, -val)
	#define AtomicAdd32FetchOrig InterlockedExchangeAdd
	#define AtomicSub32FetchOrig InterlockedExchangeSubtract

	#define AtomicAnd32(ptr, val) (InterlockedAnd(ptr, val) & val)
	#define AtomicOr32(ptr, val) (InterlockedOr(ptr, val) | val)
	#define AtomicXor32(ptr, val) (InterlockedXor(ptr, val) ^ val)
	#define AtomicAnd32FetchOrig InterlockedAnd
	#define AtomicOr32FetchOrig InterlockedOr
	#define AtomicXor32FetchOrig InterlockedXor

	// Only supports long long
	#define AtomicCompareExchange64 InterlockedCompareExchange64
	#define AtomicAdd64 InterlockedAdd64
	#define AtomicSub64(ptr, val) InterlockedAdd64(ptr, -val)
	#define AtomicIncrement64 InterlockedIncrement64
	#define AtomicDecrement64 InterlockedDecrement64
	#define AtomicAnd64(ptr, val) (InterlockedAnd64(ptr, val) & val)
	#define AtomicOr64(ptr, val) (InterlockedOr64(ptr, val) | val)
	#define AtomicXor64(ptr, val) (InterlockedXor64(ptr, val) ^ val)
	#define AtomicAnd64FetchOrig InterlockedAnd64
	#define AtomicOr64FetchOrig InterlockedOr64
	#define AtomicXor64FetchOrig InterlockedXor64

	// Supports any pointer type (except probably member function pointers)
	#define AtomicCompareExchangePointer InterlockedCompareExchangePointer

	#define AtomicExchange InterlockedExchange

	// Only supports unsigned long, unsigned long long, and unsigned int
	#define AtomicUAdd(ptr, val) (InterlockedExchangeAdd(ptr, val) + val)
	#define AtomicUSub(ptr, val) (InterlockedExchangeSubtract(ptr, val) - val)
	#define AtomicUAddFetchOrig InterlockedExchangeAdd
	#define AtomicUSubFetchOrig InterlockedExchangeSubtract

#elif defined(__linux__) || defined(__APPLE__)
	// Supports unsigned long, unsigned long long, unsigned int, and long types
	#define AtomicCompareExchange(dest, new_val, old_val) __sync_val_compare_and_swap(dest, old_val, new_val)
	#define AtomicIncrement(ptr) __sync_add_and_fetch(ptr, 1)
	#define AtomicDecrement(ptr) __sync_sub_and_fetch(ptr, 1)
	#define AtomicAcquire(ptr) __sync_lock_test_and_set(ptr, 1)
	#define AtomicRelease __sync_lock_release

	// Only supports long
	#define AtomicAdd32 __sync_add_and_fetch
	#define AtomicSub32 __sync_sub_and_fetch
	#define AtomicAdd32FetchOrig __sync_fetch_and_add
	#define AtomicSub32FetchOrig __sync_fetch_and_sub

	#define AtomicAnd32 __sync_and_and_fetch
	#define AtomicOr32 __sync_or_and_fetch
	#define AtomicXor32 __sync_xor_and_fetch
	#define AtomicAnd32FetchOrig __sync_fetch_and_and
	#define AtomicOr32FetchOrig __sync_fetch_and_or
	#define AtomicXor32FetchOrig __sync_fetch_and_xor

	// Only supports long long
	#define AtomicCompareExchange64(dest, new_val, old_val) __sync_val_compare_and_swap(dest, old_val, new_val)
	#define AtomicAdd64 __sync_add_and_fetch
	#define AtomicSub64 __sync_sub_and_fetch
	#define AtomicIncrement64(ptr) __sync_add_and_fetch(ptr, 1)
	#define AtomicDecrement64(ptr) __sync_sub_and_fetch(ptr, 1)
	#define AtomicAnd64 __sync_and_and_fetch
	#define AtomicOr64 __sync_or_and_fetch
	#define AtomicXor64 __sync_xor_and_fetch
	#define AtomicAnd64FetchOrig __sync_fetch_and_and
	#define AtomicOr64FetchOrig __sync_fetch_and_or
	#define AtomicXor64FetchOrig __sync_fetch_and_xor

	// Supports any pointer type (except probably member function pointers)
	#define AtomicCompareExchangePointer(dest, new_val, old_val) __sync_val_compare_and_swap(dest, old_val, new_val)

	#define AtomicExchange(dest, new_val) __sync_lock_test_and_set(dest, new_val), __sync_synchronize(), new_val

	// Only supports unsigned long, unsigned long long, and unsigned int
	#define AtomicUAdd __sync_add_and_fetch
	#define AtomicUSub __sync_sub_and_fetch
	#define AtomicUAddFetchOrig __sync_fetch_and_add
	#define AtomicUSubFetchOrig __sync_fetch_and_sub

#else
	#error Platform not supported.
#endif
