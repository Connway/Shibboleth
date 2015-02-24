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

NS_GLEAM

class TransformSIMD;
class Matrix4x4SIMD;
class AABBSIMD;
class OBBSIMD;

class PlaneSIMD
{
public:
	enum ContainResult
	{
		BACK = 0,
		FRONT,
		INTERSECTS
	};

	PlaneSIMD(const Vector4SIMD& point_1, const Vector4SIMD& point_2, const Vector4SIMD& point_3);
	PlaneSIMD(const Vector4SIMD& point, const Vector4SIMD& normal);
	PlaneSIMD(const Vector4SIMD& normal, float distance);
	PlaneSIMD(const Vector4SIMD& raw);
	PlaneSIMD(const PlaneSIMD& plane);
	PlaneSIMD(void);
	~PlaneSIMD(void);

	const PlaneSIMD& operator=(const PlaneSIMD& rhs);

	INLINE const Vector4SIMD& getRawRepresentation(void) const;
	INLINE Vector4SIMD getNormal(void) const;
	INLINE Vector4SIMD getPoint(void) const;

	INLINE Vector4SIMD getDistanceVec(void) const;
	INLINE float getDistance(void) const;

	PlaneSIMD normalize(void) const;
	void normalizeThis(void);

	INLINE ContainResult contains(const AABBSIMD& aabb) const;
	INLINE ContainResult contains(const OBBSIMD& obb) const;

	void transform(const TransformSIMD& transform);
	void transform(const Matrix4x4SIMD& matrix);

	void set(const Vector4SIMD& point_1, const Vector4SIMD& point_2, const Vector4SIMD& point_3);
	void set(const Vector4SIMD& point, const Vector4SIMD& normal);
	void set(const Vector4SIMD& normal, float distance);
	void set(const Vector4SIMD& raw);

private:
	Vector4SIMD _plane;
};

NS_END
