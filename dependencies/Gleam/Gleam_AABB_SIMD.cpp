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

#include "Gleam_AABB_SIMD.h"
#include "Gleam_Transform_SIMD.h"

NS_GLEAM

AABBSIMD::AABBSIMD(const Vector4SIMD& min, const Vector4SIMD& max):
	_min(min), _max(max)
{
}

AABBSIMD::AABBSIMD(const AABBSIMD& aabb):
	_min(aabb._min), _max(aabb._max)
{
}

AABBSIMD::AABBSIMD(void):
	_min(0.0f, 0.0f, 0.0f, 1.0f), _max(0.0f, 0.0f, 0.0f, 1.0f)
{
}

AABBSIMD::~AABBSIMD(void)
{
}

const Vector4SIMD& AABBSIMD::getMin(void) const
{
	return _min;
}

const Vector4SIMD& AABBSIMD::getMax(void) const
{
	return _max;
}

Vector4SIMD AABBSIMD::getExtent(void) const
{
	return (_max - _min) * 0.5f;
}

Vector4SIMD AABBSIMD::getCenter(void) const
{
	return _min + (_max - _min) * 0.5f;
}

void AABBSIMD::setMin(const Vector4SIMD& min)
{
	_min = min;
}

void AABBSIMD::setMax(const Vector4SIMD& max)
{
	_max = max;
}

void AABBSIMD::addPoint(float x, float y, float z)
{
	addPoint(Vector4SIMD(x, y, z, 1.0f));
}

void AABBSIMD::addPoint(const float* point)
{
	addPoint(Vector4SIMD(point));
}

void AABBSIMD::addPoint(const Vector4SIMD& point)
{
	_min.minimumThis(point);
	_max.maximumThis(point);
}

void AABBSIMD::addPoints(const float* points, unsigned int num_points, unsigned int stride)
{
	for (unsigned int i = 0; i < num_points; ++i) {
		addPoint(points);
		points += stride;
	}
}

void AABBSIMD::addPoints(const Vector4SIMD* points, unsigned int num_points)
{
	for (unsigned int i = 0; i < num_points; ++i) {
		addPoint(points[i]);
	}
}

void AABBSIMD::addPoints(const GleamArray<Vector4SIMD>& points)
{
	for (unsigned int i = 0; i < points.size(); ++i) {
		addPoint(points[i]);
	}
}

void AABBSIMD::addAABB(const AABBSIMD& aabb)
{
	addPoint(aabb.getMin());
	addPoint(aabb.getMax());
}

void AABBSIMD::reset(void)
{
	_min.set(0.0f, 0.0f, 0.0f, 1.0f);
	_max.set(0.0f, 0.0f, 0.0f, 1.0f);
}

const GleamArray<Vector4SIMD>& AABBSIMD::generatePoints(GleamArray<Vector4SIMD>& out) const
{
	if (out.size() < 8) {
		out.resize(8);
	}

	// bottom plane
	out[0] = _min;
	out[1] = Vector4SIMD(_min[0], _min[1], _max[2], 1.0f);
	out[2] = Vector4SIMD(_max[0], _min[1], _min[2], 1.0f);
	out[3] = Vector4SIMD(_max[0], _min[1], _max[2], 1.0f);

	// top plane
	out[4] = _max;
	out[5] = Vector4SIMD(_max[0], _max[1], _min[2], 1.0f);
	out[6] = Vector4SIMD(_min[0], _max[1], _max[2], 1.0f);
	out[7] = Vector4SIMD(_min[0], _max[1], _min[2], 1.0f);

	return out;
}

GleamArray<Vector4SIMD> AABBSIMD::generatePoints(void) const
{
	GleamArray<Vector4SIMD> points(8);

	// bottom plane
	points.emplacePush(_min);
	points.emplacePush(Vector4SIMD(_min[0], _min[1], _max[2], 1.0f));
	points.emplacePush(Vector4SIMD(_max[0], _min[1], _min[2], 1.0f));
	points.emplacePush(Vector4SIMD(_max[0], _min[1], _max[2], 1.0f));

	// top plane
	points.emplacePush(_max);
	points.emplacePush(Vector4SIMD(_max[0], _max[1], _min[2], 1.0f));
	points.emplacePush(Vector4SIMD(_min[0], _max[1], _max[2], 1.0f));
	points.emplacePush(Vector4SIMD(_min[0], _max[1], _min[2], 1.0f));

	return points;
}

const Vector4SIMD* AABBSIMD::generatePoints(Vector4SIMD* out) const
{
	// bottom plane
	out[0] = _min;
	out[1] = Vector4SIMD(_min[0], _min[1], _max[2], 1.0f);
	out[2] = Vector4SIMD(_max[0], _min[1], _min[2], 1.0f);
	out[3] = Vector4SIMD(_max[0], _min[1], _max[2], 1.0f);

	// top plane
	out[4] = _max;
	out[5] = Vector4SIMD(_max[0], _max[1], _min[2], 1.0f);
	out[6] = Vector4SIMD(_min[0], _max[1], _max[2], 1.0f);
	out[7] = Vector4SIMD(_min[0], _max[1], _min[2], 1.0f);

	return out;
}

void AABBSIMD::transform(const TransformSIMD& transform)
{
	generatePoints(_transform_cache);
	setMin(Vector4SIMD::Zero);
	setMax(Vector4SIMD::Zero);

	_transform_cache[0] = transform.transform(_transform_cache[0]);
	_transform_cache[1] = transform.transform(_transform_cache[1]);
	_transform_cache[2] = transform.transform(_transform_cache[2]);
	_transform_cache[3] = transform.transform(_transform_cache[3]);
	_transform_cache[4] = transform.transform(_transform_cache[4]);
	_transform_cache[5] = transform.transform(_transform_cache[5]);
	_transform_cache[6] = transform.transform(_transform_cache[6]);
	_transform_cache[7] = transform.transform(_transform_cache[7]);

	addPoints(_transform_cache, 8);
}

void AABBSIMD::transform(const Matrix4x4SIMD& transform)
{
	generatePoints(_transform_cache);
	setMin(Vector4SIMD::Zero);
	setMax(Vector4SIMD::Zero);

	_transform_cache[0] = transform * _transform_cache[0];
	_transform_cache[1] = transform * _transform_cache[1];
	_transform_cache[2] = transform * _transform_cache[2];
	_transform_cache[3] = transform * _transform_cache[3];
	_transform_cache[4] = transform * _transform_cache[4];
	_transform_cache[5] = transform * _transform_cache[5];
	_transform_cache[6] = transform * _transform_cache[6];
	_transform_cache[7] = transform * _transform_cache[7];

	addPoints(_transform_cache, 8);
}

bool AABBSIMD::contains(const Vector4SIMD& point) const
{
	return point[0] >= _min[0] && point[0] <= _max[0] &&
			point[1] >= _min[1] && point[1] <= _max[1] &&
			point[2] >= _min[2] && point[2] <= _max[2];
}

NS_END
