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

//#define ATTEMPT_INLINE
//#define DYNAMIC_LIBRARY

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#define NS_GAFF namespace Gaff {
#ifndef NS_END
	#define NS_END }
#endif

#define GAFF_NO_COPY(x) \
	private: \
		x(const x&); \
		const x& operator=(const x&)

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

#ifndef INLINE
	#ifdef ATTEMPT_INLINE
		#define INLINE inline
	#else
		#define INLINE
	#endif
#endif

#ifndef SAFERELEASE
	#define SAFERELEASE(x) if (x) { x->Release(); x = nullptr; }
#endif

#ifndef SAFEADDREF
	#define SAFEADDREF(x) if (x) { x->AddRef(); }
#endif

#define SAFEGAFFRELEASE(x) if (x) { x->release(); x = nullptr; }
#define SAFEGAFFADDREF(x) if (x) { x->addRef(); }

#if defined(_WIN32) || defined(_WIN64)
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif

	#define DYNAMICEXPORT extern "C" __declspec(dllexport)
	#define DYNAMICIMPORT extern "C" __declspec(dllimport)

	#ifdef _UNICODE
		#define GaffFullMain int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd)
		#define GaffMain WINAPI wWinMain
	#else
		#define GaffFullMain int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd)
		#define GaffMain WINAPI WinMain
	#endif
#else
	#define GaffFullMain int main(int argc, char** argv)
	#define GaffMain main
#endif

#if defined(__linux__)
	#define DYNAMICEXPORT extern "C" __attribute__((visibility("default")))
	#define DYNAMICIMPORT
#elif defined(__APPLE__)
	#define DYNAMICEXPORT
	#define DYNAMICIMPORT
#endif

#ifdef DYNAMIC_LIBRARY
	#define DYNAMICTAG DYNAMICEXPORT
#else
	#define DYNAMICTAG DYNAMICIMPORT
#endif

#if defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || \
	defined(__i686__) || defined(_M_IX86) || defined(_X86_)

	#define BITS_PER_BYTE 8

#else
	#error "Unknown architecture. Cannot deduce number of bits per byte."
#endif

NS_GAFF

#if defined(__LP64__) || defined(_WIN64)
	typedef long long OffsetType;
#elif defined(__LP32__) || defined(_WIN32)
	typedef int OffsetType;
#else
	#error "Cannot deduce platform bit-age."
#endif

template <class T>
T&& Move(T& data)
{
	return (T&&)data;
}

template <class T>
void Swap(T& lhs, T& rhs)
{
	T temp = lhs;
	lhs = rhs;
	rhs = temp;
}

NS_END
