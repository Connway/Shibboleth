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

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
	#define NOMINMAX
#endif

#ifdef PRId8
	#undef PRId8
#endif

#ifdef PRId64
	#undef PRId64
#endif

#ifdef PRIdPTR
	#undef PRIdPTR
#endif

#ifdef PRIi8
	#undef PRIi8
#endif

#ifdef PRIi64
	#undef PRIi64
#endif

#ifdef PRIiPTR
	#undef PRIiPTR
#endif

#ifdef PRIo8
	#undef PRIo8
#endif

#ifdef PRIo64
	#undef PRIo64
#endif

#ifdef PRIoPTR
	#undef PRIoPTR
#endif

#ifdef PRIu8
	#undef PRIu8
#endif

#ifdef PRIu64
	#undef PRIu64
#endif

#ifdef PRIuPTR
	#undef PRIuPTR
#endif

#ifdef PRIx8
	#undef PRIx8
#endif

#ifdef PRIx64
	#undef PRIx64
#endif

#ifdef PRIxPTR
	#undef PRIxPTR
#endif

#ifdef PRIX8
	#undef PRIX8
#endif

#ifdef PRIX64
	#undef PRIX64
#endif

#ifdef PRIXPTR
	#undef PRIXPTR
#endif

#ifdef SCNd8
	#undef SCNd8
#endif

#ifdef SCNd64
	#undef SCNd64
#endif

#ifdef SCNdPTR
	#undef SCNdPTR
#endif

#ifdef SCNi8
	#undef SCNi8
#endif

#ifdef SCNi64
	#undef SCNi64
#endif

#ifdef SCNiPTR
	#undef SCNiPTR
#endif

#ifdef SCNi8
	#undef SCNi8
#endif

#ifdef SCNi64
	#undef SCNi64
#endif

#ifdef SCNiPTR
	#undef SCNiPTR
#endif

#ifdef SCNo8
	#undef SCNo8
#endif

#ifdef SCNo64
	#undef SCNo64
#endif

#ifdef SCNoPTR
	#undef SCNoPTR
#endif

#ifdef SCNu8
	#undef SCNu8
#endif

#ifdef SCNu64
	#undef SCNu64
#endif

#ifdef SCNuPTR
	#undef SCNuPTR
#endif

#ifdef SCNx8
	#undef SCNx8
#endif

#ifdef SCNx64
	#undef SCNx64
#endif

#ifdef SCNxPTR
	#undef SCNxPTR
#endif

#include <Windows.h>
