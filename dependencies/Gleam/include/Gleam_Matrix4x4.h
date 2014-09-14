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

#include "Gleam_Defines.h"

NS_GLEAM

class Vector4;

// Matrix4x4 elements are [column][row]
//
// | 00 10 20 30 |
// | 01 11 21 31 |
// | 02 12 22 23 |
// | 03 13 23 33 |
class Matrix4x4
{
public:
	Matrix4x4(void);
	Matrix4x4(const Matrix4x4& matrix);
	Matrix4x4(float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33);
	Matrix4x4(const Vector4& vec1, const Vector4& vec2,
				const Vector4& vec3, const Vector4& vec4);
	explicit Matrix4x4(const float* elements);
	~Matrix4x4(void);

	bool operator==(const Matrix4x4& rhs) const;
	INLINE bool operator!=(const Matrix4x4& rhs) const;
	INLINE const Matrix4x4& operator=(const Matrix4x4& rhs);

	const Matrix4x4& operator*=(const Matrix4x4& rhs);
	Matrix4x4 operator*(const Matrix4x4& rhs) const;
	const Matrix4x4& operator+=(const Matrix4x4& rhs);
	Matrix4x4 operator+(const Matrix4x4& rhs) const;
	const Matrix4x4& operator-=(const Matrix4x4& rhs);
	Matrix4x4 operator-(const Matrix4x4& rhs) const;

	const Matrix4x4& operator/=(float rhs);
	Matrix4x4 operator/(float rhs) const;
	const Matrix4x4& operator*=(float rhs);
	Matrix4x4 operator*(float rhs) const;
	Vector4 operator*(const Vector4& rhs) const;

	INLINE const float* operator[](int index) const;
	INLINE float* operator[](int index);

	INLINE const float* getBuffer(void) const;
	INLINE float* getBuffer(void);
	
	INLINE float at(int column, int row) const;
	INLINE float& at(int column, int row);

	void set(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);
	INLINE void set(const float* elements);

	bool isIdentity(void) const;
	void setIdentity(void);

	bool hasInfiniteElement(void) const;
	bool hasNaNElement(void) const;

	void transpose(void);
	bool inverse(void);

	INLINE void setTranslate(const Vector4& translate);
	INLINE void setTranslate(float x, float y, float z);
	INLINE void setScale(float x, float y, float z);
	INLINE void setScale(const Vector4& scale);
	INLINE void setScale(float scale);
	INLINE void setRotation(float radians, const Vector4& axis);
	void setRotation(float radians, float x, float y, float z);
	INLINE void setRotationX(float radians);
	INLINE void setRotationY(float radians);
	INLINE void setRotationZ(float radians);

	INLINE void setLookAtLH(const Vector4& eye, const Vector4& target, const Vector4& up);
	INLINE void setLookAtLH(float eye_x, float eye_y, float eye_z,
							float target_x, float target_y, float target_z,
							float up_x, float up_y, float up_z);
	INLINE void setLookAtRH(const Vector4& eye, const Vector4& target, const Vector4& up);
	INLINE void setLookAtRH(float eye_x, float eye_y, float eye_z,
							float target_x, float target_y, float target_z,
							float up_x, float up_y, float up_z);

	void setLookToLH(const Vector4& eye, const Vector4& dir, const Vector4& up);
	INLINE void setLookToLH(float eye_x, float eye_y, float eye_z,
							float dir_x, float dir_y, float dir_z,
							float up_x, float up_y, float up_z);
	INLINE void setLookToRH(const Vector4& eye, const Vector4& dir, const Vector4& up);
	INLINE void setLookToRH(float eye_x, float eye_y, float eye_z,
							float dir_x, float dir_y, float dir_z,
							float up_x, float up_y, float up_z);

	void setOrthographicLH(float left, float right, float bottom, float top, float z_near, float z_far);
	void setOrthographicLH(float width, float height, float z_near, float z_far);
	void setOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far);
	void setOrthographicRH(float width, float height, float z_near, float z_far);
	void setPerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far);
	void setPerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far);

	INLINE bool roughlyEqual(const Matrix4x4& rhs, float epsilon = 0.000001f) const;

	static Matrix4x4 identity;

	INLINE static Matrix4x4 MakeTranslate(const Vector4& translate);
	INLINE static Matrix4x4 MakeTranslate(float x, float y, float z);
	INLINE static Matrix4x4 MakeScale(float x, float y, float z);
	INLINE static Matrix4x4 MakeScale(const Vector4& scale);
	INLINE static Matrix4x4 MakeScale(float scale);
	INLINE static Matrix4x4 MakeRotation(float radians, float x, float y, float z);
	INLINE static Matrix4x4 MakeRotationX(float radians);
	INLINE static Matrix4x4 MakeRotationY(float radians);
	INLINE static Matrix4x4 MakeRotationZ(float radians);

	INLINE static Matrix4x4 MakeLookAtLH(const Vector4& eye, const Vector4& target, const Vector4& up);
	INLINE static Matrix4x4 MakeLookAtLH(float eye_x, float eye_y, float eye_z,
											float target_x, float target_y, float target_z,
											float up_x, float up_y, float up_z);
	INLINE static Matrix4x4 MakeLookAtRH(const Vector4& eye, const Vector4& target, const Vector4& up);
	INLINE static Matrix4x4 MakeLookAtRH(float eye_x, float eye_y, float eye_z,
											float target_x, float target_y, float target_z,
											float up_x, float up_y, float up_z);

	INLINE static Matrix4x4 MakeLookToLH(const Vector4& eye, const Vector4& dir, const Vector4& up);
	INLINE static Matrix4x4 MakeLookToLH(float eye_x, float eye_y, float eye_z,
											float dir_x, float dir_y, float dir_z,
											float up_x, float up_y, float up_z);
	INLINE static Matrix4x4 MakeLookToRH(const Vector4& eye, const Vector4& dir, const Vector4& up);
	INLINE static Matrix4x4 MakeLookToRH(float eye_x, float eye_y, float eye_z,
											float dir_x, float dir_y, float dir_z,
											float up_x, float up_y, float up_z);

	INLINE static Matrix4x4 MakeOrthographicLH(float left, float right, float bottom, float top, float z_near, float z_far);
	INLINE static Matrix4x4 MakeOrthographicLH(float width, float height, float z_near, float z_far);
	INLINE static Matrix4x4 MakeOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far);
	INLINE static Matrix4x4 MakeOrthographicRH(float width, float height, float z_near, float z_far);
	INLINE static Matrix4x4 MakePerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far);
	INLINE static Matrix4x4 MakePerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far);

private:
	union
	{
		float _m[4][4];
		float _a[16];
	};
};

INLINE Matrix4x4 operator*(float lhs, const Matrix4x4& rhs);

typedef Matrix4x4 Mtx4x4;

NS_END
