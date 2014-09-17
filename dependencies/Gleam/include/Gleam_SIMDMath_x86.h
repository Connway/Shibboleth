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

#include "Gleam_Defines.h"
#include <xmmintrin.h>
#include <emmintrin.h>

NS_GLEAM

typedef __m128i SIMDTypei;
typedef __m128 SIMDType;

struct COMPILERALIGN16 SIMDMaskStruct
{
	SIMDMaskStruct(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
	{
		ints[0] = a;
		ints[1] = b;
		ints[2] = c;
		ints[3] = d;
	}

	union
	{
		unsigned int ints[4];
		SIMDType vec;
	};

	INLINE operator const unsigned int*() const { return ints; }
	INLINE operator SIMDType(void) const { return vec; }
};

struct COMPILERALIGN16 SIMDTransform
{
	SIMDType translation;
	SIMDType rotation;
	SIMDType scale;
};

struct COMPILERALIGN16 SIMDMatrix
{
	SIMDType elements[4];
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

extern const SIMDMaskStruct gYZWMask;
extern const SIMDMaskStruct gXZWMask;
extern const SIMDMaskStruct gXYWMask;

extern const SIMDType gOneXYZ;
extern const SIMDType gOne;
extern const SIMDType gTwo;
extern const SIMDType gZero;
extern const SIMDType gHalf;

extern const SIMDType gNegOneXYZ;
extern const SIMDType gNegOne;

extern const SIMDType gPi;
extern const SIMDType gTwoPi;
extern const SIMDType gPiOverFour;
extern const SIMDType gThreePiOverFour;
extern const SIMDType gReciprocalTwoPi;

extern const SIMDType gXAxis;
extern const SIMDType gYAxis;
extern const SIMDType gZAxis;
extern const SIMDType gWAxis; // dunno wtf this means, but I'm callin' it that. :P

extern const SIMDType gATanCoeffs0;
extern const SIMDType gATanCoeffs1;
extern const SIMDType gArcCoeffs0;
extern const SIMDType gArcCoeffs1;
extern const SIMDType gSinCoeffs0;
extern const SIMDType gSinCoeffs1;
extern const SIMDType gCosCoeffs0;
extern const SIMDType gCosCoeffs1;

extern const SIMDTransform gTransformIdentity;
extern const SIMDMatrix gMatrixIdentity;

template <unsigned int a, unsigned int b, unsigned int c, unsigned int d>
SIMDType SIMDShuffle(const SIMDType& left, const SIMDType& right)
{
	return _mm_shuffle_ps(left, right, _MM_SHUFFLE(a, b, c, d));
}

template <int index>
float SIMDGet(const SIMDType& vec)
{
#ifdef LIMIT_TO_SSE2
	return _mm_cvtss_f32(_mm_shuffle_ps(vec, vec, _MM_SHUFFLE(index, index, index, index)));
#else
	int temp = _mm_extract_ps(vec, index);
	return *((float*)&temp);
#endif
}

template<>
inline float SIMDGet<0>(const SIMDType& vec)
{
	return _mm_cvtss_f32(vec);
}

INLINE SIMDType SIMDNegate(const SIMDType& vec);
INLINE bool SIMDEqual(const SIMDType& left, const SIMDType& right);
bool SIMDRoughlyEqual(const SIMDType& left, const SIMDType& right, const SIMDType& epsilon);
SIMDType SIMDPermute(const SIMDType& left, const SIMDType& right, unsigned int x, unsigned int y, unsigned int z, unsigned int w);
SIMDType SIMDRound(const SIMDType& vec);
SIMDType SIMDACos(const SIMDType& vec);
SIMDType SIMDSin(const SIMDType& vec);
SIMDType SIMDCos(const SIMDType& vec);
SIMDType SIMDATan2(const SIMDType& y, const SIMDType& x);
SIMDType SIMDATan(const SIMDType& vec);

SIMDType SIMDVec4Dot(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDVec4AngleUnit(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDVec4Angle(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDVec4LengthSquared(const SIMDType& vec);
INLINE SIMDType SIMDVec4Length(const SIMDType& vec);
INLINE SIMDType SIMDVec4ReciprocalLengthSquared(const SIMDType& vec);
INLINE SIMDType SIMDVec4ReciprocalLength(const SIMDType& vec);
INLINE SIMDType SIMDVec4Normalize(const SIMDType& vec);
SIMDType SIMDVec4Cross(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDVec4Reflect(const SIMDType& vec, const SIMDType& normal);
SIMDType SIMDVec4Refract(const SIMDType& vec, const SIMDType& normal, const SIMDType& refraction_index);
INLINE SIMDType SIMDVec4Lerp(const SIMDType& left, const SIMDType& right, const SIMDType& t);

INLINE SIMDType SIMDSub(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDAdd(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDMul(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDDiv(const SIMDType& left, const SIMDType& right);

INLINE SIMDType SIMDAnd(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDOr(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDXOr(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDAndNot(const SIMDType& left, const SIMDType& right);

INLINE SIMDType SIMDMin(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDMax(const SIMDType& left, const SIMDType& right);

INLINE SIMDType SIMDCreate(float x, float y, float z, float w);
INLINE SIMDType SIMDCreate(float value);

INLINE float SIMDGet(const SIMDType& vec, unsigned int index);
INLINE float SIMDGetX(const SIMDType& vec);
INLINE float SIMDGetY(const SIMDType& vec);
INLINE float SIMDGetZ(const SIMDType& vec);
INLINE float SIMDGetW(const SIMDType& vec);

INLINE void SIMDSet(SIMDType& vec, float value, unsigned int index);
void SIMDSetX(SIMDType& vec, float value);
void SIMDSetY(SIMDType& vec, float value);
void SIMDSetZ(SIMDType& vec, float value);
void SIMDSetW(SIMDType& vec, float value);

void SIMDStoreAligned(const SIMDType& vec, float* buffer);
void SIMDStore(const SIMDType& vec, float* buffer);
SIMDType SIMDLoadAligned(const float* buffer);
SIMDType SIMDLoad(const float* buffer);

SIMDType SIMDQuatMul(const SIMDType& left, const SIMDType& right);
INLINE SIMDType SIMDQuatConjugate(const SIMDType& quaternion);
INLINE SIMDType SIMDQuatInverse(const SIMDType& quaternion);
INLINE SIMDType SIMDQuatGetAxis(const SIMDType& quaternion);
INLINE SIMDType SIMDQuatGetAngle(const SIMDType& quaternion);

SIMDType SIMDQuatSlerp(const SIMDType& left, const SIMDType& right, const SIMDType& t);
SIMDType SIMDQuatTransform(const SIMDType& quaternion, const SIMDType& vec);
SIMDType SIMDQuatFromAxisAngle(const SIMDType& axis, const SIMDType& angle);
SIMDType SIMDQuatFromAngles(const SIMDType& angles);
SIMDMatrix SIMDQuatToMatrix(const SIMDType& quaternion);
SIMDType SIMDQuatFromMatrix(const SIMDMatrix& matrix);

SIMDType SIMDMatrixMulRow(const SIMDType& row, const SIMDType& col1, const SIMDType& col2, const SIMDType& col3, const SIMDType& col4);
SIMDMatrix SIMDMatrixMul(const SIMDMatrix& left, const SIMDMatrix& right);
SIMDMatrix SIMDMatrixTranspose(const SIMDMatrix& matrix);
SIMDMatrix SIMDMatrixInverse(const SIMDMatrix& matrix);
bool SIMDMatrixHasInfiniteElement(const SIMDMatrix& matrix);
bool SIMDMatrixHasNaNElement(const SIMDMatrix& matrix);

SIMDTransform SIMDTransformConcat(const SIMDTransform& left, const SIMDTransform& right);

NS_END
