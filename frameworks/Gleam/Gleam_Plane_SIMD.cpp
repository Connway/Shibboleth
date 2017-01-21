/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gleam_Plane_SIMD.h"
#include "Gleam_Transform_SIMD.h"
#include "Gleam_Matrix4x4_SIMD.h"
#include "Gleam_AABB_SIMD.h"
#include "Gleam_OBB_SIMD.h"
#include <cmath>

NS_GLEAM

PlaneSIMD::PlaneSIMD(const Vector4SIMD& point_1, const Vector4SIMD& point_2, const Vector4SIMD& point_3):
	_plane(point_2 - point_1)
{
	Vector4SIMD a = point_3 - point_1;
	_plane.crossThis(a);
	_plane.normalizeThis();

	_plane |= (point_1.dotVec(_plane) & Vector4SIMD(gWMask));
}

PlaneSIMD::PlaneSIMD(const Vector4SIMD& point, const Vector4SIMD& normal):
	_plane(normal)
{
	// assumes normal is correctly formatted with w set to zero.
	_plane.set(point.dot(normal), 3);
}

PlaneSIMD::PlaneSIMD(const Vector4SIMD& normal, float distance):
	_plane(normal)
{
	_plane.set(distance, 3);
}

PlaneSIMD::PlaneSIMD(const Vector4SIMD& raw):
	_plane(raw)
{
}

PlaneSIMD::PlaneSIMD(const PlaneSIMD& plane):
	_plane(plane._plane)
{
}

PlaneSIMD::PlaneSIMD(void)
{
}

PlaneSIMD::~PlaneSIMD(void)
{
}

const PlaneSIMD& PlaneSIMD::operator=(const PlaneSIMD& rhs)
{
	_plane = rhs._plane;
	return *this;
}

const Vector4SIMD& PlaneSIMD::getRawRepresentation(void) const
{
	return _plane;
}

Vector4SIMD PlaneSIMD::getNormal(void) const
{
	return _plane & Vector4SIMD(gXYZMask);
}

Vector4SIMD PlaneSIMD::getPoint(void) const
{
	return getNormal() * _plane[3];
}

Vector4SIMD PlaneSIMD::getDistanceVec(void) const
{
	return _plane.get(3);
}

float PlaneSIMD::getDistance(void) const
{
	return _plane[3];
}

PlaneSIMD PlaneSIMD::normalize(void) const
{
	Vector4SIMD plane = getNormal();
	Vector4SIMD distance = _plane.get(3) & Vector4SIMD(gWMask);
	plane.normalizeThis();
	plane |= distance;

	return PlaneSIMD(plane);
}

void PlaneSIMD::normalizeThis(void)
{
	Vector4SIMD distance = _plane.get(3) & Vector4SIMD(gWMask);
	_plane &= Vector4SIMD(gXYZMask);
	_plane.normalizeThis();
	_plane |= distance;
}

PlaneSIMD::ContainResult PlaneSIMD::contains(const AABBSIMD& aabb) const
{
	Vector4SIMD normal = getNormal();

	Vector4SIMD dist = aabb.getCenter().dotVec(normal);
	Vector4SIMD size = +aabb.getExtent().dotVec(normal); // unary plus makes sure all elements are positive
	Vector4SIMD d = _plane.get(3);

	if ((dist + size) < d) {
		return BACK;
	} else if ((dist - size) < d) {
		return INTERSECTS;
	}

	return FRONT;
}

PlaneSIMD::ContainResult PlaneSIMD::contains(const OBBSIMD& obb) const
{
	Vector4SIMD normal = getNormal();
	const Vector4SIMD* axes = obb.getAxes();

	Vector4SIMD dist_center = obb.getCenter().dotVec(normal);
	Vector4SIMD dist_x = +axes[0].dotVec(normal);
	Vector4SIMD dist_y = +axes[1].dotVec(normal);
	Vector4SIMD dist_z = +axes[2].dotVec(normal);

	Vector4SIMD dist_max = dist_center + dist_x + dist_y + dist_z;
	Vector4SIMD dist_min = dist_center - dist_x - dist_y - dist_z;

	Vector4SIMD d = _plane.get(3);

	if (dist_max < d) {
		return BACK;
	} else if (dist_min < d) {
		return INTERSECTS;
	}

	return FRONT;
}

void PlaneSIMD::transform(const TransformSIMD& transform)
{
	Vector4SIMD normal = getNormal();
	Vector4SIMD point = getPoint();

	normal = transform.transform(normal);
	point = transform.transform(point);

	normal.normalizeThis();

	set(point, normal);
}

void PlaneSIMD::transform(const Matrix4x4SIMD& matrix)
{
	Vector4SIMD normal = getNormal();
	Vector4SIMD point = getPoint();

	normal = matrix * normal;
	point = matrix * point;

	normal.normalizeThis();

	set(point, normal);
}

void PlaneSIMD::set(const Vector4SIMD& point_1, const Vector4SIMD& point_2, const Vector4SIMD& point_3)
{
	Vector4SIMD a = point_3 - point_1;
	_plane = point_2 - point_1;
	_plane.crossThis(a);
	_plane.normalizeThis();

	_plane |= (point_1.dotVec(_plane) & Vector4SIMD(gWMask));
}

void PlaneSIMD::set(const Vector4SIMD& point, const Vector4SIMD& normal)
{
	_plane = normal;
	// assumes normal is correctly formatted with w set to zero.
	_plane.set(point.dot(normal), 3);

}

void PlaneSIMD::set(const Vector4SIMD& normal, float distance)
{
	_plane = normal;
	_plane.set(distance, 3);
}

void PlaneSIMD::set(const Vector4SIMD& raw)
{
	_plane = raw;
}

NS_END
