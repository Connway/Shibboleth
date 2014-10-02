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

#include "Gleam_Matrix4x4_SIMD.h"

NS_GLEAM

class COMPILERALIGN16 QuaternionSIMD
{
public:
	QuaternionSIMD(void);
	QuaternionSIMD(const QuaternionSIMD& rhs);
	QuaternionSIMD(float x, float y, float z, float w);
	explicit QuaternionSIMD(const float* elements);
	explicit QuaternionSIMD(const Vector4SIMD& vec);
	explicit QuaternionSIMD(const SIMDType& quat);
	~QuaternionSIMD(void);

	INLINE bool operator==(const QuaternionSIMD& rhs) const;
	INLINE bool operator!=(const QuaternionSIMD& rhs) const;
	INLINE const QuaternionSIMD& operator=(const QuaternionSIMD& rhs);

	INLINE float operator[](int index) const;
	//INLINE float& operator[](int index);

	INLINE void set(float x, float y, float z, float w);
	INLINE void set(const float* elements);
	INLINE void set(const Vector4SIMD& vec);
	INLINE void set(float value, unsigned int index);

	INLINE const float* getBuffer(void) const;
	//INLINE float* getBuffer(void);
	INLINE const SIMDType& getSIMDType(void) const;

	INLINE QuaternionSIMD operator+(const QuaternionSIMD& rhs) const;
	INLINE const QuaternionSIMD& operator+=(const QuaternionSIMD& rhs);
	INLINE QuaternionSIMD operator*(const QuaternionSIMD& rhs) const;
	INLINE const QuaternionSIMD& operator*=(const QuaternionSIMD& rhs);

	INLINE Vector4SIMD transform(const Vector4SIMD& vec) const;

	INLINE bool roughlyEqual(const QuaternionSIMD& rhs, const Vector4SIMD& epsilon) const;
	INLINE bool roughlyEqual(const QuaternionSIMD& rhs, float epsilon = 0.000001f) const;

	INLINE QuaternionSIMD slerp(const QuaternionSIMD& rhs, float t) const;
	INLINE void slerpThis(const QuaternionSIMD& rhs, float t);
	INLINE float dot(const QuaternionSIMD& rhs) const;
	INLINE void normalizeThis(void);
	INLINE QuaternionSIMD normalize(void) const;
	INLINE void conjugateThis(void);
	INLINE QuaternionSIMD conjugate(void) const;
	INLINE void inverseThis(void);
	INLINE QuaternionSIMD inverse(void) const;
	INLINE void shortestRotationThis(const Vector4SIMD& vec1, const Vector4SIMD& vec2);
	INLINE QuaternionSIMD shortestRotation(const Vector4SIMD& vec1, const Vector4SIMD& vec2) const;

	INLINE float lengthSquared(void) const;
	INLINE float length(void) const;
	INLINE Vector4SIMD axis(void) const;
	INLINE float angle(void) const;
	INLINE Matrix4x4SIMD matrix(void) const;

	INLINE void constructFromAxis(const Vector4SIMD& axis, float angle);
	INLINE void constructFromMatrix(const Matrix4x4SIMD& matrix);
	INLINE void constructFromAngles(float x, float y, float z);
	INLINE void constructFromAngles(const Vector4SIMD& angles);

	// SIMD compatability
	INLINE void slerpThis(const QuaternionSIMD& rhs, const Vector4SIMD& t);
	INLINE QuaternionSIMD slerp(const QuaternionSIMD& rhs, const Vector4SIMD& t) const;
	INLINE QuaternionSIMD dotVec(const QuaternionSIMD& rhs) const;
	INLINE Vector4SIMD lengthSquaredVec(void) const;
	INLINE Vector4SIMD lengthVec(void) const;
	INLINE Vector4SIMD angleVec(void) const;

	INLINE void constructFromAxis(const Vector4SIMD& axis, const Vector4SIMD& angle);

	static QuaternionSIMD Identity;

	INLINE static QuaternionSIMD MakeFromAxis(const Vector4SIMD& axis, const Vector4SIMD& angle);
	INLINE static QuaternionSIMD MakeFromAxis(const Vector4SIMD& axis, float angle);
	INLINE static QuaternionSIMD MakeFromMatrix(const Matrix4x4SIMD& matrix);
	INLINE static QuaternionSIMD MakeFromAngles(float x, float y, float z);
	INLINE static QuaternionSIMD MakeFromAngles(const Vector4SIMD& angles);

private:
	SIMDType _quat;
	mutable float _get_buf_cache[4];
};

NS_END
