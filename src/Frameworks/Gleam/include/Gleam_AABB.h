/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Gleam_Matrix4x4.h"
#include "Gleam_Vec3.h"
#include <gtc/constants.hpp>

NS_GLEAM

class Transform;

class AABB
{
public:
	AABB(const Vec3& min = glm::zero<Vec3>(), const Vec3& max = glm::zero<Vec3>());
	AABB(const AABB& aabb);

	AABB& operator=(const AABB& rhs);

	const Vec3& getMin(void) const;
	const Vec3& getMax(void) const;
	Vec3 getExtent(void) const;
	Vec3 getCenter(void) const;
	void setMin(const Vec3& min);
	void setMax(const Vec3& max);

	void addPoint(float x, float y, float z);
	void addPoint(const float* point);
	void addPoint(const Vec3& point);

	void addPoints(const float* points, int32_t num_points, int32_t stride = 3);
	void addPoints(const Vec3* points, int32_t num_points);

	void addAABB(const AABB& aabb);

	void reset(void);

	Vec3* generatePoints(Vec3* out) const;

	void transform(const Mat4x4& transform);
	void transform(const Transform& transform);
	bool contains(const Vec3& point) const;

private:
	Vec3 _min;
	Vec3 _max;
	Vec3 _transform_cache[8];
};

NS_END
