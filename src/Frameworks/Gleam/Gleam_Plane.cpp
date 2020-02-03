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

#include "Gleam_Plane.h"
#include "Gleam_Transform.h"
#include "Gleam_AABB.h"
#include "Gleam_OBB.h"

NS_GLEAM

Plane::Plane(const glm::vec3& point_1, const glm::vec3& point_2, const glm::vec3& point_3):
	_normal(point_2 - point_1)
{
	_normal = glm::normalize(glm::cross(_normal, point_3 - point_1));
	_distance = glm::dot(point_1, _normal);
}

Plane::Plane(const glm::vec3& point, const glm::vec3& normal):
	_normal(normal)
{
	_distance = glm::dot(point, normal);
}

Plane::Plane(const glm::vec3& normal, float distance):
	_normal(normal), _distance(distance)
{
}

glm::vec3 Plane::getNormal(void) const
{
	return _normal;
}

glm::vec3 Plane::getPoint(void) const
{
	return _normal * _distance;
}

float Plane::getDistance(void) const
{
	return _distance;
}

Plane::ContainResult Plane::contains(const AABB& aabb) const
{
	const float dist = glm::dot(aabb.getCenter(), _normal);
	const float size = fabsf(glm::dot(aabb.getExtent(), _normal));

	if ((dist + size) < _distance) {
		return BACK;
	} else if ((dist - size) < _distance) {
		return INTERSECTS;
	}

	return FRONT;
}

Plane::ContainResult Plane::contains(const OBB& obb) const
{
	const glm::vec3* axes = obb.getAxes();

	float dist_center = glm::dot(obb.getCenter(), _normal);
	float dist_x = fabsf(glm::dot(axes[0], _normal));
	float dist_y = fabsf(glm::dot(axes[1], _normal));
	float dist_z = fabsf(glm::dot(axes[2], _normal));

	float dist_max = dist_center + dist_x + dist_y + dist_z;
	float dist_min = dist_center - dist_x - dist_y - dist_z;

	if (dist_max < _normal[3]) {
		return BACK;
	} else if (dist_min < _normal[3]) {
		return INTERSECTS;
	}

	return FRONT;
}

void Plane::transform(const Transform& transform)
{
	_normal = glm::normalize(transform.transformVector(_normal));
}

void Plane::set(const glm::vec3& point_1, const glm::vec3& point_2, const glm::vec3& point_3)
{
	_normal = glm::normalize(glm::cross(point_2 - point_1, point_3 - point_1));
	_distance = glm::dot(point_1, _normal);
}

void Plane::set(const glm::vec3& point, const glm::vec3& normal)
{
	_normal = normal;
	_distance = glm::dot(point, normal);
}

void Plane::set(const glm::vec3& normal, float distance)
{
	_normal = normal;
	_distance = distance;
}

NS_END
