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
#include <cstring>
#include <cfloat>
#include <cmath>

NS_GLEAM

Matrix4x4SIMD Matrix4x4SIMD::Identity(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);


Matrix4x4SIMD::Matrix4x4SIMD(void)
{
}

Matrix4x4SIMD::Matrix4x4SIMD(const Matrix4x4SIMD& matrix):
	_matrix(matrix._matrix)
{
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
	_matrix.elements[0] = vec1.getSIMDType();
	_matrix.elements[1] = vec2.getSIMDType();
	_matrix.elements[2] = vec3.getSIMDType();
	_matrix.elements[3] = vec4.getSIMDType();
}

Matrix4x4SIMD::Matrix4x4SIMD(const SIMDMatrix& matrix):
	_matrix(matrix)
{
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
	return SIMDEqual(_matrix.elements[0], rhs._matrix.elements[0]) &&
			SIMDEqual(_matrix.elements[1], rhs._matrix.elements[0]) &&
			SIMDEqual(_matrix.elements[2], rhs._matrix.elements[0]) &&
			SIMDEqual(_matrix.elements[3], rhs._matrix.elements[0]);
}

bool Matrix4x4SIMD::operator!=(const Matrix4x4SIMD& rhs) const
{
	return !(*this == rhs);
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator=(const Matrix4x4SIMD& rhs)
{
	_matrix = rhs._matrix;
	return *this;
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator*=(const Matrix4x4SIMD& rhs)
{
	_matrix = SIMDMatrixMul(_matrix, rhs._matrix);
	return *this;
}

Matrix4x4SIMD Matrix4x4SIMD::operator*(const Matrix4x4SIMD& rhs) const
{
	return Matrix4x4SIMD(SIMDMatrixMul(_matrix, rhs._matrix));
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator+=(const Matrix4x4SIMD& rhs)
{
	_matrix.elements[0] = SIMDAdd(_matrix.elements[0], rhs._matrix.elements[0]);
	_matrix.elements[1] = SIMDAdd(_matrix.elements[1], rhs._matrix.elements[1]);
	_matrix.elements[2] = SIMDAdd(_matrix.elements[2], rhs._matrix.elements[2]);
	_matrix.elements[3] = SIMDAdd(_matrix.elements[3], rhs._matrix.elements[3]);
	return *this;
}

Matrix4x4SIMD Matrix4x4SIMD::operator+(const Matrix4x4SIMD& rhs) const
{
	SIMDMatrix matrix = {
		SIMDAdd(_matrix.elements[0], rhs._matrix.elements[0]),
		SIMDAdd(_matrix.elements[1], rhs._matrix.elements[1]),
		SIMDAdd(_matrix.elements[2], rhs._matrix.elements[2]),
		SIMDAdd(_matrix.elements[3], rhs._matrix.elements[3])
	};

	return Matrix4x4SIMD(matrix);
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator-=(const Matrix4x4SIMD& rhs)
{
	_matrix.elements[0] = SIMDSub(_matrix.elements[0], rhs._matrix.elements[0]);
	_matrix.elements[1] = SIMDSub(_matrix.elements[1], rhs._matrix.elements[1]);
	_matrix.elements[2] = SIMDSub(_matrix.elements[2], rhs._matrix.elements[2]);
	_matrix.elements[3] = SIMDSub(_matrix.elements[3], rhs._matrix.elements[3]);
	return *this;
}

Matrix4x4SIMD Matrix4x4SIMD::operator-(const Matrix4x4SIMD& rhs) const
{
	SIMDMatrix matrix = {
		SIMDSub(_matrix.elements[0], rhs._matrix.elements[0]),
		SIMDSub(_matrix.elements[1], rhs._matrix.elements[1]),
		SIMDSub(_matrix.elements[2], rhs._matrix.elements[2]),
		SIMDSub(_matrix.elements[3], rhs._matrix.elements[3])
	};

	return Matrix4x4SIMD(matrix);
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator/=(float rhs)
{
	assert(rhs != 0);
	SIMDType denom = SIMDCreate(rhs);
	_matrix.elements[0] = SIMDDiv(_matrix.elements[0], denom);
	_matrix.elements[1] = SIMDDiv(_matrix.elements[1], denom);
	_matrix.elements[2] = SIMDDiv(_matrix.elements[2], denom);
	_matrix.elements[3] = SIMDDiv(_matrix.elements[3], denom);
	return *this;
}

Matrix4x4SIMD Matrix4x4SIMD::operator/(float rhs) const
{
	assert(rhs != 0);
	SIMDType denom = SIMDCreate(rhs);

	SIMDMatrix matrix = {
		SIMDDiv(_matrix.elements[0], denom),
		SIMDDiv(_matrix.elements[1], denom),
		SIMDDiv(_matrix.elements[2], denom),
		SIMDDiv(_matrix.elements[3], denom)
	};

	return Matrix4x4SIMD(matrix);
}

const Matrix4x4SIMD& Matrix4x4SIMD::operator*=(float rhs)
{
	assert(rhs != 0);
	SIMDType scalar = SIMDCreate(rhs);
	_matrix.elements[0] = SIMDMul(_matrix.elements[0], scalar);
	_matrix.elements[1] = SIMDMul(_matrix.elements[1], scalar);
	_matrix.elements[2] = SIMDMul(_matrix.elements[2], scalar);
	_matrix.elements[3] = SIMDMul(_matrix.elements[3], scalar);
	return *this;
}

Matrix4x4SIMD Matrix4x4SIMD::operator*(float rhs) const
{
	assert(rhs != 0);
	SIMDType scalar = SIMDCreate(rhs);

	SIMDMatrix matrix = {
		SIMDDiv(_matrix.elements[0], scalar),
		SIMDDiv(_matrix.elements[1], scalar),
		SIMDDiv(_matrix.elements[2], scalar),
		SIMDDiv(_matrix.elements[3], scalar)
	};

	return Matrix4x4SIMD(matrix);
}

Vector4SIMD Matrix4x4SIMD::operator*(const Vector4SIMD& rhs) const
{
	return Vector4SIMD(SIMDMatrixMulRow(
		rhs.getSIMDType(),
		_matrix.elements[0], _matrix.elements[1],
		_matrix.elements[2], _matrix.elements[3])
	);
}

const float* Matrix4x4SIMD::operator[](int index) const
{
	assert(index > -1 && index < 4);
	SIMDStore(_matrix.elements[index], _get_buffer_cache[index]);
	return _get_buffer_cache[index];
}

//float* Matrix4x4SIMD::operator[](int index)
//{
//	assert(index > -1 && index < 4);
//	return _m[index];
//}

const float* Matrix4x4SIMD::getBuffer(void) const
{
	SIMDStore(_matrix.elements[0], _get_buffer_cache[0]);
	SIMDStore(_matrix.elements[1], _get_buffer_cache[1]);
	SIMDStore(_matrix.elements[2], _get_buffer_cache[2]);
	SIMDStore(_matrix.elements[3], _get_buffer_cache[3]);
	return _get_buffer_cache[0];
}

const SIMDMatrix& Matrix4x4SIMD::getSIMDType(void) const
{
	return _matrix;
}

float Matrix4x4SIMD::at(int column, int row) const
{
	assert(column > -1 && column < 4);
	assert(row >-1 && row < 4);
	return SIMDGet(_matrix.elements[column], (unsigned int)row);
}

//float& Matrix4x4SIMD::at(int column, int row)
//{
//	assert(column > -1 && column < 4);
//	assert(row >-1 && row < 4);
//
//	return _m[column][row];
//}

void Matrix4x4SIMD::set(float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	_matrix.elements[0] = SIMDCreate(m00, m01, m02, m03);
	_matrix.elements[1] = SIMDCreate(m10, m11, m12, m13);
	_matrix.elements[2] = SIMDCreate(m20, m21, m22, m23);
	_matrix.elements[3] = SIMDCreate(m30, m31, m32, m33);
}

void Matrix4x4SIMD::set(const float* elements)
{
#ifdef SIMD_SET_ALIGNED
	_matrix.elements[0] = SIMDLoadAligned(elements);
	_matrix.elements[1] = SIMDLoadAligned(elements + 4);
	_matrix.elements[2] = SIMDLoadAligned(elements + 8);
	_matrix.elements[3] = SIMDLoadAligned(elements + 12);
#else
	_matrix.elements[0] = SIMDLoad(elements);
	_matrix.elements[1] = SIMDLoad(elements + 4);
	_matrix.elements[2] = SIMDLoad(elements + 8);
	_matrix.elements[3] = SIMDLoad(elements + 12);
#endif
}

void Matrix4x4SIMD::set(int column, int row, float value)
{
	assert(column > -1 && column < 4);
	assert(row >-1 && row < 4);
	SIMDSet(_matrix.elements[column], value, (unsigned int)row);
}

Vector4SIMD Matrix4x4SIMD::getColumn(int column) const
{
	assert(column > -1 && column < 4);
	return Vector4SIMD(_matrix.elements[column]);
}

bool Matrix4x4SIMD::isIdentity(void) const
{
	return SIMDEqual(_matrix.elements[0], gXAxis) &&
			SIMDEqual(_matrix.elements[1], gYAxis) &&
			SIMDEqual(_matrix.elements[2], gZAxis) &&
			SIMDEqual(_matrix.elements[3], gWAxis);
}

void Matrix4x4SIMD::setIdentity(void)
{
	_matrix.elements[0] = gXAxis;
	_matrix.elements[1] = gYAxis;
	_matrix.elements[2] = gZAxis;
	_matrix.elements[3] = gWAxis;
}

bool Matrix4x4SIMD::hasInfiniteElement(void) const
{
	return SIMDMatrixHasInfiniteElement(_matrix);
}

bool Matrix4x4SIMD::hasNaNElement(void) const
{
	return SIMDMatrixHasNaNElement(_matrix);
}

void Matrix4x4SIMD::transposeThis(void)
{
	_matrix = SIMDMatrixTranspose(_matrix);
}

bool Matrix4x4SIMD::inverseThis(void)
{
	_matrix = SIMDMatrixInverse(_matrix);
	return !isIdentity();
}

Matrix4x4SIMD Matrix4x4SIMD::transpose(void) const
{
	return Matrix4x4SIMD(SIMDMatrixTranspose(_matrix));
}

bool Matrix4x4SIMD::inverse(Matrix4x4SIMD& out) const
{
	out._matrix = SIMDMatrixInverse(_matrix);
	return !out.isIdentity();
}

void Matrix4x4SIMD::setTranslate(const Vector4SIMD& translate)
{
	_matrix.elements[3] = translate.getSIMDType();

}

void Matrix4x4SIMD::setTranslate(float x, float y, float z)
{
	_matrix.elements[3] = SIMDCreate(x, y, z, 1.0f);
}

void Matrix4x4SIMD::setScale(float x, float y, float z)
{
	SIMDSet(_matrix.elements[0], x, 0);
	SIMDSet(_matrix.elements[1], y, 1);
	SIMDSet(_matrix.elements[2], z, 2);
}

void Matrix4x4SIMD::setScale(const Vector4SIMD& scale)
{
	SIMDSet(_matrix.elements[0], scale[0], 0);
	SIMDSet(_matrix.elements[1], scale[1], 1);
	SIMDSet(_matrix.elements[2], scale[2], 2);
}

void Matrix4x4SIMD::setScale(float scale)
{
	SIMDSet(_matrix.elements[0], scale, 0);
	SIMDSet(_matrix.elements[1], scale, 1);
	SIMDSet(_matrix.elements[2], scale, 2);
}

void Matrix4x4SIMD::setRotation(float radians, const Vector4SIMD& axis)
{
	setRotation(radians, axis[0], axis[1], axis[2]);
}

void Matrix4x4SIMD::setRotation(float radians, float x, float y, float z)
{
	assert(x != 0.0f && y != 0.0f && z != 0.0f);
	float inv_length = 1.0f / sqrtf(x*x + y*y + z*z);
	x *= inv_length;
	y *= inv_length;
	z *= inv_length;

	float c = cosf(radians);
	float c1 = 1.0f - c;
	float s = sinf(radians);

	getBuffer(); // To update the buffer cache

	set(
		x*x*c1 + c, y*x*c1 + z*s, z*x*c1 - y*s, _get_buffer_cache_single[3],
		x*y*c1 - z*s, y*y*c1 + c, y*z*c1 + x*s, _get_buffer_cache_single[7],
		x*z*c1 + y*s, y*z*c1 - x*s, z*z*c1 + c, _get_buffer_cache_single[11],
		_get_buffer_cache_single[12], _get_buffer_cache_single[13], _get_buffer_cache_single[14], _get_buffer_cache_single[15]
	);
}

void Matrix4x4SIMD::setRotationX(float radians)
{
	float cf = cosf(radians);
	float sf = sinf(radians);

	getBuffer(); // To update the buffer cache

	set(
		_get_buffer_cache_single[0], _get_buffer_cache_single[1], _get_buffer_cache_single[2], _get_buffer_cache_single[3],
		_get_buffer_cache_single[4], cf, sf, _get_buffer_cache_single[7],
		_get_buffer_cache_single[8], -sf, cf, _get_buffer_cache_single[11],
		_get_buffer_cache_single[12], _get_buffer_cache_single[13], _get_buffer_cache_single[14], _get_buffer_cache_single[15]
	);
}

void Matrix4x4SIMD::setRotationY(float radians)
{
	float cf = cosf(radians);
	float sf = sinf(radians);

	getBuffer(); // To update the buffer cache

	set(
		cf, _get_buffer_cache_single[1], -sf, _get_buffer_cache_single[3],
		_get_buffer_cache_single[4], _get_buffer_cache_single[5], _get_buffer_cache_single[6], _get_buffer_cache_single[7],
		sf, _get_buffer_cache_single[9], cf, _get_buffer_cache_single[11],
		_get_buffer_cache_single[12], _get_buffer_cache_single[13], _get_buffer_cache_single[14], _get_buffer_cache_single[15]
	);
}

void Matrix4x4SIMD::setRotationZ(float radians)
{
	float cf = cosf(radians);
	float sf = sinf(radians);

	getBuffer(); // To update the buffer cache

	set(
		cf, sf, _get_buffer_cache_single[2], _get_buffer_cache_single[3],
		-sf, cf, _get_buffer_cache_single[6], _get_buffer_cache_single[7],
		_get_buffer_cache_single[8], _get_buffer_cache_single[9], _get_buffer_cache_single[10], _get_buffer_cache_single[11],
		_get_buffer_cache_single[12], _get_buffer_cache_single[13], _get_buffer_cache_single[14], _get_buffer_cache_single[15]
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
	setLookAtLH(
		Vector4SIMD(eye_x, eye_y, eye_z, 1.0f),
		Vector4SIMD(target_x, target_y, target_z, 1.0f),
		Vector4SIMD(up_x, up_y, up_z, 0.0f)
	);
}

void Matrix4x4SIMD::setLookAtRH(const Vector4SIMD& eye, const Vector4SIMD& target, const Vector4SIMD& up)
{
	setLookToRH(eye, target - eye, up);
}

void Matrix4x4SIMD::setLookAtRH(float eye_x, float eye_y, float eye_z,
	float target_x, float target_y, float target_z,
	float up_x, float up_y, float up_z)
{
	setLookAtRH(
		Vector4SIMD(eye_x, eye_y, eye_z, 1.0f),
		Vector4SIMD(target_x, target_y, target_z, 1.0f),
		Vector4SIMD(up_x, up_y, up_z, 0.0f)
	);
}

void Matrix4x4SIMD::setLookToLH(const Vector4SIMD& eye, const Vector4SIMD& dir, const Vector4SIMD& up)
{
	Vector4SIMD realDir, right, realUp;

	realDir = dir;
	realDir.normalizeThis();

	right = up.cross(dir);
	right.normalizeThis();

	realUp = dir.cross(right);
	realUp.normalizeThis();

	set(
		right[0], realUp[0], realDir[0], 0.0f,
		right[1], realUp[1], realDir[1], 0.0f,
		right[2], realUp[2], realDir[2], 0.0f,
		-right.dot(eye), -realUp.dot(eye), -realDir.dot(eye), 1.0f
	);
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

	_matrix.elements[0] = SIMDCreate(recip_width + recip_width, 0.0f, 0.0f, 0.0f);
	_matrix.elements[1] = SIMDCreate(0.0f, recip_height + recip_height, 0.0f, 0.0f);
	_matrix.elements[2] = SIMDCreate(0.0f, 0.0f, range, 0.0f);
	_matrix.elements[3] = SIMDCreate(-(left + right) * recip_width, -(top + bottom) * recip_height, z_near * -range, 1.0f);
}

void Matrix4x4SIMD::setOrthographicLH(float width, float height, float z_near, float z_far)
{
	float range = 1.0f / (z_far - z_near);

	_matrix.elements[0] = SIMDCreate(2.0f / width, 0.0f, 0.0f, 0.0f);
	_matrix.elements[1] = SIMDCreate(0.0f, 2.0f / height, 0.0f, 0.0f);
	_matrix.elements[2] = SIMDCreate(0.0f, 0.0f, range, 0.0f);
	_matrix.elements[3] = SIMDCreate(0.0f, 0.0f, z_near * -range, 1.0f);
}

void Matrix4x4SIMD::setOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far)
{
	float recip_width = 1.0f / (right - left);
	float recip_height = 1.0f / (top - bottom);
	float range = 1.0f / (z_near - z_far);

	_matrix.elements[0] = SIMDCreate(recip_width + recip_width, 0.0f, 0.0f, 0.0f);
	_matrix.elements[1] = SIMDCreate(0.0f, recip_height + recip_height, 0.0f, 0.0f);
	_matrix.elements[2] = SIMDCreate(0.0f, 0.0f, range, 0.0f);
	_matrix.elements[3] = SIMDCreate(-(left + right) * recip_width, -(top + bottom) * recip_height, z_near * range, 1.0f);
}

void Matrix4x4SIMD::setOrthographicRH(float width, float height, float z_near, float z_far)
{
	float range = 1.0f / (z_near - z_far);

	_matrix.elements[0] = SIMDCreate(2.0f / width, 0.0f, 0.0f, 0.0f);
	_matrix.elements[1] = SIMDCreate(0.0f, 2.0f / height, 0.0f, 0.0f);
	_matrix.elements[2] = SIMDCreate(0.0f, 0.0f, range, 0.0f);
	_matrix.elements[3] = SIMDCreate(0.0f, 0.0f, z_near * range, 1.0f);
}

void Matrix4x4SIMD::setPerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far)
{
	fov *= 0.5f;
	float cos_fov = cosf(fov);
	float sin_fov = sinf(fov);
	float height = cos_fov / sin_fov;
	float width = height / aspect_ratio;
	float range = z_far / (z_far - z_near);

	_matrix.elements[0] = SIMDCreate(width, 0.0f, 0.0f, 0.0f);
	_matrix.elements[1] = SIMDCreate(0.0f, height, 0.0f, 0.0f);
	_matrix.elements[2] = SIMDCreate(0.0f, 0.0f, range, 1.0f);
	_matrix.elements[3] = SIMDCreate(0.0f, 0.0f, z_near * -range, 0.0f);
}

void Matrix4x4SIMD::setPerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far)
{
	fov *= 0.5f;
	float cos_fov = cosf(fov);
	float sin_fov = sinf(fov);
	float height = cos_fov / sin_fov;
	float width = height / aspect_ratio;
	float range = z_far / (z_near - z_far);

	_matrix.elements[0] = SIMDCreate(width, 0.0f, 0.0f, 0.0f);
	_matrix.elements[1] = SIMDCreate(0.0f, height, 0.0f, 0.0f);
	_matrix.elements[2] = SIMDCreate(0.0f, 0.0f, range, -1.0f);
	_matrix.elements[3] = SIMDCreate(0.0f, 0.0f, z_near * range, 0.0f);
}

bool Matrix4x4SIMD::roughlyEqual(const Matrix4x4SIMD& rhs, const Vector4SIMD& epsilon) const
{
	return SIMDRoughlyEqual(_matrix.elements[0], rhs._matrix.elements[0], epsilon.getSIMDType()) &&
			SIMDRoughlyEqual(_matrix.elements[1], rhs._matrix.elements[1], epsilon.getSIMDType()) &&
			SIMDRoughlyEqual(_matrix.elements[2], rhs._matrix.elements[2], epsilon.getSIMDType()) &&
			SIMDRoughlyEqual(_matrix.elements[3], rhs._matrix.elements[3], epsilon.getSIMDType());
}

bool Matrix4x4SIMD::roughlyEqual(const Matrix4x4SIMD& rhs, float epsilon) const
{
	SIMDType eps = SIMDCreate(epsilon);
	return SIMDRoughlyEqual(_matrix.elements[0], rhs._matrix.elements[0], eps) &&
			SIMDRoughlyEqual(_matrix.elements[1], rhs._matrix.elements[1], eps) &&
			SIMDRoughlyEqual(_matrix.elements[2], rhs._matrix.elements[2], eps) &&
			SIMDRoughlyEqual(_matrix.elements[3], rhs._matrix.elements[3], eps);
}

void Matrix4x4SIMD::setRotation(const Vector4SIMD& radians, const Vector4SIMD& axis)
{
	setRotation(radians[0], axis);
}

void Matrix4x4SIMD::setRotationX(const Vector4SIMD& radians)
{
	setRotationX(radians[0]);
}

void Matrix4x4SIMD::setRotationY(const Vector4SIMD& radians)
{
	setRotationY(radians[0]);
}

void Matrix4x4SIMD::setRotationZ(const Vector4SIMD& radians)
{
	setRotationZ(radians[0]);
}

Matrix4x4SIMD Matrix4x4SIMD::MakeTranslate(const Vector4SIMD& translate)
{
	Matrix4x4SIMD temp = Identity;
	temp.setTranslate(translate);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeTranslate(float x, float y, float z)
{
	Matrix4x4SIMD temp = Identity;
	temp.setTranslate(x, y, z);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeScale(const Vector4SIMD& scale)
{
	Matrix4x4SIMD temp = Identity;
	temp.setScale(scale);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeScale(float x, float y, float z)
{
	Matrix4x4SIMD temp = Identity;
	temp.setScale(x, y, z);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeScale(float scale)
{
	Matrix4x4SIMD temp = Identity;
	temp.setScale(scale);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeRotation(float radians, float x, float y, float z)
{
	Matrix4x4SIMD temp = Identity;
	temp.setRotation(radians, x, y, z);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeRotationX(float radians)
{
	Matrix4x4SIMD temp = Identity;
	temp.setRotationX(radians);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeRotationY(float radians)
{
	Matrix4x4SIMD temp = Identity;
	temp.setRotationY(radians);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeRotationZ(float radians)
{
	Matrix4x4SIMD temp = Identity;
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

Matrix4x4SIMD Matrix4x4SIMD::MakeOrthographicLH(float left, float right, float bottom, float top, float z_near, float z_far)
{
	Matrix4x4SIMD temp;
	temp.setOrthographicLH(left, right, bottom, top, z_near, z_far);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeOrthographicLH(float width, float height, float z_near, float z_far)
{
	Matrix4x4SIMD temp;
	temp.setOrthographicLH(width, height, z_near, z_far);
	return temp;
}

Matrix4x4SIMD Matrix4x4SIMD::MakeOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far)
{
	Matrix4x4SIMD temp;
	temp.setOrthographicRH(left, right, bottom, top, z_near, z_far);
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
