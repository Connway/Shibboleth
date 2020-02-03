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

#include "Gleam_AABB.h"
#include "Gleam_Transform.h"

NS_GLEAM

AABB::AABB(const glm::vec3& min, const glm::vec3& max):
	_min(min), _max(max)
{
}

AABB::AABB(const AABB& aabb):
	_min(aabb._min), _max(aabb._max)
{
}

AABB& AABB::operator=(const AABB& rhs)
{
	_min = rhs._min;
	_max = rhs._max;
	return *this;
}

const glm::vec3& AABB::getMin(void) const
{
	return _min;
}

const glm::vec3& AABB::getMax(void) const
{
	return _max;
}

glm::vec3 AABB::getExtent(void) const
{
	return (_max - _min) * 0.5f;
}

glm::vec3 AABB::getCenter(void) const
{
	return _min + (_max - _min) * 0.5f;
}

void AABB::setMin(const glm::vec3& min)
{
	_min = min;
}

void AABB::setMax(const glm::vec3& max)
{
	_max = max;
}

void AABB::addPoint(float x, float y, float z)
{
	addPoint(glm::vec3(x, y, z));
}

void AABB::addPoint(const float* point)
{
	addPoint(glm::vec3(point[0], point[1], point[2]));
}

void AABB::addPoint(const glm::vec3& point)
{
	_min = glm::min(_min, point);
	_max = glm::max(_max, point);
}

void AABB::addPoints(const float* points, int32_t num_points, int32_t stride)
{
	for (int32_t i = 0; i < num_points; ++i) {
		addPoint(points);
		points += stride;
	}
}

void AABB::addPoints(const glm::vec3* points, int32_t num_points)
{
	for (int32_t i = 0; i < num_points; ++i) {
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
	constexpr float flt_min = std::numeric_limits<float>::min();
	constexpr float flt_max = std::numeric_limits<float>::max();

	_min = glm::vec3(flt_max, flt_max, flt_max);
	_max = glm::vec3(flt_min, flt_min, flt_min);
}

glm::vec3* AABB::generatePoints(glm::vec3* out) const
{
	// bottom plane
	out[0] = _min;
	out[1] = glm::vec3(_min[0], _min[1], _max[2]);
	out[2] = glm::vec3(_max[0], _min[1], _min[2]);
	out[3] = glm::vec3(_max[0], _min[1], _max[2]);

	// top plane
	out[4] = _max;
	out[5] = glm::vec3(_max[0], _max[1], _min[2]);
	out[6] = glm::vec3(_min[0], _max[1], _max[2]);
	out[7] = glm::vec3(_min[0], _max[1], _min[2]);

	return out;
}

void AABB::transform(const glm::mat4x4& transform)
{
	generatePoints(_transform_cache);
	reset();

	_transform_cache[0] = transform * glm::vec4(_transform_cache[0], 1.0f);
	_transform_cache[1] = transform * glm::vec4(_transform_cache[1], 1.0f);
	_transform_cache[2] = transform * glm::vec4(_transform_cache[2], 1.0f);
	_transform_cache[3] = transform * glm::vec4(_transform_cache[3], 1.0f);
	_transform_cache[4] = transform * glm::vec4(_transform_cache[4], 1.0f);
	_transform_cache[5] = transform * glm::vec4(_transform_cache[5], 1.0f);
	_transform_cache[6] = transform * glm::vec4(_transform_cache[6], 1.0f);
	_transform_cache[7] = transform * glm::vec4(_transform_cache[7], 1.0f);

	addPoints(_transform_cache, 8);
}

void AABB::transform(const Transform& transform)
{
	generatePoints(_transform_cache);
	reset();

	_transform_cache[0] = transform.transformPoint(_transform_cache[0]);
	_transform_cache[1] = transform.transformPoint(_transform_cache[1]);
	_transform_cache[2] = transform.transformPoint(_transform_cache[2]);
	_transform_cache[3] = transform.transformPoint(_transform_cache[3]);
	_transform_cache[4] = transform.transformPoint(_transform_cache[4]);
	_transform_cache[5] = transform.transformPoint(_transform_cache[5]);
	_transform_cache[6] = transform.transformPoint(_transform_cache[6]);
	_transform_cache[7] = transform.transformPoint(_transform_cache[7]);

	addPoints(_transform_cache, 8);
}

bool AABB::contains(const glm::vec3& point) const
{
	return point[0] >= _min[0] && point[0] <= _max[0] &&
			point[1] >= _min[1] && point[1] <= _max[1] &&
			point[2] >= _min[2] && point[2] <= _max[2];
}

NS_END
