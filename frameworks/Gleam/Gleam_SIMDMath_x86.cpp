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

#if defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || \
	defined(__i686__) || defined(_M_IX86) || defined(_X86_)

#include "Gleam_SIMDMath_x86.h"
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

const SIMDType gXAxis = { 1.0f, 0.0f, 0.0f, 0.0f };
const SIMDType gYAxis = { 0.0f, 1.0f, 0.0f, 0.0f };
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

const SIMDTransform gTransformIdentity = {
	gWAxis, gWAxis, gOne
};

const SIMDMatrix gMatrixIdentity = {{
	gXAxis, gYAxis,
	gZAxis, gWAxis
}};

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

	return (_mm_movemask_ps(temp) == 0x0f) != 0;
}

bool SIMDLessThanOrEqual(const SIMDType& left, const SIMDType& right)
{
	SIMDType temp = _mm_cmple_ps(left, right);
	return (_mm_movemask_ps(temp) == 0x0f) != 0;
}

bool SIMDLessThan(const SIMDType& left, const SIMDType& right)
{
	SIMDType temp = _mm_cmplt_ps(left, right);
	return (_mm_movemask_ps(temp) == 0x0f) != 0;
}

bool SIMDGreaterThanOrEqual(const SIMDType& left, const SIMDType& right)
{
	SIMDType temp = _mm_cmpge_ps(left, right);
	return (_mm_movemask_ps(temp) == 0x0f) != 0;
}

bool SIMDGreaterThan(const SIMDType& left, const SIMDType& right)
{
	SIMDType temp = _mm_cmpgt_ps(left, right);
	return (_mm_movemask_ps(temp) == 0x0f) != 0;
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
	static SIMDTypei fraction_test = _mm_castps_si128(_mm_set1_ps(8388608.0f));

	SIMDTypei test = _mm_and_si128(_mm_castps_si128(vec), _mm_castps_si128(gSignMask));
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
	SIMDTypei tempi = _mm_set1_epi32(-1);
	SIMDType valid_result = ((SIMDType*)&tempi)[0];
	SIMDType pi = gPi;
	SIMDType piOverTwo = gPiOverTwo;
	SIMDType piOverFour = gPiOverFour;
	SIMDType threePiOverFour = gThreePiOverFour;

	SIMDType x_zero = _mm_cmpeq_ps(x, gZero);
	SIMDType y_zero = _mm_cmpeq_ps(y, gZero);
	SIMDType x_positive = _mm_and_ps(x, gNegZero);

	{
		SIMDTypei temp = _mm_cmpeq_epi32(_mm_castps_si128(x_positive), _mm_castps_si128(gZero));
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

SIMDType SIMDSqrt(const SIMDType& vec)
{
	return _mm_sqrt_ps(vec);
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

SIMDType SIMDMin(const SIMDType& left, const SIMDType& right)
{
	return _mm_min_ps(left, right);
}

SIMDType SIMDMax(const SIMDType& left, const SIMDType& right)
{
	return _mm_max_ps(left, right);
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
	_mm_store_ps(buffer, vec);
}

void SIMDStore(const SIMDType& vec, float* buffer)
{
	_mm_storeu_ps(buffer, vec);
}

SIMDType SIMDLoadAligned(const float* buffer)
{
	return _mm_load_ps(buffer);
}

SIMDType SIMDLoad(const float* buffer)
{
	return _mm_loadu_ps(buffer);
}

SIMDType SIMDQuatMul(const SIMDType& left, const SIMDType& right)
{
	static const SIMDType controlWZYX = { 1.0f, -1.0f, 1.0f, -1.0f };
	static const SIMDType controlZWXY = { 1.0f, 1.0f, -1.0f, -1.0f };
	static const SIMDType controlYXWZ = { -1.0f, 1.0f, 1.0f, -1.0f };

	SIMDType x = _mm_shuffle_ps(left, left, _MM_SHUFFLE(0, 0, 0, 0));
	SIMDType y = _mm_shuffle_ps(left, left, _MM_SHUFFLE(1, 1, 1, 1));
	SIMDType z = _mm_shuffle_ps(left, left, _MM_SHUFFLE(2, 2, 2, 2));
	SIMDType temp1 = _mm_shuffle_ps(left, left, _MM_SHUFFLE(3, 3, 3, 3));

	temp1 = _mm_mul_ps(temp1, right);

	SIMDType temp2 = _mm_shuffle_ps(right, right, _MM_SHUFFLE(0, 1, 2, 3));
	x = _mm_mul_ps(x, temp2);
	x = _mm_mul_ps(x, controlWZYX);

	temp2 = _mm_shuffle_ps(temp2, temp2, _MM_SHUFFLE(2, 3, 0, 1));
	y = _mm_mul_ps(y, temp2);
	y = _mm_mul_ps(y, controlZWXY);

	temp2 = _mm_shuffle_ps(temp2, temp2, _MM_SHUFFLE(0, 1, 2, 3));
	z = _mm_mul_ps(z, temp2);
	z = _mm_mul_ps(z, controlYXWZ);

	temp1 = _mm_add_ps(temp1, x);
	temp1 = _mm_add_ps(temp1, y);
	temp1 = _mm_add_ps(temp1, z);

	return temp1;
}

SIMDType SIMDQuatConjugate(const SIMDType& quaternion)
{
	return _mm_mul_ps(quaternion, gNegOneXYZ);
}

SIMDType SIMDQuatInverse(const SIMDType& quaternion)
{
	SIMDType scale = SIMDVec4ReciprocalLengthSquared(quaternion);
	SIMDType quat = SIMDQuatConjugate(quaternion);
	return _mm_mul_ps(quat, scale);
}

SIMDType SIMDQuatGetAxis(const SIMDType& quaternion)
{
	return _mm_and_ps(quaternion, gXYZMask);
}

SIMDType SIMDQuatGetAngle(const SIMDType& quaternion)
{
	SIMDType w = _mm_shuffle_ps(quaternion, quaternion, _MM_SHUFFLE(3, 3, 3, 3));
	return SIMDMul(gTwo, SIMDACos(w));
}

SIMDType SIMDQuatSlerp(const SIMDType& left, const SIMDType& right, const SIMDType& t)
{
	static const SIMDType eps = { 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f };
	static const SIMDType sign_x = { 0x80000000, 0x00000000, 0x00000000, 0x00000000 };

	SIMDType cos_theta = SIMDVec4Dot(left, right);
	SIMDType control = _mm_cmplt_ps(cos_theta, gZero);
	SIMDType sign = _mm_or_ps(_mm_andnot_ps(control, gOne), _mm_and_ps(gNegOne, control));

	cos_theta = _mm_mul_ps(cos_theta, sign);
	control = _mm_cmplt_ps(cos_theta, eps);

	SIMDType sin_theta = _mm_mul_ps(cos_theta, cos_theta);
	sin_theta = _mm_sub_ps(gOne, sin_theta);
	sin_theta = _mm_sqrt_ps(sin_theta);

	SIMDType theta = SIMDATan2(sin_theta, cos_theta);
	SIMDType v = _mm_shuffle_ps(t, t, _MM_SHUFFLE(2, 3, 0, 1));
	v = _mm_and_ps(v, gXYMask);
	v = _mm_xor_ps(v, sign_x);
	v = _mm_and_ps(v, gXAxis);

	SIMDType s0 = _mm_mul_ps(v, theta);
	s0 = SIMDSin(s0);
	s0 = _mm_div_ps(s0, sin_theta);
	s0 = _mm_or_ps(_mm_andnot_ps(control, v), _mm_and_ps(s0, control));

	SIMDType s1 = _mm_shuffle_ps(s0, s0, _MM_SHUFFLE(1, 1, 1, 1));
	s0 = _mm_shuffle_ps(s0, s0, _MM_SHUFFLE(0, 0, 0, 0));
	s1 = _mm_mul_ps(s1, sign);
	s1 = _mm_mul_ps(s1, right);

	return _mm_add_ps(_mm_mul_ps(left, s0), s1);
}

SIMDType SIMDQuatTransform(const SIMDType& quaternion, const SIMDType& vec)
{
	SIMDType q_conj = SIMDQuatConjugate(quaternion);
	SIMDType q = _mm_and_ps(gXYZMask, quaternion);

	q = SIMDQuatMul(quaternion, vec);
	q = SIMDQuatMul(q, q_conj);

	return _mm_and_ps(q, gXYZMask);
}

SIMDType SIMDQuatFromAxisAngle(const SIMDType& axis, const SIMDType& angle)
{
	SIMDType a = _mm_mul_ps(gTwo, angle);
	SIMDType v = _mm_or_ps(_mm_and_ps(axis, gXYZMask), gWAxis); // v = [x, y, z, 1]
	SIMDType s = SIMDSin(a);

	// s = [sin,sin,sin,cos]
	s = _mm_and_ps(s, gXYZMask);
	s = _mm_or_ps(s, _mm_and_ps(SIMDCos(a), gWMask));

	return _mm_mul_ps(v, s);
}

SIMDType SIMDQuatFromAngles(const SIMDType& angles)
{
	static SIMDType sign = { 1.0f, -1.0f, -1.0f, 1.0f };

	SIMDType vec = _mm_mul_ps(gHalf, angles);
	SIMDType sin_angles = SIMDSin(vec);
	SIMDType cos_angles = SIMDCos(vec);

	SIMDType p0 = SIMDPermute(sin_angles, cos_angles, 0, 4, 4, 4);
	SIMDType y0 = SIMDPermute(sin_angles, cos_angles, 5, 1, 5, 5);
	SIMDType r0 = SIMDPermute(sin_angles, cos_angles, 6, 6, 2, 6);
	SIMDType p1 = SIMDPermute(cos_angles, sin_angles, 0, 4, 4, 4);
	SIMDType y1 = SIMDPermute(cos_angles, sin_angles, 5, 1, 5, 5);
	SIMDType r1 = SIMDPermute(cos_angles, sin_angles, 6, 6, 2, 6);

	SIMDType q0 = _mm_mul_ps(p0, y0);
	SIMDType q1 = _mm_mul_ps(p1, sign);
	q0 = _mm_mul_ps(q0, r0);
	q1 = _mm_mul_ps(q1, y1);

	return _mm_add_ps(_mm_mul_ps(q1, r1), q0);
}

SIMDMatrix SIMDQuatToMatrix(const SIMDType& quaternion)
{
	SIMDType q0 = _mm_add_ps(quaternion, quaternion);
	SIMDType q1 = _mm_mul_ps(q0, quaternion);
	SIMDType v0 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 0, 0, 1));
	SIMDType v1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 1, 2, 2));
	v0 = _mm_and_ps(v0, gXYZMask);
	v1 = _mm_and_ps(v1, gXYZMask);
	SIMDType r0 = _mm_sub_ps(gOneXYZ, v0);
	r0 = _mm_sub_ps(r0, v1);

	v0 = _mm_shuffle_ps(quaternion, quaternion, _MM_SHUFFLE(3, 1, 0, 0));
	v1 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3, 2, 1, 2));
	v0 = _mm_mul_ps(v0, v1);

	v1 = _mm_shuffle_ps(quaternion, quaternion, _MM_SHUFFLE(3, 3, 3, 3));
	SIMDType v2 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3, 0, 2, 1));
	v1 = _mm_mul_ps(v1, v2);

	SIMDType r1 = _mm_add_ps(v0, v1);
	SIMDType r2 = _mm_sub_ps(v0, v1);

	v0 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(1, 0, 2, 1));
	v0 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(1, 3, 2, 0));
	v1 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(2, 2, 0, 0));
	v1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(2, 0, 2, 0));

	q1 = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(1, 0, 3, 0));
	q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 2, 0));

	SIMDType col1, col2, col3, col4;
	col1 = q1;

	q1 = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(3, 2, 3, 1));
	q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 0, 2));
	col2 = q1;

	col3 = _mm_shuffle_ps(v1, r0, _MM_SHUFFLE(3, 2, 1, 0));
	col4 = gWAxis;

	SIMDMatrix matrix = { {col1, col2, col3, col4} };

	return matrix;
}

SIMDType SIMDQuatFromMatrix(const SIMDMatrix& matrix)
{
	static const SIMDType sign1 = { +1.0f, -1.0f, -1.0f, +1.0f };
	static const SIMDType sign2 = { -1.0f, +1.0f, -1.0f, +1.0f };
	static const SIMDType sign3 = { -1.0f, -1.0f, +1.0f, +1.0f };

	SIMDType r0 = matrix.elements[0];
	SIMDType r1 = matrix.elements[1];
	SIMDType r2 = matrix.elements[3];
	SIMDType r00 = _mm_shuffle_ps(r0, r0, _MM_SHUFFLE(0, 0, 0, 0));
	SIMDType r11 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(1, 1, 1, 1));
	SIMDType r22 = _mm_shuffle_ps(r2, r2, _MM_SHUFFLE(2, 2, 2, 2));

	SIMDType r11sr00 = _mm_sub_ps(r11, r00);
	SIMDType r11sr00_le = _mm_cmple_ps(r11sr00, gZero);

	SIMDType r11pr00 = _mm_add_ps(r11, r00);
	SIMDType r11pr00_le = _mm_cmple_ps(r11pr00, gZero);

	SIMDType r22_le = _mm_cmple_ps(r22, gZero);

	SIMDType t0 = _mm_mul_ps(sign1, r00);
	SIMDType t1 = _mm_mul_ps(sign2, r11);
	SIMDType t2 = _mm_mul_ps(sign3, r22);

	SIMDType temp1 = _mm_add_ps(t0, t1);
	temp1 = _mm_add_ps(t2, temp1);
	temp1 = _mm_add_ps(gOne, temp1);

	t0 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(1, 2, 2, 1));
	t1 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(1, 0, 0, 0));
	t1 = _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(1, 3, 2, 0));

	SIMDType temp2 = _mm_add_ps(t0, t1);

	t0 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(0, 0, 0, 1));
	t1 = _mm_shuffle_ps(r1, r0, _MM_SHUFFLE(1, 2, 2, 2));
	t1 = _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(1, 3, 2, 0));

	SIMDType temp3 = _mm_add_ps(t0, t1);
	temp2 = _mm_mul_ps(temp2, sign2);

	t0 = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(0, 0, 1, 0));
	t1 = _mm_shuffle_ps(temp1, temp3, _MM_SHUFFLE(0, 2, 3, 2));
	t2 = _mm_shuffle_ps(temp2, temp3, _MM_SHUFFLE(1, 0, 2, 1));

	SIMDType tensor0 = _mm_shuffle_ps(t0, t2, _MM_SHUFFLE(2, 0, 2, 0));
	SIMDType tensor1 = _mm_shuffle_ps(t0, t2, _MM_SHUFFLE(3, 1, 1, 2));
	SIMDType tensor2 = _mm_shuffle_ps(t2, t1, _MM_SHUFFLE(2, 0, 1, 0));
	SIMDType tensor3 = _mm_shuffle_ps(t2, t1, _MM_SHUFFLE(1, 2, 3, 2));

	t0 = _mm_and_ps(r11sr00_le, tensor0);
	t1 = _mm_andnot_ps(r11sr00_le, tensor1);
	t0 = _mm_or_ps(t0, t1);
	t1 = _mm_and_ps(r11pr00_le, tensor2);
	t2 = _mm_andnot_ps(r11pr00_le, tensor3);
	t1 = _mm_or_ps(t1, t2);
	t0 = _mm_and_ps(r22_le, t0);
	t1 = _mm_andnot_ps(r22_le, t1);
	t2 = _mm_or_ps(t0, t1);

	t0 = SIMDVec4Length(t2);
	return _mm_div_ps(t2, t0);
}

SIMDType SIMDQuatShortestRotation(const SIMDType& vec1, const SIMDType& vec2)
{
	//return CalQuaternion(cross[0], cross[1], cross[2], -dot / 2);

	SIMDType cross = SIMDVec4Cross(vec1, vec2);
	SIMDType dot = SIMDVec4Dot(vec1, vec2);

	// sqrt(2 * (dot + 1))
	dot = _mm_add_ps(dot, gOne);
	dot = _mm_or_ps(dot, gTwo);
	dot = _mm_sqrt_ps(dot);

	cross = _mm_div_ps(cross, dot);

	// dot = -dot / 2
	dot = _mm_sub_ps(gZero, dot);
	dot = _mm_div_ps(dot, gTwo);
	
	dot = _mm_shuffle_ps(cross, dot, _MM_SHUFFLE(0, 0, 1, 0)); // [-dot / 2, -dot / 2, cross[2], cross[3]]
	return _mm_shuffle_ps(dot, cross, _MM_SHUFFLE(3, 2, 1, 3)); // [cross[0], cross[1], cross[2], -dot / 2]
}

SIMDType SIMDMatrixMulRow(const SIMDType& row, const SIMDType& col1, const SIMDType& col2, const SIMDType& col3, const SIMDType& col4)
{
	SIMDType x = _mm_shuffle_ps(row, row, _MM_SHUFFLE(0,0,0,0));
	SIMDType y = _mm_shuffle_ps(row, row, _MM_SHUFFLE(1,1,1,1));
	SIMDType z = _mm_shuffle_ps(row, row, _MM_SHUFFLE(2,2,2,2));
	SIMDType w = _mm_shuffle_ps(row, row, _MM_SHUFFLE(3,3,3,3));

	x = _mm_mul_ps(x, col1);
	y = _mm_mul_ps(y, col2);
	z = _mm_mul_ps(z, col3);
	w = _mm_mul_ps(w, col4);

	x = _mm_add_ps(x, z);
	y = _mm_add_ps(y, w);
	x = _mm_add_ps(x, y);

	return x;
}

SIMDMatrix SIMDMatrixMul(const SIMDMatrix& left, const SIMDMatrix& right)
{
	SIMDMatrix result = {{
		SIMDMatrixMulRow(right.elements[0], left.elements[1], left.elements[2], left.elements[3], left.elements[4]),
		SIMDMatrixMulRow(right.elements[1], left.elements[1], left.elements[2], left.elements[3], left.elements[4]),
		SIMDMatrixMulRow(right.elements[2], left.elements[1], left.elements[2], left.elements[3], left.elements[4]),
		SIMDMatrixMulRow(right.elements[3], left.elements[1], left.elements[2], left.elements[3], left.elements[4])
	}};

	return result;
}

SIMDMatrix SIMDMatrixTranspose(const SIMDMatrix& matrix)
{
	//SIMDMatrix result = {{
	//	_mm_shuffle_ps(matrix.elements[0], matrix.elements[1], _MM_SHUFFLE(1, 0, 1, 0)),
	//	_mm_shuffle_ps(matrix.elements[2], matrix.elements[3], _MM_SHUFFLE(1, 0, 1, 0)),
	//	_mm_shuffle_ps(matrix.elements[0], matrix.elements[1], _MM_SHUFFLE(3, 2, 3, 2)),
	//	_mm_shuffle_ps(matrix.elements[2], matrix.elements[3], _MM_SHUFFLE(3, 2, 3, 2))
	//}};

	SIMDType temp1 = _mm_shuffle_ps(matrix.elements[0], matrix.elements[1], _MM_SHUFFLE(1,0,1,0));
	SIMDType temp3 = _mm_shuffle_ps(matrix.elements[0], matrix.elements[1],_MM_SHUFFLE(3,2,3,2));
	SIMDType temp2 = _mm_shuffle_ps(matrix.elements[2], matrix.elements[3],_MM_SHUFFLE(1,0,1,0));
	SIMDType temp4 = _mm_shuffle_ps(matrix.elements[2], matrix.elements[3],_MM_SHUFFLE(3,2,3,2));

	SIMDMatrix result = {{
		_mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(2,0,2,0)),
		_mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(3,1,3,1)),
		_mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(2,0,2,0)),
		_mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(3,1,3,1))
	}};

	return result;
}

SIMDMatrix SIMDMatrixInverse(const SIMDMatrix& matrix)
{
	SIMDType col1 = matrix.elements[0];
	SIMDType col2 = matrix.elements[1];
	SIMDType col3 = matrix.elements[2];
	SIMDType col4 = matrix.elements[3];

	SIMDType v00 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(1, 1, 0, 0));
	SIMDType v10 = _mm_shuffle_ps(col4, col4, _MM_SHUFFLE(3, 2, 3, 2));
	SIMDType v01 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(1, 1, 0, 0));
	SIMDType v11 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(3, 2, 3, 2));
	SIMDType v02 = _mm_shuffle_ps(col3, col1, _MM_SHUFFLE(2, 0, 2, 0));
	SIMDType v12 = _mm_shuffle_ps(col4, col2, _MM_SHUFFLE(3, 1, 3, 1));

	SIMDType d0 = _mm_mul_ps(v00, v10);
	SIMDType d1 = _mm_mul_ps(v01, v11);
	SIMDType d2 = _mm_mul_ps(v02, v12);

	v00 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(3, 2, 3, 2));
	v10 = _mm_shuffle_ps(col4, col4, _MM_SHUFFLE(1, 1, 0, 0));
	v01 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(3, 2, 3, 2));
	v11 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(1, 1, 0, 0));
	v02 = _mm_shuffle_ps(col3, col1, _MM_SHUFFLE(3, 1, 3, 1));
	v12 = _mm_shuffle_ps(col4, col2, _MM_SHUFFLE(2, 0, 2, 0));

	v00 = _mm_mul_ps(v00, v10);
	v01 = _mm_mul_ps(v01, v11);
	v02 = _mm_mul_ps(v02, v12);
	d0 = _mm_sub_ps(d0, v00);
	d1 = _mm_sub_ps(d1, v01);
	d2 = _mm_sub_ps(d2, v02);

	v11 = _mm_shuffle_ps(d0, d2, _MM_SHUFFLE(1, 1, 3, 1));
	v00 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(1, 0, 2, 1));
	v10 = _mm_shuffle_ps(v11, d0, _MM_SHUFFLE(0, 3, 0, 2));
	v01 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(0, 1, 0, 2));
	v11 = _mm_shuffle_ps(v11, d0, _MM_SHUFFLE(2, 1, 2, 1));

	SIMDType v13 = _mm_shuffle_ps(d1, d2, _MM_SHUFFLE(3, 3, 3, 1));
	v02 = _mm_shuffle_ps(col4, col4, _MM_SHUFFLE(1, 0, 2, 1));
	v12 = _mm_shuffle_ps(v13, d1, _MM_SHUFFLE(0, 3, 0, 2));
	SIMDType v03 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(0, 1, 0, 2));
	v13 = _mm_shuffle_ps(v13, d1, _MM_SHUFFLE(2, 1, 2, 1));

	SIMDType c0 = _mm_mul_ps(v00, v10);
	SIMDType c2 = _mm_mul_ps(v01, v11);
	SIMDType c4 = _mm_mul_ps(v02, v12);
	SIMDType c6 = _mm_mul_ps(v03, v13);

	v11 = _mm_shuffle_ps(d0, d2, _MM_SHUFFLE(0, 0, 1, 0));
	v00 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(2, 1, 3, 2));
	v10 = _mm_shuffle_ps(d0, v11, _MM_SHUFFLE(2, 1, 0, 3));
	v01 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(1, 3, 2, 3));
	v11 = _mm_shuffle_ps(d0, v11, _MM_SHUFFLE(0, 2, 1, 2));

	v13 = _mm_shuffle_ps(d1, d2, _MM_SHUFFLE(2, 2, 1, 0));
	v02 = _mm_shuffle_ps(col4, col4, _MM_SHUFFLE(2, 1, 3, 2));
	v12 = _mm_shuffle_ps(d1, v13, _MM_SHUFFLE(2, 1, 0, 3));
	v03 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(1, 3, 2, 3));
	v13 = _mm_shuffle_ps(d1, v13, _MM_SHUFFLE(0, 2, 1, 2));

	v00 = _mm_mul_ps(v00, v10);
	v01 = _mm_mul_ps(v01, v11);
	v02 = _mm_mul_ps(v02, v12);
	v03 = _mm_mul_ps(v03, v13);
	c0 = _mm_sub_ps(c0, v00);
	c2 = _mm_sub_ps(c2, v01);
	c4 = _mm_sub_ps(c4, v02);
	c6 = _mm_sub_ps(c6, v03);

	v00 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(0, 3, 0, 3));

	v10 = _mm_shuffle_ps(d0, d2, _MM_SHUFFLE(1, 0, 2, 2));
	v10 = _mm_shuffle_ps(v10, v10, _MM_SHUFFLE(0, 2, 3, 0));
	v01 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(2, 0, 3, 1));

	v11 = _mm_shuffle_ps(d0, d2, _MM_SHUFFLE(1, 0, 3, 0));
	v11 = _mm_shuffle_ps(v11, v11, _MM_SHUFFLE(2, 1, 0, 3));
	v02 = _mm_shuffle_ps(col4, col4, _MM_SHUFFLE(0, 3, 0, 3));

	v12 = _mm_shuffle_ps(d1, d2, _MM_SHUFFLE(3, 2, 2, 2));
	v12 = _mm_shuffle_ps(v12, v12, _MM_SHUFFLE(0, 2, 3, 0));
	v03 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(2, 0, 3, 1));

	v13 = _mm_shuffle_ps(d1, d2, _MM_SHUFFLE(3, 2, 3, 0));
	v13 = _mm_shuffle_ps(v13, v13, _MM_SHUFFLE(2, 1, 0, 3));

	v00 = _mm_mul_ps(v00, v10);
	v01 = _mm_mul_ps(v01, v11);
	v02 = _mm_mul_ps(v02, v12);
	v03 = _mm_mul_ps(v03, v13);
	SIMDType c1 = _mm_sub_ps(c0, v00);
	c0 = _mm_add_ps(c0, v00);
	SIMDType c3 = _mm_add_ps(c2, v01);
	c2 = _mm_sub_ps(c2, v01);
	SIMDType c5 = _mm_sub_ps(c4, v02);
	c4 = _mm_add_ps(c4, v02);
	SIMDType c7 = _mm_add_ps(c6, v03);
	c6 = _mm_sub_ps(c6, v03);

	c0 = _mm_shuffle_ps(c0, c1, _MM_SHUFFLE(3, 1, 2, 0));
	c2 = _mm_shuffle_ps(c2, c3, _MM_SHUFFLE(3, 1, 2, 0));
	c4 = _mm_shuffle_ps(c4, c5, _MM_SHUFFLE(3, 1, 2, 0));
	c6 = _mm_shuffle_ps(c6, c7, _MM_SHUFFLE(3, 1, 2, 0));
	c0 = _mm_shuffle_ps(c0, c0, _MM_SHUFFLE(3, 1, 2, 0));
	c2 = _mm_shuffle_ps(c2, c2, _MM_SHUFFLE(3, 1, 2, 0));
	c4 = _mm_shuffle_ps(c4, c4, _MM_SHUFFLE(3, 1, 2, 0));
	c6 = _mm_shuffle_ps(c6, c6, _MM_SHUFFLE(3, 1, 2, 0));

	// dot product
	SIMDType temp1 = _mm_mul_ps(c0, col1);
	SIMDType temp2 = col1;

	temp2 = _mm_shuffle_ps(temp2, temp1, _MM_SHUFFLE(1, 0, 0, 0));
	temp2 = _mm_add_ps(temp2, temp1);
	temp1 = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(0, 3, 0, 0));
	temp1 = _mm_add_ps(temp1, temp2);

	SIMDType determinant = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(2, 2, 2, 2));
	determinant = _mm_div_ps(_mm_set1_ps(1.0f), determinant);

	col1 = _mm_mul_ps(c0, determinant);
	col2 = _mm_mul_ps(c2, determinant);
	col3 = _mm_mul_ps(c4, determinant);
	col4 = _mm_mul_ps(c6, determinant);

	// check for an infinite element
	SIMDType sign_mask = _mm_set1_ps(0x7FFFFFFF);
	SIMDType infinity = _mm_set1_ps(0x7F800000);

	c1 = _mm_and_ps(col1, sign_mask);
	c2 = _mm_and_ps(col2, sign_mask);
	c3 = _mm_and_ps(col3, sign_mask);
	c4 = _mm_and_ps(col4, sign_mask);

	c1 = _mm_cmpeq_ps(c1, infinity);
	c2 = _mm_cmpeq_ps(c2, infinity);
	c3 = _mm_cmpeq_ps(c3, infinity);
	c4 = _mm_cmpeq_ps(c4, infinity);

	c1 = _mm_or_ps(c1, c2);
	c2 = _mm_or_ps(c3, c4);
	c1 = _mm_or_ps(c1, c2);

	if (_mm_movemask_ps(c1) != 0) {
		return gMatrixIdentity;
	}

	temp1 = _mm_shuffle_ps(col1, col2, _MM_SHUFFLE(1, 0, 1, 0));
	temp2 = _mm_shuffle_ps(col3, col4, _MM_SHUFFLE(1, 0, 1, 0));
	SIMDType temp3 = _mm_shuffle_ps(col1, col2, _MM_SHUFFLE(3, 2, 3, 2));
	SIMDType temp4 = _mm_shuffle_ps(col3, col4, _MM_SHUFFLE(3, 2, 3, 2));

	SIMDMatrix result = {{
		_mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(2, 0, 2, 0)),
		_mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(3, 1, 3, 1)),
		_mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(2, 0, 2, 0)),
		_mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(3, 1, 3, 1))
	}};

	return result;
}

bool SIMDMatrixHasInfiniteElement(const SIMDMatrix& matrix)
{
	SIMDType col1 = _mm_and_ps(matrix.elements[0], gSignMask);
	SIMDType col2 = _mm_and_ps(matrix.elements[1], gSignMask);
	SIMDType col3 = _mm_and_ps(matrix.elements[2], gSignMask);
	SIMDType col4 = _mm_and_ps(matrix.elements[3], gSignMask);

	col1 = _mm_cmpeq_ps(col1, gInfinity);
	col2 = _mm_cmpeq_ps(col2, gInfinity);
	col3 = _mm_cmpeq_ps(col3, gInfinity);
	col4 = _mm_cmpeq_ps(col4, gInfinity);

	col1 = _mm_or_ps(col1, col2);
	col2 = _mm_or_ps(col3, col4);
	col1 = _mm_or_ps(col1, col2);

	return _mm_movemask_ps(col1) != 0;
}

bool SIMDMatrixHasNaNElement(const SIMDMatrix& matrix)
{
	SIMDType col1 = _mm_cmpneq_ps(matrix.elements[0], matrix.elements[0]);
	SIMDType col2 = _mm_cmpneq_ps(matrix.elements[1], matrix.elements[1]);
	SIMDType col3 = _mm_cmpneq_ps(matrix.elements[2], matrix.elements[2]);
	SIMDType col4 = _mm_cmpneq_ps(matrix.elements[3], matrix.elements[3]);

	col1 = _mm_or_ps(col1, col2);
	col2 = _mm_or_ps(col3, col4);
	col1 = _mm_or_ps(col1, col2);

	return _mm_movemask_ps(col1) != 0;
}

SIMDTransform SIMDTransformConcat(const SIMDTransform& left, const SIMDTransform& right)
{
	SIMDTransform result = {
		SIMDAdd(left.translation, right.translation),
		SIMDQuatMul(left.rotation, right.rotation),
		SIMDMul(left.scale, right.scale)
	};

	return result;
}

NS_END

#endif
