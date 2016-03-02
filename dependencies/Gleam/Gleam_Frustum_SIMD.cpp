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

#include "Gleam_Frustum_SIMD.h"
//#include "Gleam_Matrix4x4_SIMD.h"
#include <Gaff_Assert.h>
#include <cmath>

NS_GLEAM

FrustumSIMD::FrustumSIMD(float fov, float aspect_ratio, float z_near, float z_far)
{
	construct(fov, aspect_ratio, z_near, z_far);
}

//FrustumSIMD::FrustumSIMD(const Matrix4x4SIMD& matrix)
//{
//	construct(matrix);
//}

FrustumSIMD::FrustumSIMD(const FrustumSIMD& frustum)
{
	*this = frustum;
}

FrustumSIMD::FrustumSIMD(void)
{
}

FrustumSIMD::~FrustumSIMD(void)
{
}

const FrustumSIMD& FrustumSIMD::operator=(const FrustumSIMD& rhs)
{
	_planes[0] = rhs._planes[0];
	_planes[1] = rhs._planes[1];
	_planes[2] = rhs._planes[2];
	_planes[3] = rhs._planes[3];
	_planes[4] = rhs._planes[4];
	_planes[5] = rhs._planes[5];

	return *this;
}

const PlaneSIMD& FrustumSIMD::getPlane(unsigned int index) const
{
	GAFF_ASSERT(index >= 0 && index < 6);
	return _planes[index];
}

const PlaneSIMD* FrustumSIMD::getPlanes(void) const
{
	return _planes;
}

void FrustumSIMD::construct(float fov, float aspect_ratio, float z_near, float z_far)
{
	fov *= 0.5f;
	float tan_fov = tanf(fov);
	float near_height = tan_fov * z_near;
	float far_height = tan_fov * z_far;
	float near_width = near_height * aspect_ratio;
	float far_width = far_height * aspect_ratio;

	Vector4SIMD nlt(-near_width, near_height, z_near, 1.0f);
	Vector4SIMD nlb(-near_width, -near_height, z_near, 1.0f);
	Vector4SIMD nrt(near_width, near_height, z_near, 1.0f);
	Vector4SIMD nrb(near_width, -near_height, z_near, 1.0f);
	Vector4SIMD flt(-far_width, far_height, z_far, 1.0f);
	Vector4SIMD flb(-far_width, -far_height, z_far, 1.0f);
	Vector4SIMD frt(far_width, far_height, z_far, 1.0f);
	Vector4SIMD frb(far_width, -far_height, z_far, 1.0f);

	_planes[0] = PlaneSIMD(nlb, flb, nlt);
	_planes[1] = PlaneSIMD(nrb, nrt, frb);
	_planes[2] = PlaneSIMD(nrb, frb, nlb);
	_planes[3] = PlaneSIMD(nrt, nlt, frt);
	_planes[4] = PlaneSIMD(-Vector4SIMD::Z_Axis, -z_near);
	_planes[5] = PlaneSIMD(Vector4SIMD::Z_Axis, z_far);
}

//void FrustumSIMD::construct(const Matrix4x4SIMD& matrix)
//{
//	Vector4SIMD column_0 = matrix.getColumn(0);
//	Vector4SIMD column_1 = matrix.getColumn(1);
//	Vector4SIMD column_2 = matrix.getColumn(2);
//	Vector4SIMD column_3 = matrix.getColumn(3);
//
//	_planes[0] = column_3 - column_0;	// Left
//	_planes[1] = column_3 + column_0;	// Right
//	_planes[2] = column_3 - column_1;	// Bottom
//	_planes[3] = column_3 + column_1;	// Top
//	_planes[4] = column_2;				// Near
//	_planes[5] = column_3 - column_2;	// Far
//
//	_planes[0].normalizeThis();
//	_planes[1].normalizeThis();
//	_planes[2].normalizeThis();
//	_planes[3].normalizeThis();
//	_planes[4].normalizeThis();
//	_planes[5].normalizeThis();
//}

bool FrustumSIMD::contains(const AABBSIMD& aabb) const
{
	for (unsigned int i = 0; i < 6; ++i) {
		// Plane normals are pointing outside. Therefore, if we are in front of a plane, we are outside the frustum.
		if (_planes[i].contains(aabb) == PlaneSIMD::FRONT) {
			return false;
		}
	}

	return true;
}

bool FrustumSIMD::contains(const OBBSIMD& obb) const
{
	for (unsigned int i = 0; i < 6; ++i) {
		// Plane normals are pointing outside. Therefore, if we are in front of a plane, we are outside the frustum.
		if (_planes[i].contains(obb) == PlaneSIMD::FRONT) {
			return false;
		}
	}

	return true;
}

NS_END
