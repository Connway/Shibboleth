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

#include "Gleam_Vector4_SIMD.h"
#include "Gleam_Array.h"

WARNING("SIMD data structures are potentially going to be deprecated.")

NS_GLEAM

class TransformSIMD;
class Matrix4x4SIMD;

class AABBSIMD
{
public:
	AABBSIMD(const Vector4SIMD& min, const Vector4SIMD& max);
	AABBSIMD(const AABBSIMD& aabb);
	AABBSIMD(void);
	~AABBSIMD(void);

	INLINE const Vector4SIMD& getMin(void) const;
	INLINE const Vector4SIMD& getMax(void) const;
	INLINE Vector4SIMD getExtent(void) const;
	INLINE Vector4SIMD getCenter(void) const;
	INLINE void setMin(const Vector4SIMD& min);
	INLINE void setMax(const Vector4SIMD& max);

	INLINE void addPoint(float x, float y, float z);
	INLINE void addPoint(const float* point);
	INLINE void addPoint(const Vector4SIMD& point);

	void addPoints(const float* points, unsigned int num_points, unsigned int stride = 3);
	void addPoints(const Vector4SIMD* points, unsigned int num_points);
	void addPoints(const GleamArray<Vector4SIMD>& points);

	INLINE void addAABB(const AABBSIMD& aabb);

	INLINE void reset(void);

	const GleamArray<Vector4SIMD>& generatePoints(GleamArray<Vector4SIMD>& out) const;
	GleamArray<Vector4SIMD> generatePoints(void) const;
	const Vector4SIMD* generatePoints(Vector4SIMD* out) const;

	void transform(const TransformSIMD& transform);
	void transform(const Matrix4x4SIMD& transform);
	INLINE bool contains(const Vector4SIMD& point) const;

private:
	Vector4SIMD _min;
	Vector4SIMD _max;
	Vector4SIMD _transform_cache[8];
};

NS_END
