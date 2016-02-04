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

#include "Gleam_Matrix4x4_CPU.h"
#include "Gleam_Vector4_CPU.h"
#include <Gaff_IncludeAssert.h>
#include <cstring>
#include <cfloat>
#include <cmath>

NS_GLEAM

Matrix4x4CPU Matrix4x4CPU::Identity(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);


Matrix4x4CPU::Matrix4x4CPU(void)
{
}

Matrix4x4CPU::Matrix4x4CPU(const Matrix4x4CPU& matrix)
{
	set(matrix._a);
}

Matrix4x4CPU::Matrix4x4CPU(float m00, float m01, float m02, float m03,
						float m10, float m11, float m12, float m13,
						float m20, float m21, float m22, float m23,
						float m30, float m31, float m32, float m33)
{
	set(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);
}

Matrix4x4CPU::Matrix4x4CPU(const Vector4CPU& vec1, const Vector4CPU& vec2,
							const Vector4CPU& vec3, const Vector4CPU& vec4)
{
	set(
		vec1[0], vec1[1], vec1[2], vec1[3],
		vec2[0], vec2[1], vec2[2], vec2[3],
		vec3[0], vec3[1], vec3[2], vec3[3],
		vec4[0], vec4[1], vec4[2], vec4[3]
	);
}

Matrix4x4CPU::Matrix4x4CPU(const float* elements)
{
	set(elements);
}

Matrix4x4CPU::~Matrix4x4CPU(void)
{
}

bool Matrix4x4CPU::operator==(const Matrix4x4CPU& rhs) const
{
	return _a[0] == rhs._a[0] && _a[1] == rhs._a[1] && _a[2] == rhs._a[2] && _a[3] == rhs._a[3] &&
			_a[4] == rhs._a[4] && _a[5] == rhs._a[5] && _a[6] == rhs._a[6] && _a[7] == rhs._a[7] &&
			_a[8] == rhs._a[8] && _a[9] == rhs._a[9] && _a[10] == rhs._a[10] && _a[11] == rhs._a[11] &&
			_a[12] == rhs._a[12] && _a[13] == rhs._a[13] && _a[14] == rhs._a[14] && _a[15] == rhs._a[15];
}

bool Matrix4x4CPU::operator!=(const Matrix4x4CPU& rhs) const
{
	return !(*this == rhs);
}

const Matrix4x4CPU& Matrix4x4CPU::operator=(const Matrix4x4CPU& rhs)
{
	set(rhs._a);
	return *this;
}

const Matrix4x4CPU& Matrix4x4CPU::operator*=(const Matrix4x4CPU& rhs)
{
	float m00 = _a[0]*rhs._a[0] + _a[4]*rhs._a[1] + _a[8]*rhs._a[2] + _a[12]*rhs._a[3];
	float m01 = _a[1]*rhs._a[0] + _a[5]*rhs._a[1] + _a[9]*rhs._a[2] + _a[13]*rhs._a[3];
	float m02 = _a[2]*rhs._a[0] + _a[6]*rhs._a[1] + _a[10]*rhs._a[2] + _a[14]*rhs._a[3];
	float m03 = _a[3]*rhs._a[0] + _a[7]*rhs._a[1] + _a[11]*rhs._a[2] + _a[15]*rhs._a[3];

	float m10 = _a[0]*rhs._a[4] + _a[4]*rhs._a[5] + _a[8]*rhs._a[6] + _a[12]*rhs._a[7];
	float m11 = _a[1]*rhs._a[4] + _a[5]*rhs._a[5] + _a[9]*rhs._a[6] + _a[13]*rhs._a[7];
	float m12 = _a[2]*rhs._a[4] + _a[6]*rhs._a[5] + _a[10]*rhs._a[6] + _a[14]*rhs._a[7];
	float m13 = _a[3]*rhs._a[4] + _a[7]*rhs._a[5] + _a[11]*rhs._a[6] + _a[15]*rhs._a[7];

	float m20 = _a[0]*rhs._a[8] + _a[4]*rhs._a[9] + _a[8]*rhs._a[10] + _a[12]*rhs._a[11];
	float m21 = _a[1]*rhs._a[8] + _a[5]*rhs._a[9] + _a[9]*rhs._a[10] + _a[13]*rhs._a[11];
	float m22 = _a[2]*rhs._a[8] + _a[6]*rhs._a[9] + _a[10]*rhs._a[10] + _a[14]*rhs._a[11];
	float m23 = _a[3]*rhs._a[8] + _a[7]*rhs._a[9] + _a[11]*rhs._a[10] + _a[15]*rhs._a[11];

	float m30 = _a[0]*rhs._a[12] + _a[4]*rhs._a[13] + _a[8]*rhs._a[14] + _a[12]*rhs._a[15];
	float m31 = _a[1]*rhs._a[12] + _a[5]*rhs._a[13] + _a[9]*rhs._a[14] + _a[13]*rhs._a[15];
	float m32 = _a[2]*rhs._a[12] + _a[6]*rhs._a[13] + _a[10]*rhs._a[14] + _a[14]*rhs._a[15];
	float m33 = _a[3]*rhs._a[12] + _a[7]*rhs._a[13] + _a[11]*rhs._a[14] + _a[15]*rhs._a[15];

	set(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);

	return *this;
}

Matrix4x4CPU Matrix4x4CPU::operator*(const Matrix4x4CPU& rhs) const
{
	Matrix4x4CPU temp(*this);
	temp *= rhs;
	return temp;
}

const Matrix4x4CPU& Matrix4x4CPU::operator+=(const Matrix4x4CPU& rhs)
{
	_a[0] += rhs._a[0];
	_a[1] += rhs._a[1];
	_a[2] += rhs._a[2];
	_a[3] += rhs._a[3];
	_a[4] += rhs._a[4];
	_a[5] += rhs._a[5];
	_a[6] += rhs._a[6];
	_a[7] += rhs._a[7];
	_a[8] += rhs._a[8];
	_a[9] += rhs._a[9];
	_a[10] += rhs._a[10];
	_a[11] += rhs._a[11];
	_a[12] += rhs._a[12];
	_a[13] += rhs._a[13];
	_a[14] += rhs._a[14];
	_a[15] += rhs._a[15];
	return *this;
}

Matrix4x4CPU Matrix4x4CPU::operator+(const Matrix4x4CPU& rhs) const
{
	Matrix4x4CPU temp(*this);
	temp += rhs;
	return temp;
}

const Matrix4x4CPU& Matrix4x4CPU::operator-=(const Matrix4x4CPU& rhs)
{
	_a[0] -= rhs._a[0];
	_a[1] -= rhs._a[1];
	_a[2] -= rhs._a[2];
	_a[3] -= rhs._a[3];
	_a[4] -= rhs._a[4];
	_a[5] -= rhs._a[5];
	_a[6] -= rhs._a[6];
	_a[7] -= rhs._a[7];
	_a[8] -= rhs._a[8];
	_a[9] -= rhs._a[9];
	_a[10] -= rhs._a[10];
	_a[11] -= rhs._a[11];
	_a[12] -= rhs._a[12];
	_a[13] -= rhs._a[13];
	_a[14] -= rhs._a[14];
	_a[15] -= rhs._a[15];
	return *this;
}

Matrix4x4CPU Matrix4x4CPU::operator-(const Matrix4x4CPU& rhs) const
{
	Matrix4x4CPU temp(*this);
	temp -= rhs;
	return temp;
}

const Matrix4x4CPU& Matrix4x4CPU::operator/=(float rhs)
{
	assert(rhs != 0);
	_a[0] /= rhs;
	_a[1] /= rhs;
	_a[2] /= rhs;
	_a[3] /= rhs;
	_a[4] /= rhs;
	_a[5] /= rhs;
	_a[6] /= rhs;
	_a[7] /= rhs;
	_a[8] /= rhs;
	_a[9] /= rhs;
	_a[10] /= rhs;
	_a[11] /= rhs;
	_a[12] /= rhs;
	_a[13] /= rhs;
	_a[14] /= rhs;
	_a[15] /= rhs;
	return *this;
}

Matrix4x4CPU Matrix4x4CPU::operator/(float rhs) const
{
	assert(rhs != 0);
	Matrix4x4CPU temp(*this);
	temp /= rhs;
	return temp;
}

const Matrix4x4CPU& Matrix4x4CPU::operator*=(float rhs)
{
	_a[0] *= rhs;
	_a[1] *= rhs;
	_a[2] *= rhs;
	_a[3] *= rhs;
	_a[4] *= rhs;
	_a[5] *= rhs;
	_a[6] *= rhs;
	_a[7] *= rhs;
	_a[8] *= rhs;
	_a[9] *= rhs;
	_a[10] *= rhs;
	_a[11] *= rhs;
	_a[12] *= rhs;
	_a[13] *= rhs;
	_a[14] *= rhs;
	_a[15] *= rhs;
	return *this;
}

Matrix4x4CPU Matrix4x4CPU::operator*(float rhs) const
{
	Matrix4x4CPU temp(*this);
	temp *= rhs;
	return temp;
}

Vector4CPU Matrix4x4CPU::operator*(const Vector4CPU& rhs) const
{
	float v0 = rhs[0]*_a[0] + rhs[1]*_a[4] + rhs[2]*_a[8] + rhs[3]*_a[12];
	float v1 = rhs[0]*_a[1] + rhs[1]*_a[5] + rhs[2]*_a[9] + rhs[3]*_a[13];
	float v2 = rhs[0]*_a[2] + rhs[1]*_a[6] + rhs[2]*_a[10] + rhs[3]*_a[14];
	float v3 = rhs[0]*_a[3] + rhs[1]*_a[7] + rhs[2]*_a[11] + rhs[3]*_a[15];
	return Vector4CPU(v0, v1, v2, v3);
}

const float* Matrix4x4CPU::operator[](int index) const
{
	assert(index > -1 && index < 4);
	return _m[index];
}

//float* Matrix4x4CPU::operator[](int index)
//{
//	assert(index > -1 && index < 4);
//	return _m[index];
//}

const float* Matrix4x4CPU::getBuffer(void) const
{
	return _a;
}

//float* Matrix4x4CPU::getBuffer(void)
//{
//	return _a;
//}

float Matrix4x4CPU::at(int column, int row) const
{
	assert(column > -1 && column < 4);
	assert(row >-1 && row < 4);
	return _m[column][row];
}

//float& Matrix4x4CPU::at(int column, int row)
//{
//	assert(column > -1 && column < 4);
//	assert(row >-1 && row < 4);
//
//	return _m[column][row];
//}

void Matrix4x4CPU::set(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
{
	_a[0] = m00; _a[1] = m01; _a[2] = m02; _a[3] = m03;
	_a[4] = m10; _a[5] = m11; _a[6] = m12; _a[7] = m13;
	_a[8] = m20; _a[9] = m21; _a[10] = m22; _a[11] = m23;
	_a[12] = m30; _a[13] = m31; _a[14] = m32; _a[15] = m33;
}

void Matrix4x4CPU::set(const float* elements)
{
	memcpy(_a, elements, sizeof(float) * 16);
}

void Matrix4x4CPU::set(int column, int row, float value)
{
	assert(column > -1 && column < 4);
	assert(row >-1 && row < 4);
	_m[column][row] = value;
}

Vector4CPU Matrix4x4CPU::getColumn(int column) const
{
	assert(column > -1 && column < 4);
	return Vector4CPU(_m[column]);
}

bool Matrix4x4CPU::isIdentity(void) const
{
	return _a[0] == 1.0f && _a[1] == 0.0f && _a[2] == 0.0f && _a[3] == 0.0f &&
			_a[4] == 0.0f && _a[5] == 1.0f && _a[6] == 0.0f && _a[7] == 0.0f &&
			_a[8] == 0.0f && _a[9] == 0.0f && _a[10] == 1.0f && _a[11] == 0.0f &&
			_a[12] == 0.0f && _a[13] == 0.0f && _a[14] == 0.0f && _a[15] == 1.0f;
}

void Matrix4x4CPU::setIdentity(void)
{
	set(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

bool Matrix4x4CPU::hasInfiniteElement(void) const
{
	return _a[0] <= -FLT_MAX || _a[0] >= FLT_MAX ||
			_a[1] <= -FLT_MAX || _a[1] >= FLT_MAX ||
			_a[2] <= -FLT_MAX || _a[2] >= FLT_MAX ||
			_a[3] <= -FLT_MAX || _a[3] >= FLT_MAX ||
			_a[4] <= -FLT_MAX || _a[4] >= FLT_MAX ||
			_a[5] <= -FLT_MAX || _a[5] >= FLT_MAX ||
			_a[6] <= -FLT_MAX || _a[6] >= FLT_MAX ||
			_a[7] <= -FLT_MAX || _a[7] >= FLT_MAX ||
			_a[8] <= -FLT_MAX || _a[8] >= FLT_MAX ||
			_a[9] <= -FLT_MAX || _a[9] >= FLT_MAX ||
			_a[10] <= -FLT_MAX || _a[10] >= FLT_MAX ||
			_a[11] <= -FLT_MAX || _a[11] >= FLT_MAX ||
			_a[12] <= -FLT_MAX || _a[12] >= FLT_MAX ||
			_a[13] <= -FLT_MAX || _a[13] >= FLT_MAX ||
			_a[14] <= -FLT_MAX || _a[14] >= FLT_MAX ||
			_a[15] <= -FLT_MAX || _a[15] >= FLT_MAX;
}

bool Matrix4x4CPU::hasNaNElement(void) const
{
	return !(_a[0] == _a[0]) ||
			!(_a[1] == _a[1]) ||
			!(_a[2] == _a[2]) ||
			!(_a[3] == _a[3]) ||
			!(_a[4] == _a[4]) ||
			!(_a[5] == _a[5]) ||
			!(_a[6] == _a[6]) ||
			!(_a[7] == _a[7]) ||
			!(_a[8] == _a[8]) ||
			!(_a[9] == _a[9]) ||
			!(_a[10] == _a[10]) ||
			!(_a[11] == _a[11]) ||
			!(_a[12] == _a[12]) ||
			!(_a[13] == _a[13]) ||
			!(_a[14] == _a[14]) ||
			!(_a[15] == _a[15]);
}

void Matrix4x4CPU::transposeThis(void)
{
	float tmp = _a[1]; _a[1] = _a[4]; _a[4] = tmp;
	tmp = _a[2]; _a[2] = _a[8]; _a[8] = tmp;
	tmp = _a[3]; _a[3] = _a[12]; _a[12] = tmp;
	tmp = _a[6]; _a[6] = _a[9]; _a[9] = tmp;
	tmp = _a[7]; _a[7] = _a[13]; _a[13] = tmp;
	tmp = _a[11]; _a[11] = _a[14]; _a[14] = tmp;
}

bool Matrix4x4CPU::inverseThis(void)
{
	float d = (_a[0] * _a[5] - _a[1] * _a[4]) * (_a[10] * _a[15] - _a[11] * _a[14]) -
			(_a[0] * _a[6] - _a[2] * _a[4]) * (_a[9] * _a[15] - _a[11] * _a[13]) +
			(_a[0] * _a[7] - _a[3] * _a[4]) * (_a[9] * _a[14] - _a[10] * _a[13]) +
			(_a[1] * _a[6] - _a[2] * _a[5]) * (_a[8] * _a[15] - _a[11] * _a[12]) -
			(_a[1] * _a[7] - _a[3] * _a[5]) * (_a[8] * _a[14] - _a[10] * _a[12]) +
			(_a[2] * _a[7] - _a[3] * _a[6]) * (_a[8] * _a[13] - _a[9] * _a[12]);

	if (d == 0.0f) {
		return false;
	}

	d = 1 / d;

	float tmp[16];

	tmp[0] = d * (_a[5] * (_a[10] * _a[15] - _a[11] * _a[14]) +
			_a[6] * (_a[11] * _a[13] - _a[9] * _a[15]) +
			_a[7] * (_a[9] * _a[14] - _a[10] * _a[13]));
	tmp[1] = d * (_a[9] * (_a[2] * _a[15] - _a[3] * _a[14]) +
			_a[10] * (_a[3] * _a[13] - _a[1] * _a[15]) +
			_a[11] * (_a[1] * _a[14] - _a[2] * _a[13]));
	tmp[2] = d * (_a[13] * (_a[2] * _a[7] - _a[3] * _a[6]) +
			_a[14] * (_a[3] * _a[5] - _a[1] * _a[7]) +
			_a[15] * (_a[1] * _a[6] - _a[2] * _a[5]));
	tmp[3] = d * (_a[1] * (_a[7] * _a[10] - _a[6] * _a[11]) +
			_a[2] * (_a[5] * _a[11] - _a[7] * _a[9]) +
			_a[3] * (_a[6] * _a[9] - _a[5] * _a[10]));
	tmp[4] = d * (_a[6] * (_a[8] * _a[15] - _a[11] * _a[12]) +
			_a[7] * (_a[10] * _a[12] - _a[8] * _a[14]) +
			_a[4] * (_a[11] * _a[14] - _a[10] * _a[15]));
	tmp[5] = d * (_a[10] * (_a[0] * _a[15] - _a[3] * _a[12]) +
			_a[11] * (_a[2] * _a[12] - _a[0] * _a[14]) +
			_a[8] * (_a[3] * _a[14] - _a[2] * _a[15]));
	tmp[6] = d * (_a[14] * (_a[0] * _a[7] - _a[3] * _a[4]) +
			_a[15] * (_a[2] * _a[4] - _a[0] * _a[6]) +
			_a[12] * (_a[3] * _a[6] - _a[2] * _a[7]));
	tmp[7] = d * (_a[2] * (_a[7] * _a[8] - _a[4] * _a[11]) +
			_a[3] * (_a[4] * _a[10] - _a[6] * _a[8]) +
			_a[0] * (_a[6] * _a[11] - _a[7] * _a[10]));
	tmp[8] = d * (_a[7] * (_a[8] * _a[13] - _a[9] * _a[12]) +
			_a[4] * (_a[9] * _a[15] - _a[11] * _a[13]) +
			_a[5] * (_a[11] * _a[12] - _a[8] * _a[15]));
	tmp[9] = d * (_a[11] * (_a[0] * _a[13] - _a[1] * _a[12]) +
			_a[8] * (_a[1] * _a[15] - _a[3] * _a[13]) +
			_a[9] * (_a[3] * _a[12] - _a[0] * _a[15]));
	tmp[10] = d * (_a[15] * (_a[0] * _a[5] - _a[1] * _a[4]) +
			_a[12] * (_a[1] * _a[7] - _a[3] * _a[5]) +
			_a[13] * (_a[3] * _a[4] - _a[0] * _a[7]));
	tmp[11] = d * (_a[3] * (_a[5] * _a[8] - _a[4] * _a[9]) +
			_a[0] * (_a[7] * _a[9] - _a[5] * _a[11]) +
			_a[1] * (_a[4] * _a[11] - _a[7] * _a[8]));
	tmp[12] = d * (_a[4] * (_a[10] * _a[13] - _a[9] * _a[14]) +
			_a[5] * (_a[8] * _a[14] - _a[10] * _a[12]) +
			_a[6] * (_a[9] * _a[12] - _a[8] * _a[13]));
	tmp[13] = d * (_a[8] * (_a[2] * _a[13] - _a[1] * _a[14]) +
			_a[9] * (_a[0] * _a[14] - _a[2] * _a[12]) +
			_a[10] * (_a[1] * _a[12] - _a[0] * _a[13]));
	tmp[14] = d * (_a[12] * (_a[2] * _a[5] - _a[1] * _a[6]) +
			_a[13] * (_a[0] * _a[6] - _a[2] * _a[4]) +
			_a[14] * (_a[1] * _a[4] - _a[0] * _a[5]));
	tmp[15] = d * (_a[0] * (_a[5] * _a[10] - _a[6] * _a[9]) +
			_a[1] * (_a[6] * _a[8] - _a[4] * _a[10]) +
			_a[2] * (_a[4] * _a[9] - _a[5] * _a[8]));

	_a[0] = tmp[0];
	_a[1] = tmp[1];
	_a[2] = tmp[2];
	_a[3] = tmp[3];
	_a[4] = tmp[4];
	_a[5] = tmp[5];
	_a[6] = tmp[6];
	_a[7] = tmp[7];
	_a[8] = tmp[8];
	_a[9] = tmp[9];
	_a[10] = tmp[10];
	_a[11] = tmp[11];
	_a[12] = tmp[12];
	_a[13] = tmp[13];
	_a[14] = tmp[14];
	_a[15] = tmp[15];

	return true;
}

Matrix4x4CPU Matrix4x4CPU::transpose(void) const
{
	Matrix4x4CPU temp = *this;
	temp.transposeThis();
	return temp;
}

bool Matrix4x4CPU::inverse(Matrix4x4CPU& out) const
{
	out = *this;
	return out.inverseThis();
}

void Matrix4x4CPU::setTranslate(const Vector4CPU& translate)
{
	_m[3][0] = translate[0];
	_m[3][1] = translate[1];
	_m[3][2] = translate[2];
	_m[3][3] = 1.0f;
}

void Matrix4x4CPU::setTranslate(float x, float y, float z)
{
	_m[3][0] = x;
	_m[3][1] = y;
	_m[3][2] = z;
	_m[3][3] = 1.0f;
}

void Matrix4x4CPU::setScale(float x, float y, float z)
{
	_m[0][0] = x;
	_m[1][1] = y;
	_m[2][2] = z;
}

void Matrix4x4CPU::setScale(const Vector4CPU& scale)
{
	_m[0][0] = scale[0];
	_m[1][1] = scale[1];
	_m[2][2] = scale[2];
}

void Matrix4x4CPU::setScale(float scale)
{
	_m[0][0] = scale;
	_m[1][1] = scale;
	_m[2][2] = scale;
}

void Matrix4x4CPU::setRotation(float radians, const Vector4CPU& axis)
{
	setRotation(radians, axis[0], axis[1], axis[2]);
}

void Matrix4x4CPU::setRotation(float radians, float x, float y, float z)
{
	assert(x != 0.0f && y != 0.0f && z != 0.0f);
	float inv_length = 1.0f / sqrtf(x*x + y*y + z*z);
	x *= inv_length;
	y *= inv_length;
	z *= inv_length;

	float c = cosf(radians);
	float c1 = 1.0f - c;
	float s = sinf(radians);

	set(
		x*x*c1 + c, y*x*c1 + z*s, z*x*c1 - y*s, _a[3],
		x*y*c1 - z*s, y*y*c1 + c, y*z*c1 + x*s, _a[7],
		x*z*c1 + y*s, y*z*c1 - x*s, z*z*c1 + c, _a[11],
		_a[12], _a[13], _a[14], _a[15]
	);
}

void Matrix4x4CPU::setRotationX(float radians)
{
	float cf = cosf(radians);
	float sf = sinf(radians);

	set(
		_a[0], _a[1], _a[2], _a[3],
		_a[4], cf, sf, _a[7],
		_a[8], -sf, cf, _a[11],
		_a[12], _a[13], _a[14], _a[15]
	);
}

void Matrix4x4CPU::setRotationY(float radians)
{
	float cf = cosf(radians);
	float sf = sinf(radians);

	set(
		cf, _a[1], -sf, _a[3],
		_a[4], _a[5], _a[6], _a[7],
		sf, _a[9], cf, _a[11],
		_a[12], _a[13], _a[14], _a[15]
	);
}

void Matrix4x4CPU::setRotationZ(float radians)
{
	float cf = cosf(radians);
	float sf = sinf(radians);

	set(
		cf, sf, _a[2], _a[3],
		-sf, cf, _a[6], _a[7],
		_a[8], _a[9], _a[10], _a[11],
		_a[12], _a[13], _a[14], _a[15]
	);
}

void Matrix4x4CPU::setLookAtLH(const Vector4CPU& eye, const Vector4CPU& target, const Vector4CPU& up)
{
	setLookToLH(eye, target - eye, up);
}

void Matrix4x4CPU::setLookAtLH(float eye_x, float eye_y, float eye_z,
							float target_x, float target_y, float target_z,
							float up_x, float up_y, float up_z)
{
	setLookAtLH(
		Vector4CPU(eye_x, eye_y, eye_z, 1.0f),
		Vector4CPU(target_x, target_y, target_z, 1.0f),
		Vector4CPU(up_x, up_y, up_z, 0.0f)
	);
}

void Matrix4x4CPU::setLookAtRH(const Vector4CPU& eye, const Vector4CPU& target, const Vector4CPU& up)
{
	setLookToRH(eye, target - eye, up);
}

void Matrix4x4CPU::setLookAtRH(float eye_x, float eye_y, float eye_z,
							float target_x, float target_y, float target_z,
							float up_x, float up_y, float up_z)
{
	setLookAtRH(
		Vector4CPU(eye_x, eye_y, eye_z, 1.0f),
		Vector4CPU(target_x, target_y, target_z, 1.0f),
		Vector4CPU(up_x, up_y, up_z, 0.0f)
	);
}

void Matrix4x4CPU::setLookToLH(const Vector4CPU& eye, const Vector4CPU& dir, const Vector4CPU& up)
{
	Vector4CPU realDir, right, realUp;

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

void Matrix4x4CPU::setLookToLH(float eye_x, float eye_y, float eye_z,
							float dir_x, float dir_y, float dir_z,
							float up_x, float up_y, float up_z)
{
	setLookToLH(
		Vector4CPU(eye_x, eye_y, eye_z, 1.0f),
		Vector4CPU(dir_x, dir_y, dir_z, 1.0f),
		Vector4CPU(up_x, up_y, up_z, 0.0f)
	);
}

void Matrix4x4CPU::setLookToRH(const Vector4CPU& eye, const Vector4CPU& dir, const Vector4CPU& up)
{
	setLookToLH(eye, -dir, up);
}

void Matrix4x4CPU::setLookToRH(float eye_x, float eye_y, float eye_z,
							float dir_x, float dir_y, float dir_z,
							float up_x, float up_y, float up_z)
{
	setLookToRH(
		Vector4CPU(eye_x, eye_y, eye_z, 1.0f),
		Vector4CPU(dir_x, dir_y, dir_z, 1.0f),
		Vector4CPU(up_x, up_y, up_z, 0.0f)
	);
}

void Matrix4x4CPU::setOrthographicLH(float left, float right, float bottom, float top, float z_near, float z_far)
{
	float recip_width = 1.0f / (right - left);
	float recip_height = 1.0f / (top - bottom);
	float range = 1.0f / (z_far - z_near);

	_a[0] = recip_width + recip_width;
	_a[1] = 0.0f;
	_a[2] = 0.0f;
	_a[3] = 0.0f;
	_a[4] = 0.0f;
	_a[5] = recip_height + recip_height;
	_a[6] = 0.0f;
	_a[7] = 0.0f;
	_a[8] = 0.0f;
	_a[9] = 0.0f;
	_a[10] = range;
	_a[11] = 0.0f;
	_a[12] = -(left + right) * recip_width;
	_a[13] = -(top + bottom) * recip_height;
	_a[14] = z_near * -range;
	_a[15] = 1.0f;
}

void Matrix4x4CPU::setOrthographicLH(float width, float height, float z_near, float z_far)
{
	float range = 1.0f / (z_far - z_near);

	_a[0] = 2.0f / width;
	_a[1] = 0.0f;
	_a[2] = 0.0f;
	_a[3] = 0.0f;
	_a[4] = 0.0f;
	_a[5] = 2.0f / height;
	_a[6] = 0.0f;
	_a[7] = 0.0f;
	_a[8] = 0.0f;
	_a[9] = 0.0f;
	_a[10] = range;
	_a[11] = 0.0f;
	_a[12] = 0.0f;
	_a[13] = 0.0f;
	_a[14] = z_near * -range;
	_a[15] = 1.0f;
}

void Matrix4x4CPU::setOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far)
{
	float recip_width = 1.0f / (right - left);
	float recip_height = 1.0f / (top - bottom);
	float range = 1.0f / (z_near - z_far);

	_a[0] = recip_width + recip_width;
	_a[1] = 0.0f;
	_a[2] = 0.0f;
	_a[3] = 0.0f;
	_a[4] = 0.0f;
	_a[5] = recip_height + recip_height;
	_a[6] = 0.0f;
	_a[7] = 0.0f;
	_a[8] = 0.0f;
	_a[9] = 0.0f;
	_a[10] = range;
	_a[11] = 0.0f;
	_a[12] = -(left + right) * recip_width;
	_a[13] = -(top + bottom) * recip_height;
	_a[14] = z_near * range;
	_a[15] = 1.0f;
}

void Matrix4x4CPU::setOrthographicRH(float width, float height, float z_near, float z_far)
{
	float range = 1.0f / (z_near - z_far);

	_a[0] = 2.0f / width;
	_a[1] = 0.0f;
	_a[2] = 0.0f;
	_a[3] = 0.0f;
	_a[4] = 0.0f;
	_a[5] = 2.0f / height;
	_a[6] = 0.0f;
	_a[7] = 0.0f;
	_a[8] = 0.0f;
	_a[9] = 0.0f;
	_a[10] = range;
	_a[11] = 0.0f;
	_a[12] = 0.0f;
	_a[13] = 0.0f;
	_a[14] = z_near * range;
	_a[15] = 1.0f;
}

void Matrix4x4CPU::setPerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far)
{
	fov *= 0.5f;
	float tan_fov = tanf(fov);
	float width = tan_fov / aspect_ratio;
	float range = z_far / (z_far - z_near);

	_a[0] = width;
	_a[1] = 0.0f;
	_a[2] = 0.0f;
	_a[3] = 0.0f;
	_a[4] = 0.0f;
	_a[5] = tan_fov;
	_a[6] = 0.0f;
	_a[7] = 0.0f;
	_a[8] = 0.0f;
	_a[9] = 0.0f;
	_a[10] = range;
	_a[11] = 1.0f;
	_a[12] = 0.0f;
	_a[13] = 0.0f;
	_a[14] = z_near * -range;
	_a[15] = 0.0f;
}

void Matrix4x4CPU::setPerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far)
{
	fov *= 0.5f;
	float tan_fov = tanf(fov);
	float width = tan_fov / aspect_ratio;
	float range = z_far / (z_near - z_far);

	_a[0] = width;
	_a[1] = 0.0f;
	_a[2] = 0.0f;
	_a[3] = 0.0f;
	_a[4] = 0.0f;
	_a[5] = tan_fov;
	_a[6] = 0.0f;
	_a[7] = 0.0f;
	_a[8] = 0.0f;
	_a[9] = 0.0f;
	_a[10] = range;
	_a[11] = -1.0f;
	_a[12] = 0.0f;
	_a[13] = 0.0f;
	_a[14] = z_near * range;
	_a[15] = 0.0f;
}

bool Matrix4x4CPU::roughlyEqual(const Matrix4x4CPU& rhs, const Vector4CPU& epsilon) const
{
	return roughlyEqual(rhs, epsilon[0]);
}

bool Matrix4x4CPU::roughlyEqual(const Matrix4x4CPU& rhs, float epsilon) const
{
	return fabsf(_a[0] - rhs._a[0]) <= epsilon && fabsf(_a[1] - rhs._a[1]) <= epsilon &&
			fabsf(_a[2] - rhs._a[2]) <= epsilon && fabsf(_a[3] - rhs._a[3]) <= epsilon &&
			fabsf(_a[4] - rhs._a[4]) <= epsilon && fabsf(_a[5] - rhs._a[5]) <= epsilon &&
			fabsf(_a[6] - rhs._a[6]) <= epsilon && fabsf(_a[7] - rhs._a[7]) <= epsilon &&
			fabsf(_a[8] - rhs._a[8]) <= epsilon && fabsf(_a[9] - rhs._a[9]) <= epsilon &&
			fabsf(_a[10] - rhs._a[10]) <= epsilon && fabsf(_a[11] - rhs._a[11]) <= epsilon &&
			fabsf(_a[12] - rhs._a[12]) <= epsilon && fabsf(_a[13] - rhs._a[13]) <= epsilon &&
			fabsf(_a[14] - rhs._a[14]) <= epsilon && fabsf(_a[15] - rhs._a[15]) <= epsilon;
}

void Matrix4x4CPU::setRotation(const Vector4CPU& radians, const Vector4CPU& axis)
{
	setRotation(radians[0], axis);
}

void Matrix4x4CPU::setRotationX(const Vector4CPU& radians)
{
	setRotationX(radians[0]);
}

void Matrix4x4CPU::setRotationY(const Vector4CPU& radians)
{
	setRotationY(radians[0]);
}

void Matrix4x4CPU::setRotationZ(const Vector4CPU& radians)
{
	setRotationZ(radians[0]);
}

Matrix4x4CPU Matrix4x4CPU::MakeTranslate(const Vector4CPU& translate)
{
	Matrix4x4CPU temp = Identity;
	temp.setTranslate(translate);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeTranslate(float x, float y, float z)
{
	Matrix4x4CPU temp = Identity;
	temp.setTranslate(x, y, z);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeScale(const Vector4CPU& scale)
{
	Matrix4x4CPU temp = Identity;
	temp.setScale(scale);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeScale(float x, float y, float z)
{
	Matrix4x4CPU temp = Identity;
	temp.setScale(x, y, z);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeScale(float scale)
{
	Matrix4x4CPU temp = Identity;
	temp.setScale(scale);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeRotation(float radians, float x, float y, float z)
{
	Matrix4x4CPU temp = Identity;
	temp.setRotation(radians, x, y, z);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeRotationX(float radians)
{
	Matrix4x4CPU temp = Identity;
	temp.setRotationX(radians);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeRotationY(float radians)
{
	Matrix4x4CPU temp = Identity;
	temp.setRotationY(radians);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeRotationZ(float radians)
{
	Matrix4x4CPU temp = Identity;
	temp.setRotationZ(radians);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeLookAtLH(const Vector4CPU& eye, const Vector4CPU& target, const Vector4CPU& up)
{
	Matrix4x4CPU temp;
	temp.setLookAtLH(eye, target, up);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeLookAtLH(float eye_x, float eye_y, float eye_z,
									float target_x, float target_y, float target_z,
									float up_x, float up_y, float up_z)
{
	Matrix4x4CPU temp;
	temp.setLookAtLH(eye_x, eye_y, eye_z, target_x, target_y, target_z, up_x, up_y, up_z);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeLookAtRH(const Vector4CPU& eye, const Vector4CPU& target, const Vector4CPU& up)
{
	Matrix4x4CPU temp;
	temp.setLookAtRH(eye, target, up);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeLookAtRH(float eye_x, float eye_y, float eye_z,
									float target_x, float target_y, float target_z,
									float up_x, float up_y, float up_z)
{
	Matrix4x4CPU temp;
	temp.setLookAtRH(eye_x, eye_y, eye_z, target_x, target_y, target_z, up_x, up_y, up_z);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeLookToLH(const Vector4CPU& eye, const Vector4CPU& dir, const Vector4CPU& up)
{
	Matrix4x4CPU temp;
	temp.setLookToLH(eye, dir, up);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeLookToLH(float eye_x, float eye_y, float eye_z,
									float dir_x, float dir_y, float dir_z,
									float up_x, float up_y, float up_z)
{
	Matrix4x4CPU temp;
	temp.setLookToLH(eye_x, eye_y, eye_z, dir_x, dir_y, dir_z, up_x, up_y, up_z);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeLookToRH(const Vector4CPU& eye, const Vector4CPU& dir, const Vector4CPU& up)
{
	Matrix4x4CPU temp;
	temp.setLookToRH(eye, dir, up);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeLookToRH(float eye_x, float eye_y, float eye_z,
									float dir_x, float dir_y, float dir_z,
									float up_x, float up_y, float up_z)
{
	Matrix4x4CPU temp;
	temp.setLookToRH(eye_x, eye_y, eye_z, dir_x, dir_y, dir_z, up_x, up_y, up_z);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeOrthographicLH(float left, float right, float bottom, float top, float z_near, float z_far)
{
	Matrix4x4CPU temp;
	temp.setOrthographicLH(left, right, bottom, top, z_near, z_far);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeOrthographicLH(float width, float height, float z_near, float z_far)
{
	Matrix4x4CPU temp;
	temp.setOrthographicLH(width, height, z_near, z_far);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far)
{
	Matrix4x4CPU temp;
	temp.setOrthographicRH(left, right, bottom, top, z_near, z_far);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakeOrthographicRH(float width, float height, float z_near, float z_far)
{
	Matrix4x4CPU temp;
	temp.setOrthographicRH(width, height, z_near, z_far);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakePerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far)
{
	Matrix4x4CPU temp;
	temp.setPerspectiveLH(fov, aspect_ratio, z_near, z_far);
	return temp;
}

Matrix4x4CPU Matrix4x4CPU::MakePerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far)
{
	Matrix4x4CPU temp;
	temp.setPerspectiveRH(fov, aspect_ratio, z_near, z_far);
	return temp;
}

Matrix4x4CPU operator*(float lhs, const Matrix4x4CPU& rhs)
{
	return rhs * lhs;
}

NS_END
