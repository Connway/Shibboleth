/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#ifndef NO_LINKING
	#ifdef USE_DX
		#pragma message("Linking against DirectX libraries")
		#pragma comment(lib, "d3d11.lib")
		#pragma comment(lib, "D3dcompiler.lib")
		#pragma comment(lib, "dxgi.lib")
	#else
		#pragma message("Linking against OpenGL libraries")
		#pragma comment(lib, "Opengl32.lib")
	#endif

	#ifdef USE_DI
		#pragma message("Linking against DirectInput libraries")
		#pragma comment(lib, "Dinput8.lib")
		#pragma comment(lib,"dxguid.lib")
	#endif
#endif

#define NS_GLEAM namespace Gleam {
#ifndef NS_END
	#define NS_END }
#endif

#ifndef NULLPTR
	#define NULLPTR nullptr
#endif

#define RETURNIFFAILEDCLEANUP(r, c) if (FAILED(r)) { c; return false; }
#define RETURNIFFAILED(r) if (FAILED(r)) { return false; }
#define RETURNIFFALSECLEANUP(r, c) if (!r) { c; return false; }
#define RETURNIFFALSE(r) if (!r) { return false; }
#define SAFEGLEAMRELEASE(x) if (x) { x->release(); x = NULLPTR; }

#ifndef SAFERELEASE
	#define SAFERELEASE(x) if (x) { x->Release(); x = NULLPTR; }
#endif

#ifndef INLINE
	#ifdef ATTEMPT_INLINE
		#define INLINE inline
	#else
		#define INLINE
	#endif
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(_WIN32) || defined(_WIN64)
	#pragma warning(disable : 4201)
#endif
