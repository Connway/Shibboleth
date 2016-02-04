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

NS_GLEAM

class TransformCPU;
class Matrix4x4CPU;
class AABBCPU;
class OBBCPU;

class PlaneCPU
{
public:
	enum ContainResult
	{
		BACK = 0,
		FRONT,
		INTERSECTS
	};

	PlaneCPU(const Vector4CPU& point_1, const Vector4CPU& point_2, const Vector4CPU& point_3);
	PlaneCPU(const Vector4CPU& point, const Vector4CPU& normal);
	PlaneCPU(const Vector4CPU& normal, float distance);
	PlaneCPU(const Vector4CPU& raw);
	PlaneCPU(const PlaneCPU& plane);
	PlaneCPU(void);
	~PlaneCPU(void);

	const PlaneCPU& operator=(const PlaneCPU& rhs);

	INLINE const Vector4CPU& getRawRepresentation(void) const;
	INLINE Vector4CPU getNormal(void) const;
	INLINE Vector4CPU getPoint(void) const;

	INLINE Vector4CPU getDistanceVec(void) const;
	INLINE float getDistance(void) const;

	PlaneCPU normalize(void) const;
	void normalizeThis(void);

	INLINE ContainResult contains(const AABBCPU& aabb) const;
	INLINE ContainResult contains(const OBBCPU& obb) const;

	void transform(const TransformCPU& transform);
	void transform(const Matrix4x4CPU& matrix);

	void set(const Vector4CPU& point_1, const Vector4CPU& point_2, const Vector4CPU& point_3);
	void set(const Vector4CPU& point, const Vector4CPU& normal);
	void set(const Vector4CPU& normal, float distance);
	void set(const Vector4CPU& raw);

private:
	Vector4CPU _plane;
};

NS_END
