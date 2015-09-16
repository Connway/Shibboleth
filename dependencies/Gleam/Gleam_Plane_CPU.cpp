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

#include "Gleam_Plane_CPU.h"
#include "Gleam_Transform_CPU.h"
#include "Gleam_Matrix4x4_CPU.h"
#include "Gleam_AABB_CPU.h"
#include "Gleam_OBB_CPU.h"
#include <cmath>

NS_GLEAM

PlaneCPU::PlaneCPU(const Vector4CPU& point_1, const Vector4CPU& point_2, const Vector4CPU& point_3):
	_plane(point_2 - point_1)
{
	Vector4CPU a = point_3 - point_1;
	_plane.crossThis(a);
	_plane.normalizeThis();

	_plane.set(point_1.dot(_plane), 3);
}

PlaneCPU::PlaneCPU(const Vector4CPU& point, const Vector4CPU& normal):
	_plane(normal)
{
	// assumes normal is correctly formatted with w set to zero.
	_plane.set(point.dot(normal), 3);
}

PlaneCPU::PlaneCPU(const Vector4CPU& normal, float distance):
	_plane(normal)
{
	_plane.set(distance, 3);
}

PlaneCPU::PlaneCPU(const Vector4CPU& raw):
	_plane(raw)
{
}

PlaneCPU::PlaneCPU(const PlaneCPU& plane):
	_plane(plane._plane)
{
}

PlaneCPU::PlaneCPU(void)
{
}

PlaneCPU::~PlaneCPU(void)
{
}

const PlaneCPU& PlaneCPU::operator=(const PlaneCPU& rhs)
{
	_plane = rhs._plane;
	return *this;
}

const Vector4CPU& PlaneCPU::getRawRepresentation(void) const
{
	return _plane;
}

Vector4CPU PlaneCPU::getNormal(void) const
{
	return Vector4CPU(_plane[0], _plane[1], _plane[2], 0.0f);
}

Vector4CPU PlaneCPU::getPoint(void) const
{
	return getNormal() * _plane[3];
}

Vector4CPU PlaneCPU::getDistanceVec(void) const
{
	return Vector4CPU(_plane[3]);
}

float PlaneCPU::getDistance(void) const
{
	return _plane[3];
}

PlaneCPU PlaneCPU::normalize(void) const
{
	Vector4CPU plane = _plane;
	float d = _plane[3];
	plane.set(0.0f, 3);
	float rec_len = plane.reciprocalLength();
	plane.set(d, 3);

	plane *= rec_len;

	return PlaneCPU(plane);
}

void PlaneCPU::normalizeThis(void)
{
	float d = _plane[3];
	_plane.set(0.0f, 3);
	float rec_len = _plane.reciprocalLength();
	_plane.set(d, 3);
	_plane *= rec_len;
}

PlaneCPU::ContainResult PlaneCPU::contains(const AABBCPU& aabb) const
{
	Vector4CPU normal(_plane[0], _plane[1], _plane[2], 0.0f);

	float dist = aabb.getCenter().dot(normal);
	float size = fabsf(aabb.getExtent().dot(normal));

	if ((dist + size) < _plane[3]) {
		return BACK;
	} else if ((dist - size) < _plane[3]) {
		return INTERSECTS;
	}

	return FRONT;
}

PlaneCPU::ContainResult PlaneCPU::contains(const OBBCPU& obb) const
{
	Vector4CPU normal(_plane[0], _plane[1], _plane[2], 0.0f);
	const Vector4CPU* axes = obb.getAxes();

	float dist_center = obb.getCenter().dot(normal);
	float dist_x = fabsf(axes[0].dot(normal));
	float dist_y = fabsf(axes[1].dot(normal));
	float dist_z = fabsf(axes[2].dot(normal));

	float dist_max = dist_center + dist_x + dist_y + dist_z;
	float dist_min = dist_center - dist_x - dist_y - dist_z;

	if (dist_max < _plane[3]) {
		return BACK;
	} else if (dist_min < _plane[3]) {
		return INTERSECTS;
	}

	return FRONT;
}

void PlaneCPU::transform(const TransformCPU& transform)
{
	Vector4CPU normal = getNormal();
	Vector4CPU point = getPoint();

	normal = transform.transformVector(normal);
	point = transform.transformPoint(point);

	normal.normalizeThis();

	set(point, normal);
}

void PlaneCPU::transform(const Matrix4x4CPU& matrix)
{
	Vector4CPU normal = getNormal();
	Vector4CPU point = getPoint();

	normal = matrix * normal;
	point = matrix * point;

	normal.normalizeThis();

	set(point, normal);
}

void PlaneCPU::set(const Vector4CPU& point_1, const Vector4CPU& point_2, const Vector4CPU& point_3)
{
	Vector4CPU a = point_3 - point_1;
	_plane = point_2 - point_1;
	_plane.crossThis(a);
	_plane.normalizeThis();

	_plane.set(point_1.dot(_plane), 3);
}

void PlaneCPU::set(const Vector4CPU& point, const Vector4CPU& normal)
{
	_plane = normal;

	// assumes normal is correctly formatted with w set to zero.
	_plane.set(point.dot(normal), 3);
}

void PlaneCPU::set(const Vector4CPU& normal, float distance)
{
	_plane = normal;
	_plane.set(distance, 3);
}

void PlaneCPU::set(const Vector4CPU& raw)
{
	_plane = raw;
}

NS_END
