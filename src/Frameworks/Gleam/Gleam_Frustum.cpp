/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Gleam_Frustum.h"
#include "Gleam_Transform.h"
#include "Gleam_AABB.h"
#include "Gleam_OBB.h"
#include <cmath>

NS_GLEAM

Frustum::Frustum(float fov, float aspect_ratio, float z_near, float z_far)
{
	construct(fov, aspect_ratio, z_near, z_far);
}

void Frustum::construct(float fov, float aspect_ratio, float z_near, float z_far)
{
	fov *= 0.5f;
	float tan_fov = tanf(fov);
	float near_height = tan_fov * z_near;
	float far_height = tan_fov * z_far;
	float near_width = near_height * aspect_ratio;
	float far_width = far_height * aspect_ratio;

	Vec3 nlt(-near_width, near_height, z_near);
	Vec3 nlb(-near_width, -near_height, z_near);
	Vec3 nrt(near_width, near_height, z_near);
	Vec3 nrb(near_width, -near_height, z_near);
	//Vec3 flt(-far_width, far_height, z_far);
	Vec3 flb(-far_width, -far_height, z_far);
	Vec3 frt(far_width, far_height, z_far);
	Vec3 frb(far_width, -far_height, z_far);

	_planes[0] = Plane(nlb, flb, nlt);
	_planes[1] = Plane(nrb, nrt, frb);
	_planes[2] = Plane(nrb, frb, nlb);
	_planes[3] = Plane(nrt, nlt, frt);
	_planes[4] = Plane(Vec3(0.0f, 0.0f, -1.0f), -z_near);
	_planes[5] = Plane(Vec3(0.0f, 0.0f, 1.0f), z_far);
}

bool Frustum::contains(const AABB& aabb) const
{
	const int32_t size = static_cast<int32_t>(std::size(_planes));

	for (int32_t i = 0; i < size; ++i) {
		// Plane normals are pointing outside. Therefore, if we are in front of a plane, we are outside the frustum.
		if (_planes[i].contains(aabb) == Plane::FRONT) {
			return false;
		}
	}

	return true;
}

bool Frustum::contains(const OBB& obb) const
{
	// Assuming compiler will unroll this loop
	for (int32_t i = 0; i < 6; ++i) {
		// Plane normals are pointing outside. Therefore, if we are in front of a plane, we are outside the frustum.
		if (_planes[i].contains(obb) == Plane::FRONT) {
			return false;
		}
	}

	return true;
}

void Frustum::transform(const Transform& transform)
{
	_planes[0].transform(transform);
	_planes[1].transform(transform);
	_planes[2].transform(transform);
	_planes[3].transform(transform);
	_planes[4].transform(transform);
	_planes[5].transform(transform);
}

NS_END
