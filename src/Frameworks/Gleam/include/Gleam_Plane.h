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

#include "Gleam_Vec3.h"
#include <Gaff_Defines.h>

NS_GLEAM

class Transform;
class AABB;
class OBB;

class Plane
{
public:
	enum ContainResult
	{
		BACK = 0,
		FRONT,
		INTERSECTS
	};

	Plane(const Vec3& point_1, const Vec3& point_2, const Vec3& point_3);
	Plane(const Vec3& point, const Vec3& normal);
	Plane(const Vec3& normal, float distance);

	GAFF_STRUCTORS_DEFAULT(Plane);
	GAFF_COPY_DEFAULT(Plane);

	Vec3 getPoint(void) const;

	void setNormal(const Vec3& normal);
	Vec3 getNormal(void) const;

	void setDistance(float distance);
	float getDistance(void) const;

	ContainResult contains(const AABB& aabb) const;
	ContainResult contains(const OBB& obb) const;

	void transform(const Transform& transform);

	void set(const Vec3& point_1, const Vec3& point_2, const Vec3& point_3);
	void set(const Vec3& point, const Vec3& normal);
	void set(const Vec3& normal, float distance);

private:
	Vec3 _normal;
	float _distance;
};

NS_END
