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

#include "Gleam_Defines.h"
#include <xmmintrin.h>
#include <emmintrin.h>

#ifdef ALIGN_SIMD
	#define SIMDLOAD _mm_load_ps
	#define SIMDSTORE _mm_store_ps
#else
	#define SIMDLOAD _mm_loadu_ps
	#define SIMDSTORE _mm_storeu_ps
#endif

NS_GLEAM

__declspec(align(16)) struct SIMDMaskStruct
{
	union
	{
		int ints[4];
		__m128 vec;
	};

	INLINE operator const int*() const { return ints; }
	INLINE operator __m128(void) const { return vec; }
};

extern const SIMDMaskStruct gXMask;
extern const SIMDMaskStruct gYMask;
extern const SIMDMaskStruct gZMask;
extern const SIMDMaskStruct gWMask;
extern const SIMDMaskStruct gXYMask;
extern const SIMDMaskStruct gXZMask;
extern const SIMDMaskStruct gYZMask;
extern const SIMDMaskStruct gXYZMask;
extern const SIMDMaskStruct gXYZWMask;
extern const SIMDMaskStruct gSignMask;
extern const SIMDMaskStruct gInfinity;
extern const SIMDMaskStruct gNegZero;

extern const __m128 gOneXYZ;
extern const __m128 gOne;
extern const __m128 gTwo;
extern const __m128 gZero;
extern const __m128 gHalf;

extern const __m128 gNegOneXYZ;
extern const __m128 gNegOne;

extern const __m128 gPi;
extern const __m128 gTwoPi;
extern const __m128 gPiOverFour;
extern const __m128 gThreePiOverFour;
extern const __m128 gReciprocalTwoPi;

extern const __m128 gXAxis;
extern const __m128 gYAxis;
extern const __m128 gZAxis;
extern const __m128 gWAxis; // dunno wtf this means, but I'm callin' it that. :P

extern const __m128 gATanCoeffs0;
extern const __m128 gATanCoeffs1;
extern const __m128 gArcCoeffs0;
extern const __m128 gArcCoeffs1;
extern const __m128 gSinCoeffs0;
extern const __m128 gSinCoeffs1;
extern const __m128 gCosCoeffs0;
extern const __m128 gCosCoeffs1;

INLINE __m128 SIMDNegate(const __m128& vec);
INLINE bool SIMDEqual(const __m128& left, const __m128& right);
bool SIMDRoughlyEqual(const __m128& left, const __m128& right, float epsilon);
__m128 SIMDPermute(const __m128& left, const __m128& right, unsigned int x, unsigned int y, unsigned int z, unsigned int w);
__m128 SIMDRound(const __m128& vec);
__m128 SIMDACos(const __m128& vec);
__m128 SIMDSin(const __m128& vec);
__m128 SIMDCos(const __m128& vec);
__m128 SIMDATan2(const __m128& y, const __m128& x);
__m128 SIMDATan(const __m128& vec);

__m128 SIMDVec4Dot(const __m128& left, const __m128& right);
INLINE __m128 SIMDVec4AngleUnit(const __m128& left, const __m128& right);
INLINE __m128 SIMDVec4Angle(const __m128& left, const __m128& right);
INLINE __m128 SIMDVec4LengthSquared(const __m128& vec);
INLINE __m128 SIMDVec4Length(const __m128& vec);
INLINE __m128 SIMDVec4ReciprocalLengthSquared(const __m128& vec);
INLINE __m128 SIMDVec4ReciprocalLength(const __m128& vec);
__m128 SIMDVec4Normalize(const __m128& vec);
__m128 SIMDVec4Cross(const __m128& left, const __m128& right);
__m128 SIMDVec4Reflect(const __m128& vec, const __m128& normal);
__m128 SIMDVec4Refract(const __m128& vec, const __m128& normal, float refraction_index);
__m128 SIMDVec4Lerp(const __m128& left, const __m128& right, float t);

NS_END
