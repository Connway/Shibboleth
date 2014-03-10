/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gleam_OBB.h"
#include "Gleam_AABB.h"

NS_GLEAM

OBB::OBB(const Vec4& center, const Vec4& right, const Vec4& up, const Vec4& forward,
	float right_length, float up_length, float dir_length):
	_center(center)
{
	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = forward;

	_lengths[0] = right_length;
	_lengths[1] = up_length;
	_lengths[2] = dir_length;
}

OBB::OBB(const AABB& aabb, const Mtx4x4& transform):
	_center(aabb.getCenter())
{
	Vec4 half_extent = aabb.getMax() - _center;
	Vec4 right(half_extent[0], 0.0f, 0.0f, 1.0f);
	Vec4 up(0.0f, half_extent[1], 0.0f, 1.0f);
	Vec4 dir(0.0f, 0.0f, half_extent[2], 1.0f);

	_lengths[0] = right.length();
	_lengths[1] = up.length();
	_lengths[2] = dir.length();

	right.normalize();
	up.normalize();
	dir.normalize();

	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = dir;

	OBB::transform(transform);
}

OBB::OBB(const AABB& aabb):
	_center(aabb.getCenter())
{
	Vec4 half_extent = aabb.getMax() - _center;
	Vec4 right(half_extent[0], 0.0f, 0.0f, 1.0f);
	Vec4 up(0.0f, half_extent[1], 0.0f, 1.0f);
	Vec4 dir(0.0f, 0.0f, half_extent[2], 1.0f);

	_lengths[0] = right.length();
	_lengths[1] = up.length();
	_lengths[2] = dir.length();

	right.normalize();
	up.normalize();
	dir.normalize();

	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = dir;
}

OBB::OBB(const OBB& obb):
	_center(obb._center)
{
	_axes[0] = obb._axes[0];
	_axes[1] = obb._axes[1];
	_axes[2] = obb._axes[2];

	_lengths[0] = obb._lengths[0];
	_lengths[1] = obb._lengths[1];
	_lengths[2] = obb._lengths[2];
}

OBB::OBB(void):
	_center(0.0f, 0.0f, 0.0f, 1.0f)
{
	_axes[0] = _axes[1] = _axes[2] = Vec4::zero;
	_lengths[0] = _lengths[1] = _lengths[2] = 0.0f;
}

OBB::~OBB(void)
{
}

void OBB::setLength(int axis, float length)
{
	assert(axis > -1 && axis < 3);
	_lengths[axis] = length;
}

float OBB::getLength(int axis) const
{
	assert(axis > -1 && axis < 3);
	return _lengths[axis];
}

const Vec4& OBB::getAxis(int axis) const
{
	assert(axis > -1 && axis < 3);
	return _axes[axis];
}

const GleamArray<Vec4>& OBB::generatePoints(GleamArray<Vec4>& out) const
{
	if (out.size() < 8) {
		out.resize(8);
	}

	// top plane
	out[0] = _center + _axes[0] * _lengths[0] + _axes[1] * _lengths[1] + _axes[2] * _lengths[2];
	out[1] = out[0] - _axes[0] * _lengths[0];
	out[2] = out[1] - _axes[2] * _lengths[2];
	out[3] = out[2] + _axes[0] * _lengths[0];

	// bottom plane
	out[4] = _center + _axes[0] * _lengths[0] - _axes[1] * _lengths[1] + _axes[2] * _lengths[2];
	out[5] = out[5] - _axes[0] * _lengths[0];
	out[6] = out[6] - _axes[2] * _lengths[2];
	out[7] = out[7] + _axes[0] * _lengths[0];

	return out;
}

GleamArray<Vec4> OBB::generatePoints(void) const
{
	GleamArray<Vec4> points(8);

	// top plane
	points.push(_center + _axes[0] * _lengths[0] + _axes[1] * _lengths[1] + _axes[2] * _lengths[2]);
	points.push(points[0] - _axes[0] * _lengths[0]);
	points.push(points[1] - _axes[2] * _lengths[2]);
	points.push(points[2] + _axes[0] * _lengths[0]);

	// bottom plane
	points.push(_center + _axes[0] * _lengths[0] - _axes[1] * _lengths[1] + _axes[2] * _lengths[2]);
	points.push(points[5] - _axes[0] * _lengths[0]);
	points.push(points[6] - _axes[2] * _lengths[2]);
	points.push(points[7] + _axes[0] * _lengths[0]);

	return points;
}

const Vec4* OBB::generatePoints(Vec4* out) const
{
	// top plane
	out[0] = _center + _axes[0] * _lengths[0] + _axes[1] * _lengths[1] + _axes[2] * _lengths[2];
	out[1] = out[0] - _axes[0] * _lengths[0];
	out[2] = out[1] - _axes[2] * _lengths[2];
	out[3] = out[2] + _axes[0] * _lengths[0];

	// bottom plane
	out[4] = _center + _axes[0] * _lengths[0] - _axes[1] * _lengths[1] + _axes[2] * _lengths[2];
	out[5] = out[5] - _axes[0] * _lengths[0];
	out[6] = out[6] - _axes[2] * _lengths[2];
	out[7] = out[7] + _axes[0] * _lengths[0];

	return out;
}

void OBB::transform(const Mtx4x4& transform)
{
	_axes[0] = transform * _axes[0];
	_axes[1] = transform * _axes[1];
	_axes[2] = transform * _axes[2];
}

bool OBB::contains(const Vec4& point) const
{
	Vec4 left, right, bottom, top, back, front;
	left = _center - _axes[0] * _lengths[0];
	right = _center + _axes[0] * _lengths[0];
	bottom = _center - _axes[1] * _lengths[1];
	top = _center + _axes[1] * _lengths[1];
	back = _center - _axes[2] * _lengths[2];
	front = _center + _axes[2] * _lengths[2];

	return (-_axes[0]).dot(point - left) <= 0.0f && _axes[0].dot(point - right) <= 0.0f &&
			(-_axes[1]).dot(point - bottom) <= 0.0f && _axes[1].dot(point - top) <= 0.0f &&
			(-_axes[2]).dot(point - back) <= 0.0f && _axes[2].dot(point - front) <= 0.0f;
}

NS_END
