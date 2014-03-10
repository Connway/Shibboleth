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

class Vector4CPU
{
public:
	Vector4CPU(void);
	Vector4CPU(const Vector4CPU& rhs);
	Vector4CPU(float x, float y, float z, float w);
	explicit Vector4CPU(const float* elements);
	~Vector4CPU(void);

	bool operator==(const Vector4CPU& rhs) const;
	INLINE bool operator!=(const Vector4CPU& rhs) const;
	INLINE const Vector4CPU& operator=(const Vector4CPU& rhs);

	INLINE float operator[](int index) const;
	INLINE float& operator[](int index);

	const Vector4CPU& operator-=(const Vector4CPU& rhs);
	Vector4CPU operator-(const Vector4CPU& rhs) const;
	Vector4CPU operator-(void) const;

	const Vector4CPU& operator+=(const Vector4CPU& rhs);
	Vector4CPU operator+(const Vector4CPU& rhs) const;
	Vector4CPU operator+(void) const;

	const Vector4CPU& operator*=(const Vector4CPU& rhs);
	Vector4CPU operator*(const Vector4CPU& rhs) const;
	const Vector4CPU& operator*=(float rhs);
	Vector4CPU operator*(float rhs) const;

	const Vector4CPU& operator/=(const Vector4CPU& rhs);
	Vector4CPU operator/(const Vector4CPU& rhs) const;
	const Vector4CPU& operator/=(float rhs);
	Vector4CPU operator/(float rhs) const;

	void set(float x, float y, float z, float w);
	void set(const float* elements);

	INLINE const float* getBuffer(void) const;
	INLINE float* getBuffer(void);

	float lengthSquared(void) const;
	float length(void) const;
	float reciprocalLengthSquared(void) const;
	float reciprocalLength(void) const;
	void normalize(void);

	Vector4CPU cross(const Vector4CPU& rhs) const;
	float dot(const Vector4CPU& rhs) const;
	float angleUnit(const Vector4CPU& rhs) const;
	float angle(const Vector4CPU& rhs) const;
	Vector4CPU reflect(const Vector4CPU& normal) const;
	Vector4CPU refract(const Vector4CPU& normal, float refraction_index) const;
	Vector4CPU lerp(const Vector4CPU& end, float t) const;

	bool roughlyEqual(const Vector4CPU& rhs, float epsilon = 0.000001f) const;

	static Vector4CPU zero;
	static Vector4CPU x_axis;
	static Vector4CPU y_axis;
	static Vector4CPU z_axis;
	static Vector4CPU origin;

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

INLINE Vector4CPU operator*(float lhs, const Vector4CPU& rhs);

NS_END
