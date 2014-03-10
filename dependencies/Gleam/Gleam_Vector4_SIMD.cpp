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

#include "Gleam_Vector4_SIMD.h"
#include <Gaff_IncludeAssert.h>
#include <cmath>

NS_GLEAM

Vector4SIMD Vector4SIMD::zero(0.0f, 0.0f, 0.0f, 0.0f);
Vector4SIMD Vector4SIMD::x_axis(1.0f, 0.0f, 0.0f, 0.0f);
Vector4SIMD Vector4SIMD::y_axis(0.0f, 1.0f, 0.0f, 0.0f);
Vector4SIMD Vector4SIMD::z_axis(0.0f, 0.0f, 1.0f, 0.0f);
Vector4SIMD Vector4SIMD::origin(0.0f, 0.0f, 0.0f, 1.0f);

Vector4SIMD::Vector4SIMD(void)
{
}

Vector4SIMD::Vector4SIMD(const Vector4SIMD& rhs):
	_x(rhs._x), _y(rhs._y), _z(rhs._z), _w(rhs._w)
{
}

Vector4SIMD::Vector4SIMD(float x, float y, float z, float w):
	_x(x), _y(y), _z(z), _w(w)
{
}

Vector4SIMD::Vector4SIMD(const float* elements)
{
	set(elements);
}

Vector4SIMD::Vector4SIMD(const __m128& rhs)
{
	SIMDSTORE(_vec, rhs);
}

Vector4SIMD::~Vector4SIMD(void)
{
}

bool Vector4SIMD::operator==(const Vector4SIMD& rhs) const
{
	return SIMDEqual(SIMDLOAD(_vec), SIMDLOAD(rhs._vec));
}

bool Vector4SIMD::operator!=(const Vector4SIMD& rhs) const
{
	return !SIMDEqual(SIMDLOAD(_vec), SIMDLOAD(rhs._vec));
}

const Vector4SIMD& Vector4SIMD::operator=(const Vector4SIMD& rhs)
{
	set(rhs._vec);
	return *this;
}

float Vector4SIMD::operator[](int index) const
{
	assert(index > -1 && index < 4);
	return _vec[index];
}

float& Vector4SIMD::operator[](int index)
{
	assert(index > -1 && index < 4);
	return _vec[index];
}

const Vector4SIMD& Vector4SIMD::operator-=(const Vector4SIMD& rhs)
{
	__m128 left = SIMDLOAD(_vec);
	__m128 right = SIMDLOAD(rhs._vec);
	SIMDSTORE(_vec, _mm_sub_ps(left, right));
	return *this;
}

Vector4SIMD Vector4SIMD::operator-(const Vector4SIMD& rhs) const
{
	__m128 left = SIMDLOAD(_vec);
	__m128 right = SIMDLOAD(rhs._vec);
	return Vector4SIMD(_mm_sub_ps(left, right));
}

Vector4SIMD Vector4SIMD::operator-(void) const
{
	return zero - *this;
}

const Vector4SIMD& Vector4SIMD::operator+=(const Vector4SIMD& rhs)
{
	__m128 left = SIMDLOAD(_vec);
	__m128 right = SIMDLOAD(rhs._vec);
	SIMDSTORE(_vec, _mm_add_ps(left, right));
	return *this;
}

Vector4SIMD Vector4SIMD::operator+(const Vector4SIMD& rhs) const
{
	__m128 left = SIMDLOAD(_vec);
	__m128 right = SIMDLOAD(rhs._vec);
	return Vector4SIMD(_mm_add_ps(left, right));
}

Vector4SIMD Vector4SIMD::operator+(void) const
{
	return Vector4SIMD(+_x, +_y, +_z, +_w);
}

const Vector4SIMD& Vector4SIMD::operator*=(const Vector4SIMD& rhs)
{
	__m128 left = SIMDLOAD(_vec);
	__m128 right = SIMDLOAD(rhs._vec);
	SIMDSTORE(_vec, _mm_mul_ps(left, right));
	return *this;
}

Vector4SIMD Vector4SIMD::operator*(const Vector4SIMD& rhs) const
{
	__m128 left = SIMDLOAD(_vec);
	__m128 right = SIMDLOAD(rhs._vec);
	return Vector4SIMD(_mm_mul_ps(left, right));
}

const Vector4SIMD& Vector4SIMD::operator*=(float rhs)
{
	__m128 vec = SIMDLOAD(_vec);
	__m128 scalar = _mm_set_ps1(rhs);
	SIMDSTORE(_vec, _mm_mul_ps(vec, scalar));
	return *this;
}

Vector4SIMD Vector4SIMD::operator*(float rhs) const
{
	__m128 vec = SIMDLOAD(_vec);
	__m128 scalar = _mm_set_ps1(rhs);
	return Vector4SIMD(_mm_mul_ps(vec, scalar));
}

const Vector4SIMD& Vector4SIMD::operator/=(const Vector4SIMD& rhs)
{
	__m128 left = SIMDLOAD(_vec);
	__m128 right = SIMDLOAD(rhs._vec);
	SIMDSTORE(_vec, _mm_div_ps(left, right));
	return *this;
}

Vector4SIMD Vector4SIMD::operator/(const Vector4SIMD& rhs) const
{
	__m128 left = SIMDLOAD(_vec);
	__m128 right = SIMDLOAD(rhs._vec);
	return Vector4SIMD(_mm_div_ps(left, right));
}

const Vector4SIMD& Vector4SIMD::operator/=(float rhs)
{
	__m128 vec = SIMDLOAD(_vec);
	__m128 denominator = _mm_set_ps1(rhs);
	SIMDSTORE(_vec, _mm_div_ps(vec, denominator));
	return *this;
}

Vector4SIMD Vector4SIMD::operator/(float rhs) const
{
	__m128 vec = SIMDLOAD(_vec);
	__m128 denominator = _mm_set_ps1(rhs);
	return Vector4SIMD(_mm_div_ps(vec, denominator));
}

void Vector4SIMD::set(float x, float y, float z, float w)
{
	_vec[0] = x;
	_vec[1] = y;
	_vec[2] = z;
	_vec[3] = w;
}

void Vector4SIMD::set(const float* elements)
{
	assert(elements);
	_vec[0] = elements[0];
	_vec[1] = elements[1];
	_vec[2] = elements[2];
	_vec[3] = elements[3];
}

const float* Vector4SIMD::getBuffer(void) const
{
	return _vec;
}

float* Vector4SIMD::getBuffer(void)
{
	return _vec;
}

float Vector4SIMD::lengthSquared(void) const
{
	return _mm_cvtss_f32(SIMDVec4LengthSquared(SIMDLOAD(_vec)));
}

float Vector4SIMD::length(void) const
{
	return _mm_cvtss_f32(SIMDVec4Length(SIMDLOAD(_vec)));
}

float Vector4SIMD::reciprocalLengthSquared(void) const
{
	assert(length() != 0.0f);
	return _mm_cvtss_f32(SIMDVec4ReciprocalLengthSquared(SIMDLOAD(_vec)));
}

float Vector4SIMD::reciprocalLength(void) const
{
	assert(length() != 0.0f);
	return _mm_cvtss_f32(SIMDVec4ReciprocalLength(SIMDLOAD(_vec)));
}

void Vector4SIMD::normalize(void)
{
	assert(length() != 0.0f);
	SIMDSTORE(_vec, SIMDVec4Normalize(SIMDLOAD(_vec)));
}

Vector4SIMD Vector4SIMD::cross(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDVec4Cross(SIMDLOAD(_vec), SIMDLOAD(rhs._vec)));
}

float Vector4SIMD::dot(const Vector4SIMD& rhs) const
{
	return _mm_cvtss_f32(SIMDVec4Dot(SIMDLOAD(_vec), SIMDLOAD(rhs._vec)));
}

float Vector4SIMD::angleUnit(const Vector4SIMD& rhs) const
{
	return _mm_cvtss_f32(SIMDVec4AngleUnit(SIMDLOAD(_vec), SIMDLOAD(rhs._vec)));
}

float Vector4SIMD::angle(const Vector4SIMD& rhs) const
{
	return _mm_cvtss_f32(SIMDVec4Angle(SIMDLOAD(_vec), SIMDLOAD(rhs._vec)));
}

Vector4SIMD Vector4SIMD::reflect(const Vector4SIMD& normal) const
{
	return Vector4SIMD(SIMDVec4Reflect(SIMDLOAD(_vec), SIMDLOAD(normal._vec)));
}

Vector4SIMD Vector4SIMD::refract(const Vector4SIMD& normal, float refraction_index) const
{
	return Vector4SIMD(SIMDVec4Refract(SIMDLOAD(_vec), SIMDLOAD(normal._vec), refraction_index));
}

bool Vector4SIMD::roughlyEqual(const Vector4SIMD& rhs, float epsilon) const
{
	return SIMDRoughlyEqual(SIMDLOAD(_vec), SIMDLOAD(rhs._vec), epsilon);
}

Vector4SIMD operator*(float lhs, const Vector4SIMD& rhs)
{
	return rhs * lhs;
}

NS_END
