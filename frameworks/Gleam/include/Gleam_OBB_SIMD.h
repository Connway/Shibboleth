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

#include "Gleam_Vector4_SIMD.h"
#include "Gleam_Array.h"

WARNING("SIMD data structures are potentially going to be deprecated.")

NS_GLEAM

class TransformSIMD;
class Matrix4x4SIMD;
class AABBSIMD;

class OBBSIMD
{
public:
	OBBSIMD(const Vector4SIMD& center, const Vector4SIMD& right, const Vector4SIMD& up, const Vector4SIMD& forward);
	OBBSIMD(const AABBSIMD& aabb, const Matrix4x4SIMD& transform);
	OBBSIMD(const AABBSIMD& aabb);
	OBBSIMD(const OBBSIMD& obb);
	OBBSIMD(void);
	~OBBSIMD(void);

	INLINE const Vector4SIMD& getCenter(void) const;
	INLINE Vector4SIMD getExtent(void) const;

	INLINE void setAxis(int axis, const Vector4SIMD& vec);
	INLINE const Vector4SIMD& getAxis(int axis) const;
	INLINE const Vector4SIMD* getAxes(void) const;

	INLINE const GleamArray<Vector4SIMD>& generatePoints(GleamArray<Vector4SIMD>& out) const;
	INLINE GleamArray<Vector4SIMD> generatePoints(void) const;
	const Vector4SIMD* generatePoints(Vector4SIMD* out) const;

	INLINE void transform(const TransformSIMD& transform);
	INLINE void transform(const Matrix4x4SIMD& transform);
	bool contains(const Vector4SIMD& point) const;

private:
	Vector4SIMD _center;
	Vector4SIMD _axes[3];
};

NS_END
