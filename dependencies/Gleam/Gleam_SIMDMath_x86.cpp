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

#if defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || \
	defined(__i686__) || defined(_M_IX86) || defined(_X86_)

#include "Gleam_SIMDMath_x86.h"
#include <emmintrin.h>
#include <intrin.h>

NS_GLEAM

const SIMDMaskStruct gXMask(0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000);
const SIMDMaskStruct gYMask(0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000);
const SIMDMaskStruct gZMask(0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000);
const SIMDMaskStruct gWMask(0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF);
const SIMDMaskStruct gXYMask(0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000);
const SIMDMaskStruct gXZMask(0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000);
const SIMDMaskStruct gYZMask(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000);
const SIMDMaskStruct gXYZMask(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000);
const SIMDMaskStruct gXYZWMask(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
const SIMDMaskStruct gSignMask(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF);
const SIMDMaskStruct gInfinity(0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000);
const SIMDMaskStruct gNegZero(0x80000000, 0x80000000, 0x80000000, 0x80000000);

const SIMDMaskStruct gYZWMask(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
const SIMDMaskStruct gXZWMask(0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF);
const SIMDMaskStruct gXYWMask(0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF);

const SIMDType gOneXYZ = { 1.0f, 1.0f, 1.0f, 0.0f };
const SIMDType gOne = { 1.0f, 1.0f, 1.0f, 1.0f };
const SIMDType gTwo = { 2.0f, 2.0f, 2.0f, 2.0f };
const SIMDType gZero = { 0.0f, 0.0f, 0.0f, 0.0f };
const SIMDType gHalf = { 0.5f, 0.5f, 0.5f, 0.5f };

const SIMDType gNegOneXYZ = { -1.0f, -1.0f, -1.0f, 1.0f };
const SIMDType gNegOne = { -1.0f, -1.0f, -1.0f, -1.0f };

const SIMDType gPi = { 3.141592654f, 3.141592654f, 3.141592654f, 3.141592654f };
const SIMDType gTwoPi = { 2.0f*3.141592654f, 2.0f*3.141592654f, 2.0f*3.141592654f, 2.0f*3.141592654f };
const SIMDType gPiOverTwo = { 3.141592654f/2.0f, 3.141592654f/2.0f, 3.141592654f/2.0f, 3.141592654f/2.0f };
const SIMDType gPiOverFour = { 3.141592654f/4.0f, 3.141592654f/4.0f, 3.141592654f/4.0f, 3.141592654f/4.0f };
const SIMDType gThreePiOverFour = { 3.141592654f*3.0f/4.0f, 3.141592654f*3.0f/4.0f, 3.141592654f*3.0f/4.0f, 3.141592654f*3.0f/4.0f };
const SIMDType gReciprocalTwoPi = { 1.0f/(2.0f*3.141592654f), 1.0f/(2.0f*3.141592654f), 1.0f/(2.0f*3.141592654f), 1.0f/(2.0f*3.141592654f) };

const SIMDType gXAxis = { 1.0f, 0.0f, 0.0f, 1.0f };
const SIMDType gYAxis = { 0.0f, 1.0f, 0.0f, 1.0f };
const SIMDType gZAxis = { 0.0f, 0.0f, 1.0f, 0.0f };
const SIMDType gWAxis = { 0.0f, 0.0f, 0.0f, 1.0f };

const SIMDType gATanCoeffs0 = { -0.3333314528f, +0.1999355085f, -0.1420889944f, +0.1065626393f };
const SIMDType gATanCoeffs1 = { -0.0752896400f, +0.0429096138f, -0.0161657367f, +0.0028662257f };
const SIMDType gArcCoeffs0 = { +1.5707963050f, -0.2145988016f, +0.0889789874f, -0.0501743046f };
const SIMDType gArcCoeffs1 = { +0.0308918810f, -0.0170881256f, +0.0066700901f, -0.0012624911f };
const SIMDType gSinCoeffs0 = { -0.16666667f, +0.0083333310f, -0.00019840874f, +2.7525562e-06f };
const SIMDType gSinCoeffs1 = { -2.3889859e-08f, -0.16665852f, +0.0083139502f, -0.00018524670f };
const SIMDType gCosCoeffs0 = { -0.5f, +0.041666638f, -0.0013888378f, +2.4760495e-05f };
const SIMDType gCosCoeffs1 = { -2.6051615e-07f, -0.49992746f, +0.041493919f, -0.0012712436f };

typedef float (*GetElementFunc)(const SIMDType&);
typedef void (*SetElementFunc)(SIMDType&, float);

static GetElementFunc gGetFuncs[4] = {
	&SIMDGet<0>, &SIMDGet<1>,
	&SIMDGet<2>, &SIMDGet<3>,
};

static SetElementFunc gSetFuncs[4] = {
	&SIMDSetX, &SIMDSetY,
	&SIMDSetZ, &SIMDSetW
};

SIMDType SIMDNegate(const SIMDType& vec)
{
	return _mm_sub_ps(gZero, vec);
}

bool SIMDEqual(const SIMDType& left, const SIMDType& right)
{
	SIMDType temp = _mm_cmpeq_ps(left, right);
	return (_mm_movemask_ps(temp) == 0x0f) != 0;
}

bool SIMDRoughlyEqual(const SIMDType& left, const SIMDType& right, const SIMDType& epsilon)
{
	SIMDType delta = _mm_sub_ps(left, right);
	SIMDType temp = gZero;

	temp = _mm_sub_ps(temp, delta);
	temp = _mm_max_ps(temp, delta);
	temp = _mm_cmple_ps(temp, epsilon);

	return (_mm_movemask_ps(temp) == 0xf) != 0;
}

SIMDType SIMDPermute(const SIMDType& left, const SIMDType& right, unsigned int x, unsigned int y, unsigned int z, unsigned int w)
{
	SIMDType temp;
	const unsigned int* uintptr[2];
	unsigned int* workptr = (unsigned int*)&temp;
	uintptr[0] = (const unsigned int*)&left;
	uintptr[1] = (const unsigned int*)&right;

	unsigned int i0, i1, i2, i3, vi0, vi1, vi2, vi3;
	i0 = x & 3;
	vi0 = x >> 2;
	workptr[0] = uintptr[vi0][i0];

	i1 = y & 3;
	vi1 = y >> 2;
	workptr[1] = uintptr[vi1][i1];

	i2 = z & 3;
	vi2 = z >> 2;
	workptr[2] = uintptr[vi2][i2];

	i3 = w & 3;
	vi3 = w >> 2;
	workptr[3] = uintptr[vi3][i3];

	return temp;
}

SIMDType SIMDRound(const SIMDType& vec)
{
	static __m128i fraction_test = _mm_castps_si128(_mm_set1_ps(8388608.0f));

	__m128i test = _mm_and_si128(_mm_castps_si128(vec), _mm_castps_si128(gSignMask));
	test = _mm_cmplt_epi32(test, fraction_test);

	SIMDType temp = _mm_cvtepi32_ps(_mm_cvtps_epi32(vec));
	temp = _mm_and_ps(temp, ((SIMDType*)(&test))[0]);
	test = _mm_andnot_si128(test, _mm_castps_si128(vec));
	temp = _mm_or_ps(temp, ((SIMDType*)(&test))[0]);

	return temp;
}

SIMDType SIMDAbs(const SIMDType& vec)
{
	SIMDType temp = _mm_sub_ps(gZero, vec);
	return _mm_max_ps(temp, vec);
}

SIMDType SIMDACos(const SIMDType& vec)
{
	SIMDType abs = _mm_max_ps(vec, _mm_sub_ps(gZero, vec));
	SIMDType root = _mm_sub_ps(gOne, abs);
	root = _mm_max_ps(gZero, root);
	root = _mm_sqrt_ps(root);

	SIMDType consts = _mm_shuffle_ps(gArcCoeffs1, gArcCoeffs1, _MM_SHUFFLE(3,3,3,3));
	SIMDType t0 = _mm_mul_ps(consts, abs);

	consts = _mm_shuffle_ps(gArcCoeffs1, gArcCoeffs1, _MM_SHUFFLE(2,2,2,2));
	t0 = _mm_add_ps(t0, consts);
	t0 = _mm_mul_ps(t0, abs);

	consts = _mm_shuffle_ps(gArcCoeffs1, gArcCoeffs1, _MM_SHUFFLE(1,1,1,1));
	t0 = _mm_add_ps(t0, consts);
	t0 = _mm_mul_ps(t0, abs);

	consts = _mm_shuffle_ps(gArcCoeffs1, gArcCoeffs1, _MM_SHUFFLE(0,0,0,0));
	t0 = _mm_add_ps(t0, consts);
	t0 = _mm_mul_ps(t0, abs);


	consts = _mm_shuffle_ps(gArcCoeffs0, gArcCoeffs0, _MM_SHUFFLE(3,3,3,3));
	t0 = _mm_add_ps(t0, consts);
	t0 = _mm_mul_ps(t0, abs);

	consts = _mm_shuffle_ps(gArcCoeffs0, gArcCoeffs0, _MM_SHUFFLE(2,2,2,2));
	t0 = _mm_add_ps(t0, consts);
	t0 = _mm_mul_ps(t0, abs);

	consts = _mm_shuffle_ps(gArcCoeffs0, gArcCoeffs0, _MM_SHUFFLE(1,1,1,1));
	t0 = _mm_add_ps(t0, consts);
	t0 = _mm_mul_ps(t0, abs);

	consts = _mm_shuffle_ps(gArcCoeffs0, gArcCoeffs0, _MM_SHUFFLE(0,0,0,0));
	t0 = _mm_add_ps(t0, consts);
	t0 = _mm_mul_ps(t0, root);

	SIMDType nn = _mm_cmpge_ps(vec, gZero);
	SIMDType t1 = _mm_sub_ps(gPi, t0);
	t0 = _mm_and_ps(nn, t0);
	t1 = _mm_andnot_ps(nn, t1);
	t0 = _mm_or_ps(t0, t1);

	return t0;
}

SIMDType SIMDSin(const SIMDType& vec)
{
	SIMDType x = _mm_mul_ps(vec, gReciprocalTwoPi);
	x = SIMDRound(x);
	x = _mm_mul_ps(x, gTwoPi);
	x = _mm_sub_ps(vec, x);

	SIMDType sign = _mm_and_ps(x, gNegZero);
	SIMDType c = _mm_or_ps(gPi, sign);
	SIMDType absx = _mm_andnot_ps(sign, x);
	SIMDType rflx = _mm_sub_ps(c, x);
	SIMDType comp = _mm_cmple_ps(absx, gPiOverTwo);
	x = _mm_or_ps(_mm_and_ps(comp, x), _mm_andnot_ps(comp, rflx));
	SIMDType x_squared = _mm_mul_ps(x, x);

	SIMDType consts = _mm_shuffle_ps(gSinCoeffs1, gSinCoeffs1, _MM_SHUFFLE(0,0,0,0));
	SIMDType temp = _mm_mul_ps(consts, x_squared);

	consts = _mm_shuffle_ps(gSinCoeffs0, gSinCoeffs0, _MM_SHUFFLE(3,3,3,3));
	temp = _mm_add_ps(temp, consts);
	temp = _mm_mul_ps(temp, x_squared);

	consts = _mm_shuffle_ps(gSinCoeffs0, gSinCoeffs0, _MM_SHUFFLE(2,2,2,2));
	temp = _mm_add_ps(temp, consts);
	temp = _mm_mul_ps(temp, x_squared);

	consts = _mm_shuffle_ps(gSinCoeffs0, gSinCoeffs0, _MM_SHUFFLE(1,1,1,1));
	temp = _mm_add_ps(temp, consts);
	temp = _mm_mul_ps(temp, x_squared);

	consts = _mm_shuffle_ps(gSinCoeffs0, gSinCoeffs0, _MM_SHUFFLE(0,0,0,0));
	temp = _mm_add_ps(temp, consts);
	temp = _mm_mul_ps(temp, x_squared);

	temp = _mm_add_ps(temp, gOne);
	temp = _mm_mul_ps(temp, x);

	return temp;
}

SIMDType SIMDCos(const SIMDType& vec)
{
	SIMDType x = _mm_mul_ps(vec, gReciprocalTwoPi);
	x = SIMDRound(x);
	x = _mm_mul_ps(x, gTwoPi);
	x = _mm_sub_ps(vec, x);

	SIMDType sign = _mm_and_ps(x, gNegZero);
	SIMDType c = _mm_or_ps(gPi, sign);
	SIMDType absx = _mm_andnot_ps(sign, x);
	SIMDType rflx = _mm_sub_ps(c, x);
	SIMDType comp = _mm_cmple_ps(absx, gPiOverTwo);
	x = _mm_or_ps(_mm_and_ps(comp, x), _mm_andnot_ps(comp, rflx));
	sign = _mm_or_ps(_mm_and_ps(comp, gOne), _mm_andnot_ps(comp, gNegOne));

	SIMDType x_squared = _mm_mul_ps(x, x);

	SIMDType consts = _mm_shuffle_ps(gCosCoeffs1, gCosCoeffs1, _MM_SHUFFLE(0,0,0,0));
	SIMDType temp = _mm_mul_ps(consts, x_squared);

	consts = _mm_shuffle_ps(gCosCoeffs0, gCosCoeffs0, _MM_SHUFFLE(3,3,3,3));
	temp = _mm_add_ps(temp, consts);
	temp = _mm_mul_ps(temp, x_squared);

	consts = _mm_shuffle_ps(gCosCoeffs0, gCosCoeffs0, _MM_SHUFFLE(2,2,2,2));
	temp = _mm_add_ps(temp, consts);
	temp = _mm_mul_ps(temp, x_squared);

	consts = _mm_shuffle_ps(gCosCoeffs0, gCosCoeffs0, _MM_SHUFFLE(1,1,1,1));
	temp = _mm_add_ps(temp, consts);
	temp = _mm_mul_ps(temp, x_squared);

	consts = _mm_shuffle_ps(gCosCoeffs0, gCosCoeffs0, _MM_SHUFFLE(0,0,0,0));
	temp = _mm_add_ps(temp, consts);
	temp = _mm_mul_ps(temp, x_squared);

	temp = _mm_add_ps(temp, gOne);
	temp = _mm_mul_ps(temp, sign);

	return temp;
}

SIMDType SIMDATan2(const SIMDType& y, const SIMDType& x)
{
	__m128i tempi = _mm_set1_epi32(-1);
	SIMDType valid_result = ((SIMDType*)&tempi)[0];
	SIMDType pi = gPi;
	SIMDType piOverTwo = gPiOverTwo;
	SIMDType piOverFour = gPiOverFour;
	SIMDType threePiOverFour = gThreePiOverFour;

	SIMDType x_zero = _mm_cmpeq_ps(x, gZero);
	SIMDType y_zero = _mm_cmpeq_ps(y, gZero);
	SIMDType x_positive = _mm_and_ps(x, gNegZero);

	{
		__m128i temp = _mm_cmpeq_epi32(_mm_castps_si128(x_positive), _mm_castps_si128(gZero));
		x_positive = ((SIMDType*)(&temp))[0];
	}

	SIMDType x_infinite = _mm_and_ps(x, gSignMask);
	x_infinite = _mm_cmpeq_ps(x_infinite, gInfinity);

	SIMDType y_infinite = _mm_and_ps(y, gSignMask);
	y_infinite = _mm_cmpeq_ps(y_infinite, gInfinity);

	SIMDType y_sign = _mm_and_ps(y, gNegZero);
	tempi = _mm_or_si128(_mm_castps_si128(pi), _mm_castps_si128(y_sign));
	pi = ((SIMDType*)(&tempi))[0];
	tempi = _mm_or_si128(_mm_castps_si128(piOverTwo), _mm_castps_si128(y_sign));
	piOverTwo = ((SIMDType*)(&tempi))[0];
	tempi = _mm_or_si128(_mm_castps_si128(piOverFour), _mm_castps_si128(y_sign));
	piOverFour = ((SIMDType*)(&tempi))[0];
	tempi = _mm_or_si128(_mm_castps_si128(threePiOverFour), _mm_castps_si128(y_sign));
	threePiOverFour = ((SIMDType*)(&tempi))[0];

	SIMDType r1 = _mm_or_ps(_mm_andnot_ps(x_positive, pi), _mm_and_ps(y_sign, x_positive));
	SIMDType r2 = _mm_or_ps(_mm_andnot_ps(x_zero, valid_result), _mm_and_ps(piOverTwo, x_zero));
	SIMDType r3 = _mm_or_ps(_mm_andnot_ps(y_zero, r2), _mm_and_ps(r1, y_zero));
	SIMDType r4 = _mm_or_ps(_mm_andnot_ps(x_positive, threePiOverFour), _mm_and_ps(piOverFour, x_positive));
	SIMDType r5 = _mm_or_ps(_mm_andnot_ps(x_infinite, piOverTwo), _mm_and_ps(r4, x_infinite));
	SIMDType temp = _mm_or_ps(_mm_andnot_ps(y_infinite, r3), _mm_and_ps(r5, y_infinite));
	tempi = _mm_cmpeq_epi32(_mm_castps_si128(temp), _mm_castps_si128(valid_result));
	valid_result = ((SIMDType*)(&tempi))[0];

	SIMDType v = _mm_div_ps(y, x);
	SIMDType r0 = SIMDATan(v);
	r1 = _mm_or_ps(_mm_andnot_ps(x_positive, pi), _mm_and_ps(gZero, x_positive));
	r2 = _mm_add_ps(r0, r1);

	return _mm_or_ps(_mm_andnot_ps(valid_result, temp), _mm_and_ps(r2, valid_result));
}

SIMDType SIMDATan(const SIMDType& vec)
{
	SIMDType abs = SIMDAbs(vec);
	SIMDType inv = _mm_div_ps(gOne, vec);
	SIMDType comp = _mm_cmpgt_ps(vec, gOne);
	SIMDType sign = _mm_or_ps(_mm_and_ps(comp, gOne), _mm_andnot_ps(comp, gNegOne));
	comp = _mm_cmple_ps(abs, gOne);
	sign = _mm_or_ps(_mm_and_ps(comp, gZero), _mm_andnot_ps(comp, sign));
	SIMDType x = _mm_or_ps(_mm_and_ps(comp, vec), _mm_andnot_ps(comp, inv));
	SIMDType x_squared = _mm_mul_ps(x, x);

	SIMDType consts = _mm_shuffle_ps(gATanCoeffs1, gATanCoeffs1, _MM_SHUFFLE(3,3,3,3));
	SIMDType temp1 = _mm_mul_ps(consts, x_squared);

	consts = _mm_shuffle_ps(gATanCoeffs1, gATanCoeffs1, _MM_SHUFFLE(2,2,2,2));
	temp1 = _mm_add_ps(temp1, consts);
	temp1 = _mm_mul_ps(temp1, x_squared);

	consts = _mm_shuffle_ps(gATanCoeffs1, gATanCoeffs1, _MM_SHUFFLE(1,1,1,1));
	temp1 = _mm_add_ps(temp1, consts);
	temp1 = _mm_mul_ps(temp1, x_squared);

	consts = _mm_shuffle_ps(gATanCoeffs1, gATanCoeffs1, _MM_SHUFFLE(0,0,0,0));
	temp1 = _mm_add_ps(temp1, consts);
	temp1 = _mm_mul_ps(temp1, x_squared);


	consts = _mm_shuffle_ps(gATanCoeffs0, gATanCoeffs0, _MM_SHUFFLE(3,3,3,3));
	temp1 = _mm_add_ps(temp1, consts);
	temp1 = _mm_mul_ps(temp1, x_squared);

	consts = _mm_shuffle_ps(gATanCoeffs0, gATanCoeffs0, _MM_SHUFFLE(2,2,2,2));
	temp1 = _mm_add_ps(temp1, consts);
	temp1 = _mm_mul_ps(temp1, x_squared);

	consts = _mm_shuffle_ps(gATanCoeffs0, gATanCoeffs0, _MM_SHUFFLE(1,1,1,1));
	temp1 = _mm_add_ps(temp1, consts);
	temp1 = _mm_mul_ps(temp1, x_squared);

	consts = _mm_shuffle_ps(gATanCoeffs0, gATanCoeffs0, _MM_SHUFFLE(0,0,0,0));
	temp1 = _mm_add_ps(temp1, consts);
	temp1 = _mm_mul_ps(temp1, x_squared);

	temp1 = _mm_add_ps(temp1, gOne);
	temp1 = _mm_mul_ps(temp1, x);

	SIMDType temp2 = _mm_mul_ps(sign, gPiOverTwo);
	temp2 = _mm_sub_ps(temp2, temp1);

	comp = _mm_cmpeq_ps(sign, gZero);
	return _mm_or_ps(_mm_and_ps(comp, temp1), _mm_andnot_ps(comp, temp2));
}

SIMDType SIMDVec4Dot(const SIMDType& left, const SIMDType& right)
{
#ifdef LIMIT_TO_SSE2
	SIMDType temp1 = _mm_mul_ps(left, right);
	SIMDType temp2 = right;

	temp2 = _mm_shuffle_ps(temp2, temp1, _MM_SHUFFLE(1,0,0,0));
	temp2 = _mm_add_ps(temp2, temp1);
	temp1 = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(0,3,0,0));
	temp1 = _mm_add_ps(temp1, temp2);

	return _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(2,2,2,2));
#else
	SIMDType temp = _mm_mul_ps(left, right);
	temp = _mm_hadd_ps(temp, temp); // a[3]*b[3]+a[2]*b[2], a[1]*b[1]+a[0]*b[0], a[3]*b[3]+a[2]*b[2], a[1]*b[1]+a[0]*b[0]
	return _mm_hadd_ps(temp, temp); // fill all elements with dot product
#endif
}

SIMDType SIMDVec4AngleUnit(const SIMDType& left, const SIMDType& right)
{
	return SIMDACos(SIMDVec4Dot(left, right));
}

SIMDType SIMDVec4Angle(const SIMDType& left, const SIMDType& right)
{
	return SIMDVec4AngleUnit(SIMDVec4Normalize(left), SIMDVec4Normalize(right));
}

SIMDType SIMDVec4LengthSquared(const SIMDType& vec)
{
	return SIMDVec4Dot(vec, vec);
}

SIMDType SIMDVec4Length(const SIMDType& vec)
{
	return _mm_sqrt_ps(SIMDVec4Dot(vec, vec));
}

SIMDType SIMDVec4ReciprocalLengthSquared(const SIMDType& vec)
{
	return _mm_div_ps(gOne, SIMDVec4Dot(vec, vec));
}

SIMDType SIMDVec4ReciprocalLength(const SIMDType& vec)
{
	return _mm_div_ps(gOne, _mm_sqrt_ps(SIMDVec4Dot(vec, vec)));
}

SIMDType SIMDVec4Normalize(const SIMDType& vec)
{
	SIMDType length = _mm_sqrt_ps(SIMDVec4Dot(vec, vec));
	return _mm_div_ps(vec, length);
}

SIMDType SIMDVec4Cross(const SIMDType& left, const SIMDType& right)
{
	SIMDType temp1 = _mm_shuffle_ps(left, left, _MM_SHUFFLE(3,0,2,1));
	SIMDType temp2 = _mm_shuffle_ps(right, right, _MM_SHUFFLE(3,1,0,2));

	SIMDType result = _mm_mul_ps(temp1, temp2);

	temp1 = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(3,0,2,1));
	temp2 = _mm_shuffle_ps(temp2, temp2,_MM_SHUFFLE(3,1,0,2));

	temp1 = _mm_mul_ps(temp1, temp2);
	result = _mm_sub_ps(result, temp1);

	return _mm_and_ps(result, gXYZMask);
}

SIMDType SIMDVec4Reflect(const SIMDType& vec, const SIMDType& normal)
{
	SIMDType result = _mm_mul_ps(gTwo, SIMDVec4Dot(vec, normal));
	result = _mm_mul_ps(result, normal);
	return _mm_sub_ps(vec, result);
}

SIMDType SIMDVec4Refract(const SIMDType& vec, const SIMDType& normal, const SIMDType& refraction_index)
{
	SIMDType idn = SIMDVec4Dot(vec, normal);
	SIMDType k = _mm_mul_ps(idn, idn);
	k = _mm_sub_ps(gOne, k);
	k = _mm_mul_ps(k, refraction_index);
	k = _mm_mul_ps(k, refraction_index);
	k = _mm_sub_ps(gOne, k);

	if (_mm_movemask_ps(_mm_cmple_ps(k, gZero))) {
		return gZero;
	}

	k = _mm_add_ps(_mm_sqrt_ps(k), _mm_mul_ps(refraction_index, idn));
	k = _mm_mul_ps(k, normal);
	return _mm_sub_ps(_mm_mul_ps(refraction_index, vec), k);
}

SIMDType SIMDVec4Lerp(const SIMDType& left, const SIMDType& right, const SIMDType& t)
{
	SIMDType temp = _mm_mul_ps(t, _mm_sub_ps(right, left));
	return _mm_add_ps(left, temp);
}

SIMDType SIMDSub(const SIMDType& left, const SIMDType& right)
{
	return _mm_sub_ps(left, right);
}

SIMDType SIMDAdd(const SIMDType& left, const SIMDType& right)
{
	return _mm_add_ps(left, right);
}

SIMDType SIMDMul(const SIMDType& left, const SIMDType& right)
{
	return _mm_mul_ps(left, right);
}

SIMDType SIMDDiv(const SIMDType& left, const SIMDType& right)
{
	return _mm_div_ps(left, right);
}

SIMDType SIMDAnd(const SIMDType& left, const SIMDType& right)
{
	return _mm_and_ps(left, right);
}

SIMDType SIMDOr(const SIMDType& left, const SIMDType& right)
{
	return _mm_or_ps(left, right);
}

SIMDType SIMDXOr(const SIMDType& left, const SIMDType& right)
{
	return _mm_xor_ps(left, right);
}

SIMDType SIMDAndNot(const SIMDType& left, const SIMDType& right)
{
	return _mm_andnot_ps(left, right);
}

SIMDType SIMDCreate(float x, float y, float z, float w)
{
	return _mm_set_ps(w, z, y, x);
}

SIMDType SIMDCreate(float value)
{
	return _mm_set_ps1(value);
}

float SIMDGet(const SIMDType& vec, unsigned int index)
{
	return gGetFuncs[index](vec);
}

float SIMDGetX(const SIMDType& vec)
{
	return _mm_cvtss_f32(vec);
}

float SIMDGetY(const SIMDType& vec)
{
	return SIMDGet<1>(vec);
}

float SIMDGetZ(const SIMDType& vec)
{
	return SIMDGet<2>(vec);
}

float SIMDGetW(const SIMDType& vec)
{
	return SIMDGet<3>(vec);
}

void SIMDSet(SIMDType& vec, float value, unsigned int index)
{
	gSetFuncs[index](vec, value);
}

void SIMDSetX(SIMDType& vec, float value)
{
	SIMDType temp = _mm_set_ps(value, 0.0f, 0.0f, 0.0f);
	vec = _mm_and_ps(vec, gYZWMask); // clear x value
	vec = _mm_or_ps(temp, vec);
}

void SIMDSetY(SIMDType& vec, float value)
{
	SIMDType temp = _mm_set_ps(0.0f, value, 0.0f, 0.0f);
	vec = _mm_and_ps(vec, gXZWMask); // clear y value
	vec = _mm_or_ps(temp, vec);
}

void SIMDSetZ(SIMDType& vec, float value)
{
	SIMDType temp = _mm_set_ps(0.0f, 0.0f, value, 0.0f);
	vec = _mm_and_ps(vec, gXYWMask); // clear z value
	vec = _mm_or_ps(temp, vec);
}

void SIMDSetW(SIMDType& vec, float value)
{
	SIMDType temp = _mm_set_ps(0.0f, 0.0f, 0.0f, value);
	vec = _mm_and_ps(vec, gXYZMask); // clear w value
	vec = _mm_or_ps(temp, vec);
}

void SIMDStoreAligned(const SIMDType& vec, float* buffer)
{
	_mm_storeu_ps(buffer, vec);
}

void SIMDStore(const SIMDType& vec, float* buffer)
{
	_mm_store_ps(buffer, vec);
}

SIMDType SIMDLoadAligned(float* buffer)
{
	return _mm_loadu_ps(buffer);
}

SIMDType SIMDLoad(float* buffer)
{
	return _mm_load_ps(buffer);
}

NS_END

#endif
