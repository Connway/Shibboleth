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

#include "Gleam_AABB.h"
#include <Gaff_Math.h>

NS_GLEAM

AABB::AABB(const Vec4& min, const Vec4& max):
	_min(min), _max(max)
{
}

AABB::AABB(const AABB& aabb):
	_min(aabb._min), _max(aabb._max)
{
}

AABB::AABB(void):
	_min(0.0f, 0.0f, 0.0f, 1.0f), _max(0.0f, 0.0f, 0.0f, 1.0f)
{
}

AABB::~AABB(void)
{
}

const Vec4& AABB::getMin(void) const
{
	return _min;
}

const Vec4& AABB::getMax(void) const
{
	return _max;
}

Vec4 AABB::getCenter(void) const
{
	return _min + (_max - _min) * 0.5f;
}

void AABB::setMin(const Vec4& min)
{
	_min = min;
}

void AABB::setMax(const Vec4& max)
{
	_max = max;
}

void AABB::addPoint(float x, float y, float z)
{
	addPoint(Vec4(x, y, z, 1.0f));
}

void AABB::addPoint(const float* point)
{
	addPoint(Vec4(point));
}

void AABB::addPoint(const Vec4& point)
{
	_min.minimumThis(point);
	_max.maximumThis(point);
}

void AABB::addPoints(const float* points, unsigned int num_points, unsigned int stride)
{
	for (unsigned int i = 0; i < num_points; ++i) {
		addPoint(points);
		points += stride;
	}
}

void AABB::addPoints(const Vec4* points, unsigned int num_points)
{
	for (unsigned int i = 0; i < num_points; ++i) {
		addPoint(points[i]);
	}
}

void AABB::addPoints(const GleamArray<Vec4>& points)
{
	for (unsigned int i = 0; i < points.size(); ++i) {
		addPoint(points[i]);
	}
}

void AABB::addAABB(const AABB& aabb)
{
	addPoint(aabb.getMin());
	addPoint(aabb.getMax());
}

void AABB::reset(void)
{
	_min.set(0.0f, 0.0f, 0.0f, 1.0f);
	_max.set(0.0f, 0.0f, 0.0f, 1.0f);
}

const GleamArray<Vec4>& AABB::generatePoints(GleamArray<Vec4>& out) const
{
	if (out.size() < 8) {
		out.resize(8);
	}

	// bottom plane
	out[0] = _min;
	out[1] = Vec4(_min[0], _min[1], _max[2], 1.0f);
	out[2] = Vec4(_max[0], _min[1], _min[2], 1.0f);
	out[3] = Vec4(_max[0], _min[1], _max[2], 1.0f);

	// top plane
	out[4] = _max;
	out[5] = Vec4(_max[0], _max[1], _min[2], 1.0f);
	out[6] = Vec4(_min[0], _max[1], _max[2], 1.0f);
	out[7] = Vec4(_min[0], _max[1], _min[2], 1.0f);

	return out;
}

GleamArray<Vec4> AABB::generatePoints(void) const
{
	GleamArray<Vec4> points(8);

	// bottom plane
	points.push(_min);
	points.push(Vec4(_min[0], _min[1], _max[2], 1.0f));
	points.push(Vec4(_max[0], _min[1], _min[2], 1.0f));
	points.push(Vec4(_max[0], _min[1], _max[2], 1.0f));

	// top plane
	points.push(_max);
	points.push(Vec4(_max[0], _max[1], _min[2], 1.0f));
	points.push(Vec4(_min[0], _max[1], _max[2], 1.0f));
	points.push(Vec4(_min[0], _max[1], _min[2], 1.0f));

	return points;
}

const Vec4* AABB::generatePoints(Vec4* out) const
{
	// bottom plane
	out[0] = _min;
	out[1] = Vec4(_min[0], _min[1], _max[2], 1.0f);
	out[2] = Vec4(_max[0], _min[1], _min[2], 1.0f);
	out[3] = Vec4(_max[0], _min[1], _max[2], 1.0f);

	// top plane
	out[4] = _max;
	out[5] = Vec4(_max[0], _max[1], _min[2], 1.0f);
	out[6] = Vec4(_min[0], _max[1], _max[2], 1.0f);
	out[7] = Vec4(_min[0], _max[1], _min[2], 1.0f);

	return out;
}

void AABB::transform(const Mtx4x4& transform)
{
	GleamArray<Vec4> points = generatePoints();
	setMin(Vec4::Zero);
	setMax(Vec4::Zero);

	points[0] = transform * points[0];
	points[1] = transform * points[1];
	points[2] = transform * points[2];
	points[3] = transform * points[3];
	points[4] = transform * points[4];
	points[5] = transform * points[5];
	points[6] = transform * points[6];
	points[7] = transform * points[7];

	addPoints(points);
}

bool AABB::contains(const Vec4& point) const
{
	return point[0] >= _min[0] && point[0] <= _max[0] &&
			point[1] >= _min[1] && point[1] <= _max[1] &&
			point[2] >= _min[2] && point[2] <= _max[2];
}

NS_END
