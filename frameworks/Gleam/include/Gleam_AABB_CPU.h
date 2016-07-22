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

#include "Gleam_Vector4_CPU.h"
#include "Gleam_Array.h"

NS_GLEAM

class TransformCPU;
class Matrix4x4CPU;

class AABBCPU
{
public:
	AABBCPU(const Vector4CPU& min, const Vector4CPU& max);
	AABBCPU(const AABBCPU& aabb);
	AABBCPU(void);
	~AABBCPU(void);

	INLINE const Vector4CPU& getMin(void) const;
	INLINE const Vector4CPU& getMax(void) const;
	INLINE Vector4CPU getExtent(void) const;
	INLINE Vector4CPU getCenter(void) const;
	INLINE void setMin(const Vector4CPU& min);
	INLINE void setMax(const Vector4CPU& max);

	INLINE void addPoint(float x, float y, float z);
	INLINE void addPoint(const float* point);
	INLINE void addPoint(const Vector4CPU& point);

	void addPoints(const float* points, unsigned int num_points, unsigned int stride = 3);
	void addPoints(const Vector4CPU* points, unsigned int num_points);
	void addPoints(const GleamArray<Vector4CPU>& points);

	INLINE void addAABB(const AABBCPU& aabb);

	INLINE void reset(void);

	const GleamArray<Vector4CPU>& generatePoints(GleamArray<Vector4CPU>& out) const;
	GleamArray<Vector4CPU> generatePoints(void) const;
	const Vector4CPU* generatePoints(Vector4CPU* out) const;

	void transform(const TransformCPU& transform);
	void transform(const Matrix4x4CPU& transform);
	INLINE bool contains(const Vector4CPU& point) const;

private:
	Vector4CPU _min;
	Vector4CPU _max;
	Vector4CPU _transform_cache[8];
};

NS_END
