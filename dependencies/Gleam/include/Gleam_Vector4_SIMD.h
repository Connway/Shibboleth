/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

class COMPILERALIGN16 Vector4SIMD
{
public:
	Vector4SIMD(void);
	Vector4SIMD(const Vector4SIMD& rhs);
	Vector4SIMD(float x, float y, float z, float w);
	explicit Vector4SIMD(const float* elements);
	explicit Vector4SIMD(const SIMDType& value);
	explicit Vector4SIMD(float value);
	~Vector4SIMD(void);

	INLINE bool operator==(const Vector4SIMD& rhs) const;
	INLINE bool operator!=(const Vector4SIMD& rhs) const;
	INLINE const Vector4SIMD& operator=(const Vector4SIMD& rhs);

	INLINE float operator[](int index) const;

	INLINE const Vector4SIMD& operator-=(const Vector4SIMD& rhs);
	INLINE Vector4SIMD operator-(const Vector4SIMD& rhs) const;
	INLINE Vector4SIMD operator-(void) const;

	INLINE const Vector4SIMD& operator+=(const Vector4SIMD& rhs);
	INLINE Vector4SIMD operator+(const Vector4SIMD& rhs) const;
	INLINE Vector4SIMD operator+(void) const;

	INLINE const Vector4SIMD& operator*=(const Vector4SIMD& rhs);
	INLINE Vector4SIMD operator*(const Vector4SIMD& rhs) const;
	INLINE const Vector4SIMD& operator*=(float rhs);
	INLINE Vector4SIMD operator*(float rhs) const;

	INLINE const Vector4SIMD& operator/=(const Vector4SIMD& rhs);
	INLINE Vector4SIMD operator/(const Vector4SIMD& rhs) const;
	INLINE const Vector4SIMD& operator/=(float rhs);
	INLINE Vector4SIMD operator/(float rhs) const;

	INLINE const Vector4SIMD& operator&=(const Vector4SIMD& rhs);
	INLINE Vector4SIMD operator&(const Vector4SIMD& rhs) const;

	INLINE const Vector4SIMD& operator|=(const Vector4SIMD& rhs);
	INLINE Vector4SIMD operator|(const Vector4SIMD& rhs) const;

	INLINE const Vector4SIMD& operator^=(const Vector4SIMD& rhs);
	INLINE Vector4SIMD operator^(const Vector4SIMD& rhs) const;

	INLINE bool operator<=(const Vector4SIMD& rhs) const;
	INLINE bool operator<(const Vector4SIMD& rhs) const;
	INLINE bool operator>=(const Vector4SIMD& rhs) const;
	INLINE bool operator>(const Vector4SIMD& rhs) const;

	INLINE void set(float x, float y, float z, float w);
	INLINE void set(const float* elements);
	INLINE void set(float value, unsigned int index);

	INLINE Vector4SIMD get(unsigned int index) const;

	INLINE const float* getBuffer(void) const;
	INLINE const SIMDType& getSIMDType(void) const;

	INLINE float lengthSquared(void) const;
	INLINE float length(void) const;
	INLINE float reciprocalLengthSquared(void) const;
	INLINE float reciprocalLength(void) const;
	INLINE Vector4SIMD normalize(void) const;

	INLINE Vector4SIMD cross(const Vector4SIMD& rhs) const;
	INLINE float dot(const Vector4SIMD& rhs) const;
	INLINE float angleUnit(const Vector4SIMD& rhs) const;
	INLINE float angle(const Vector4SIMD& rhs) const;
	INLINE Vector4SIMD reflect(const Vector4SIMD& normal) const;
	INLINE Vector4SIMD refract(const Vector4SIMD& normal, float refraction_index) const;
	INLINE Vector4SIMD lerp(const Vector4SIMD& end, float t) const;
	INLINE Vector4SIMD minimum(const Vector4SIMD& rhs) const;
	INLINE Vector4SIMD maximum(const Vector4SIMD& rhs) const;

	INLINE void reflectThis(const Vector4SIMD& normal);
	INLINE void refractThis(const Vector4SIMD& normal, float refraction_index);
	INLINE void lerpThis(const Vector4SIMD& end, float t);
	INLINE void minimumThis(const Vector4SIMD& rhs);
	INLINE void maximumThis(const Vector4SIMD& rhs);
	INLINE void crossThis(const Vector4SIMD& rhs);
	INLINE void normalizeThis(void);

	INLINE bool roughlyEqual(const Vector4SIMD& rhs, float epsilon = 0.000001f) const;

	// For compatability with SIMD version
	INLINE Vector4SIMD lengthSquaredVec(void) const;
	INLINE Vector4SIMD lengthVec(void) const;
	INLINE Vector4SIMD reciprocalLengthSquaredVec(void) const;
	INLINE Vector4SIMD reciprocalLengthVec(void) const;
	INLINE Vector4SIMD dotVec(const Vector4SIMD& rhs) const;
	INLINE Vector4SIMD angleUnitVec(const Vector4SIMD& rhs) const;
	INLINE Vector4SIMD angleVec(const Vector4SIMD& rhs) const;

	INLINE Vector4SIMD refract(const Vector4SIMD& normal, const Vector4SIMD& refraction_index) const;
	INLINE Vector4SIMD lerp(const Vector4SIMD& end, const Vector4SIMD& t) const;
	INLINE bool roughlyEqual(const Vector4SIMD& rhs, const Vector4SIMD& epsilon = Epsilon) const;

	INLINE void refractThis(const Vector4SIMD& normal, const Vector4SIMD& refraction_index);
	INLINE void lerpThis(const Vector4SIMD& end, const Vector4SIMD& t);

	static Vector4SIMD Zero;
	static Vector4SIMD X_Axis;
	static Vector4SIMD Y_Axis;
	static Vector4SIMD Z_Axis;
	static Vector4SIMD Origin;
	static Vector4SIMD Epsilon;

private:
	SIMDType _vec;
	mutable float _get_buf_cache[4];
};

INLINE Vector4SIMD operator*(float lhs, const Vector4SIMD& rhs);

NS_END
