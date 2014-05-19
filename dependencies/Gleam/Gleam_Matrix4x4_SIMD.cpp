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

#include "Gleam_Matrix4x4_SIMD.h"
#include "Gleam_Vector4_SIMD.h"
#include <Gaff_IncludeAssert.h>
#include <Gaff_Defines.h>
#include <cmath>

#define LOADMATRIX(matrix, prefix) \
	__m128 prefix##1 = SIMDLOAD(matrix[0]); \
	__m128 prefix##2 = SIMDLOAD(matrix[1]); \
	__m128 prefix##3 = SIMDLOAD(matrix[2]); \
	__m128 prefix##4 = SIMDLOAD(matrix[3])

#define MULROW(row, x, y, z, w, result, col1, col2, col3, col4) \
	x = _mm_shuffle_ps(row, row, _MM_SHUFFLE(0,0,0,0)); \
	y = _mm_shuffle_ps(row, row, _MM_SHUFFLE(1,1,1,1)); \
	z = _mm_shuffle_ps(row, row, _MM_SHUFFLE(2,2,2,2)); \
	w = _mm_shuffle_ps(row, row, _MM_SHUFFLE(3,3,3,3)); \
							\
	x = _mm_mul_ps(x, col1); \
	y = _mm_mul_ps(y, col2); \
	z = _mm_mul_ps(z, col3); \
	w = _mm_mul_ps(w, col4); \
							\
	x = _mm_add_ps(x, z); \
	y = _mm_add_ps(y, w); \
	x = _mm_add_ps(x, y); \
							\
	result = x

NS_GLEAM

Matrix4x4SIMD Matrix4x4SIMD::identity(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);

Matrix4x4SIMD::Matrix4x4SIMD(void)
{
}

Matrix4x4SIMD::Matrix4x4SIMD(const Matrix4x4SIMD& matrix)
{
	set(matrix._a);
}

Matrix4x4SIMD::Matrix4x4SIMD(float m00, float m01, float m02, float m03,
							float m10, float m11, float m12, float m13,
							float m20, float m21, float m22, float m23,
							float m30, float m31, float m32, float m33)
{
	set(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);
}

Matrix4x4SIMD::Matrix4x4SIMD(const Vector4SIMD& vec1, const Vector4SIMD& vec2,
							const Vector4SIMD& vec3, const Vector4SIMD& vec4)
{
	set(
		vec1[0], vec1[1], vec1[2], vec1[3],
		vec2[0], vec2[1], vec2[2], vec2[3],
		vec3[0], vec3[1], vec3[2], vec3[3],
		vec4[0], vec4[1], vec4[2], vec4[3]
	);
}

Matrix4x4SIMD::Matrix4x4SIMD(const __m128& vec1, const __m128& vec2,
							const __m128& vec3, const __m128& vec4)
{
	SIMDSTORE(_m[0], vec1);
	SIMDSTORE(_m[1], vec2);
	SIMDSTORE(_m[2], vec3);
	SIMDSTORE(_m[3], vec4);
}

Matrix4x4SIMD::Matrix4x4SIMD(const float* elements)
{
	set(elements);
}

Matrix4x4SIMD::~Matrix4x4SIMD(void)
{
}

bool Matrix4x4SIMD::operator==(const Matrix4x4SIMD& rhs) const
{
	LOADMATRIX(_m, lcol);
	LOADMATRIX(rhs._m, rcol);

	return SIMDEqual(lcol1, rcol1) &&
			SIMDEqual(lcol2, rcol2) &&
			SIMDEqual(lcol3, rcol3) &&
			SIMDEqual(lcol4, rcol4);
}

bool Matrix4x4SIMD::operator!=(const Matrix4x4SIMD& rhs) const
{
	return !(*this == rhs);
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator=(const Matrix4x4SIMD& rhs)
{
	set(rhs._a);
	return *this;
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator*=(const Matrix4x4SIMD& rhs)
{
	LOADMATRIX(_m, lcol);
	LOADMATRIX(rhs._m, rcol);
	__m128 x, y, z, w;

	MULROW(rcol1, x, y, z, w, rcol1, lcol1, lcol2, lcol3, lcol4);
	MULROW(rcol2, x, y, z, w, rcol2, lcol1, lcol2, lcol3, lcol4);
	MULROW(rcol3, x, y, z, w, rcol3, lcol1, lcol2, lcol3, lcol4);
	MULROW(rcol4, x, y, z, w, rcol4, lcol1, lcol2, lcol3, lcol4);

	SIMDSTORE(_m[0], rcol1);
	SIMDSTORE(_m[1], rcol2);
	SIMDSTORE(_m[2], rcol3);
	SIMDSTORE(_m[3], rcol4);

	return *this;
}

Matrix4x4SIMD Matrix4x4SIMD::operator*(const Matrix4x4SIMD& rhs) const
{
	LOADMATRIX(_m, lcol);
	LOADMATRIX(rhs._m, rcol);
	__m128 x, y, z, w;

	MULROW(rcol1, x, y, z, w, rcol1, lcol1, lcol2, lcol3, lcol4);
	MULROW(rcol2, x, y, z, w, rcol2, lcol1, lcol2, lcol3, lcol4);
	MULROW(rcol3, x, y, z, w, rcol3, lcol1, lcol2, lcol3, lcol4);
	MULROW(rcol4, x, y, z, w, rcol4, lcol1, lcol2, lcol3, lcol4);

	return Matrix4x4SIMD(rcol1, rcol2, rcol3, rcol4);
}

Vector4SIMD Matrix4x4SIMD::operator*(const Vector4SIMD& rhs) const
{
	LOADMATRIX(_m, col);
	__m128 vec = SIMDLOAD(rhs.getBuffer());
	__m128 x, y, z, w;

	MULROW(vec, x, y, z, w, vec, col1, col2, col3, col4);

	return Vector4SIMD(vec);
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator+=(const Matrix4x4SIMD& rhs)
{
	LOADMATRIX(_m, lcol);
	LOADMATRIX(rhs._m, rcol);

	SIMDSTORE(_m[0], _mm_add_ps(lcol1, rcol1));
	SIMDSTORE(_m[1], _mm_add_ps(lcol2, rcol2));
	SIMDSTORE(_m[2], _mm_add_ps(lcol3, rcol3));
	SIMDSTORE(_m[3], _mm_add_ps(lcol4, rcol4));

	return *this;
}

Matrix4x4SIMD Matrix4x4SIMD::operator+(const Matrix4x4SIMD& rhs) const
{
	LOADMATRIX(_m, lcol);
	LOADMATRIX(rhs._m, rcol);

	return Matrix4x4SIMD(
		_mm_add_ps(lcol1, rcol1),
		_mm_add_ps(lcol2, rcol2),
		_mm_add_ps(lcol3, rcol3),
		_mm_add_ps(lcol4, rcol4)
	);
}

Matrix4x4SIMD Matrix4x4SIMD::operator+(void) const
{
	return Matrix4x4SIMD(
		+_a[0], +_a[1], +_a[2], +_a[3],
		+_a[4], +_a[5], +_a[6], +_a[7],
		+_a[8], +_a[9], +_a[10], +_a[11],
		+_a[12], +_a[13], +_a[14], +_a[15]
	);
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator-=(const Matrix4x4SIMD& rhs)
{
	LOADMATRIX(_m, lcol);
	LOADMATRIX(rhs._m, rcol);

	SIMDSTORE(_m[0], _mm_sub_ps(lcol1, rcol1));
	SIMDSTORE(_m[1], _mm_sub_ps(lcol2, rcol2));
	SIMDSTORE(_m[2], _mm_sub_ps(lcol3, rcol3));
	SIMDSTORE(_m[3], _mm_sub_ps(lcol4, rcol4));

	return *this;
}

Matrix4x4SIMD Matrix4x4SIMD::operator-(const Matrix4x4SIMD& rhs) const
{
	LOADMATRIX(_m, lcol);
	LOADMATRIX(rhs._m, rcol);

	return Matrix4x4SIMD(
		_mm_sub_ps(lcol1, rcol1),
		_mm_sub_ps(lcol2, rcol2),
		_mm_sub_ps(lcol3, rcol3),
		_mm_sub_ps(lcol4, rcol4)
	);
}

Matrix4x4SIMD Matrix4x4SIMD::operator-(void) const
{
	__m128 zero = _mm_setzero_ps();
	LOADMATRIX(_m, col);

	return Matrix4x4SIMD(
		_mm_sub_ps(zero, col1),
		_mm_sub_ps(zero, col2),
		_mm_sub_ps(zero, col3),
		_mm_sub_ps(zero, col4)
	);
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator/=(float rhs)
{
	__m128 denominator = _mm_set1_ps(rhs);
	LOADMATRIX(_m, col);

	SIMDSTORE(_m[0], _mm_div_ps(col1, denominator));
	SIMDSTORE(_m[1], _mm_div_ps(col2, denominator));
	SIMDSTORE(_m[2], _mm_div_ps(col3, denominator));
	SIMDSTORE(_m[3], _mm_div_ps(col4, denominator));

	return *this;
}

Matrix4x4SIMD Matrix4x4SIMD::operator/(float rhs) const
{
	__m128 denominator = _mm_set1_ps(rhs);
	LOADMATRIX(_m, col);

	return Matrix4x4SIMD(
		_mm_div_ps(col1, denominator),
		_mm_div_ps(col2, denominator),
		_mm_div_ps(col3, denominator),
		_mm_div_ps(col4, denominator)
	);
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator*=(float rhs)
{
	__m128 scalar = _mm_set1_ps(rhs);
	LOADMATRIX(_m, col);

	SIMDSTORE(_m[0], _mm_mul_ps(col1, scalar));
	SIMDSTORE(_m[1], _mm_mul_ps(col2, scalar));
	SIMDSTORE(_m[2], _mm_mul_ps(col3, scalar));
	SIMDSTORE(_m[3], _mm_mul_ps(col4, scalar));

	return *this;
}

Matrix4x4SIMD Matrix4x4SIMD::operator*(float rhs) const
{
	__m128 scalar = _mm_set1_ps(rhs);
	LOADMATRIX(_m, col);

	return Matrix4x4SIMD(
		_mm_mul_ps(col1, scalar),
		_mm_mul_ps(col2, scalar),
		_mm_mul_ps(col3, scalar),
		_mm_mul_ps(col4, scalar)
	);
}

const float* Matrix4x4SIMD::operator[](int index) const
{
	assert(index > -1 && index < 4);
	return _m[index];
}

float* Matrix4x4SIMD::operator[](int index)
{
	assert(index > -1 && index < 4);
	return _m[index];
}

const float* Matrix4x4SIMD::getBuffer(void) const
{
	return _a;
}

float* Matrix4x4SIMD::getBuffer(void)
{
	return _a;
}

float Matrix4x4SIMD::at(int column, int row) const
{
	assert(column > -1 && column < 4);
	assert(row >-1 && row < 4);
	return _m[column][row];
}

float& Matrix4x4SIMD::at(int column, int row)
{
	assert(column > -1 && column < 4);
	assert(row >-1 && row < 4);
	return _m[column][row];
}

void Matrix4x4SIMD::set(float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33)
{
	_m[0][0] = m00;
	_m[0][1] = m01;
	_m[0][2] = m02;
	_m[0][3] = m03;

	_m[1][0] = m10;
	_m[1][1] = m11;
	_m[1][2] = m12;
	_m[1][3] = m13;

	_m[2][0] = m20;
	_m[2][1] = m21;
	_m[2][2] = m22;
	_m[2][3] = m23;

	_m[3][0] = m30;
	_m[3][1] = m31;
	_m[3][2] = m32;
	_m[3][3] = m33;
}

void Matrix4x4SIMD::set(const __m128& col1, const __m128& col2,
						const __m128& col3, const __m128& col4)
{
	SIMDSTORE(_m[0], col1);
	SIMDSTORE(_m[1], col2);
	SIMDSTORE(_m[2], col3);
	SIMDSTORE(_m[3], col4);
}

void Matrix4x4SIMD::set(const float* elements)
{
	set(
		elements[0], elements[1], elements[2], elements[3],
		elements[4], elements[5], elements[6], elements[7],
		elements[8], elements[9], elements[10], elements[11],
		elements[12], elements[13], elements[14], elements[15]
	);
}

bool Matrix4x4SIMD::isIdentity(void) const
{
	return *this == identity;
}

void Matrix4x4SIMD::setIdentity(void)
{
	*this = identity;
}

bool Matrix4x4SIMD::hasInfiniteElement(void) const
{
	LOADMATRIX(_m, col);
	__m128 sign_mask = _mm_set1_ps(0x7FFFFFFF);
	__m128 infinity = _mm_set1_ps(0x7F800000);

	col1 = _mm_and_ps(col1, sign_mask);
	col2 = _mm_and_ps(col2, sign_mask);
	col3 = _mm_and_ps(col3, sign_mask);
	col4 = _mm_and_ps(col4, sign_mask);

	col1 = _mm_cmpeq_ps(col1, infinity);
	col2 = _mm_cmpeq_ps(col2, infinity);
	col3 = _mm_cmpeq_ps(col3, infinity);
	col4 = _mm_cmpeq_ps(col4, infinity);

	col1 = _mm_or_ps(col1, col2);
	col2 = _mm_or_ps(col3, col4);
	col1 = _mm_or_ps(col1, col2);

	return _mm_movemask_ps(col1) != 0;
}

bool Matrix4x4SIMD::hasNaNElement(void) const
{
	LOADMATRIX(_m, col);
	col1 = _mm_cmpneq_ps(col1, col1);
	col2 = _mm_cmpneq_ps(col2, col2);
	col3 = _mm_cmpneq_ps(col3, col3);
	col4 = _mm_cmpneq_ps(col4, col4);

	col1 = _mm_or_ps(col1, col2);
	col2 = _mm_or_ps(col3, col4);
	col1 = _mm_or_ps(col1, col2);

	return _mm_movemask_ps(col1) != 0;
}

void Matrix4x4SIMD::transpose(void)
{
	LOADMATRIX(_m, col);
	__m128 temp1 = _mm_shuffle_ps(col1, col2, _MM_SHUFFLE(1,0,1,0));
	__m128 temp2 = _mm_shuffle_ps(col3, col4, _MM_SHUFFLE(1,0,1,0));
	__m128 temp3 = _mm_shuffle_ps(col1, col2, _MM_SHUFFLE(3,2,3,2));
	__m128 temp4 = _mm_shuffle_ps(col3, col4, _MM_SHUFFLE(3,2,3,2));

	SIMDSTORE(_m[0], _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(2,0,2,0)));
	SIMDSTORE(_m[1], _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(3,1,3,1)));
	SIMDSTORE(_m[2], _mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(2,0,2,0)));
	SIMDSTORE(_m[3], _mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(3,1,3,1)));
}

bool Matrix4x4SIMD::inverse(void)
{
	LOADMATRIX(_m, col);

	__m128 v00 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(1,1,0,0));
	__m128 v10 = _mm_shuffle_ps(col4, col4, _MM_SHUFFLE(3,2,3,2));
	__m128 v01 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(1,1,0,0));
	__m128 v11 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(3,2,3,2));
	__m128 v02 = _mm_shuffle_ps(col3, col1,_MM_SHUFFLE(2,0,2,0));
	__m128 v12 = _mm_shuffle_ps(col4, col2,_MM_SHUFFLE(3,1,3,1));

	__m128 d0 = _mm_mul_ps(v00, v10);
	__m128 d1 = _mm_mul_ps(v01, v11);
	__m128 d2 = _mm_mul_ps(v02, v12);

	v00 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(3,2,3,2));
	v10 = _mm_shuffle_ps(col4, col4, _MM_SHUFFLE(1,1,0,0));
	v01 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(3,2,3,2));
	v11 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(1,1,0,0));
	v02 = _mm_shuffle_ps(col3, col1,_MM_SHUFFLE(3,1,3,1));
	v12 = _mm_shuffle_ps(col4, col2,_MM_SHUFFLE(2,0,2,0));

	v00 = _mm_mul_ps(v00, v10);
	v01 = _mm_mul_ps(v01, v11);
	v02 = _mm_mul_ps(v02, v12);
	d0 = _mm_sub_ps(d0, v00);
	d1 = _mm_sub_ps(d1, v01);
	d2 = _mm_sub_ps(d2, v02);

	v11 = _mm_shuffle_ps(d0, d2, _MM_SHUFFLE(1,1,3,1));
	v00 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(1,0,2,1));
	v10 = _mm_shuffle_ps(v11, d0, _MM_SHUFFLE(0,3,0,2));
	v01 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(0,1,0,2));
	v11 = _mm_shuffle_ps(v11, d0, _MM_SHUFFLE(2,1,2,1));

	__m128 v13 = _mm_shuffle_ps(d1, d2, _MM_SHUFFLE(3,3,3,1));
	v02 = _mm_shuffle_ps(col4, col4, _MM_SHUFFLE(1,0,2,1));
	v12 = _mm_shuffle_ps(v13, d1, _MM_SHUFFLE(0,3,0,2));
	__m128 v03 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(0,1,0,2));
	v13 = _mm_shuffle_ps(v13, d1, _MM_SHUFFLE(2,1,2,1));

	__m128 c0 = _mm_mul_ps(v00, v10);
	__m128 c2 = _mm_mul_ps(v01, v11);
	__m128 c4 = _mm_mul_ps(v02, v12);
	__m128 c6 = _mm_mul_ps(v03, v13);

	v11 = _mm_shuffle_ps(d0, d2, _MM_SHUFFLE(0,0,1,0));
	v00 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(2,1,3,2));
	v10 = _mm_shuffle_ps(d0, v11, _MM_SHUFFLE(2,1,0,3));
	v01 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(1,3,2,3));
	v11 = _mm_shuffle_ps(d0, v11, _MM_SHUFFLE(0,2,1,2));

	v13 = _mm_shuffle_ps(d1, d2, _MM_SHUFFLE(2,2,1,0));
	v02 = _mm_shuffle_ps(col4, col4, _MM_SHUFFLE(2,1,3,2));
	v12 = _mm_shuffle_ps(d1, v13, _MM_SHUFFLE(2,1,0,3));
	v03 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(1,3,2,3));
	v13 = _mm_shuffle_ps(d1, v13, _MM_SHUFFLE(0,2,1,2));

	v00 = _mm_mul_ps(v00, v10);
	v01 = _mm_mul_ps(v01, v11);
	v02 = _mm_mul_ps(v02, v12);
	v03 = _mm_mul_ps(v03, v13);
	c0 = _mm_sub_ps(c0,v00);
	c2 = _mm_sub_ps(c2,v01);
	c4 = _mm_sub_ps(c4,v02);
	c6 = _mm_sub_ps(c6,v03);

	v00 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(0,3,0,3));

	v10 = _mm_shuffle_ps(d0, d2, _MM_SHUFFLE(1,0,2,2));
	v10 = _mm_shuffle_ps(v10, v10, _MM_SHUFFLE(0,2,3,0));
	v01 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(2,0,3,1));

	v11 = _mm_shuffle_ps(d0, d2, _MM_SHUFFLE(1,0,3,0));
	v11 = _mm_shuffle_ps(v11, v11, _MM_SHUFFLE(2,1,0,3));
	v02 = _mm_shuffle_ps(col4, col4, _MM_SHUFFLE(0,3,0,3));

	v12 = _mm_shuffle_ps(d1, d2, _MM_SHUFFLE(3,2,2,2));
	v12 = _mm_shuffle_ps(v12, v12, _MM_SHUFFLE(0,2,3,0));
	v03 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(2,0,3,1));

	v13 = _mm_shuffle_ps(d1, d2, _MM_SHUFFLE(3,2,3,0));
	v13 = _mm_shuffle_ps(v13, v13, _MM_SHUFFLE(2,1,0,3));

	v00 = _mm_mul_ps(v00, v10);
	v01 = _mm_mul_ps(v01, v11);
	v02 = _mm_mul_ps(v02, v12);
	v03 = _mm_mul_ps(v03, v13);
	__m128 c1 = _mm_sub_ps(c0, v00);
	c0 = _mm_add_ps(c0, v00);
	__m128 c3 = _mm_add_ps(c2, v01);
	c2 = _mm_sub_ps(c2, v01);
	__m128 c5 = _mm_sub_ps(c4, v02);
	c4 = _mm_add_ps(c4, v02);
	__m128 c7 = _mm_add_ps(c6, v03);
	c6 = _mm_sub_ps(c6, v03);

	c0 = _mm_shuffle_ps(c0, c1, _MM_SHUFFLE(3,1,2,0));
	c2 = _mm_shuffle_ps(c2, c3, _MM_SHUFFLE(3,1,2,0));
	c4 = _mm_shuffle_ps(c4, c5, _MM_SHUFFLE(3,1,2,0));
	c6 = _mm_shuffle_ps(c6, c7, _MM_SHUFFLE(3,1,2,0));
	c0 = _mm_shuffle_ps(c0, c0, _MM_SHUFFLE(3,1,2,0));
	c2 = _mm_shuffle_ps(c2, c2, _MM_SHUFFLE(3,1,2,0));
	c4 = _mm_shuffle_ps(c4, c4, _MM_SHUFFLE(3,1,2,0));
	c6 = _mm_shuffle_ps(c6, c6, _MM_SHUFFLE(3,1,2,0));

	// dot product
	__m128 temp1 = _mm_mul_ps(c0, col1);
	__m128 temp2 = col1;

	temp2 = _mm_shuffle_ps(temp2, temp1, _MM_SHUFFLE(1,0,0,0));
	temp2 = _mm_add_ps(temp2, temp1);
	temp1 = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(0,3,0,0));
	temp1 = _mm_add_ps(temp1, temp2);

	__m128 determinant = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(2,2,2,2));
	determinant = _mm_div_ps(_mm_set1_ps(1.0f), determinant);

	col1 = _mm_mul_ps(c0, determinant);
	col2 = _mm_mul_ps(c2, determinant);
	col3 = _mm_mul_ps(c4, determinant);
	col4 = _mm_mul_ps(c6, determinant);


	// check for an infinite element
	__m128 sign_mask = _mm_set1_ps(0x7FFFFFFF);
	__m128 infinity = _mm_set1_ps(0x7F800000);

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
		return false;
	}

	temp1 = _mm_shuffle_ps(col1, col2, _MM_SHUFFLE(1,0,1,0));
	temp2 = _mm_shuffle_ps(col3, col4, _MM_SHUFFLE(1,0,1,0));
	__m128 temp3 = _mm_shuffle_ps(col1, col2, _MM_SHUFFLE(3,2,3,2));
	__m128 temp4 = _mm_shuffle_ps(col3, col4, _MM_SHUFFLE(3,2,3,2));

	SIMDSTORE(_m[0], _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(2,0,2,0)));
	SIMDSTORE(_m[1], _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(3,1,3,1)));
	SIMDSTORE(_m[2], _mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(2,0,2,0)));
	SIMDSTORE(_m[3], _mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(3,1,3,1)));

	return true;
}

// Not sure if these are faster than normal versions of set().
// Also unsure if faster than DXMath's versions.
void Matrix4x4SIMD::setTranslate(const Vector4SIMD& translate)
{
	set(
		_mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f),
		_mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f),
		_mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f),
		_mm_set_ps(1.0f, translate[2], translate[1], translate[0])
	);
}

void Matrix4x4SIMD::setTranslate(float x, float y, float z)
{
	set(
		_mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f),
		_mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f),
		_mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f),
		_mm_set_ps(1.0f, z, y, x)
	);
}

void Matrix4x4SIMD::setScale(float x, float y, float z)
{
	set(
		_mm_set_ps(0.0f, 0.0f, 0.0f, x),
		_mm_set_ps(0.0f, 0.0f, y, 0.0f),
		_mm_set_ps(0.0f, z, 0.0f, 0.0f),
		_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
	);
}

void Matrix4x4SIMD::setScale(const Vector4SIMD& scale)
{
	set(
		_mm_set_ps(0.0f, 0.0f, 0.0f, scale[0]),
		_mm_set_ps(0.0f, 0.0f, scale[1], 0.0f),
		_mm_set_ps(0.0f, scale[2], 0.0f, 0.0f),
		_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
	);
}

void Matrix4x4SIMD::setScale(float scale)
{
	set(
		_mm_set_ps(0.0f, 0.0f, 0.0f, scale),
		_mm_set_ps(0.0f, 0.0f, scale, 0.0f),
		_mm_set_ps(0.0f, scale, 0.0f, 0.0f),
		_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
	);
}

void Matrix4x4SIMD::setRotation(float radians, const Vector4SIMD& axis)
{
	assert(axis[0] != 0.0f && axis[1] != 0.0f && axis[2] != 0.0f);
	setRotation(radians, axis[0], axis[1], axis[2]);
}

void Matrix4x4SIMD::setRotation(float radians, float x, float y, float z)
{
	assert(x != 0.0f && y != 0.0f && z != 0.0f);

	Vector4SIMD vec(x, y, z, 0.0f);
	vec.normalize();

	x = vec[0];
	y = vec[1];
	z = vec[2];

	float c = cosf(radians);
	float s = sinf(radians);

	__m128 c0 = _mm_set1_ps(s);
	__m128 c1 = _mm_set1_ps(c);
	__m128 c2 = _mm_set1_ps(1.0f - c);
	__m128 axis = _mm_set_ps(0.0f, z, y, x);

	__m128 n0 = _mm_shuffle_ps(axis, axis, _MM_SHUFFLE(3,0,2,1));
	__m128 n1 = _mm_shuffle_ps(axis, axis, _MM_SHUFFLE(3,1,0,2));

	__m128 v0 = _mm_mul_ps(c2, n0);
	v0 = _mm_mul_ps(v0, n1);

	__m128 r0 = _mm_mul_ps(c2, axis);
	r0 = _mm_mul_ps(r0, axis);
	r0 = _mm_add_ps(r0, c1);

	__m128 r1 = _mm_mul_ps(c0, axis);
	r1 = _mm_add_ps(r1, v0);

	__m128 r2 = _mm_mul_ps(c0, axis);
	r2 = _mm_sub_ps(v0, r2);

	v0 = _mm_and_ps(r0, _mm_set_ps(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));

	__m128 v1 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(2,1,2,0));
	v1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(0,3,2,1));

	__m128 v2 = _mm_shuffle_ps(r1, r2,_MM_SHUFFLE(0,0,1,1));
	v2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(2,0,2,0));

	r2 = _mm_shuffle_ps(v0, v1, _MM_SHUFFLE(1,0,3,0));
	r2 = _mm_shuffle_ps(r2, r2, _MM_SHUFFLE(1,3,2,0));

	SIMDSTORE(_m[0], r2);

	r2 = _mm_shuffle_ps(v0, v1, _MM_SHUFFLE(3,2,3,1));
	r2 = _mm_shuffle_ps(r2, r2, _MM_SHUFFLE(1,3,0,2));

	SIMDSTORE(_m[1], r2);

	v2 = _mm_shuffle_ps(v2, v0, _MM_SHUFFLE(3,2,1,0));

	SIMDSTORE(_m[2], v2);
	SIMDSTORE(_m[3], _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f));
}

void Matrix4x4SIMD::setRotationX(float radians)
{
	float cf = cosf(radians);
	float sf = sinf(radians);

	set(
		_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f),
		_mm_set_ps(0.0f, sf, cf, 0.0f),
		_mm_set_ps(0.0f, cf, -sf, 0.0f),
		_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
	);
}

void Matrix4x4SIMD::setRotationY(float radians)
{
	float cf = cosf(radians);
	float sf = sinf(radians);

	set(
		_mm_set_ps(-sf, 0.0f, cf, 0.0f),
		_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f),
		_mm_set_ps(cf, 0.0f, sf, 0.0f),
		_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
	);
}

void Matrix4x4SIMD::setRotationZ(float radians)
{
	float cf = cosf(radians);
	float sf = sinf(radians);

	set(
		_mm_set_ps(0.0f, 0.0f, sf, cf),
		_mm_set_ps(0.0f, 0.0f, cf, -sf),
		_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f),
		_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)
	);
}

void Matrix4x4SIMD::setLookAtLH(const Vector4SIMD& eye, const Vector4SIMD& target, const Vector4SIMD& up)
{
	setLookToLH(eye, target - eye, up);
}

void Matrix4x4SIMD::setLookAtLH(float eye_x, float eye_y, float eye_z,
							float target_x, float target_y, float target_z,
							float up_x, float up_y, float up_z)
{
	setLookToLH(eye_x, eye_y, eye_z, target_x - eye_x, target_y - eye_y, target_z - eye_z, up_x, up_y, up_z);
}

void Matrix4x4SIMD::setLookAtRH(const Vector4SIMD& eye, const Vector4SIMD& target, const Vector4SIMD& up)
{
	setLookToRH(eye, target - eye, up);
}

void Matrix4x4SIMD::setLookAtRH(float eye_x, float eye_y, float eye_z,
							float target_x, float target_y, float target_z,
							float up_x, float up_y, float up_z)
{
	setLookToRH(eye_x, eye_y, eye_z, target_x - eye_x, target_y - eye_y, target_z - eye_z, up_x, up_y, up_z);
}

void Matrix4x4SIMD::setLookToLH(const Vector4SIMD& eye, const Vector4SIMD& dir, const Vector4SIMD& up)
{
	assert(dir != Vector4SIMD::zero);

	Vector4SIMD d(dir);
	d.normalize();

	Vector4SIMD r = up.cross(d);
	r.normalize();

	Vector4SIMD u = d.cross(r);
	u.normalize();

	Vector4SIMD e = -eye;

	__m128 d0 = _mm_set1_ps(r.dot(e));
	__m128 d1 = _mm_set1_ps(u.dot(e));
	__m128 d2 = _mm_set1_ps(d.dot(e));

	__m128 final_right = SIMDLOAD(r.getBuffer());
	__m128 final_dir = SIMDLOAD(d.getBuffer());
	__m128 final_up = SIMDLOAD(u.getBuffer());

	__m128 control = _mm_set_ps(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	__m128 temp1 = _mm_andnot_ps(control, d0);
	__m128 temp2 = _mm_and_ps(final_right, control);
	SIMDSTORE(_m[0], _mm_or_ps(temp1, temp2));

	temp1 = _mm_andnot_ps(control, d1);
	temp2 = _mm_and_ps(final_up, control);
	SIMDSTORE(_m[1], _mm_or_ps(temp1, temp2));

	temp1 = _mm_andnot_ps(control, d2);
	temp2 = _mm_and_ps(final_dir, control);
	SIMDSTORE(_m[2], _mm_or_ps(temp1, temp2));

	SIMDSTORE(_m[3], _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f));
}

void Matrix4x4SIMD::setLookToLH(float eye_x, float eye_y, float eye_z,
							float dir_x, float dir_y, float dir_z,
							float up_x, float up_y, float up_z)
{
	setLookToLH(
		Vector4SIMD(eye_x, eye_y, eye_z, 1.0f),
		Vector4SIMD(dir_x, dir_y, dir_z, 1.0f),
		Vector4SIMD(up_x, up_y, up_z, 0.0f)
	);
}

void Matrix4x4SIMD::setLookToRH(const Vector4SIMD& eye, const Vector4SIMD& dir, const Vector4SIMD& up)
{
	setLookToLH(eye, -dir, up);
}

void Matrix4x4SIMD::setLookToRH(float eye_x, float eye_y, float eye_z,
							float dir_x, float dir_y, float dir_z,
							float up_x, float up_y, float up_z)
{
	setLookToRH(
		Vector4SIMD(eye_x, eye_y, eye_z, 1.0f),
		Vector4SIMD(dir_x, dir_y, dir_z, 1.0f),
		Vector4SIMD(up_x, up_y, up_z, 0.0f)
	);
}

void Matrix4x4SIMD::setOrthographicLH(float left, float right, float bottom, float top, float z_near, float z_far)
{
	float recip_width = 1.0f / (right - left);
	float recip_height = 1.0f / (top - bottom);
	float range = 1.0f / (z_far - z_near);
	__m128 mem1 = _mm_set_ps(1.0f, range, recip_height, recip_width);
	__m128 mem2 = _mm_set_ps(1.0f, -z_near, -(top + bottom), -(left + right));
	__m128 temp = _mm_setzero_ps();

	temp = _mm_move_ss(temp, mem1);
	temp = _mm_add_ss(temp, temp);
	SIMDSTORE(_m[0], temp);

	temp = _mm_and_ps(mem1, gYMask);
	temp = _mm_add_ps(temp, temp);
	SIMDSTORE(_m[1], temp);

	temp = _mm_and_ps(mem1, gZMask);
	SIMDSTORE(_m[2], temp);

	temp = _mm_mul_ps(mem1, mem2);
	SIMDSTORE(_m[3], temp);
}

void Matrix4x4SIMD::setOrthographicLH(float width, float height, float z_near, float z_far)
{
	float range = 1.0f / (z_far - z_near);
	__m128 mem = _mm_set_ps(-range * z_near, range, 2.0f / height, 2.0f / width);
	__m128 temp = _mm_setzero_ps();

	temp = _mm_move_ss(temp, mem);
	SIMDSTORE(_m[0], temp);

	temp = _mm_and_ps(mem, gYMask);
	SIMDSTORE(_m[1], temp);

	__m128 temp2 = _mm_shuffle_ps(mem, gWAxis, _MM_SHUFFLE(3,2,3,2));
	temp = _mm_setzero_ps();
	temp = _mm_shuffle_ps(temp, temp2, _MM_SHUFFLE(2,0,0,0));
	SIMDSTORE(_m[2], temp);

	temp = _mm_shuffle_ps(temp, temp2, _MM_SHUFFLE(3,1,0,0));
	SIMDSTORE(_m[3], temp);
}

void Matrix4x4SIMD::setOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far)
{
	float recip_width = 1.0f / (right - left);
	float recip_height = 1.0f / (top - bottom);
	float range = 1.0f / (z_near - z_far);
	__m128 mem1 = _mm_set_ps(1.0f, range, recip_height, recip_width);
	__m128 mem2 = _mm_set_ps(1.0f, z_near, -(top + bottom), -(left + right));
	__m128 temp = _mm_setzero_ps();

	temp = _mm_move_ss(temp, mem1);
	temp = _mm_add_ss(temp, temp);
	SIMDSTORE(_m[0], temp);

	temp = _mm_and_ps(mem1, gYMask);
	temp = _mm_add_ps(temp, temp);
	SIMDSTORE(_m[1], temp);

	temp = _mm_and_ps(mem1, gZMask);
	SIMDSTORE(_m[2], temp);

	temp = _mm_mul_ps(mem1, mem2);
	SIMDSTORE(_m[3], temp);
}

void Matrix4x4SIMD::setOrthographicRH(float width, float height, float z_near, float z_far)
{
	float range = 1.0f / (z_near - z_far);
	__m128 mem = _mm_set_ps(range * z_near, range, 2.0f / height, 2.0f / width);
	__m128 temp = _mm_setzero_ps();

	temp = _mm_move_ss(temp, mem);
	SIMDSTORE(_m[0], temp);

	temp = _mm_and_ps(mem, gYMask);
	SIMDSTORE(_m[1], temp);

	__m128 temp2 = _mm_shuffle_ps(mem, gWAxis, _MM_SHUFFLE(3,2,3,2));
	temp = _mm_setzero_ps();
	temp = _mm_shuffle_ps(temp, temp2, _MM_SHUFFLE(2,0,0,0));
	SIMDSTORE(_m[2], temp);

	temp = _mm_shuffle_ps(temp, temp2, _MM_SHUFFLE(3,1,0,0));
	SIMDSTORE(_m[3], temp);
}

void Matrix4x4SIMD::setPerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far)
{
	fov *= 0.5f;
	float cos_fov = cosf(fov);
	float sin_fov = sinf(fov);
	float height = cos_fov / sin_fov;
	float width = height / aspect_ratio;
	float range = z_far / (z_far - z_near);

	__m128 mem = _mm_set_ps(-range * z_near, range, height, width);
	__m128 temp = _mm_setzero_ps();

	temp = _mm_move_ss(temp, mem);
	SIMDSTORE(_m[0], temp);

	temp = _mm_and_ps(mem, gYMask);
	SIMDSTORE(_m[1], temp);

	__m128 temp2 = _mm_shuffle_ps(mem, gWAxis, _MM_SHUFFLE(3,2,3,2));
	temp = _mm_shuffle_ps(_mm_setzero_ps(), temp2, _MM_SHUFFLE(3,0,0,0));
	SIMDSTORE(_m[2], temp);

	temp = _mm_shuffle_ps(temp, temp2, _MM_SHUFFLE(2,1,0,0));
	SIMDSTORE(_m[3], temp);
}

void Matrix4x4SIMD::setPerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far)
{
	fov *= 0.5f;
	float cos_fov = cosf(fov);
	float sin_fov = sinf(fov);
	float height = cos_fov / sin_fov;
	float width = height / aspect_ratio;
	float range = z_far / (z_near - z_far);

	__m128 mem = _mm_set_ps(range * z_near, range, height, width);
	__m128 temp = _mm_setzero_ps();

	temp = _mm_move_ss(temp, mem);
	SIMDSTORE(_m[0], temp);

	temp = _mm_and_ps(mem, gYMask);
	SIMDSTORE(_m[1], temp);

	__m128 temp2 = _mm_shuffle_ps(mem, gWAxis, _MM_SHUFFLE(3,2,3,2));
	temp = _mm_shuffle_ps(_mm_setzero_ps(), temp2, _MM_SHUFFLE(3,0,0,0));
	SIMDSTORE(_m[2], temp);

	temp = _mm_shuffle_ps(temp, temp2, _MM_SHUFFLE(2,1,0,0));
	SIMDSTORE(_m[3], temp);
}

bool Matrix4x4SIMD::roughlyEqual(const Matrix4x4SIMD& rhs, float epsilon) const
{
	LOADMATRIX(_m, lcol);
	LOADMATRIX(rhs._m, rcol);

	return SIMDRoughlyEqual(lcol1, rcol1, epsilon) &&
			SIMDRoughlyEqual(lcol2, rcol2, epsilon) &&
			SIMDRoughlyEqual(lcol3, rcol3, epsilon) &&
			SIMDRoughlyEqual(lcol4, rcol4, epsilon);
}

Matrix4x4SIMD Matrix4x4SIMD::MakeTranslate(const Vector4SIMD& translate)
{
	Matrix4x4SIMD temp;
	temp.setTranslate(translate);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeTranslate(float x, float y, float z)
{
	Matrix4x4SIMD temp;
	temp.setTranslate(x, y, z);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeScale(float x, float y, float z)
{
	Matrix4x4SIMD temp;
	temp.setScale(x, y, z);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeScale(const Vector4SIMD& scale)
{
	Matrix4x4SIMD temp;
	temp.setScale(scale);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeScale(float scale)
{
	Matrix4x4SIMD temp;
	temp.setScale(scale);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeRotation(float radians, float x, float y, float z)
{
	Matrix4x4SIMD temp;
	temp.setRotation(radians, x, y, z);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeRotationX(float radians)
{
	Matrix4x4SIMD temp;
	temp.setRotationX(radians);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeRotationY(float radians)
{
	Matrix4x4SIMD temp;
	temp.setRotationY(radians);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeRotationZ(float radians)
{
	Matrix4x4SIMD temp;
	temp.setRotationZ(radians);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeLookAtLH(const Vector4SIMD& eye, const Vector4SIMD& target, const Vector4SIMD& up)
{
	Matrix4x4SIMD temp;
	temp.setLookAtLH(eye, target, up);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeLookAtLH(float eye_x, float eye_y, float eye_z,
										float target_x, float target_y, float target_z,
										float up_x, float up_y, float up_z)
{
	Matrix4x4SIMD temp;
	temp.setLookAtLH(eye_x, eye_y, eye_z, target_x, target_y, target_z, up_x, up_y, up_z);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeLookAtRH(const Vector4SIMD& eye, const Vector4SIMD& target, const Vector4SIMD& up)
{
	Matrix4x4SIMD temp;
	temp.setLookAtRH(eye, target, up);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeLookAtRH(float eye_x, float eye_y, float eye_z,
										float target_x, float target_y, float target_z,
										float up_x, float up_y, float up_z)
{
	Matrix4x4SIMD temp;
	temp.setLookAtRH(eye_x, eye_y, eye_z, target_x, target_y, target_z, up_x, up_y, up_z);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeLookToLH(const Vector4SIMD& eye, const Vector4SIMD& dir, const Vector4SIMD& up)
{
	Matrix4x4SIMD temp;
	temp.setLookToLH(eye, dir, up);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeLookToLH(float eye_x, float eye_y, float eye_z,
										float dir_x, float dir_y, float dir_z,
										float up_x, float up_y, float up_z)
{
	Matrix4x4SIMD temp;
	temp.setLookToLH(eye_x, eye_y, eye_z, dir_x, dir_y, dir_z, up_x, up_y, up_z);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeLookToRH(const Vector4SIMD& eye, const Vector4SIMD& dir, const Vector4SIMD& up)
{
	Matrix4x4SIMD temp;
	temp.setLookToRH(eye, dir, up);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeLookToRH(float eye_x, float eye_y, float eye_z,
										float dir_x, float dir_y, float dir_z,
										float up_x, float up_y, float up_z)
{
	Matrix4x4SIMD temp;
	temp.setLookToRH(eye_x, eye_y, eye_z, dir_x, dir_y, dir_z, up_x, up_y, up_z);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeOrthographicLH(float width, float height, float z_near, float z_far)
{
	Matrix4x4SIMD temp;
	temp.setOrthographicLH(width, height, z_near, z_far);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeOrthographicRH(float width, float height, float z_near, float z_far)
{
	Matrix4x4SIMD temp;
	temp.setOrthographicRH(width, height, z_near, z_far);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakePerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far)
{
	Matrix4x4SIMD temp;
	temp.setPerspectiveLH(fov, aspect_ratio, z_near, z_far);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakePerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far)
{
	Matrix4x4SIMD temp;
	temp.setPerspectiveRH(fov, aspect_ratio, z_near, z_far);
	return temp;
}

Matrix4x4SIMD operator*(float lhs, const Matrix4x4SIMD& rhs)
{
	return rhs * lhs;
}

NS_END
