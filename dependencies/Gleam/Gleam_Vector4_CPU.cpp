/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_Vector4_CPU.h"
#include <Gaff_IncludeAssert.h>
#include <Gaff_Math.h>
#include <cmath>

NS_GLEAM

Vector4CPU Vector4CPU::Zero(0.0f, 0.0f, 0.0f, 0.0f);
Vector4CPU Vector4CPU::X_Axis(1.0f, 0.0f, 0.0f, 0.0f);
Vector4CPU Vector4CPU::Y_Axis(0.0f, 1.0f, 0.0f, 0.0f);
Vector4CPU Vector4CPU::Z_Axis(0.0f, 0.0f, 1.0f, 0.0f);
Vector4CPU Vector4CPU::Origin(0.0f, 0.0f, 0.0f, 1.0f);
Vector4CPU Vector4CPU::Epsilon(0.000001f);

Vector4CPU::Vector4CPU(void)
{
}

Vector4CPU::Vector4CPU(const Vector4CPU& rhs):
	_x(rhs._x), _y(rhs._y), _z(rhs._z), _w(rhs._w)
{
}
Vector4CPU::Vector4CPU(float x, float y, float z, float w):
	_x(x), _y(y), _z(z), _w(w)
{
}

Vector4CPU::Vector4CPU(const float* elements)
{
	set(elements);
}

Vector4CPU::Vector4CPU(float value):
	_x(value), _y(value), _z(value), _w(value)
{
}

Vector4CPU::~Vector4CPU(void)
{
}

bool Vector4CPU::operator==(const Vector4CPU& rhs) const
{
	return _x == rhs._x && _y == rhs._y && _z == rhs._z && _w == rhs._w;
}

bool Vector4CPU::operator!=(const Vector4CPU& rhs) const
{
	return !(*this == rhs);
}

const Vector4CPU& Vector4CPU::operator=(const Vector4CPU& rhs)
{
	set(rhs._vec);
	return *this;
}

float Vector4CPU::operator[](int index) const
{
	assert(index > -1 && index < 4);
	return _vec[index];
}

//float& Vector4CPU::operator[](int index)
//{
//	assert(index > -1 && index < 4);
//	return _vec[index];
//}

const Vector4CPU& Vector4CPU::operator-=(const Vector4CPU& rhs)
{
	_x -= rhs._x;
	_y -= rhs._y;
	_z -= rhs._z;
	_w -= rhs._w;
	return *this;
}

Vector4CPU Vector4CPU::operator-(const Vector4CPU& rhs) const
{
	return Vector4CPU(_x - rhs._x, _y - rhs._y, _z - rhs._z, _w - rhs._w);
}

Vector4CPU Vector4CPU::operator-(void) const
{
	return Vector4CPU(-_x, -_y, -_z, -_w);
}

const Vector4CPU& Vector4CPU::operator+=(const Vector4CPU& rhs)
{
	_x += rhs._x;
	_y += rhs._y;
	_z += rhs._z;
	_w += rhs._w;
	return *this;
}

Vector4CPU Vector4CPU::operator+(const Vector4CPU& rhs) const
{
	return Vector4CPU(_x + rhs._x, _y + rhs._y, _z + rhs._z, _w + rhs._w);
}

Vector4CPU Vector4CPU::operator+(void) const
{
	return Vector4CPU(+_x, +_y, +_z, +_w);
}

void Vector4CPU::set(float x, float y, float z, float w)
{
	_x = x;
	_y = y;
	_z = z;
	_w = w;
}

void Vector4CPU::set(const float* elements)
{
	assert(elements);
	_vec[0] = elements[0];
	_vec[1] = elements[1];
	_vec[2] = elements[2];
	_vec[3] = elements[3];
}

void Vector4CPU::set(float value, unsigned int index)
{
	assert(index >= 0 && index < 4);
	_vec[index] = value;
}

const Vector4CPU& Vector4CPU::operator*=(const Vector4CPU& rhs)
{
	_x *= rhs._x;
	_y *= rhs._y;
	_z *= rhs._z;
	_w *= rhs._w;
	return *this;
}

Vector4CPU Vector4CPU::operator*(const Vector4CPU& rhs) const
{
	return Vector4CPU(_x * rhs._x, _y * rhs._y, _z * rhs._z, _w * rhs._w);
}

const Vector4CPU& Vector4CPU::operator*=(float rhs)
{
	_x *= rhs;
	_y *= rhs;
	_z *= rhs;
	_w *= rhs;
	return *this;
}

Vector4CPU Vector4CPU::operator*(float rhs) const
{
	return Vector4CPU(_x * rhs, _y * rhs, _z * rhs, _w * rhs);
}

const Vector4CPU& Vector4CPU::operator/=(const Vector4CPU& rhs)
{
	_x /= rhs._x;
	_y /= rhs._y;
	_z /= rhs._z;
	_w /= rhs._w;
	return *this;
}

Vector4CPU Vector4CPU::operator/(const Vector4CPU& rhs) const
{
	return Vector4CPU(_x / rhs._x, _y / rhs._y, _z / rhs._z, _w / rhs._w);
}

const Vector4CPU& Vector4CPU::operator/=(float rhs)
{
	_x /= rhs;
	_y /= rhs;
	_z /= rhs;
	_w /= rhs;
	return *this;
}

Vector4CPU Vector4CPU::operator/(float rhs) const
{
	return Vector4CPU(_x / rhs, _y / rhs, _z / rhs, _w / rhs);
}

const float* Vector4CPU::getBuffer(void) const
{
	return _vec;
}

//float* Vector4CPU::getBuffer(void)
//{
//	return _vec;
//}

float Vector4CPU::lengthSquared(void) const
{
	return _x*_x + _y*_y + _z*_z + _w*_w;
}

float Vector4CPU::length(void) const
{
	return sqrtf(_x*_x + _y*_y + _z*_z + _w*_w);
}

float Vector4CPU::reciprocalLengthSquared(void) const
{
	assert(length() != 0.0f);
	return 1.0f / (_x*_x + _y*_y + _z*_z + _w*_w);
}

float Vector4CPU::reciprocalLength(void) const
{
	assert(length() != 0.0f);
	return 1.0f / sqrtf(_x*_x + _y*_y + _z*_z + _w*_w);
}

Vector4CPU Vector4CPU::normalize(void) const
{
	assert(length() != 0.0f);
	float inv_length = 1.0f / sqrtf(_x*_x + _y*_y + _z*_z + _w*_w);

	return Vector4CPU(
		_x * inv_length,
		_y * inv_length,
		_z * inv_length,
		_w * inv_length
	);
}

Vector4CPU Vector4CPU::cross(const Vector4CPU& rhs) const
{
	return Vector4CPU(
		(_y * rhs._z) - (_z * rhs._y),
		(_z * rhs._x) - (_x * rhs._z),
		(_x * rhs._y) - (_y * rhs._x),
		0.0f
	);
}

float Vector4CPU::dot(const Vector4CPU& rhs) const
{
	return _x*rhs._x + _y*rhs._y + _z*rhs._z + _w*rhs._w;
}

float Vector4CPU::angleUnit(const Vector4CPU& rhs) const
{
	return acosf(dot(rhs));
}

float Vector4CPU::angle(const Vector4CPU& rhs) const
{
	return acosf(dot(rhs) / (length() * rhs.length()));
}

Vector4CPU Vector4CPU::reflect(const Vector4CPU& normal) const
{
	float s = 2.0f * dot(normal);
	return Vector4CPU(_x - s * normal._x, _y - s * normal._y, _z - s * normal._z, _w - s * normal._w);
}

Vector4CPU Vector4CPU::refract(const Vector4CPU& normal, float refraction_index) const
{
	float t = dot(normal);
	float r = 1.0f - refraction_index * refraction_index * (1.0f - t * t);
	Vector4CPU result = Zero;

	if (r >= 0.0f) {
		float s = refraction_index * t + sqrtf(r);

		result.set(
			refraction_index * _x - s * normal._x,
			refraction_index * _y - s * normal._y,
			refraction_index * _z - s * normal._z,
			refraction_index * _w - s * normal._w
		);
	}

	return result;
}

Vector4CPU Vector4CPU::lerp(const Vector4CPU& end, float t) const
{
	return *this + t * (end - *this);
}

Vector4CPU Vector4CPU::minimum(const Vector4CPU& rhs) const
{
	return Vector4CPU(
		Gaff::Min(_x, rhs._x),
		Gaff::Min(_y, rhs._y),
		Gaff::Min(_z, rhs._z),
		Gaff::Min(_w, rhs._w)
	);
}

Vector4CPU Vector4CPU::maximum(const Vector4CPU& rhs) const
{
	return Vector4CPU(
		Gaff::Max(_x, rhs._x),
		Gaff::Max(_y, rhs._y),
		Gaff::Max(_z, rhs._z),
		Gaff::Max(_w, rhs._w)
	);
}

void Vector4CPU::reflectThis(const Vector4CPU& normal)
{
	float s = 2.0f * dot(normal);
	_x -= s * normal._x;
	_y -= s * normal._y;
	_z -= s * normal._z;
	_w -= s * normal._w;
}

void Vector4CPU::refractThis(const Vector4CPU& normal, float refraction_index)
{
	float t = dot(normal);
	float r = 1.0f - refraction_index * refraction_index * (1.0f - t * t);

	if (r >= 0.0f) {
		float s = refraction_index * t + sqrtf(r);

		_x = refraction_index * _x - s * normal._x;
		_y = refraction_index * _y - s * normal._y;
		_z = refraction_index * _z - s * normal._z;
		_w = refraction_index * _w - s * normal._w;
	}
}

void Vector4CPU::lerpThis(const Vector4CPU& end, float t)
{
	*this += t * (end - *this);
}

void Vector4CPU::minimumThis(const Vector4CPU& rhs)
{
	_x = Gaff::Min(_x, rhs._x);
	_y = Gaff::Min(_y, rhs._y);
	_z = Gaff::Min(_z, rhs._z);
	_w = Gaff::Min(_w, rhs._w);
}

void Vector4CPU::maximumThis(const Vector4CPU& rhs)
{
	_x = Gaff::Max(_x, rhs._x);
	_y = Gaff::Max(_y, rhs._y);
	_z = Gaff::Max(_z, rhs._z);
	_w = Gaff::Max(_w, rhs._w);
}

void Vector4CPU::crossThis(const Vector4CPU& rhs)
{
	*this = cross(rhs);
}

void Vector4CPU::normalizeThis(void)
{
	assert(length() != 0.0f);
	float inv_length = 1.0f / sqrtf(_x*_x + _y*_y + _z*_z + _w*_w);
	_x *= inv_length;
	_y *= inv_length;
	_z *= inv_length;
	_w *= inv_length;
}

bool Vector4CPU::roughlyEqual(const Vector4CPU& rhs, float epsilon) const
{
	return fabsf(_x - rhs._x) <= epsilon &&
			fabsf(_y - rhs._y) <= epsilon &&
			fabsf(_z - rhs._z) <= epsilon &&
			fabsf(_w - rhs._w) <= epsilon;
}

Vector4CPU Vector4CPU::lengthSquaredVec(void) const
{
	return Vector4CPU(lengthSquared());
}

Vector4CPU Vector4CPU::lengthVec(void) const
{
	return Vector4CPU(length());
}

Vector4CPU Vector4CPU::reciprocalLengthSquaredVec(void) const
{
	return Vector4CPU(reciprocalLengthSquared());
}

Vector4CPU Vector4CPU::reciprocalLengthVec(void) const
{
	return Vector4CPU(reciprocalLength());
}

Vector4CPU Vector4CPU::dotVec(const Vector4CPU& rhs) const
{
	return Vector4CPU(dot(rhs));
}

Vector4CPU Vector4CPU::angleUnitVec(const Vector4CPU& rhs) const
{
	return Vector4CPU(angleUnit(rhs));
}

Vector4CPU Vector4CPU::angleVec(const Vector4CPU& rhs) const
{
	return Vector4CPU(angle(rhs));
}

Vector4CPU Vector4CPU::refract(const Vector4CPU& normal, const Vector4CPU& refraction_index) const
{
	return refract(normal, refraction_index[0]);
}

Vector4CPU Vector4CPU::lerp(const Vector4CPU& end, const Vector4CPU& t) const
{
	return *this + t * (end - *this);
}

bool Vector4CPU::roughlyEqual(const Vector4CPU& rhs, const Vector4CPU& epsilon) const
{
	return fabsf(_x - rhs._x) <= epsilon[0] &&
			fabsf(_y - rhs._y) <= epsilon[1] &&
			fabsf(_z - rhs._z) <= epsilon[2] &&
			fabsf(_w - rhs._w) <= epsilon[3];
}

void Vector4CPU::refractThis(const Vector4CPU& normal, const Vector4CPU& refraction_index)
{
	refractThis(normal, refraction_index[0]);
}

void Vector4CPU::lerpThis(const Vector4CPU& end, const Vector4CPU& t)
{
	*this += t * (end - *this);
}

Vector4CPU operator*(float lhs, const Vector4CPU& rhs)
{
	return rhs * lhs;
}

NS_END
