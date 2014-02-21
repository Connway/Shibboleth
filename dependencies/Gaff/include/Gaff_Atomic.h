/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

	// Only supports long and unsigned long long
	#define AtomicAnd InterlockedAnd
	#define AtomicOr InterlockedOr
	#define AtomicXor InterlockedXor

	// Only supports long long
	#define AtomicCompareExchange64 InterlockedCompareExchange64
	#define AtomicAdd64 InterlockedAdd64
	#define AtomicSub64(ptr, val) InterlockedAdd64(ptr, -val)
	#define AtomicIncrement64 InterlockedIncrement64
	#define AtomicDecrement64 InterlockedDecrement64
	#define AtomicAnd64 InterlockedAnd64
	#define AtomicOr64 InterlockedOr64
	#define AtomicXor64 InterlockedXor64

	// Supports any pointer type (except probably member function pointers)
	#define AtomicCompareExchangePointer InterlockedCompareExchangePointer

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

	// Only supports long and unsigned long long
	#define AtomicAnd InterlockedAnd __sync_and_and_fetch
	#define AtomicOr InterlockedOr __sync_or_and_fetch
	#define AtomicXor InterlockedXor __sync_xor_and_fetch

	// Only supports long long
	#define AtomicCompareExchange64(dest, new_val, old_val) __sync_val_compare_and_swap(dest, old_val, new_val)
	#define AtomicAdd64 __sync_add_and_fetch
	#define AtomicSub64 __sync_sub_and_fetch
	#define AtomicIncrement64(ptr) __sync_add_and_fetch(ptr, 1)
	#define AtomicDecrement64(ptr) __sync_sub_and_fetch(ptr, 1)
	#define AtomicAnd64 InterlockedAnd64 __sync_and_and_fetch
	#define AtomicOr64 InterlockedOr64 __sync_or_and_fetch
	#define AtomicXor64 InterlockedXor64 __sync_xor_and_fetch

	// Supports any pointer type (except probably member function pointers)
	#define AtomicCompareExchangePointer(dest, new_val, old_val) __sync_val_compare_and_swap(dest, old_val, new_val)

#else
	#error Platform not supported.
#endif
