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

#include "Gleam_SIMDMath_x86.h"

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

const __m128 gOneXYZ = { 1.0f, 1.0f, 1.0f, 0.0f };
const __m128 gOne = { 1.0f, 1.0f, 1.0f, 1.0f };
const __m128 gTwo = { 2.0f, 2.0f, 2.0f, 2.0f };
const __m128 gZero = { 0.0f, 0.0f, 0.0f, 0.0f };
const __m128 gHalf = { 0.5f, 0.5f, 0.5f, 0.5f };

const __m128 gNegOneXYZ = { -1.0f, -1.0f, -1.0f, 1.0f };
const __m128 gNegOne = { -1.0f, -1.0f, -1.0f, -1.0f };

const __m128 gPi = { 3.141592654f, 3.141592654f, 3.141592654f, 3.141592654f };
const __m128 gTwoPi = { 2.0f*3.141592654f, 2.0f*3.141592654f, 2.0f*3.141592654f, 2.0f*3.141592654f };
const __m128 gPiOverTwo = { 3.141592654f/2.0f, 3.141592654f/2.0f, 3.141592654f/2.0f, 3.141592654f/2.0f };
const __m128 gPiOverFour = { 3.141592654f/4.0f, 3.141592654f/4.0f, 3.141592654f/4.0f, 3.141592654f/4.0f };
const __m128 gThreePiOverFour = { 3.141592654f*3.0f/4.0f, 3.141592654f*3.0f/4.0f, 3.141592654f*3.0f/4.0f, 3.141592654f*3.0f/4.0f };
const __m128 gReciprocalTwoPi = { 1.0f/(2.0f*3.141592654f), 1.0f/(2.0f*3.141592654f), 1.0f/(2.0f*3.141592654f), 1.0f/(2.0f*3.141592654f) };

const __m128 gXAxis = { 1.0f, 0.0f, 0.0f, 1.0f };
const __m128 gYAxis = { 0.0f, 1.0f, 0.0f, 1.0f };
const __m128 gZAxis = { 0.0f, 0.0f, 1.0f, 0.0f };
const __m128 gWAxis = { 0.0f, 0.0f, 0.0f, 1.0f };

const __m128 gATanCoeffs0 = { -0.3333314528f, +0.1999355085f, -0.1420889944f, +0.1065626393f };
const __m128 gATanCoeffs1 = { -0.0752896400f, +0.0429096138f, -0.0161657367f, +0.0028662257f };
const __m128 gArcCoeffs0 = { +1.5707963050f, -0.2145988016f, +0.0889789874f, -0.0501743046f };
const __m128 gArcCoeffs1 = { +0.0308918810f, -0.0170881256f, +0.0066700901f, -0.0012624911f };
const __m128 gSinCoeffs0 = { -0.16666667f, +0.0083333310f, -0.00019840874f, +2.7525562e-06f };
const __m128 gSinCoeffs1 = { -2.3889859e-08f, -0.16665852f, +0.0083139502f, -0.00018524670f };
const __m128 gCosCoeffs0 = { -0.5f, +0.041666638f, -0.0013888378f, +2.4760495e-05f };
const __m128 gCosCoeffs1 = { -2.6051615e-07f, -0.49992746f, +0.041493919f, -0.0012712436f };

__m128 SIMDNegate(const __m128& vec)
{
	return _mm_sub_ps(gZero, vec);
}

bool SIMDEqual(const __m128& left, const __m128& right)
{
	__m128 temp = _mm_cmpeq_ps(left, right);
	return (_mm_movemask_ps(temp) == 0x0f) != 0;
}

bool SIMDRoughlyEqual(const __m128& left, const __m128& right, float epsilon)
{
	__m128 eps = _mm_set1_ps(epsilon);
	__m128 delta = _mm_sub_ps(left, right);
	__m128 temp = gZero;

	temp = _mm_sub_ps(temp, delta);
	temp = _mm_max_ps(temp, delta);
	temp = _mm_cmple_ps(temp, eps);

	return (_mm_movemask_ps(temp) == 0xf) != 0;
}

__m128 SIMDPermute(const __m128& left, const __m128& right, unsigned int x, unsigned int y, unsigned int z, unsigned int w)
{
	__m128 temp;
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

__m128 SIMDRound(const __m128& vec)
{
	static __m128i fraction_test = _mm_castps_si128(_mm_set1_ps(8388608.0f));

	__m128i test = _mm_and_si128(_mm_castps_si128(vec), _mm_castps_si128(gSignMask));
	test = _mm_cmplt_epi32(test, fraction_test);

	__m128 temp = _mm_cvtepi32_ps(_mm_cvtps_epi32(vec));
	temp = _mm_and_ps(temp, ((__m128*)(&test))[0]);
	test = _mm_andnot_si128(test, _mm_castps_si128(vec));
	temp = _mm_or_ps(temp, ((__m128*)(&test))[0]);

	return temp;
}

__m128 SIMDAbs(const __m128& vec)
{
	__m128 temp = _mm_sub_ps(gZero, vec);
	return _mm_max_ps(temp, vec);
}

__m128 SIMDACos(const __m128& vec)
{
	__m128 abs = _mm_max_ps(vec, _mm_sub_ps(gZero, vec));
	__m128 root = _mm_sub_ps(gOne, abs);
	root = _mm_max_ps(gZero, root);
	root = _mm_sqrt_ps(root);

	__m128 consts = _mm_shuffle_ps(gArcCoeffs1, gArcCoeffs1, _MM_SHUFFLE(3,3,3,3));
	__m128 t0 = _mm_mul_ps(consts, abs);

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

	__m128 nn = _mm_cmpge_ps(vec, gZero);
	__m128 t1 = _mm_sub_ps(gPi, t0);
	t0 = _mm_and_ps(nn, t0);
	t1 = _mm_andnot_ps(nn, t1);
	t0 = _mm_or_ps(t0, t1);

	return t0;
}

__m128 SIMDSin(const __m128& vec)
{
	__m128 x = _mm_mul_ps(vec, gReciprocalTwoPi);
	x = SIMDRound(x);
	x = _mm_mul_ps(x, gTwoPi);
	x = _mm_sub_ps(vec, x);

	__m128 sign = _mm_and_ps(x, gNegZero);
	__m128 c = _mm_or_ps(gPi, sign);
	__m128 absx = _mm_andnot_ps(sign, x);
	__m128 rflx = _mm_sub_ps(c, x);
	__m128 comp = _mm_cmple_ps(absx, gPiOverTwo);
	x = _mm_or_ps(_mm_and_ps(comp, x), _mm_andnot_ps(comp, rflx));
	__m128 x_squared = _mm_mul_ps(x, x);

	__m128 consts = _mm_shuffle_ps(gSinCoeffs1, gSinCoeffs1, _MM_SHUFFLE(0,0,0,0));
	__m128 temp = _mm_mul_ps(consts, x_squared);

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

__m128 SIMDCos(const __m128& vec)
{
	__m128 x = _mm_mul_ps(vec, gReciprocalTwoPi);
	x = SIMDRound(x);
	x = _mm_mul_ps(x, gTwoPi);
	x = _mm_sub_ps(vec, x);

	__m128 sign = _mm_and_ps(x, gNegZero);
	__m128 c = _mm_or_ps(gPi, sign);
	__m128 absx = _mm_andnot_ps(sign, x);
	__m128 rflx = _mm_sub_ps(c, x);
	__m128 comp = _mm_cmple_ps(absx, gPiOverTwo);
	x = _mm_or_ps(_mm_and_ps(comp, x), _mm_andnot_ps(comp, rflx));
	sign = _mm_or_ps(_mm_and_ps(comp, gOne), _mm_andnot_ps(comp, gNegOne));

	__m128 x_squared = _mm_mul_ps(x, x);

	__m128 consts = _mm_shuffle_ps(gCosCoeffs1, gCosCoeffs1, _MM_SHUFFLE(0,0,0,0));
	__m128 temp = _mm_mul_ps(consts, x_squared);

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

__m128 SIMDATan2(const __m128& y, const __m128& x)
{
	__m128i tempi = _mm_set1_epi32(-1);
	__m128 valid_result = ((__m128*)&tempi)[0];
	__m128 pi = gPi;
	__m128 piOverTwo = gPiOverTwo;
	__m128 piOverFour = gPiOverFour;
	__m128 threePiOverFour = gThreePiOverFour;

	__m128 x_zero = _mm_cmpeq_ps(x, gZero);
	__m128 y_zero = _mm_cmpeq_ps(y, gZero);
	__m128 x_positive = _mm_and_ps(x, gNegZero);

	{
		__m128i temp = _mm_cmpeq_epi32(_mm_castps_si128(x_positive), _mm_castps_si128(gZero));
		x_positive = ((__m128*)(&temp))[0];
	}

	__m128 x_infinite = _mm_and_ps(x, gSignMask);
	x_infinite = _mm_cmpeq_ps(x_infinite, gInfinity);

	__m128 y_infinite = _mm_and_ps(y, gSignMask);
	y_infinite = _mm_cmpeq_ps(y_infinite, gInfinity);

	__m128 y_sign = _mm_and_ps(y, gNegZero);
	tempi = _mm_or_si128(_mm_castps_si128(pi), _mm_castps_si128(y_sign));
	pi = ((__m128*)(&tempi))[0];
	tempi = _mm_or_si128(_mm_castps_si128(piOverTwo), _mm_castps_si128(y_sign));
	piOverTwo = ((__m128*)(&tempi))[0];
	tempi = _mm_or_si128(_mm_castps_si128(piOverFour), _mm_castps_si128(y_sign));
	piOverFour = ((__m128*)(&tempi))[0];
	tempi = _mm_or_si128(_mm_castps_si128(threePiOverFour), _mm_castps_si128(y_sign));
	threePiOverFour = ((__m128*)(&tempi))[0];

	__m128 r1 = _mm_or_ps(_mm_andnot_ps(x_positive, pi), _mm_and_ps(y_sign, x_positive));
	__m128 r2 = _mm_or_ps(_mm_andnot_ps(x_zero, valid_result), _mm_and_ps(piOverTwo, x_zero));
	__m128 r3 = _mm_or_ps(_mm_andnot_ps(y_zero, r2), _mm_and_ps(r1, y_zero));
	__m128 r4 = _mm_or_ps(_mm_andnot_ps(x_positive, threePiOverFour), _mm_and_ps(piOverFour, x_positive));
	__m128 r5 = _mm_or_ps(_mm_andnot_ps(x_infinite, piOverTwo), _mm_and_ps(r4, x_infinite));
	__m128 temp = _mm_or_ps(_mm_andnot_ps(y_infinite, r3), _mm_and_ps(r5, y_infinite));
	tempi = _mm_cmpeq_epi32(_mm_castps_si128(temp), _mm_castps_si128(valid_result));
	valid_result = ((__m128*)(&tempi))[0];

	__m128 v = _mm_div_ps(y, x);
	__m128 r0 = SIMDATan(v);
	r1 = _mm_or_ps(_mm_andnot_ps(x_positive, pi), _mm_and_ps(gZero, x_positive));
	r2 = _mm_add_ps(r0, r1);

	return _mm_or_ps(_mm_andnot_ps(valid_result, temp), _mm_and_ps(r2, valid_result));
}

__m128 SIMDATan(const __m128& vec)
{
	__m128 abs = SIMDAbs(vec);
	__m128 inv = _mm_div_ps(gOne, vec);
	__m128 comp = _mm_cmpgt_ps(vec, gOne);
	__m128 sign = _mm_or_ps(_mm_and_ps(comp, gOne), _mm_andnot_ps(comp, gNegOne));
	comp = _mm_cmple_ps(abs, gOne);
	sign = _mm_or_ps(_mm_and_ps(comp, gZero), _mm_andnot_ps(comp, sign));
	__m128 x = _mm_or_ps(_mm_and_ps(comp, vec), _mm_andnot_ps(comp, inv));
	__m128 x_squared = _mm_mul_ps(x, x);

	__m128 consts = _mm_shuffle_ps(gATanCoeffs1, gATanCoeffs1, _MM_SHUFFLE(3,3,3,3));
	__m128 temp1 = _mm_mul_ps(consts, x_squared);

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

	__m128 temp2 = _mm_mul_ps(sign, gPiOverTwo);
	temp2 = _mm_sub_ps(temp2, temp1);

	comp = _mm_cmpeq_ps(sign, gZero);
	return _mm_or_ps(_mm_and_ps(comp, temp1), _mm_andnot_ps(comp, temp2));
}

__m128 SIMDVec4Dot(const __m128& left, const __m128& right)
{
	__m128 temp1 = _mm_mul_ps(left, right);
	__m128 temp2 = right;

	temp2 = _mm_shuffle_ps(temp2, temp1, _MM_SHUFFLE(1,0,0,0));
	temp2 = _mm_add_ps(temp2, temp1);
	temp1 = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(0,3,0,0));
	temp1 = _mm_add_ps(temp1, temp2);

	return _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(2,2,2,2));
}

__m128 SIMDVec4AngleUnit(const __m128& left, const __m128& right)
{
	return SIMDACos(SIMDVec4Dot(left, right));
}

__m128 SIMDVec4Angle(const __m128& left, const __m128& right)
{
	return SIMDVec4AngleUnit(SIMDVec4Normalize(left), SIMDVec4Normalize(right));
}

__m128 SIMDVec4LengthSquared(const __m128& vec)
{
	return SIMDVec4Dot(vec, vec);
}

__m128 SIMDVec4Length(const __m128& vec)
{
	return _mm_sqrt_ps(SIMDVec4Dot(vec, vec));
}

__m128 SIMDVec4ReciprocalLengthSquared(const __m128& vec)
{
	return _mm_div_ps(gOne, SIMDVec4Dot(vec, vec));
}

__m128 SIMDVec4ReciprocalLength(const __m128& vec)
{
	return _mm_div_ps(gOne, _mm_sqrt_ps(SIMDVec4Dot(vec, vec)));
}

__m128 SIMDVec4Normalize(const __m128& vec)
{
	__m128 length = _mm_sqrt_ps(SIMDVec4Dot(vec, vec));
	return _mm_div_ps(vec, length);
}

__m128 SIMDVec4Cross(const __m128& left, const __m128& right)
{
	__m128 temp1 = _mm_shuffle_ps(left, left, _MM_SHUFFLE(3,0,2,1));
	__m128 temp2 = _mm_shuffle_ps(right, right, _MM_SHUFFLE(3,1,0,2));

	__m128 result = _mm_mul_ps(temp1, temp2);

	temp1 = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(3,0,2,1));
	temp2 = _mm_shuffle_ps(temp2, temp2,_MM_SHUFFLE(3,1,0,2));

	temp1 = _mm_mul_ps(temp1, temp2);
	result = _mm_sub_ps(result, temp1);

	return _mm_and_ps(result, gXYZMask);
}

__m128 SIMDVec4Reflect(const __m128& vec, const __m128& normal)
{
	__m128 result = _mm_mul_ps(gTwo, SIMDVec4Dot(vec, normal));
	result = _mm_mul_ps(result, normal);
	return _mm_sub_ps(vec, result);
}

__m128 SIMDVec4Refract(const __m128& vec, const __m128& normal, float refraction_index)
{
	__m128 ri = _mm_set1_ps(refraction_index);
	__m128 idn = SIMDVec4Dot(vec, normal);
	__m128 k = _mm_mul_ps(idn, idn);
	k = _mm_sub_ps(gOne, k);
	k = _mm_mul_ps(k, ri);
	k = _mm_mul_ps(k, ri);
	k = _mm_sub_ps(gOne, k);

	if (_mm_movemask_ps(_mm_cmple_ps(k, gZero))) {
		return gZero;
	}

	k = _mm_add_ps(_mm_sqrt_ps(k), _mm_mul_ps(ri, idn));
	k = _mm_mul_ps(k, normal);
	return _mm_sub_ps(_mm_mul_ps(ri, vec), k);
}

__m128 SIMDVec4Lerp(const __m128& left, const __m128& right, float t)
{
	__m128 temp = _mm_set1_ps(t);
	temp = _mm_mul_ps(temp, _mm_sub_ps(right, left));
	return _mm_add_ps(left, temp);
}

NS_END
