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

#include "Gleam_Quaternion.h"
#include "Gaff_IncludeAssert.h"
#include <cmath>

NS_GLEAM

Quaternion Quaternion::identity(0.0f, 0.0f, 0.0f, 1.0f);

Quaternion::Quaternion(void)
{
}

Quaternion::Quaternion(const Quaternion& rhs):
	_x(rhs._x), _y(rhs._y), _z(rhs._z), _w(rhs._w)
{
}
Quaternion::Quaternion(float x, float y, float z, float w):
	_x(x), _y(y), _z(z), _w(w)
{
}

Quaternion::Quaternion(const float* elements)
{
	set(elements[0], elements[1], elements[2], elements[3]);
}

Quaternion::Quaternion(const Vector4& vec):
	_x(vec[0]), _y(vec[1]), _z(vec[2]), _w(vec[3])
{
}

Quaternion::~Quaternion(void)
{
}

bool Quaternion::operator==(const Quaternion& rhs) const
{
	return _x == rhs._x && _y == rhs._y && _z == rhs._z && _w == rhs._w;
}

bool Quaternion::operator!=(const Quaternion& rhs) const
{
	return !(*this == rhs);
}

const Quaternion& Quaternion::operator=(const Quaternion& rhs)
{
	set(rhs._quat);
	return *this;
}

float Quaternion::operator[](int index) const
{
	assert(index > -1 && index < 4);
	return _quat[index];
}

float& Quaternion::operator[](int index)
{
	assert(index > -1 && index < 4);
	return _quat[index];
}

void Quaternion::set(float x, float y, float z, float w)
{
	_x = x;
	_y = y;
	_z = z;
	_w = w;
}

void Quaternion::set(const float* elements)
{
	_quat[0] = elements[0];
	_quat[1] = elements[1];
	_quat[2] = elements[2];
	_quat[3] = elements[3];
}

void Quaternion::set(const Vector4& vec)
{
	_quat[0] = vec[0];
	_quat[1] = vec[1];
	_quat[2] = vec[2];
	_quat[3] = vec[3];
}

const float* Quaternion::getBuffer(void) const
{
	return _quat;
}

float* Quaternion::getBuffer(void)
{
	return _quat;
}

Quaternion Quaternion::operator+(const Quaternion& rhs) const
{
	return Quaternion(
		_x + rhs._x,
		_y + rhs._y,
		_z + rhs._z,
		_w + rhs._w
	);
}

const Quaternion& Quaternion::operator+=(const Quaternion& rhs)
{
	_x += rhs._x;
	_y += rhs._y;
	_z += rhs._z;
	_w += rhs._w;

	return *this;
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
	return Quaternion(
		(_quat[3] * rhs._quat[0]) + (_quat[0] * rhs._quat[3]) + (_quat[1] * rhs._quat[2]) - (_quat[2] * rhs._quat[1]),
		(_quat[3] * rhs._quat[1]) - (_quat[0] * rhs._quat[2]) + (_quat[1] * rhs._quat[3]) + (_quat[2] * rhs._quat[0]),
		(_quat[3] * rhs._quat[2]) + (_quat[0] * rhs._quat[1]) - (_quat[1] * rhs._quat[0]) + (_quat[2] * rhs._quat[3]),
		(_quat[3] * rhs._quat[3]) - (_quat[0] * rhs._quat[0]) - (_quat[1] * rhs._quat[1]) - (_quat[2] * rhs._quat[2])
	);
}

const Quaternion& Quaternion::operator*=(const Quaternion& rhs)
{
	float x = (_quat[3] * rhs._quat[0]) + (_quat[0] * rhs._quat[3]) + (_quat[1] * rhs._quat[2]) - (_quat[2] * rhs._quat[1]);
	float y = (_quat[3] * rhs._quat[1]) - (_quat[0] * rhs._quat[2]) + (_quat[1] * rhs._quat[3]) + (_quat[2] * rhs._quat[0]);
	float z = (_quat[3] * rhs._quat[2]) + (_quat[0] * rhs._quat[1]) - (_quat[1] * rhs._quat[0]) + (_quat[2] * rhs._quat[3]);
	float w = (_quat[3] * rhs._quat[3]) - (_quat[0] * rhs._quat[0]) - (_quat[1] * rhs._quat[1]) - (_quat[2] * rhs._quat[2]);

	_x = x;
	_y = y;
	_z = z;
	_w = w;

	return *this;
}

Vector4 Quaternion::transform(const Vector4& vec) const
{
	Quaternion p, q_conj;
	q_conj = *this;
	q_conj.conjugate();

	p._x = vec[0];
	p._y = vec[1];
	p._z = vec[2];
	p._w = 0.0f;

	p = *this * p * q_conj;

	return Vector4(p._x, p._y, p._z, 0.0f);
}

bool Quaternion::roughlyEqual(const Quaternion& rhs, const Vector4& epsilon) const
{
	return fabsf(_x - rhs._x) <= epsilon[0] &&
			fabsf(_y - rhs._y) <= epsilon[1] &&
			fabsf(_z - rhs._z) <= epsilon[2] &&
			fabsf(_w - rhs._w) <= epsilon[3];
}

bool Quaternion::roughlyEqual(const Quaternion& rhs, float epsilon) const
{
	return fabsf(_x - rhs._x) <= epsilon &&
			fabsf(_y - rhs._y) <= epsilon &&
			fabsf(_z - rhs._z) <= epsilon &&
			fabsf(_w - rhs._w) <= epsilon;
}

Quaternion Quaternion::slerp(const Quaternion& rhs, float t)
{
	float theta = acosf(dot(rhs));
	float st = sinf(theta);
	float scale1 = sinf(theta * (1.0f - t)) / st;
	float scale2 = sinf(theta * t) / st;

	return Quaternion(
		_x * scale1 + rhs._x * scale2,
		_y * scale1 + rhs._y * scale2,
		_z * scale1 + rhs._z * scale2,
		_w * scale1 + rhs._w * scale2
	);
}

float Quaternion::dot(const Quaternion& rhs) const
{
	return _x*rhs._x + _y*rhs._y + _z*rhs._z + _w*rhs._w;
}

void Quaternion::normalize(void)
{
	float scale = 1.0f / length();
	_x *= scale;
	_y *= scale;
	_z *= scale;
	_w *= scale;
}

void Quaternion::conjugate(void)
{
	_x = -_x;
	_y = -_y;
	_z = -_z;
}

void Quaternion::inverse(void)
{
	float norm = length();
	norm = 1.0f / (norm * norm);

	conjugate();

	_x *= norm;
	_y *= norm;
	_z *= norm;
	_w *= norm;
}

float Quaternion::lengthSquared(void) const
{
	return _x*_x + _y*_y + _z*_z;
}

float Quaternion::length(void) const
{
	return sqrtf(_x*_x + _y*_y + _z*_z);
}

Vector4 Quaternion::axis(void) const
{
	return Vector4(_x, _y, _z, 0.0f);
}

float Quaternion::angle(void) const
{
	return 2.0f * acosf(_w);
}

Matrix4x4 Quaternion::matrix(void) const
{
	Matrix4x4 matrix;
	float* m = matrix.getBuffer();

	float xx = _x * _x;
	float yy = _y * _y;
	float zz = _z * _z;
	float xy = _x * _y;
	float xz = _x * _z;
	float yz = _y * _z;
	float wx = _w * _x;
	float wy = _w * _y;
	float wz = _w * _z;

	m[0] = 1.0f - 2.0f * (yy + zz);
	m[1] = 2.0f * (xy + wz);
	m[2] = 2.0f * (xz - wy);
	m[3] = 0.0f;
	m[4] = 2.0f * (xy - wz);
	m[5] = 1.0f - 2.0f * (xx + zz);
	m[6] = 2.0f * (yz + wx);
	m[7] = 0.0f;
	m[8] = 2.0f * (xz + wy);
	m[9] = 2.0f * (yz - wx);
	m[10] = 1.0f - 2.0f * (xx + yy);
	m[11] = 0.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;

	return matrix;
}

void Quaternion::constructFromAxis(const Vector4& axis, float angle)
{
	angle *= 0.5f;
	float s = sinf(angle);

	_x = axis[0] * s;
	_y = axis[1] * s;
	_z = axis[2] * s;
	_w = cosf(angle);
}

void Quaternion::constructFromMatrix(const Matrix4x4& matrix)
{
	const float* m = matrix.getBuffer();
	float t = m[0] + m[5] + m[10] + 1.0f;

	if (t > 0.00000001f) {
		float s = sqrtf(t) * 2.0f;
		_x = (m[6] - m[9]) / s;
		_y = (m[8] - m[2]) / s;
		_z = (m[1] - m[4]) / s;
		_w = 0.25f * s;

	} else if (m[0] > m[5] && m[0] > m[10]) {
		float s = sqrtf(1.0f + m[0] - m[5] - m[10]) * 2.0f;
		_x = 0.25f * s;
		_y = (m[1] - m[4]) / s;
		_z = (m[8] - m[2]) / s;
		_w = (m[6] - m[9]) / s;

	} else if (m[5] > m[10]) {
		float s = sqrtf(1.0f + m[5] - m[0] - m[10]) * 2.0f;
		_x = (m[1] - m[4]) / s;
		_y = 0.25f * s;
		_z = (m[6] - m[9]) / s;
		_w = (m[8] - m[2]) / s;

	} else {
		float s = sqrtf(1.0f + m[10] - m[0] - m[5]) * 2.0f;
		_x = (m[8] - m[2]) / s;
		_y = (m[6] - m[9]) / s;
		_z = 0.25f * s;
		_w = (m[1] - m[4]) / s;
	}

	// normalize
	float scale = 1.0f / length();
	_x *= scale;
	_y *= scale;
	_z *= scale;
	_w *= scale;
}

void Quaternion::constructFromAngles(float x, float y, float z)
{
	float sr, sp, sy, cr, cp, cy;
	x *= 0.5f;
	y *= 0.5f;
	z *= 0.5f;

	sr = sinf(x);
	cr = cosf(x);
	sp = sinf(y);
	cp = cosf(y);
	sy = sinf(z);
	cy = cosf(z);

	_x = sr*cp*cy - cr*sp*sy;
	_y = cr*sp*cy + sr*cp*sy;
	_z = cr*cp*sy - sr*sp*cy;
	_w = cr*cp*cy + sr*sp*sy;
}

void Quaternion::constructFromAngles(const Vector4& angles)
{
	constructFromAngles(angles[0], angles[1], angles[2]);
}

Quaternion MakeFromAxis(const Vector4& axis, float angle)
{
	Quaternion temp;
	temp.constructFromAxis(axis, angle);
	return temp;
}

Quaternion MakeFromMatrix(const Matrix4x4& matrix)
{
	Quaternion temp;
	temp.constructFromMatrix(matrix);
	return temp;
}

Quaternion MakeFromAngles(float x, float y, float z)
{
	Quaternion temp;
	temp.constructFromAngles(x, y, z);
	return temp;
}

Quaternion MakeFromAngles(const Vector4& angles)
{
	Quaternion temp;
	temp.constructFromAngles(angles);
	return temp;
}

NS_END
