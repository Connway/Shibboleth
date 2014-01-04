/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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
#include <xmmintrin.h>

NS_GLEAM

class Vector4SIMD;

// Matrix4x4 elements are [column][row]
//
// | 00 10 20 30 |
// | 01 11 21 31 |
// | 02 12 22 23 |
// | 03 13 23 33 |

#ifdef ALIGN_SIMD
__declspec(align(16)) class Matrix4x4SIMD
#else
class Matrix4x4SIMD
#endif
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
	Matrix4x4SIMD(const __m128& vec1, const __m128& vec2,
				const __m128& vec3, const __m128& vec4);
	explicit Matrix4x4SIMD(const float* elements);
	~Matrix4x4SIMD(void);

	bool operator==(const Matrix4x4SIMD& rhs) const;
	INLINE bool operator!=(const Matrix4x4SIMD& rhs) const;
	INLINE const Matrix4x4SIMD& operator=(const Matrix4x4SIMD& rhs);

	const Matrix4x4SIMD& operator*=(const Matrix4x4SIMD& rhs);
	Matrix4x4SIMD operator*(const Matrix4x4SIMD& rhs) const;
	const Matrix4x4SIMD& operator+=(const Matrix4x4SIMD& rhs);
	Matrix4x4SIMD operator+(const Matrix4x4SIMD& rhs) const;
	Matrix4x4SIMD operator+(void) const;
	const Matrix4x4SIMD& operator-=(const Matrix4x4SIMD& rhs);
	Matrix4x4SIMD operator-(const Matrix4x4SIMD& rhs) const;
	Matrix4x4SIMD operator-(void) const;

	const Matrix4x4SIMD& operator/=(float rhs);
	Matrix4x4SIMD operator/(float rhs) const;
	const Matrix4x4SIMD& operator*=(float rhs);
	Matrix4x4SIMD operator*(float rhs) const;
	Vector4SIMD operator*(const Vector4SIMD& rhs) const;

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
	void set(const __m128& col1, const __m128& col2,
			const __m128& col3, const __m128& col4);
	INLINE void set(const float* elements);

	bool isIdentity(void) const;
	void setIdentity(void);

	bool hasInfiniteElement(void) const;
	bool hasNaNElement(void) const;

	void transpose(void);
	bool inverse(void);

	void setTranslate(const Vector4SIMD& translate);
	void setTranslate(float x, float y, float z);
	void setScale(float x, float y, float z);
	void setScale(const Vector4SIMD& scale);
	void setScale(float scale);
	void setRotation(float radians, const Vector4SIMD& axis);
	void setRotation(float radians, float x, float y, float z);
	void setRotationX(float radians);
	void setRotationY(float radians);
	void setRotationZ(float radians);

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

	bool roughlyEqual(const Matrix4x4SIMD& rhs, float epsilon = 0.000001f) const;

	static Matrix4x4SIMD identity;

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
	union
	{
		float _m[4][4];
		float _a[16];
	};
};

INLINE Matrix4x4SIMD operator*(float lhs, const Matrix4x4SIMD& rhs);

NS_END
