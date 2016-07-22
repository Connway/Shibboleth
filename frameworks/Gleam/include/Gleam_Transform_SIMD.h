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

#include "Gleam_Quaternion_SIMD.h"
#include "Gleam_Matrix4x4_SIMD.h"
#include "Gleam_Vector4_SIMD.h"

WARNING("SIMD data structures are potentially going to be deprecated.")

NS_GLEAM

class alignas(16) TransformSIMD
{
public:
	TransformSIMD(const Vector4SIMD& scale, const QuaternionSIMD& rotation, const Vector4SIMD& translation);
	TransformSIMD(void);
	~TransformSIMD(void);

	const TransformSIMD& operator=(const TransformSIMD& rhs);
	bool operator==(const TransformSIMD& rhs) const;
	bool operator!=(const TransformSIMD& rhs) const;

	const TransformSIMD& operator+=(const TransformSIMD& rhs);
	TransformSIMD operator+(const TransformSIMD& rhs) const;

	INLINE const Vector4SIMD& getScale(void) const;
	INLINE void setScale(const Vector4SIMD& scale);
	INLINE const QuaternionSIMD& getRotation(void) const;
	INLINE void setRotation(const QuaternionSIMD& rotation);
	INLINE const Vector4SIMD& getTranslation(void) const;
	INLINE void setTranslation(const Vector4SIMD& translation);

	TransformSIMD concat(const TransformSIMD& rhs) const;
	TransformSIMD inverse(void) const;
	void concatThis(const TransformSIMD& rhs);
	void inverseThis(void);

	Vector4SIMD transform(const Vector4SIMD& rhs) const;
	Matrix4x4SIMD matrix(void) const;

	TransformSIMD lerp(const TransformSIMD& end, const Vector4SIMD& t);
	void lerpThis(const TransformSIMD& end, const Vector4SIMD& t);

	INLINE TransformSIMD lerp(const TransformSIMD& end, float t);
	INLINE void lerpThis(const TransformSIMD& end, float t);

private:
	QuaternionSIMD _rotation;
	Vector4SIMD _translation;
	Vector4SIMD _scale;
};

NS_END
