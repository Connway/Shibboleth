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

#include "Gleam_Matrix4x4_CPU.h"

NS_GLEAM

class QuaternionCPU
{
public:
	QuaternionCPU(void);
	QuaternionCPU(const QuaternionCPU& rhs);
	QuaternionCPU(float x, float y, float z, float w);
	explicit QuaternionCPU(const float* elements);
	explicit QuaternionCPU(const Vector4CPU& vec);
	~QuaternionCPU(void);

	bool operator==(const QuaternionCPU& rhs) const;
	INLINE bool operator!=(const QuaternionCPU& rhs) const;
	INLINE const QuaternionCPU& operator=(const QuaternionCPU& rhs);

	INLINE float operator[](int index) const;
	INLINE float& operator[](int index);

	void set(float x, float y, float z, float w);
	void set(const float* elements);
	void set(const Vector4CPU& vec);
	INLINE void set(float value, unsigned int index);

	INLINE const float* getBuffer(void) const;
	//INLINE float* getBuffer(void);

	QuaternionCPU operator+(const QuaternionCPU& rhs) const;
	const QuaternionCPU& operator+=(const QuaternionCPU& rhs);
	QuaternionCPU operator*(const QuaternionCPU& rhs) const;
	const QuaternionCPU& operator*=(const QuaternionCPU& rhs);

	Vector4CPU transform(const Vector4CPU& vec) const;

	bool roughlyEqual(const QuaternionCPU& rhs, const Vector4CPU& epsilon = Vector4CPU::Epsilon) const;
	bool roughlyEqual(const QuaternionCPU& rhs, float epsilon = 0.000001f) const;

	QuaternionCPU slerp(const QuaternionCPU& rhs, float t) const;
	void slerpThis(const QuaternionCPU& rhs, float t);
	float dot(const QuaternionCPU& rhs) const;
	void normalizeThis(void);
	QuaternionCPU normalize(void) const;
	void conjugateThis(void);
	QuaternionCPU conjugate(void) const;
	void inverseThis(void);
	QuaternionCPU inverse(void) const;
	void shortestRotationThis(const Vector4CPU& vec1, const Vector4CPU& vec2);
	QuaternionCPU shortestRotation(const Vector4CPU& vec1, const Vector4CPU& vec2) const;

	INLINE float lengthSquared(void) const;
	INLINE float length(void) const;
	INLINE Vector4CPU axis(void) const;
	INLINE float angle(void) const;
	Matrix4x4CPU matrix(void) const;

	void constructFromAxis(const Vector4CPU& axis, float angle);
	void constructFromMatrix(const Matrix4x4CPU& matrix);
	void constructFromAngles(float x, float y, float z);
	INLINE void constructFromAngles(const Vector4CPU& angles);

	// SIMD compatability
	INLINE void slerpThis(const QuaternionCPU& rhs, const Vector4CPU& t);
	INLINE QuaternionCPU slerp(const QuaternionCPU& rhs, const Vector4CPU& t) const;
	INLINE QuaternionCPU dotVec(const QuaternionCPU& rhs) const;
	INLINE Vector4CPU lengthSquaredVec(void) const;
	INLINE Vector4CPU lengthVec(void) const;
	INLINE Vector4CPU angleVec(void) const;

	INLINE void constructFromAxis(const Vector4CPU& axis, const Vector4CPU& angle);

	static QuaternionCPU Identity;

	INLINE static QuaternionCPU MakeFromAxis(const Vector4CPU& axis, const Vector4CPU& angle);
	INLINE static QuaternionCPU MakeFromAxis(const Vector4CPU& axis, float angle);
	INLINE static QuaternionCPU MakeFromMatrix(const Matrix4x4CPU& matrix);
	INLINE static QuaternionCPU MakeFromAngles(float x, float y, float z);
	INLINE static QuaternionCPU MakeFromAngles(const Vector4CPU& angles);

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
