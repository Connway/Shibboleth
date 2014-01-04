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

#include "Gleam_Quaternion_SIMD.h"
#include "Gleam_SIMDMath.h"
#include <Gaff_IncludeAssert.h>
#include <cmath>

NS_GLEAM

QuaternionSIMD QuaternionSIMD::identity(0.0f, 0.0f, 0.0f, 1.0f);

QuaternionSIMD::QuaternionSIMD(void)
{
}

QuaternionSIMD::QuaternionSIMD(const QuaternionSIMD& rhs):
	_x(rhs._x), _y(rhs._y), _z(rhs._z), _w(rhs._w)
{
}
QuaternionSIMD::QuaternionSIMD(float x, float y, float z, float w):
	_x(x), _y(y), _z(z), _w(w)
{
}

QuaternionSIMD::QuaternionSIMD(const float* elements)
{
	set(elements[0], elements[1], elements[2], elements[3]);
}

QuaternionSIMD::QuaternionSIMD(const Vector4SIMD& vec):
	_x(vec[0]), _y(vec[1]), _z(vec[2]), _w(vec[3])
{
}

QuaternionSIMD::QuaternionSIMD(const __m128& rhs)
{
	SIMDSTORE(_quat, rhs);
}

QuaternionSIMD::~QuaternionSIMD(void)
{
}

bool QuaternionSIMD::operator==(const QuaternionSIMD& rhs) const
{
	__m128 left = SIMDLOAD(_quat);
	__m128 right = SIMDLOAD(rhs._quat);
	__m128 temp = _mm_cmpeq_ps(left, right);
	return (_mm_movemask_ps(temp) == 0x0f) != 0;
}

bool QuaternionSIMD::operator!=(const QuaternionSIMD& rhs) const
{
	return !(*this == rhs);
}

const QuaternionSIMD& QuaternionSIMD::operator=(const QuaternionSIMD& rhs)
{
	set(rhs._quat);
	return *this;
}

float QuaternionSIMD::operator[](int index) const
{
	assert(index > -1 && index < 4);
	return _quat[index];
}

float& QuaternionSIMD::operator[](int index)
{
	assert(index > -1 && index < 4);
	return _quat[index];
}

void QuaternionSIMD::set(float x, float y, float z, float w)
{
	_x = x;
	_y = y;
	_z = z;
	_w = w;
}

void QuaternionSIMD::set(const float* elements)
{
	_quat[0] = elements[0];
	_quat[1] = elements[1];
	_quat[2] = elements[2];
	_quat[3] = elements[3];
}

void QuaternionSIMD::set(const Vector4SIMD& vec)
{
	_quat[0] = vec[0];
	_quat[1] = vec[1];
	_quat[2] = vec[2];
	_quat[3] = vec[3];
}

const float* QuaternionSIMD::getBuffer(void) const
{
	return _quat;
}

float* QuaternionSIMD::getBuffer(void)
{
	return _quat;
}

QuaternionSIMD QuaternionSIMD::operator+(const QuaternionSIMD& rhs) const
{
	return QuaternionSIMD(_mm_add_ps(SIMDLOAD(_quat), SIMDLOAD(rhs._quat)));
}

const QuaternionSIMD& QuaternionSIMD::operator+=(const QuaternionSIMD& rhs)
{
	SIMDSTORE(_quat, _mm_add_ps(SIMDLOAD(_quat), SIMDLOAD(rhs._quat)));
	return *this;
}

QuaternionSIMD QuaternionSIMD::operator*(const QuaternionSIMD& rhs) const
{
	QuaternionSIMD temp(*this);
	temp *= rhs;
	return temp;
}

const QuaternionSIMD& QuaternionSIMD::operator*=(const QuaternionSIMD& rhs)
{
	static const __m128 controlWZYX = { 1.0f,-1.0f, 1.0f,-1.0f};
	static const __m128 controlZWXY = { 1.0f, 1.0f,-1.0f,-1.0f};
	static const __m128 controlYXWZ = {-1.0f, 1.0f, 1.0f,-1.0f};

	__m128 temp1 = SIMDLOAD(_quat);
	__m128 x = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(0,0,0,0));
	__m128 y = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(1,1,1,1));
	__m128 z = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(2,2,2,2));
	temp1 = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(3,3,3,3));

	__m128 temp2 = SIMDLOAD(rhs._quat);
	temp1 = _mm_mul_ps(temp1, temp2);

	temp2 = _mm_shuffle_ps(temp2, temp2, _MM_SHUFFLE(0,1,2,3));
	x = _mm_mul_ps(x, temp2);
	x = _mm_mul_ps(x, controlWZYX);

	temp2 = _mm_shuffle_ps(temp2, temp2, _MM_SHUFFLE(2,3,0,1));
	y = _mm_mul_ps(y, temp2);
	y = _mm_mul_ps(y, controlZWXY);

	temp2 = _mm_shuffle_ps(temp2, temp2, _MM_SHUFFLE(0,1,2,3));
	z = _mm_mul_ps(z, temp2);
	z = _mm_mul_ps(z, controlYXWZ);

	temp1 = _mm_add_ps(temp1, x);
	temp1 = _mm_add_ps(temp1, y);
	temp1 = _mm_add_ps(temp1, z);

	return *this;
}

Vector4SIMD QuaternionSIMD::transform(const Vector4SIMD& vec) const
{
	QuaternionSIMD p, q_conj;
	q_conj = *this;
	q_conj.conjugate();

	p._x = vec[0];
	p._y = vec[1];
	p._z = vec[2];
	p._w = 0.0f;

	p = *this * p * q_conj;

	return Vector4SIMD(p._x, p._y, p._z, 0.0f);
}

bool QuaternionSIMD::roughlyEqual(const QuaternionSIMD& rhs, float epsilon) const
{
	return SIMDRoughlyEqual(SIMDLOAD(_quat), SIMDLOAD(rhs._quat), epsilon);
}

QuaternionSIMD QuaternionSIMD::slerp(const QuaternionSIMD& rhs, float t)
{
	static const __m128 eps = { 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f };
	static const __m128 sign_x = { 0x80000000, 0x00000000, 0x00000000, 0x00000000 };

	__m128 temp = _mm_set1_ps(t);
	__m128 left = SIMDLOAD(_quat);
	__m128 right = SIMDLOAD(rhs._quat);
	__m128 cos_theta = SIMDVec4Dot(left, right);
	__m128 control = _mm_cmplt_ps(cos_theta, gZero);
	__m128 sign = _mm_or_ps(_mm_andnot_ps(control, gOne), _mm_and_ps(gNegOne, control));

	cos_theta = _mm_mul_ps(cos_theta, sign);
	control = _mm_cmplt_ps(cos_theta, eps);

	__m128 sin_theta = _mm_mul_ps(cos_theta, cos_theta);
	sin_theta = _mm_sub_ps(gOne, sin_theta);
	sin_theta = _mm_sqrt_ps(sin_theta);

	__m128 theta = SIMDATan2(sin_theta, cos_theta);
	__m128 v = _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(2,3,0,1));
	v = _mm_and_ps(v, gXYMask);
	v = _mm_xor_ps(v, sign_x);
	v = _mm_and_ps(v, gXAxis);

	__m128 s0 = _mm_mul_ps(v, theta);
	s0 = SIMDSin(s0);
	s0 = _mm_div_ps(s0, sin_theta);
	s0 = _mm_or_ps(_mm_andnot_ps(control, v), _mm_and_ps(s0, control));

	__m128 s1 = _mm_shuffle_ps(s0, s0, _MM_SHUFFLE(1,1,1,1));
	s0 = _mm_shuffle_ps(s0, s0, _MM_SHUFFLE(0,0,0,0));
	s1 = _mm_mul_ps(s1, sign);
	s1 = _mm_mul_ps(s1, right);

	return QuaternionSIMD(_mm_add_ps(_mm_mul_ps(left, s0), s1));
}

float QuaternionSIMD::dot(const QuaternionSIMD& rhs) const
{
	return _mm_cvtss_f32(SIMDVec4Dot(SIMDLOAD(_quat), SIMDLOAD(rhs._quat)));
}

void QuaternionSIMD::normalize(void)
{
	assert(length() != 0.0f);
	SIMDSTORE(_quat, SIMDVec4Normalize(SIMDLOAD(_quat)));
}

void QuaternionSIMD::conjugate(void)
{
	SIMDSTORE(_quat, _mm_mul_ps(SIMDLOAD(_quat), gNegOneXYZ));
}

void QuaternionSIMD::inverse(void)
{
	__m128 quat = SIMDLOAD(_quat);
	__m128 scale = SIMDVec4ReciprocalLengthSquared(quat);
	conjugate();
	SIMDSTORE(_quat, _mm_mul_ps(quat, scale));
}

float QuaternionSIMD::lengthSquared(void) const
{
	return _mm_cvtss_f32(SIMDVec4LengthSquared(SIMDLOAD(_quat)));
}

float QuaternionSIMD::length(void) const
{
	return _mm_cvtss_f32(SIMDVec4Length(SIMDLOAD(_quat)));
}

Vector4SIMD QuaternionSIMD::axis(void) const
{
	return Vector4SIMD(_mm_and_ps(SIMDLOAD(_quat), gXYZMask));
}

float QuaternionSIMD::angle(void) const
{
	return 2.0f * acosf(_w);
}

Matrix4x4SIMD QuaternionSIMD::matrix(void) const
{
	__m128 quat = SIMDLOAD(_quat);
	__m128 q0 = _mm_add_ps(quat, quat);
	__m128 q1 = _mm_mul_ps(q0, quat);
	__m128 v0 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3,0,0,1));
	__m128 v1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3,1,2,2));
	v0 = _mm_and_ps(v0, gXYZMask);
	v1 = _mm_and_ps(v1, gXYZMask);
	__m128 r0 = _mm_sub_ps(gOneXYZ, v0);
	r0 = _mm_sub_ps(r0, v1);

	v0 = _mm_shuffle_ps(quat, quat, _MM_SHUFFLE(3,1,0,0));
	v1 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3,2,1,2));
	v0 = _mm_mul_ps(v0, v1);

	v1 = _mm_shuffle_ps(quat, quat, _MM_SHUFFLE(3,3,3,3));
	__m128 v2 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3,0,2,1));
	v1 = _mm_mul_ps(v1, v2);

	__m128 r1 = _mm_add_ps(v0, v1);
	__m128 r2 = _mm_sub_ps(v0, v1);

	v0 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(1,0,2,1));
	v0 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(1,3,2,0));
	v1 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(2,2,0,0));
	v1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(2,0,2,0));

	q1 = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(1,0,3,0));
	q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1,3,2,0));

	__m128 col1, col2, col3, col4;
	col1 = q1;

	q1 = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(3,2,3,1));
	q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1,3,0,2));
	col2 = q1;

	col3 = _mm_shuffle_ps(v1, r0,_MM_SHUFFLE(3,2,1,0));
	col4 = gWAxis;

	return Matrix4x4SIMD(col1, col2, col3, col4);

	//__m128 xxyyzz = _mm_mul_ps(quat, quat);
	//__m128 xyxzyz = _mm_shuffle_ps(quat, quat, _MM_SHUFFLE(3,1,0,0));
	//__m128 wxwywz = _mm_shuffle_ps(quat, quat, _MM_SHUFFLE(3,2,2,1));

	//xyxzyz = _mm_mul_ps(xyxzyz, wxwywz);

	//wxwywz = _mm_shuffle_ps(quat, quat, _MM_SHUFFLE(3,3,3,3));
	//wxwywz = _mm_mul_ps(wxwywz, _mm_shuffle_ps(quat, quat, _MM_SHUFFLE(3,2,1,0)));

	//// make w component of all vectors 0
	//xxyyzz = _mm_and_ps(xxyyzz, gXYZMask);
	//xyxzyz = _mm_and_ps(xyxzyz, gXYZMask);
	//wxwywz = _mm_and_ps(wxwywz, gXYZMask);

	//// 1 - 2 * [yy+zz, xx+zz, xx+yy]
	//__m128 temp1 = _mm_shuffle_ps(xxyyzz, xxyyzz, _MM_SHUFFLE(3,1,0,0));
	//temp1 = _mm_add_ps(temp1, _mm_shuffle_ps(xxyyzz, xxyyzz, _MM_SHUFFLE(3,2,2,1)));
	//temp1 = _mm_mul_ps(temp1, gTwo);
	//temp1 = _mm_sub_ps(gOne, temp1);

	//// 2 * [xy+wz, yz+wx, xz+wy]
	//__m128 temp2 = _mm_shuffle_ps(xyxzyz, xyxzyz, _MM_SHUFFLE(3,1,2,0));
	//temp2 = _mm_add_ps(temp2, _mm_shuffle_ps(wxwywz, wxwywz, _MM_SHUFFLE(3,1,0,2)));
	//temp2 = _mm_mul_ps(temp2, gTwo);

	//// 2 * [xz-wy, xy-wz, yz-wx]
	//__m128 temp3 = _mm_shuffle_ps(xyxzyz, xyxzyz, _MM_SHUFFLE(3,2,0,1));
	//temp3 = _mm_sub_ps(temp3, _mm_shuffle_ps(wxwywz, wxwywz, _MM_SHUFFLE(3,0,2,1)));
	//temp3 = _mm_mul_ps(temp3, gTwo);

	//__m128 col1 = _mm_shuffle_ps(temp2, temp3, _MM_SHUFFLE(3,0,0,3));
	//__m128 col2 = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(3,1,1,3));
	//__m128 col3 = _mm_shuffle_ps(temp3, temp1, _MM_SHUFFLE(3,2,2,3));
	//// slide the elements down by one
	//col1 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(2,1,3,3));
	//col2 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(2,1,3,3));
	//col3 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(2,1,3,3));

	//col1 = _mm_shuffle_ps(col1, temp1, _MM_SHUFFLE(3,2,3,0));
	//col2 = _mm_shuffle_ps(col2, temp3, _MM_SHUFFLE(3,2,3,1));
	//col3 = _mm_shuffle_ps(col3, temp2, _MM_SHUFFLE(3,2,3,2));

	//// put elements in their proper order
	//col1 = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(1,3,2,0));
	//col2 = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(1,3,2,0));
	//col3 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(1,3,2,0));

	//return Matrix4x4SIMD(col1, col2, col3, gWAxis);

	//Matrix4x4CPU matrix;
	//float* m = matrix.getBuffer();

	//float xx = _x * _x;
	//float yy = _y * _y;
	//float zz = _z * _z;
	//float xy = _x * _y;
	//float xz = _x * _z;
	//float yz = _y * _z;
	//float wx = _w * _x;
	//float wy = _w * _y;
	//float wz = _w * _z;

	//m[0] = 1.0f - 2.0f * (yy + zz);
	//m[1] = 2.0f * (xy + wz);
	//m[2] = 2.0f * (xz - wy);
	//m[3] = 0.0f;
	//m[4] = 2.0f * (xy - wz);
	//m[5] = 1.0f - 2.0f * (xx + zz);
	//m[6] = 2.0f * (yz + wx);
	//m[7] = 0.0f;
	//m[8] = 2.0f * (xz + wy);
	//m[9] = 2.0f * (yz - wx);
	//m[10] = 1.0f - 2.0f * (xx + yy);
	//m[11] = 0.0f;
	//m[12] = 0.0f;
	//m[13] = 0.0f;
	//m[14] = 0.0f;
	//m[15] = 1.0f;

	//return matrix;
}

void QuaternionSIMD::constructFromAxis(const Vector4SIMD& axis, float angle)
{
	// assert that the vector is normalized
	assert(1.0f - axis.lengthSquared() < 0.000001f);
	angle *= 2.0f;

	// v = [x, y, z, 1]
	__m128 v = SIMDLOAD(axis.getBuffer());
	v = _mm_or_ps(_mm_and_ps(v, gXYZMask), gWMask);

	// s = [sin,sin,sin,cos]
	__m128 s = _mm_set1_ps(sinf(angle));
	s = _mm_and_ps(s, gXYZMask);
	s = _mm_or_ps(s, _mm_and_ps(_mm_set1_ps(cosf(angle)), gWMask));

	SIMDSTORE(_quat, _mm_mul_ps(v, s));
}

void QuaternionSIMD::constructFromMatrix(const Matrix4x4SIMD& matrix)
{
	static __m128 sign1 = { +1.0f, -1.0f, -1.0f, +1.0f };
	static __m128 sign2 = { -1.0f, +1.0f, -1.0f, +1.0f };
	static __m128 sign3 = { -1.0f, -1.0f, +1.0f, +1.0f };

	const float* buffer = matrix.getBuffer();

	__m128 r0 = SIMDLOAD(buffer);
	__m128 r1 = SIMDLOAD(buffer + 4);
	__m128 r2 = SIMDLOAD(buffer + 8);
	__m128 r00 = _mm_shuffle_ps(r0, r0, _MM_SHUFFLE(0,0,0,0));
	__m128 r11 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(1,1,1,1));
	__m128 r22 = _mm_shuffle_ps(r2, r2, _MM_SHUFFLE(2,2,2,2));

	__m128 r11sr00 = _mm_sub_ps(r11, r00);
	__m128 r11sr00_le = _mm_cmple_ps(r11sr00, gZero);

	__m128 r11pr00 = _mm_add_ps(r11, r00);
	__m128 r11pr00_le = _mm_cmple_ps(r11pr00, gZero);

	__m128 r22_le = _mm_cmple_ps(r22, gZero);

	__m128 t0 = _mm_mul_ps(sign1, r00);
	__m128 t1 = _mm_mul_ps(sign2, r11);
	__m128 t2 = _mm_mul_ps(sign3, r22);

	__m128 temp1 = _mm_add_ps(t0, t1);
	temp1 = _mm_add_ps(t2, temp1);
	temp1 = _mm_add_ps(gOne, temp1);

	t0 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(1,2,2,1));
	t1 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(1,0,0,0));
	t1 = _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(1,3,2,0));

	__m128 temp2 = _mm_add_ps(t0, t1);

	t0 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(0,0,0,1));
	t1 = _mm_shuffle_ps(r1, r0, _MM_SHUFFLE(1,2,2,2));
	t1 = _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(1,3,2,0));

	__m128 temp3 = _mm_add_ps(t0, t1);
	temp2 = _mm_mul_ps(temp2, sign2);

	t0 = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(0,0,1,0));
	t1 = _mm_shuffle_ps(temp1, temp3, _MM_SHUFFLE(0,2,3,2));
	t2 = _mm_shuffle_ps(temp2, temp3, _MM_SHUFFLE(1,0,2,1));

	__m128 tensor0 = _mm_shuffle_ps(t0, t2, _MM_SHUFFLE(2,0,2,0));
	__m128 tensor1 = _mm_shuffle_ps(t0, t2, _MM_SHUFFLE(3,1,1,2));
	__m128 tensor2 = _mm_shuffle_ps(t2, t1, _MM_SHUFFLE(2,0,1,0));
	__m128 tensor3 = _mm_shuffle_ps(t2, t1, _MM_SHUFFLE(1,2,3,2));

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
	SIMDSTORE(_quat, _mm_div_ps(t2, t0));
}

void QuaternionSIMD::constructFromAngles(float x, float y, float z)
{
	constructFromAngles(Vector4SIMD(x, y, z, 0.0f));
}

void QuaternionSIMD::constructFromAngles(const Vector4SIMD& angles)
{
	static __m128 sign = { 1.0f, -1.0f, -1.0f, 1.0f };

	__m128 vec = _mm_mul_ps(gHalf, SIMDLOAD(angles.getBuffer()));
	__m128 sin_angles, cos_angles;
	sin_angles = SIMDSin(vec);
	cos_angles = SIMDCos(vec);

	__m128 p0 = SIMDPermute(sin_angles, cos_angles, 0, 4, 4, 4);
	__m128 y0 = SIMDPermute(sin_angles, cos_angles, 5, 1, 5, 5);
	__m128 r0 = SIMDPermute(sin_angles, cos_angles, 6, 6, 2, 6);
	__m128 p1 = SIMDPermute(cos_angles, sin_angles, 0, 4, 4, 4);
	__m128 y1 = SIMDPermute(cos_angles, sin_angles, 5, 1, 5, 5);
	__m128 r1 = SIMDPermute(cos_angles, sin_angles, 6, 6, 2, 6);

	__m128 q0 = _mm_mul_ps(p0, y0);
	__m128 q1 = _mm_mul_ps(p1, sign);
	q0 = _mm_mul_ps(q0, r0);
	q1 = _mm_mul_ps(q1, y1);

	SIMDSTORE(_quat, _mm_add_ps(_mm_mul_ps(q1, r1), q0));
}

QuaternionSIMD MakeFromAxis(const Vector4SIMD& axis, float angle)
{
	QuaternionSIMD temp;
	temp.constructFromAxis(axis, angle);
	return temp;
}

QuaternionSIMD MakeFromMatrix(const Matrix4x4SIMD& matrix)
{
	QuaternionSIMD temp;
	temp.constructFromMatrix(matrix);
	return temp;
}

QuaternionSIMD MakeFromAngles(float x, float y, float z)
{
	QuaternionSIMD temp;
	temp.constructFromAngles(x, y, z);
	return temp;
}

QuaternionSIMD MakeFromAngles(const Vector4SIMD& angles)
{
	QuaternionSIMD temp;
	temp.constructFromAngles(angles);
	return temp;
}

NS_END
