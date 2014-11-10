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

/*! \file */

#pragma once

//#define ATTEMPT_INLINE

#define NS_GAFF namespace Gaff {
#ifndef NS_END
	#define NS_END }
#endif

//! Declares copy constructor and assignment operator as private.
#define GAFF_NO_COPY(x) \
	private: \
		x(const x&); \
		const x& operator=(const x&)

//! Declares move constructor and assignment operator as private.
#define GAFF_NO_MOVE(x) \
	private: \
	x(x&&); \
	const x& operator=(x&&)

#ifdef _UNICODE
	#define GC(x) L##x
	typedef wchar_t GChar;
#else
	#define GC(x) x
	typedef char GChar;
#endif

#ifdef ATTEMPT_INLINE
	#define INLINE inline
#else
	#define INLINE
#endif

#define SAFERELEASE(x) if (x) { x->Release(); x = nullptr; } //!< Safely releases Microsoft COM pointers.
#define SAFEADDREF(x) if (x) { x->AddRef(); } //!< Safely adds a reference to Microsoft COM pointers.

#define SAFEGAFFRELEASE(x) if (x) { x->release(); x = nullptr; } //!< Safely releases pointers that implement IRefCounted.
#define SAFEGAFFADDREF(x) if (x) { x->addRef(); } //!< Safely adds a reference to pointers that implement IRefCounted.

#if defined(_WIN32) || defined(_WIN64)
	#pragma warning(disable : 4201 4512 4129)

	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif

	#define DYNAMICEXPORT __declspec(dllexport) //!< Specifies a symbol for export.
	#define DYNAMICIMPORT __declspec(dllimport) //!< Specifies a symbol for import.

	#ifdef _UNICODE
		#define GaffFullMain int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd)
		#define GaffMain WINAPI wWinMain
	#else
		#define GaffFullMain int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd)
		#define GaffMain WINAPI WinMain
	#endif

	#define YieldThread() Sleep(0) //!< Yields the thread to the scheduler.
#else
	#define GaffFullMain int main(int argc, char** argv)
	#define GaffMain main

	#define YieldThread sched_yield //!< Yields the thread to the scheduler.
#endif

#if defined(__linux__)
	#define DYNAMICEXPORT __attribute__((visibility("default"))) //!< Specifies a symbol for export.
	#define DYNAMICIMPORT //!< Specifies a symbol for import.
#elif defined(__APPLE__)
	#define DYNAMICEXPORT //!< Specifies a symbol for export.
	#define DYNAMICIMPORT //!< Specifies a symbol for import.
#endif

#if defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || \
	defined(__i686__) || defined(_M_IX86) || defined(_X86_)

	#define BITS_PER_BYTE 8 //!< Specifies the number of bits in a byte on the target platform.

#else
	#error "Unknown architecture. Cannot deduce number of bits per byte."
#endif

#define UINT_FAIL static_cast<unsigned int>(-1) //!< Returned from functions that use unsigned ints, but can potentially fail
#define DYNAMICEXPORT_C extern "C" DYNAMICEXPORT //!< Exports a function with C-style symbol names.

NS_GAFF

#if defined(__LP64__) || defined(_WIN64)
	typedef long long OffsetType; //!< Integer type that can hold a pointer on the target platform.
#elif defined(__LP32__) || defined(_WIN32)
	typedef int OffsetType; //!< Integer type that can hold a pointer on the target platform.
#else
	#error "Cannot deduce platform bit-age."
#endif

//! Used to specify data should be moved instead of copied. Similar to std::move.
template <class T>
T&& Move(T& data)
{
	return (T&&)data;
}

//! Swaps two variables using move semantics.
template <class T>
void Swap(T& lhs, T& rhs)
{
	T temp = Move(lhs);
	lhs = Move(rhs);
	rhs = Move(temp);
}

NS_END
