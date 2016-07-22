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

#include "Gleam_OBB_SIMD.h"
#include "Gleam_Transform_SIMD.h"
#include "Gleam_AABB_SIMD.h"

NS_GLEAM

OBBSIMD::OBBSIMD(const Vector4SIMD& center, const Vector4SIMD& right, const Vector4SIMD& up, const Vector4SIMD& forward):
	_center(center)
{
	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = forward;
}

OBBSIMD::OBBSIMD(const AABBSIMD& aabb, const Matrix4x4SIMD& transform):
	_center(aabb.getCenter())
{
	Vector4SIMD half_extent = aabb.getMax() - _center;
	Vector4SIMD right(half_extent[0], 0.0f, 0.0f, 1.0f);
	Vector4SIMD up(0.0f, half_extent[1], 0.0f, 1.0f);
	Vector4SIMD dir(0.0f, 0.0f, half_extent[2], 1.0f);

	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = dir;

	OBBSIMD::transform(transform);
}

OBBSIMD::OBBSIMD(const AABBSIMD& aabb):
	_center(aabb.getCenter())
{
	Vector4SIMD half_extent = aabb.getMax() - _center;
	Vector4SIMD right(half_extent[0], 0.0f, 0.0f, 1.0f);
	Vector4SIMD up(0.0f, half_extent[1], 0.0f, 1.0f);
	Vector4SIMD dir(0.0f, 0.0f, half_extent[2], 1.0f);

	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = dir;
}

OBBSIMD::OBBSIMD(const OBBSIMD& obb):
	_center(obb._center)
{
	_axes[0] = obb._axes[0];
	_axes[1] = obb._axes[1];
	_axes[2] = obb._axes[2];
}

OBBSIMD::OBBSIMD(void):
	_center(0.0f, 0.0f, 0.0f, 1.0f)
{
	_axes[0] = _axes[1] = _axes[2] = Vector4SIMD::Zero;
}

OBBSIMD::~OBBSIMD(void)
{
}

const Vector4SIMD& OBBSIMD::getCenter(void) const
{
	return _center;
}

Vector4SIMD OBBSIMD::getExtent(void) const
{
	return _axes[0] + _axes[1] + _axes[2];
}

const Vector4SIMD& OBBSIMD::getAxis(int axis) const
{
	GAFF_ASSERT(axis > -1 && axis < 3);
	return _axes[axis];
}

const Vector4SIMD* OBBSIMD::getAxes(void) const
{
	return _axes;
}

void OBBSIMD::setAxis(int axis, const Vector4SIMD& vec)
{
	GAFF_ASSERT(axis > -1 && axis < 3);
	_axes[axis] = vec;
}

const GleamArray<Vector4SIMD>& OBBSIMD::generatePoints(GleamArray<Vector4SIMD>& out) const
{
	if (out.size() < 8) {
		out.resize(8);
	}

	generatePoints(out.getArray());

	return out;
}

GleamArray<Vector4SIMD> OBBSIMD::generatePoints(void) const
{
	GleamArray<Vector4SIMD> points(8);

	// top plane
	points.emplacePush(_center + _axes[0] + _axes[1] + _axes[2]);
	points.emplacePush(points[0] - 2.0f * _axes[0]);
	points.emplacePush(points[1] - 2.0f * _axes[2]);
	points.emplacePush(points[2] + 2.0f * _axes[0]);

	// bottom plane
	points.emplacePush(_center + _axes[0] - _axes[1] + _axes[2]);
	points.emplacePush(points[4] - 2.0f * _axes[0]);
	points.emplacePush(points[5] - 2.0f * _axes[2]);
	points.emplacePush(points[6] + 2.0f * _axes[0]);

	return points;
}

const Vector4SIMD* OBBSIMD::generatePoints(Vector4SIMD* out) const
{
	// top plane
	out[0] = _center + _axes[0] + _axes[1] + _axes[2];
	out[1] = out[0] - 2.0f * _axes[0];
	out[2] = out[1] - 2.0f * _axes[2];
	out[3] = out[2] + 2.0f * _axes[0];

	// bottom plane
	out[4] = _center + _axes[0] - _axes[1] + _axes[2];
	out[5] = out[4] - 2.0f * _axes[0];
	out[6] = out[5] - 2.0f * _axes[2];
	out[7] = out[6] + 2.0f * _axes[0];

	return out;
}

void OBBSIMD::transform(const TransformSIMD& transform)
{
	_axes[0] = transform.transform(_axes[0]);
	_axes[1] = transform.transform(_axes[1]);
	_axes[2] = transform.transform(_axes[2]);
	_center = transform.transform(_center);
}

void OBBSIMD::transform(const Matrix4x4SIMD& transform)
{
	_axes[0] = transform * _axes[0];
	_axes[1] = transform * _axes[1];
	_axes[2] = transform * _axes[2];
	_center = transform * _center;
}

bool OBBSIMD::contains(const Vector4SIMD& point) const
{
	Vector4SIMD left, right, bottom, top, back, front;
	left = _center - _axes[0];
	right = _center + _axes[0];
	bottom = _center - _axes[1];
	top = _center + _axes[1];
	back = _center - _axes[2];
	front = _center + _axes[2];

	return (-_axes[0]).dot(point - left) <= 0.0f && _axes[0].dot(point - right) <= 0.0f &&
			(-_axes[1]).dot(point - bottom) <= 0.0f && _axes[1].dot(point - top) <= 0.0f &&
			(-_axes[2]).dot(point - back) <= 0.0f && _axes[2].dot(point - front) <= 0.0f;
}

NS_END
