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

#include "Gleam_OBB_CPU.h"
#include "Gleam_Transform_CPU.h"
#include "Gleam_AABB_CPU.h"

NS_GLEAM

OBBCPU::OBBCPU(const Vector4CPU& center, const Vector4CPU& right, const Vector4CPU& up, const Vector4CPU& forward):
	_center(center)
{
	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = forward;
}

OBBCPU::OBBCPU(const AABBCPU& aabb, const Matrix4x4CPU& transform):
	_center(aabb.getCenter())
{
	Vector4CPU half_extent = aabb.getMax() - _center;
	Vector4CPU right(half_extent[0], 0.0f, 0.0f, 1.0f);
	Vector4CPU up(0.0f, half_extent[1], 0.0f, 1.0f);
	Vector4CPU dir(0.0f, 0.0f, half_extent[2], 1.0f);

	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = dir;

	OBBCPU::transform(transform);
}

OBBCPU::OBBCPU(const AABBCPU& aabb):
	_center(aabb.getCenter())
{
	Vector4CPU half_extent = aabb.getMax() - _center;
	Vector4CPU right(half_extent[0], 0.0f, 0.0f, 1.0f);
	Vector4CPU up(0.0f, half_extent[1], 0.0f, 1.0f);
	Vector4CPU dir(0.0f, 0.0f, half_extent[2], 1.0f);

	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = dir;
}

OBBCPU::OBBCPU(const OBBCPU& obb):
	_center(obb._center)
{
	_axes[0] = obb._axes[0];
	_axes[1] = obb._axes[1];
	_axes[2] = obb._axes[2];
}

OBBCPU::OBBCPU(void):
	_center(0.0f, 0.0f, 0.0f, 1.0f)
{
	_axes[0] = _axes[1] = _axes[2] = Vector4CPU::Zero;
}

OBBCPU::~OBBCPU(void)
{
}

const Vector4CPU& OBBCPU::getCenter(void) const
{
	return _center;
}

Vector4CPU OBBCPU::getExtent(void) const
{
	return _axes[0] + _axes[1] + _axes[2];
}

const Vector4CPU& OBBCPU::getAxis(int axis) const
{
	GAFF_ASSERT(axis > -1 && axis < 3);
	return _axes[axis];
}

void OBBCPU::setAxis(int axis, const Vector4CPU& vec)
{
	GAFF_ASSERT(axis > -1 && axis < 3);
	_axes[axis] = vec;
}

const Vector4CPU* OBBCPU::getAxes(void) const
{
	return _axes;
}

const GleamArray<Vector4CPU>& OBBCPU::generatePoints(GleamArray<Vector4CPU>& out) const
{
	if (out.size() < 8) {
		out.resize(8);
	}

	generatePoints(out.getArray());

	return out;
}

GleamArray<Vector4CPU> OBBCPU::generatePoints(void) const
{
	GleamArray<Vector4CPU> points(8);

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

const Vector4CPU* OBBCPU::generatePoints(Vector4CPU* out) const
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

void OBBCPU::transform(const TransformCPU& transform)
{
	_axes[0] = transform.transformVector(_axes[0]);
	_axes[1] = transform.transformVector(_axes[1]);
	_axes[2] = transform.transformVector(_axes[2]);
	_center = transform.transformPoint(_center);
}

void OBBCPU::transform(const Matrix4x4CPU& transform)
{
	_axes[0] = transform * _axes[0];
	_axes[1] = transform * _axes[1];
	_axes[2] = transform * _axes[2];
	_center = transform * _center;
}

bool OBBCPU::contains(const Vector4CPU& point) const
{
	Vector4CPU left, right, bottom, top, back, front;
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
