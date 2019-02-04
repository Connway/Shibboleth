/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
#include "Gleam_Transform.h"
#include "Gleam_AABB.h"
#include <Gaff_Assert.h>

NS_GLEAM

OBB::OBB(const glm::vec3& center, const glm::vec3& right, const glm::vec3& up, const glm::vec3& forward):
	_center(center)
{
	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = forward;
}

OBB::OBB(const AABB& aabb, const glm::mat4x4& transform):
	_center(aabb.getCenter())
{
	glm::vec3 half_extent = aabb.getMax() - _center;
	glm::vec3 right(half_extent.x, 0.0f, 0.0f);
	glm::vec3 up(0.0f, half_extent.x, 0.0f);
	glm::vec3 dir(0.0f, 0.0f, half_extent.z);

	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = dir;

	OBB::transform(transform);
}

OBB::OBB(const AABB& aabb, const Transform& transform):
	_center(aabb.getCenter())
{
	glm::vec3 half_extent = aabb.getMax() - _center;
	glm::vec3 right(half_extent.x, 0.0f, 0.0f);
	glm::vec3 up(0.0f, half_extent.y, 0.0f);
	glm::vec3 dir(0.0f, 0.0f, half_extent.z);

	_axes[0] = right;
	_axes[1] = up;
	_axes[2] = dir;

	OBB::transform(transform);
}

OBB::OBB(const AABB& aabb):
	_center(aabb.getCenter())
{
	glm::vec3 half_extent = aabb.getMax() - _center;
	glm::vec3 right(half_extent.x, 0.0f, 0.0f);
	glm::vec3 up(0.0f, half_extent.y, 0.0f);
	glm::vec3 dir(0.0f, 0.0f, half_extent.z);

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
}

OBB::OBB(void):
	_center(0.0f, 0.0f, 0.0f)
{
	_axes[0] = _axes[1] = _axes[2] = _center;
}

OBB::~OBB(void)
{
}

const glm::vec3& OBB::getCenter(void) const
{
	return _center;
}

glm::vec3 OBB::getExtent(void) const
{
	return _axes[0] + _axes[1] + _axes[2];
}

const glm::vec3& OBB::getAxis(int32_t axis) const
{
	GAFF_ASSERT(axis > -1 && axis < 3);
	return _axes[axis];
}

void OBB::setAxis(int axis, const glm::vec3& vec)
{
	GAFF_ASSERT(axis > -1 && axis < 3);
	_axes[axis] = vec;
}

const glm::vec3* OBB::getAxes(void) const
{
	return _axes;
}

glm::vec3* OBB::generatePoints(glm::vec3* out) const
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

void OBB::transform(const Transform& transform)
{
	_axes[0] = transform.transformVector(_axes[0]);
	_axes[1] = transform.transformVector(_axes[1]);
	_axes[2] = transform.transformVector(_axes[2]);
	_center = transform.transformPoint(_center);
}

void OBB::transform(const glm::mat4x4& transform)
{
	_axes[0] = transform * glm::vec4(_axes[0], 1.0f);
	_axes[1] = transform * glm::vec4(_axes[1], 1.0f);
	_axes[2] = transform * glm::vec4(_axes[2], 1.0f);
	_center = transform * glm::vec4(_center, 1.0f);
}

bool OBB::contains(const glm::vec3& point) const
{
	glm::vec3 left, right, bottom, top, back, front;
	left = _center - _axes[0];
	right = _center + _axes[0];
	bottom = _center - _axes[1];
	top = _center + _axes[1];
	back = _center - _axes[2];
	front = _center + _axes[2];

	return	glm::dot(-_axes[0], point - left) <= 0.0f && glm::dot(_axes[0], point - right) <= 0.0f &&
			glm::dot(-_axes[1], point - bottom) <= 0.0f && glm::dot(_axes[1], point - top) <= 0.0f &&
			glm::dot(-_axes[2], point - back) <= 0.0f && glm::dot(_axes[2], point - front) <= 0.0f;
}

NS_END
