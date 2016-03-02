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

#include "Gleam_Quaternion_SIMD.h"
#include <Gaff_Assert.h>
#include <cmath>

NS_GLEAM

QuaternionSIMD QuaternionSIMD::Identity(0.0f, 0.0f, 0.0f, 1.0f);

QuaternionSIMD::QuaternionSIMD(void)
{
}

QuaternionSIMD::QuaternionSIMD(const QuaternionSIMD& rhs):
	_quat(rhs._quat)
{
}

QuaternionSIMD::QuaternionSIMD(float x, float y, float z, float w):
	_quat(SIMDCreate(x, y, z, w))
{
}

QuaternionSIMD::QuaternionSIMD(const float* elements)
{
	set(elements);
}

QuaternionSIMD::QuaternionSIMD(const Vector4SIMD& vec):
	_quat(vec.getSIMDType())
{
}

QuaternionSIMD::QuaternionSIMD(const SIMDType& quat):
	_quat(quat)
{
}

QuaternionSIMD::~QuaternionSIMD(void)
{
}

bool QuaternionSIMD::operator==(const QuaternionSIMD& rhs) const
{
	return SIMDEqual(_quat, rhs._quat);
}

bool QuaternionSIMD::operator!=(const QuaternionSIMD& rhs) const
{
	return !(*this == rhs);
}

const QuaternionSIMD& QuaternionSIMD::operator=(const QuaternionSIMD& rhs)
{
	_quat = rhs._quat;
	return *this;
}

float QuaternionSIMD::operator[](int index) const
{
	GAFF_ASSERT(index > -1 && index < 4);
	return SIMDGet(_quat, index);
}

//float& QuaternionSIMD::operator[](int index)
//{
//	GAFF_ASSERT(index > -1 && index < 4);
//	return _quat[index];
//}

void QuaternionSIMD::set(float x, float y, float z, float w)
{
	_quat = SIMDCreate(x, y, z, w);
}

void QuaternionSIMD::set(const float* elements)
{
#ifdef SIMD_SET_ALIGNED
	_quat = SIMDLoadAligned(elements);
#else
	_quat = SIMDLoad(elements);
#endif
}

void QuaternionSIMD::set(const Vector4SIMD& vec)
{
	_quat = vec.getSIMDType();
}

void QuaternionSIMD::set(float value, unsigned int index)
{
	GAFF_ASSERT(index >= 0 && index < 4);
	SIMDSet(_quat, value, index);
}

const float* QuaternionSIMD::getBuffer(void) const
{
	SIMDStoreAligned(_quat, _get_buf_cache);
	return _get_buf_cache;
}

//float* QuaternionSIMD::getBuffer(void)
//{
//	SIMDStoreAligned(_quat, _get_buf_cache);
//	return _get_buf_cache;
//}

const SIMDType& QuaternionSIMD::getSIMDType(void) const
{
	return _quat;
}

QuaternionSIMD QuaternionSIMD::operator+(const QuaternionSIMD& rhs) const
{
	return QuaternionSIMD(SIMDAdd(_quat, rhs._quat));
}

const QuaternionSIMD& QuaternionSIMD::operator+=(const QuaternionSIMD& rhs)
{
	_quat = SIMDAdd(_quat, rhs._quat);
	return *this;
}

QuaternionSIMD QuaternionSIMD::operator*(const QuaternionSIMD& rhs) const
{
	return QuaternionSIMD(SIMDQuatMul(_quat, rhs._quat));
}

const QuaternionSIMD& QuaternionSIMD::operator*=(const QuaternionSIMD& rhs)
{
	_quat = SIMDQuatMul(_quat, rhs._quat);
	return *this;
}

Vector4SIMD QuaternionSIMD::transform(const Vector4SIMD& vec) const
{
	return Vector4SIMD(SIMDQuatTransform(_quat, vec.getSIMDType()));
}

bool QuaternionSIMD::roughlyEqual(const QuaternionSIMD& rhs, const Vector4SIMD& epsilon) const
{
	return SIMDRoughlyEqual(_quat, rhs._quat, epsilon.getSIMDType());
}

bool QuaternionSIMD::roughlyEqual(const QuaternionSIMD& rhs, float epsilon) const
{
	return SIMDRoughlyEqual(_quat, rhs._quat, SIMDCreate(epsilon));
}

QuaternionSIMD QuaternionSIMD::slerp(const QuaternionSIMD& rhs, float t) const
{
	return QuaternionSIMD(SIMDQuatSlerp(_quat, rhs._quat, SIMDCreate(t)));
}

void QuaternionSIMD::slerpThis(const QuaternionSIMD& rhs, float t)
{
	_quat = SIMDQuatSlerp(_quat, rhs._quat, SIMDCreate(t));
}

float QuaternionSIMD::dot(const QuaternionSIMD& rhs) const
{
	return dotVec(rhs)[0];
}

void QuaternionSIMD::normalizeThis(void)
{
	_quat = SIMDVec4Normalize(_quat);
}

QuaternionSIMD QuaternionSIMD::normalize(void) const
{
	return QuaternionSIMD(SIMDVec4Normalize(_quat));
}

void QuaternionSIMD::conjugateThis(void)
{
	_quat = SIMDQuatConjugate(_quat);
}

QuaternionSIMD QuaternionSIMD::conjugate(void) const
{
	return QuaternionSIMD(SIMDQuatConjugate(_quat));
}

void QuaternionSIMD::inverseThis(void)
{
	_quat = SIMDQuatInverse(_quat);
}

QuaternionSIMD QuaternionSIMD::inverse(void) const
{
	return QuaternionSIMD(SIMDQuatInverse(_quat));
}

void QuaternionSIMD::shortestRotationThis(const Vector4SIMD& vec1, const Vector4SIMD& vec2)
{
	_quat = SIMDQuatShortestRotation(vec1.getSIMDType(), vec2.getSIMDType());
}

QuaternionSIMD QuaternionSIMD::shortestRotation(const Vector4SIMD& vec1, const Vector4SIMD& vec2) const
{
	return QuaternionSIMD(SIMDQuatShortestRotation(vec1.getSIMDType(), vec2.getSIMDType()));
}

float QuaternionSIMD::lengthSquared(void) const
{
	return lengthSquaredVec()[0];
}

float QuaternionSIMD::length(void) const
{
	return lengthVec()[0];
}

Vector4SIMD QuaternionSIMD::axis(void) const
{
	return Vector4SIMD(SIMDQuatGetAxis(_quat));
}

float QuaternionSIMD::angle(void) const
{
	return angleVec()[0];
}

Matrix4x4SIMD QuaternionSIMD::matrix(void) const
{
	return Matrix4x4SIMD(SIMDQuatToMatrix(_quat));
}

void QuaternionSIMD::constructFromAxis(const Vector4SIMD& axis, float angle)
{
	_quat = SIMDQuatFromAxisAngle(axis.getSIMDType(), SIMDCreate(angle));
}

void QuaternionSIMD::constructFromMatrix(const Matrix4x4SIMD& matrix)
{
	_quat = SIMDQuatFromMatrix(matrix.getSIMDType());
}

void QuaternionSIMD::constructFromAngles(float x, float y, float z)
{
	_quat = SIMDQuatFromAngles(SIMDCreate(x, y, z, 0.0f));
}

void QuaternionSIMD::constructFromAngles(const Vector4SIMD& angles)
{
	_quat = SIMDQuatFromAngles(angles.getSIMDType());
}

void QuaternionSIMD::slerpThis(const QuaternionSIMD& rhs, const Vector4SIMD& t)
{
	_quat = SIMDQuatSlerp(_quat, rhs._quat, t.getSIMDType());
}
QuaternionSIMD QuaternionSIMD::slerp(const QuaternionSIMD& rhs, const Vector4SIMD& t) const
{
	return QuaternionSIMD(SIMDQuatSlerp(_quat, rhs._quat, t.getSIMDType()));
}

QuaternionSIMD QuaternionSIMD::dotVec(const QuaternionSIMD& rhs) const
{
	return QuaternionSIMD(SIMDVec4Dot(_quat, rhs._quat));
}

Vector4SIMD QuaternionSIMD::lengthSquaredVec(void) const
{
	return Vector4SIMD(SIMDVec4LengthSquared(_quat));
}

Vector4SIMD QuaternionSIMD::lengthVec(void) const
{
	return Vector4SIMD(SIMDVec4Length(_quat));
}

Vector4SIMD QuaternionSIMD::angleVec(void) const
{
	return Vector4SIMD(SIMDQuatGetAngle(_quat));
}

void QuaternionSIMD::constructFromAxis(const Vector4SIMD& axis, const Vector4SIMD& angle)
{
	_quat = SIMDQuatFromAxisAngle(axis.getSIMDType(), angle.getSIMDType());
}

QuaternionSIMD QuaternionSIMD::MakeFromAxis(const Vector4SIMD& axis, const Vector4SIMD& angle)
{
	return QuaternionSIMD(SIMDQuatFromAxisAngle(axis.getSIMDType(), angle.getSIMDType()));
}

QuaternionSIMD QuaternionSIMD::MakeFromAxis(const Vector4SIMD& axis, float angle)
{
	return QuaternionSIMD(SIMDQuatFromAxisAngle(axis.getSIMDType(), SIMDCreate(angle)));
}

QuaternionSIMD QuaternionSIMD::MakeFromMatrix(const Matrix4x4SIMD& matrix)
{
	return QuaternionSIMD(SIMDQuatFromMatrix(matrix.getSIMDType()));
}

QuaternionSIMD QuaternionSIMD::MakeFromAngles(float x, float y, float z)
{
	return QuaternionSIMD(SIMDQuatFromAngles(SIMDCreate(x, y, z, 0.0f)));
}

QuaternionSIMD QuaternionSIMD::MakeFromAngles(const Vector4SIMD& angles)
{
	return QuaternionSIMD(SIMDQuatFromAngles(angles.getSIMDType()));
}

NS_END
