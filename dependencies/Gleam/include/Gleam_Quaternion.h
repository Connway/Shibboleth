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

#include "Gleam_Matrix4x4.h"
#include "Gleam_Vector4.h"

NS_GLEAM

class Quaternion
{
public:
	Quaternion(void);
	Quaternion(const Quaternion& rhs);
	Quaternion(float x, float y, float z, float w);
	explicit Quaternion(const float* elements);
	explicit Quaternion(const Vector4& vec);
	~Quaternion(void);

	bool operator==(const Quaternion& rhs) const;
	INLINE bool operator!=(const Quaternion& rhs) const;
	INLINE const Quaternion& operator=(const Quaternion& rhs);

	INLINE float operator[](int index) const;
	INLINE float& operator[](int index);

	void set(float x, float y, float z, float w);
	void set(const float* elements);
	void set(const Vector4& vec);

	INLINE const float* getBuffer(void) const;
	INLINE float* getBuffer(void);

	Quaternion operator+(const Quaternion& rhs) const;
	const Quaternion& operator+=(const Quaternion& rhs);
	Quaternion operator*(const Quaternion& rhs) const;
	const Quaternion& operator*=(const Quaternion& rhs);

	Vector4 transform(const Vector4& vec) const;

	bool roughlyEqual(const Quaternion& rhs, const Vector4& epsilon) const;
	bool roughlyEqual(const Quaternion& rhs, float epsilon = 0.000001f) const;

	Quaternion slerp(const Quaternion& rhs, float t);
	INLINE float dot(const Quaternion& rhs) const;
	void normalize(void);
	void conjugate(void);
	void inverse(void);

	INLINE float lengthSquared(void) const;
	INLINE float length(void) const;
	INLINE Vector4 axis(void) const;
	INLINE float angle(void) const;
	Matrix4x4 matrix(void) const;

	void constructFromAxis(const Vector4& axis, float angle);
	void constructFromMatrix(const Matrix4x4& matrix);
	void constructFromAngles(float x, float y, float z);
	INLINE void constructFromAngles(const Vector4& angles);

	static Quaternion identity;

	INLINE static Quaternion MakeFromAxis(const Vector4& axis, float angle);
	INLINE static Quaternion MakeFromMatrix(const Matrix4x4& matrix);
	INLINE static Quaternion MakeFromAngles(float x, float y, float z);
	INLINE static Quaternion MakeFromAngles(const Vector4& angles);

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
