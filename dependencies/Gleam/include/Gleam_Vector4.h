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

class Vector4
{
public:
	Vector4(void);
	Vector4(const Vector4& rhs);
	Vector4(float x, float y, float z, float w);
	explicit Vector4(const float* elements);
	~Vector4(void);

	bool operator==(const Vector4& rhs) const;
	INLINE bool operator!=(const Vector4& rhs) const;
	INLINE const Vector4& operator=(const Vector4& rhs);

	INLINE float operator[](int index) const;
	INLINE float& operator[](int index);

	const Vector4& operator-=(const Vector4& rhs);
	Vector4 operator-(const Vector4& rhs) const;
	Vector4 operator-(void) const;

	const Vector4& operator+=(const Vector4& rhs);
	Vector4 operator+(const Vector4& rhs) const;
	Vector4 operator+(void) const;

	const Vector4& operator*=(const Vector4& rhs);
	Vector4 operator*(const Vector4& rhs) const;
	const Vector4& operator*=(float rhs);
	Vector4 operator*(float rhs) const;

	const Vector4& operator/=(const Vector4& rhs);
	Vector4 operator/(const Vector4& rhs) const;
	const Vector4& operator/=(float rhs);
	Vector4 operator/(float rhs) const;

	void set(float x, float y, float z, float w);
	void set(const float* elements);
	void set(float value, unsigned int index);

	INLINE const float* getBuffer(void) const;
	INLINE float* getBuffer(void);

	float lengthSquared(void) const;
	float length(void) const;
	float reciprocalLengthSquared(void) const;
	float reciprocalLength(void) const;
	void normalize(void);

	Vector4 cross(const Vector4& rhs) const;
	float dot(const Vector4& rhs) const;
	float angleUnit(const Vector4& rhs) const;
	float angle(const Vector4& rhs) const;
	Vector4 reflect(const Vector4& normal) const;
	Vector4 refract(const Vector4& normal, float refraction_index) const;
	Vector4 lerp(const Vector4& end, float t) const;

	bool roughlyEqual(const Vector4& rhs, float epsilon = 0.000001f) const;

	static Vector4 zero;
	static Vector4 x_axis;
	static Vector4 y_axis;
	static Vector4 z_axis;
	static Vector4 origin;

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

INLINE Vector4 operator*(float lhs, const Vector4& rhs);

typedef Vector4 Vec4;

NS_END
