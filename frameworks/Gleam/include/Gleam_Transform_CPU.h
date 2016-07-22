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

#include "Gleam_Quaternion_CPU.h"
#include "Gleam_Matrix4x4_CPU.h"
#include "Gleam_Vector4_CPU.h"

NS_GLEAM

class TransformCPU
{
public:
	TransformCPU(const Vector4CPU& scale, const QuaternionCPU& rotation, const Vector4CPU& translation);
	TransformCPU(void);
	~TransformCPU(void);

	const TransformCPU& operator=(const TransformCPU& rhs);
	bool operator==(const TransformCPU& rhs) const;
	bool operator!=(const TransformCPU& rhs) const;

	const TransformCPU& operator+=(const TransformCPU& rhs);
	TransformCPU operator+(const TransformCPU& rhs) const;

	INLINE const Vector4CPU& getScale(void) const;
	INLINE void setScale(const Vector4CPU& scale);
	INLINE const QuaternionCPU& getRotation(void) const;
	INLINE void setRotation(const QuaternionCPU& rotation);
	INLINE const Vector4CPU& getTranslation(void) const;
	INLINE void setTranslation(const Vector4CPU& translation);

	TransformCPU concat(const TransformCPU& rhs) const;
	TransformCPU inverse(void) const;
	void concatThis(const TransformCPU& rhs);
	void inverseThis(void);

	Vector4CPU transformVector(const Vector4CPU& rhs) const;
	Vector4CPU transformPoint(const Vector4CPU& rhs) const;
	Matrix4x4CPU matrix(void) const;

	TransformCPU lerp(const TransformCPU& end, const Vector4CPU& t);
	void lerpThis(const TransformCPU& end, const Vector4CPU& t);

	INLINE TransformCPU lerp(const TransformCPU& end, float t);
	INLINE void lerpThis(const TransformCPU& end, float t);

private:
	QuaternionCPU _rotation;
	Vector4CPU _translation;
	Vector4CPU _scale;
};

NS_END
