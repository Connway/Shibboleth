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

#include "Gleam_SIMDMath.h"

NS_GLEAM

class Matrix4x4SIMD;
class Vector4SIMD;

class ALIGN16 QuaternionSIMD
{
public:
	QuaternionSIMD(void);
	QuaternionSIMD(const QuaternionSIMD& rhs);
	QuaternionSIMD(float x, float y, float z, float w);
	explicit QuaternionSIMD(const float* elements);
	explicit QuaternionSIMD(const Vector4SIMD& vec);
	explicit QuaternionSIMD(const __m128& rhs);
	~QuaternionSIMD(void);

	bool operator==(const QuaternionSIMD& rhs) const;
	INLINE bool operator!=(const QuaternionSIMD& rhs) const;
	INLINE const QuaternionSIMD& operator=(const QuaternionSIMD& rhs);

	INLINE float operator[](int index) const;
	INLINE float& operator[](int index);

	void set(float x, float y, float z, float w);
	void set(const float* elements);
	void set(const Vector4SIMD& vec);

	INLINE const float* getBuffer(void) const;
	INLINE float* getBuffer(void);

	QuaternionSIMD operator+(const QuaternionSIMD& rhs) const;
	const QuaternionSIMD& operator+=(const QuaternionSIMD& rhs);
	QuaternionSIMD operator*(const QuaternionSIMD& rhs) const;
	const QuaternionSIMD& operator*=(const QuaternionSIMD& rhs);

	Vector4SIMD transform(const Vector4SIMD& vec) const;

	bool roughlyEqual(const QuaternionSIMD& rhs, float epsilon = 0.000001f) const;

	QuaternionSIMD slerp(const QuaternionSIMD& rhs, float t);
	float dot(const QuaternionSIMD& rhs) const;
	void normalize(void);
	void conjugate(void);
	void inverse(void);

	float lengthSquared(void) const;
	float length(void) const;
	Vector4SIMD axis(void) const;
	float angle(void) const;
	Matrix4x4SIMD matrix(void) const;

	void constructFromAxis(const Vector4SIMD& axis, float angle);
	void constructFromMatrix(const Matrix4x4SIMD& matrix);
	void constructFromAngles(float x, float y, float z);
	INLINE void constructFromAngles(const Vector4SIMD& angles);

	static QuaternionSIMD identity;

	INLINE static QuaternionSIMD MakeFromAxis(const Vector4SIMD& axis, float angle);
	INLINE static QuaternionSIMD MakeFromMatrix(const Matrix4x4SIMD& matrix);
	INLINE static QuaternionSIMD MakeFromAngles(float x, float y, float z);
	INLINE static QuaternionSIMD MakeFromAngles(const Vector4SIMD& angles);

private:
	union
	{
		struct
		{
			float _x, _y, _z, _w;
		};

		float _quat[4];
	};
};

NS_END
