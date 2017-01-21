/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include <Gaff_Platform.h>

#define OPENGL_MULTITHREAD

#define NS_GLEAM namespace Gleam {
#ifndef NS_END
	#define NS_END }
#endif

#define RETURNIFFAILEDCLEANUP(r, c) if (FAILED(r)) { c; return false; }
#define RETURNIFFAILED(r) if (FAILED(r)) { return false; }
#define RETURNIFFALSECLEANUP(r, c) if (!r) { c; return false; }
#define RETURNIFFALSE(r) if (!r) { return false; }

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

#ifdef PLATFORM_WINDOWS
	#pragma warning(disable : 4201)
#else
	#define __stdcall
#endif


NS_GLEAM

enum RendererType
{
	RENDERER_DIRECT3D = 0,
	RENDERER_OPENGL
};

NS_END
