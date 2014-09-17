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

#pragma once

#include "Gleam_Vector4_CPU.h"

NS_GLEAM

class Vector4CPU;

// Matrix4x4CPU elements are [column][row]
//
// | 00 10 20 30 |
// | 01 11 21 31 |
// | 02 12 22 23 |
// | 03 13 23 33 |
class Matrix4x4CPU
{
public:
	Matrix4x4CPU(void);
	Matrix4x4CPU(const Matrix4x4CPU& matrix);
	Matrix4x4CPU(float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33);
	Matrix4x4CPU(const Vector4CPU& vec1, const Vector4CPU& vec2,
				const Vector4CPU& vec3, const Vector4CPU& vec4);
	explicit Matrix4x4CPU(const float* elements);
	~Matrix4x4CPU(void);

	bool operator==(const Matrix4x4CPU& rhs) const;
	INLINE bool operator!=(const Matrix4x4CPU& rhs) const;
	INLINE const Matrix4x4CPU& operator=(const Matrix4x4CPU& rhs);

	const Matrix4x4CPU& operator*=(const Matrix4x4CPU& rhs);
	Matrix4x4CPU operator*(const Matrix4x4CPU& rhs) const;
	const Matrix4x4CPU& operator+=(const Matrix4x4CPU& rhs);
	Matrix4x4CPU operator+(const Matrix4x4CPU& rhs) const;
	const Matrix4x4CPU& operator-=(const Matrix4x4CPU& rhs);
	Matrix4x4CPU operator-(const Matrix4x4CPU& rhs) const;

	const Matrix4x4CPU& operator/=(float rhs);
	Matrix4x4CPU operator/(float rhs) const;
	const Matrix4x4CPU& operator*=(float rhs);
	Matrix4x4CPU operator*(float rhs) const;
	Vector4CPU operator*(const Vector4CPU& rhs) const;

	INLINE const float* operator[](int index) const;
	//INLINE float* operator[](int index);

	INLINE const float* getBuffer(void) const;
	//INLINE float* getBuffer(void);
	
	INLINE float at(int column, int row) const;
	//INLINE float& at(int column, int row);

	void set(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);
	INLINE void set(const float* elements);
	INLINE void set(int column, int row, float value);

	bool isIdentity(void) const;
	void setIdentity(void);

	bool hasInfiniteElement(void) const;
	bool hasNaNElement(void) const;

	void transposeThis(void);
	bool inverseThis(void);
	Matrix4x4CPU transpose(void) const;
	bool inverse(Matrix4x4CPU& out) const;

	INLINE void setTranslate(const Vector4CPU& translate);
	INLINE void setTranslate(float x, float y, float z);
	INLINE void setScale(float x, float y, float z);
	INLINE void setScale(const Vector4CPU& scale);
	INLINE void setScale(float scale);
	INLINE void setRotation(float radians, const Vector4CPU& axis);
	void setRotation(float radians, float x, float y, float z);
	INLINE void setRotationX(float radians);
	INLINE void setRotationY(float radians);
	INLINE void setRotationZ(float radians);

	INLINE void setLookAtLH(const Vector4CPU& eye, const Vector4CPU& target, const Vector4CPU& up);
	INLINE void setLookAtLH(float eye_x, float eye_y, float eye_z,
							float target_x, float target_y, float target_z,
							float up_x, float up_y, float up_z);
	INLINE void setLookAtRH(const Vector4CPU& eye, const Vector4CPU& target, const Vector4CPU& up);
	INLINE void setLookAtRH(float eye_x, float eye_y, float eye_z,
							float target_x, float target_y, float target_z,
							float up_x, float up_y, float up_z);

	void setLookToLH(const Vector4CPU& eye, const Vector4CPU& dir, const Vector4CPU& up);
	INLINE void setLookToLH(float eye_x, float eye_y, float eye_z,
							float dir_x, float dir_y, float dir_z,
							float up_x, float up_y, float up_z);
	INLINE void setLookToRH(const Vector4CPU& eye, const Vector4CPU& dir, const Vector4CPU& up);
	INLINE void setLookToRH(float eye_x, float eye_y, float eye_z,
							float dir_x, float dir_y, float dir_z,
							float up_x, float up_y, float up_z);

	void setOrthographicLH(float left, float right, float bottom, float top, float z_near, float z_far);
	void setOrthographicLH(float width, float height, float z_near, float z_far);
	void setOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far);
	void setOrthographicRH(float width, float height, float z_near, float z_far);
	void setPerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far);
	void setPerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far);

	INLINE bool roughlyEqual(const Matrix4x4CPU& rhs, const Vector4CPU& epsilon = Vector4CPU::Epsilon) const;
	INLINE bool roughlyEqual(const Matrix4x4CPU& rhs, float epsilon = 0.000001f) const;

	// SIMD compatability
	INLINE void setRotation(const Vector4CPU& radians, const Vector4CPU& axis);
	INLINE void setRotationX(const Vector4CPU& radians);
	INLINE void setRotationY(const Vector4CPU& radians);
	INLINE void setRotationZ(const Vector4CPU& radians);

	static Matrix4x4CPU Identity;

	INLINE static Matrix4x4CPU MakeTranslate(const Vector4CPU& translate);
	INLINE static Matrix4x4CPU MakeTranslate(float x, float y, float z);
	INLINE static Matrix4x4CPU MakeScale(float x, float y, float z);
	INLINE static Matrix4x4CPU MakeScale(const Vector4CPU& scale);
	INLINE static Matrix4x4CPU MakeScale(float scale);
	INLINE static Matrix4x4CPU MakeRotation(float radians, float x, float y, float z);
	INLINE static Matrix4x4CPU MakeRotationX(float radians);
	INLINE static Matrix4x4CPU MakeRotationY(float radians);
	INLINE static Matrix4x4CPU MakeRotationZ(float radians);

	INLINE static Matrix4x4CPU MakeLookAtLH(const Vector4CPU& eye, const Vector4CPU& target, const Vector4CPU& up);
	INLINE static Matrix4x4CPU MakeLookAtLH(float eye_x, float eye_y, float eye_z,
											float target_x, float target_y, float target_z,
											float up_x, float up_y, float up_z);
	INLINE static Matrix4x4CPU MakeLookAtRH(const Vector4CPU& eye, const Vector4CPU& target, const Vector4CPU& up);
	INLINE static Matrix4x4CPU MakeLookAtRH(float eye_x, float eye_y, float eye_z,
											float target_x, float target_y, float target_z,
											float up_x, float up_y, float up_z);

	INLINE static Matrix4x4CPU MakeLookToLH(const Vector4CPU& eye, const Vector4CPU& dir, const Vector4CPU& up);
	INLINE static Matrix4x4CPU MakeLookToLH(float eye_x, float eye_y, float eye_z,
											float dir_x, float dir_y, float dir_z,
											float up_x, float up_y, float up_z);
	INLINE static Matrix4x4CPU MakeLookToRH(const Vector4CPU& eye, const Vector4CPU& dir, const Vector4CPU& up);
	INLINE static Matrix4x4CPU MakeLookToRH(float eye_x, float eye_y, float eye_z,
											float dir_x, float dir_y, float dir_z,
											float up_x, float up_y, float up_z);

	INLINE static Matrix4x4CPU MakeOrthographicLH(float left, float right, float bottom, float top, float z_near, float z_far);
	INLINE static Matrix4x4CPU MakeOrthographicLH(float width, float height, float z_near, float z_far);
	INLINE static Matrix4x4CPU MakeOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far);
	INLINE static Matrix4x4CPU MakeOrthographicRH(float width, float height, float z_near, float z_far);
	INLINE static Matrix4x4CPU MakePerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far);
	INLINE static Matrix4x4CPU MakePerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far);

private:
	union
	{
		float _m[4][4];
		float _a[16];
	};
};

INLINE Matrix4x4CPU operator*(float lhs, const Matrix4x4CPU& rhs);

NS_END
