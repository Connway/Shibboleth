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
#include <Gaff_Math.h>
#include <cmath>

NS_GLEAM

Vector4SIMD Vector4SIMD::Zero(0.0f, 0.0f, 0.0f, 0.0f);
Vector4SIMD Vector4SIMD::X_Axis(1.0f, 0.0f, 0.0f, 0.0f);
Vector4SIMD Vector4SIMD::Y_Axis(0.0f, 1.0f, 0.0f, 0.0f);
Vector4SIMD Vector4SIMD::Z_Axis(0.0f, 0.0f, 1.0f, 0.0f);
Vector4SIMD Vector4SIMD::Origin(0.0f, 0.0f, 0.0f, 1.0f);
Vector4SIMD Vector4SIMD::Epsilon(0.000001f);

Vector4SIMD::Vector4SIMD(void)
{
}

Vector4SIMD::Vector4SIMD(const Vector4SIMD& rhs):
	_vec(rhs._vec)
{
}

Vector4SIMD::Vector4SIMD(float x, float y, float z, float w):
	_vec(SIMDCreate(x, y, z, w))
{
}

Vector4SIMD::Vector4SIMD(const float* elements)
{
	set(elements);
}

Vector4SIMD::Vector4SIMD(const SIMDType& value):
	_vec(value)
{
}

Vector4SIMD::Vector4SIMD(float value)
{
	_vec = SIMDCreate(value);
}

Vector4SIMD::~Vector4SIMD(void)
{
}

bool Vector4SIMD::operator==(const Vector4SIMD& rhs) const
{
	return SIMDEqual(_vec, rhs._vec);
}

bool Vector4SIMD::operator!=(const Vector4SIMD& rhs) const
{
	return !(*this == rhs);
}

const Vector4SIMD& Vector4SIMD::operator=(const Vector4SIMD& rhs)
{
	_vec = rhs._vec;
	return *this;
}

float Vector4SIMD::operator[](int index) const
{
	assert(index > -1 && index < 4);
	return SIMDGet(_vec, index);
}

//float& Vector4SIMD::operator[](int index)
//{
//	assert(index > -1 && index < 4);
//	return _vec[index];
//}

const Vector4SIMD& Vector4SIMD::operator-=(const Vector4SIMD& rhs)
{
	_vec = SIMDSub(_vec, rhs._vec);
	return *this;
}

Vector4SIMD Vector4SIMD::operator-(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDSub(_vec, rhs._vec));
}

Vector4SIMD Vector4SIMD::operator-(void) const
{
	return Vector4SIMD(SIMDNegate(_vec));
}

const Vector4SIMD& Vector4SIMD::operator+=(const Vector4SIMD& rhs)
{
	_vec = SIMDAdd(_vec, rhs._vec);
	return *this;
}

Vector4SIMD Vector4SIMD::operator+(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDAdd(_vec, rhs._vec));
}

Vector4SIMD Vector4SIMD::operator+(void) const
{
	return Vector4SIMD(SIMDAnd(gSignMask, _vec));
}

void Vector4SIMD::set(float x, float y, float z, float w)
{
	_vec = SIMDCreate(x, y, z, w);
}

void Vector4SIMD::set(const float* elements)
{
	assert(elements);

#ifdef SIMD_SET_ALIGNED
	_vec = SIMDLoadAligned(elements);
#else
	_vec = SIMDLoad(elements);
#endif
}

void Vector4SIMD::set(float value, unsigned int index)
{
	assert(index >= 0 && index < 4);
	SIMDSet(_vec, value, index);
}

const Vector4SIMD& Vector4SIMD::operator*=(const Vector4SIMD& rhs)
{
	_vec = SIMDMul(_vec, rhs._vec);
	return *this;
}

Vector4SIMD Vector4SIMD::operator*(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDMul(_vec, rhs._vec));
}

const Vector4SIMD& Vector4SIMD::operator*=(float rhs)
{
	_vec = SIMDMul(_vec, SIMDCreate(rhs));
	return *this;
}

Vector4SIMD Vector4SIMD::operator*(float rhs) const
{
	return Vector4SIMD(SIMDMul(_vec, SIMDCreate(rhs)));
}

const Vector4SIMD& Vector4SIMD::operator/=(const Vector4SIMD& rhs)
{
	_vec = SIMDDiv(_vec, rhs._vec);
	return *this;
}

Vector4SIMD Vector4SIMD::operator/(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDDiv(_vec, rhs._vec));
}

const Vector4SIMD& Vector4SIMD::operator/=(float rhs)
{
	_vec = SIMDDiv(_vec, SIMDCreate(rhs));
	return *this;
}

Vector4SIMD Vector4SIMD::operator/(float rhs) const
{
	return Vector4SIMD(SIMDDiv(_vec, SIMDCreate(rhs)));
}

const float* Vector4SIMD::getBuffer(void) const
{
	SIMDStoreAligned(_vec, _get_buf_cache);
	return _get_buf_cache;
}

//float* Vector4SIMD::getBuffer(void)
//{
//	SIMDStoreAligned(_vec, _get_buf_cache);
//	return _get_buf_cache;
//}

const SIMDType& Vector4SIMD::getSIMDType(void) const
{
	return _vec;
}

float Vector4SIMD::lengthSquared(void) const
{
	return lengthSquaredVec()[0];
}

float Vector4SIMD::length(void) const
{
	return lengthVec()[0];
}

float Vector4SIMD::reciprocalLengthSquared(void) const
{
	return reciprocalLengthSquaredVec()[0];
}

float Vector4SIMD::reciprocalLength(void) const
{
	return reciprocalLengthVec()[0];
}

Vector4SIMD Vector4SIMD::normalize(void) const
{
	return Vector4SIMD(SIMDVec4Normalize(_vec));
}

Vector4SIMD Vector4SIMD::cross(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDVec4Cross(_vec, rhs._vec));
}

float Vector4SIMD::dot(const Vector4SIMD& rhs) const
{
	return dotVec(rhs)[0];
}

float Vector4SIMD::angleUnit(const Vector4SIMD& rhs) const
{
	return angleUnitVec(rhs)[0];
}

float Vector4SIMD::angle(const Vector4SIMD& rhs) const
{
	return angleVec(rhs)[0];
}

Vector4SIMD Vector4SIMD::reflect(const Vector4SIMD& normal) const
{
	return Vector4SIMD(SIMDVec4Reflect(_vec, normal._vec));
}

Vector4SIMD Vector4SIMD::refract(const Vector4SIMD& normal, float refraction_index) const
{
	return Vector4SIMD(SIMDVec4Refract(_vec, normal._vec, SIMDCreate(refraction_index)));
}

Vector4SIMD Vector4SIMD::lerp(const Vector4SIMD& end, float t) const
{
	return Vector4SIMD(SIMDVec4Lerp(_vec, end._vec, SIMDCreate(t)));
}

Vector4SIMD Vector4SIMD::minimum(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDMin(_vec, rhs._vec));
}

Vector4SIMD Vector4SIMD::maximum(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDMax(_vec, rhs._vec));
}

void Vector4SIMD::reflectThis(const Vector4SIMD& normal)
{
	_vec = SIMDVec4Reflect(_vec, normal._vec);
}

void Vector4SIMD::refractThis(const Vector4SIMD& normal, float refraction_index)
{
	_vec = SIMDVec4Refract(_vec, normal._vec, SIMDCreate(refraction_index));
}

void Vector4SIMD::lerpThis(const Vector4SIMD& end, float t)
{
	_vec = SIMDVec4Lerp(_vec, end._vec, SIMDCreate(t));
}

void Vector4SIMD::minimumThis(const Vector4SIMD& rhs)
{
	_vec = SIMDMin(_vec, rhs._vec);
}

void Vector4SIMD::maximumThis(const Vector4SIMD& rhs)
{
	_vec = SIMDMax(_vec, rhs._vec);
}

void Vector4SIMD::crossThis(const Vector4SIMD& rhs)
{
	_vec = SIMDVec4Cross(_vec, rhs._vec);
}

void Vector4SIMD::normalizeThis(void)
{
	_vec = SIMDVec4Normalize(_vec);
}

bool Vector4SIMD::roughlyEqual(const Vector4SIMD& rhs, float epsilon) const
{
	return SIMDRoughlyEqual(_vec, rhs._vec, SIMDCreate(epsilon));
}

Vector4SIMD Vector4SIMD::lengthSquaredVec(void) const
{
	return Vector4SIMD(SIMDVec4LengthSquared(_vec));
}

Vector4SIMD Vector4SIMD::lengthVec(void) const
{
	return Vector4SIMD(SIMDVec4Length(_vec));
}

Vector4SIMD Vector4SIMD::reciprocalLengthSquaredVec(void) const
{
	return Vector4SIMD(SIMDVec4ReciprocalLengthSquared(_vec));
}

Vector4SIMD Vector4SIMD::reciprocalLengthVec(void) const
{
	return Vector4SIMD(SIMDVec4ReciprocalLength(_vec));
}

Vector4SIMD Vector4SIMD::dotVec(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDVec4Dot(_vec, rhs._vec));
}

Vector4SIMD Vector4SIMD::angleUnitVec(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDVec4AngleUnit(_vec, rhs._vec));
}

Vector4SIMD Vector4SIMD::angleVec(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDVec4Angle(_vec, rhs._vec));
}

Vector4SIMD Vector4SIMD::refract(const Vector4SIMD& normal, const Vector4SIMD& refraction_index) const
{
	return Vector4SIMD(SIMDVec4Refract(_vec, normal._vec, refraction_index._vec));
}

Vector4SIMD Vector4SIMD::lerp(const Vector4SIMD& end, const Vector4SIMD& t) const
{
	return Vector4SIMD(SIMDVec4Lerp(_vec, end._vec, t._vec));
}

bool Vector4SIMD::roughlyEqual(const Vector4SIMD& rhs, const Vector4SIMD& epsilon) const
{
	return SIMDRoughlyEqual(_vec, rhs._vec, epsilon._vec);
}

void Vector4SIMD::refractThis(const Vector4SIMD& normal, const Vector4SIMD& refraction_index)
{
	_vec = SIMDVec4Refract(_vec, normal._vec, refraction_index._vec);
}

void Vector4SIMD::lerpThis(const Vector4SIMD& end, const Vector4SIMD& t)
{
	_vec = SIMDVec4Lerp(_vec, end._vec, t._vec);
}

Vector4SIMD operator*(float lhs, const Vector4SIMD& rhs)
{
	return rhs * lhs;
}

NS_END
