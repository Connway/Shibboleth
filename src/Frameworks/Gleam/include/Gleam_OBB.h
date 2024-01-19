/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

NS_GLEAM

class Transform;
class AABB;

class OBB
{
public:
	OBB(const Vec3& center, const Vec3& right, const Vec3& up, const Vec3& forward);
	OBB(const AABB& aabb, const Mat4x4& transform);
	OBB(const AABB& aabb, const Transform& transform);
	OBB(const AABB& aabb);
	OBB(const OBB& obb);
	OBB(void);
	~OBB(void);

 	const Vec3& getCenter(void) const;
	Vec3 getExtent(void) const;

	void setAxis(int axis, const Vec3& vec);
	const Vec3& getAxis(int axis) const;
	const Vec3* getAxes(void) const;

	Vec3* generatePoints(Vec3* out) const;

	void transform(const Mat4x4& transform);
	void transform(const Transform& transform);
	bool contains(const Vec3& point) const;

private:
	Vec3 _center;
	Vec3 _axes[3];
};

NS_END
