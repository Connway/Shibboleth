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

#pragma once

#include "Gleam_Vector4_SIMD.h"

WARNING("SIMD data structures are potentially going to be deprecated.")

NS_GLEAM

class Vector4SIMD;

// Matrix4x4SIMD elements are [column][row]
//
// | 00 10 20 30 |
// | 01 11 21 31 |
// | 02 12 22 32 |
// | 03 13 23 33 |
class alignas(16) Matrix4x4SIMD
{
public:
	Matrix4x4SIMD(void);
	Matrix4x4SIMD(const Matrix4x4SIMD& matrix);
	Matrix4x4SIMD(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);
	Matrix4x4SIMD(const Vector4SIMD& vec1, const Vector4SIMD& vec2,
		const Vector4SIMD& vec3, const Vector4SIMD& vec4);
	explicit Matrix4x4SIMD(const SIMDMatrix& matrix);
	explicit Matrix4x4SIMD(const float* elements);
	~Matrix4x4SIMD(void);

	INLINE bool operator==(const Matrix4x4SIMD& rhs) const;
	INLINE bool operator!=(const Matrix4x4SIMD& rhs) const;
	INLINE const Matrix4x4SIMD& operator=(const Matrix4x4SIMD& rhs);

	INLINE const Matrix4x4SIMD& operator*=(const Matrix4x4SIMD& rhs);
	INLINE Matrix4x4SIMD operator*(const Matrix4x4SIMD& rhs) const;
	INLINE const Matrix4x4SIMD& operator+=(const Matrix4x4SIMD& rhs);
	INLINE Matrix4x4SIMD operator+(const Matrix4x4SIMD& rhs) const;
	INLINE const Matrix4x4SIMD& operator-=(const Matrix4x4SIMD& rhs);
	INLINE Matrix4x4SIMD operator-(const Matrix4x4SIMD& rhs) const;

	INLINE const Matrix4x4SIMD& operator/=(float rhs);
	INLINE Matrix4x4SIMD operator/(float rhs) const;
	INLINE const Matrix4x4SIMD& operator*=(float rhs);
	INLINE Matrix4x4SIMD operator*(float rhs) const;
	INLINE Vector4SIMD operator*(const Vector4SIMD& rhs) const;

	INLINE const float* operator[](int index) const;
	//INLINE float* operator[](int index);

	INLINE const float* getBuffer(void) const;
	//INLINE float* getBuffer(void);
	INLINE const SIMDMatrix& getSIMDType(void) const;

	INLINE float at(int column, int row) const;
	//INLINE float& at(int column, int row);

	void set(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);
	INLINE void set(const float* elements);
	INLINE void set(int column, int row, float value);

	INLINE Vector4SIMD getColumn(int column) const;

	INLINE bool isIdentity(void) const;
	INLINE void setIdentity(void);

	INLINE bool hasInfiniteElement(void) const;
	INLINE bool hasNaNElement(void) const;

	INLINE void transposeThis(void);
	INLINE bool inverseThis(void);
	INLINE Matrix4x4SIMD transpose(void) const;
	INLINE bool inverse(Matrix4x4SIMD& out) const;

	INLINE void setTranslate(const Vector4SIMD& translate);
	INLINE void setTranslate(float x, float y, float z);
	INLINE void setScale(float x, float y, float z);
	INLINE void setScale(const Vector4SIMD& scale);
	INLINE void setScale(float scale);
	INLINE void setRotation(float radians, const Vector4SIMD& axis);
	void setRotation(float radians, float x, float y, float z);
	INLINE void setRotationX(float radians);
	INLINE void setRotationY(float radians);
	INLINE void setRotationZ(float radians);

	INLINE void setLookAtLH(const Vector4SIMD& eye, const Vector4SIMD& target, const Vector4SIMD& up);
	INLINE void setLookAtLH(float eye_x, float eye_y, float eye_z,
							float target_x, float target_y, float target_z,
							float up_x, float up_y, float up_z);
	INLINE void setLookAtRH(const Vector4SIMD& eye, const Vector4SIMD& target, const Vector4SIMD& up);
	INLINE void setLookAtRH(float eye_x, float eye_y, float eye_z,
							float target_x, float target_y, float target_z,
							float up_x, float up_y, float up_z);

	void setLookToLH(const Vector4SIMD& eye, const Vector4SIMD& dir, const Vector4SIMD& up);
	INLINE void setLookToLH(float eye_x, float eye_y, float eye_z,
							float dir_x, float dir_y, float dir_z,
							float up_x, float up_y, float up_z);
	INLINE void setLookToRH(const Vector4SIMD& eye, const Vector4SIMD& dir, const Vector4SIMD& up);
	INLINE void setLookToRH(float eye_x, float eye_y, float eye_z,
							float dir_x, float dir_y, float dir_z,
							float up_x, float up_y, float up_z);

	void setOrthographicLH(float left, float right, float bottom, float top, float z_near, float z_far);
	void setOrthographicLH(float width, float height, float z_near, float z_far);
	void setOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far);
	void setOrthographicRH(float width, float height, float z_near, float z_far);
	void setPerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far);
	void setPerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far);

	INLINE bool roughlyEqual(const Matrix4x4SIMD& rhs, const Vector4SIMD& epsilon = Vector4SIMD::Epsilon) const;
	INLINE bool roughlyEqual(const Matrix4x4SIMD& rhs, float epsilon = 0.000001f) const;

	// SIMD compatability
	INLINE void setRotation(const Vector4SIMD& radians, const Vector4SIMD& axis);
	INLINE void setRotationX(const Vector4SIMD& radians);
	INLINE void setRotationY(const Vector4SIMD& radians);
	INLINE void setRotationZ(const Vector4SIMD& radians);

	static Matrix4x4SIMD Identity;

	INLINE static Matrix4x4SIMD MakeTranslate(const Vector4SIMD& translate);
	INLINE static Matrix4x4SIMD MakeTranslate(float x, float y, float z);
	INLINE static Matrix4x4SIMD MakeScale(float x, float y, float z);
	INLINE static Matrix4x4SIMD MakeScale(const Vector4SIMD& scale);
	INLINE static Matrix4x4SIMD MakeScale(float scale);
	INLINE static Matrix4x4SIMD MakeRotation(float radians, float x, float y, float z);
	INLINE static Matrix4x4SIMD MakeRotationX(float radians);
	INLINE static Matrix4x4SIMD MakeRotationY(float radians);
	INLINE static Matrix4x4SIMD MakeRotationZ(float radians);

	INLINE static Matrix4x4SIMD MakeLookAtLH(const Vector4SIMD& eye, const Vector4SIMD& target, const Vector4SIMD& up);
	INLINE static Matrix4x4SIMD MakeLookAtLH(float eye_x, float eye_y, float eye_z,
											float target_x, float target_y, float target_z,
											float up_x, float up_y, float up_z);
	INLINE static Matrix4x4SIMD MakeLookAtRH(const Vector4SIMD& eye, const Vector4SIMD& target, const Vector4SIMD& up);
	INLINE static Matrix4x4SIMD MakeLookAtRH(float eye_x, float eye_y, float eye_z,
											float target_x, float target_y, float target_z,
											float up_x, float up_y, float up_z);

	INLINE static Matrix4x4SIMD MakeLookToLH(const Vector4SIMD& eye, const Vector4SIMD& dir, const Vector4SIMD& up);
	INLINE static Matrix4x4SIMD MakeLookToLH(float eye_x, float eye_y, float eye_z,
											float dir_x, float dir_y, float dir_z,
											float up_x, float up_y, float up_z);
	INLINE static Matrix4x4SIMD MakeLookToRH(const Vector4SIMD& eye, const Vector4SIMD& dir, const Vector4SIMD& up);
	INLINE static Matrix4x4SIMD MakeLookToRH(float eye_x, float eye_y, float eye_z,
											float dir_x, float dir_y, float dir_z,
											float up_x, float up_y, float up_z);

	INLINE static Matrix4x4SIMD MakeOrthographicLH(float left, float right, float bottom, float top, float z_near, float z_far);
	INLINE static Matrix4x4SIMD MakeOrthographicLH(float width, float height, float z_near, float z_far);
	INLINE static Matrix4x4SIMD MakeOrthographicRH(float left, float right, float bottom, float top, float z_near, float z_far);
	INLINE static Matrix4x4SIMD MakeOrthographicRH(float width, float height, float z_near, float z_far);
	INLINE static Matrix4x4SIMD MakePerspectiveLH(float fov, float aspect_ratio, float z_near, float z_far);
	INLINE static Matrix4x4SIMD MakePerspectiveRH(float fov, float aspect_ratio, float z_near, float z_far);

private:
	SIMDMatrix _matrix;

	union
	{
		mutable float _get_buffer_cache[4][4];
		mutable float _get_buffer_cache_single[16];
	};
};

INLINE Matrix4x4SIMD operator*(float lhs, const Matrix4x4SIMD& rhs);

NS_END
