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

#ifdef ALIGN_SIMD
__declspec(align(16)) class Vector4SIMD
#else
class Vector4SIMD
#endif
{
public:
	Vector4SIMD(void);
	Vector4SIMD(const Vector4SIMD& rhs);
	Vector4SIMD(float x, float y, float z, float w);
	explicit Vector4SIMD(const float* elements);
	explicit Vector4SIMD(const __m128& rhs);
	~Vector4SIMD(void);

	bool operator==(const Vector4SIMD& rhs) const;
	INLINE bool operator!=(const Vector4SIMD& rhs) const;
	INLINE const Vector4SIMD& operator=(const Vector4SIMD& rhs);

	INLINE float operator[](int index) const;
	INLINE float& operator[](int index);

	const Vector4SIMD& operator-=(const Vector4SIMD& rhs);
	Vector4SIMD operator-(const Vector4SIMD& rhs) const;
	Vector4SIMD operator-(void) const;

	const Vector4SIMD& operator+=(const Vector4SIMD& rhs);
	Vector4SIMD operator+(const Vector4SIMD& rhs) const;
	Vector4SIMD operator+(void) const;

	const Vector4SIMD& operator*=(const Vector4SIMD& rhs);
	Vector4SIMD operator*(const Vector4SIMD& rhs) const;
	const Vector4SIMD& operator*=(float rhs);
	Vector4SIMD operator*(float rhs) const;

	const Vector4SIMD& operator/=(const Vector4SIMD& rhs);
	Vector4SIMD operator/(const Vector4SIMD& rhs) const;
	const Vector4SIMD& operator/=(float rhs);
	Vector4SIMD operator/(float rhs) const;

	void set(float x, float y, float z, float w);
	void set(const float* elements);

	INLINE const float* getBuffer(void) const;
	INLINE float* getBuffer(void);

	float lengthSquared(void) const;
	float length(void) const;
	float reciprocalLengthSquared(void) const;
	float reciprocalLength(void) const;
	void normalize(void);

	Vector4SIMD cross(const Vector4SIMD& rhs) const;
	float dot(const Vector4SIMD& rhs) const;
	float angleUnit(const Vector4SIMD& rhs) const;
	float angle(const Vector4SIMD& rhs) const;
	Vector4SIMD reflect(const Vector4SIMD& normal) const;
	Vector4SIMD refract(const Vector4SIMD& normal, float refraction_index) const;

	bool roughlyEqual(const Vector4SIMD& rhs, float epsilon = 0.000001f) const;

	static Vector4SIMD zero;
	static Vector4SIMD x_axis;
	static Vector4SIMD y_axis;
	static Vector4SIMD z_axis;
	static Vector4SIMD origin;

private:
	union
	{
		struct
		{
			float _x, _y, _z, _w;
		};

		float _vec[4];
	};
};

INLINE Vector4SIMD operator*(float lhs, const Vector4SIMD& rhs);

NS_END
