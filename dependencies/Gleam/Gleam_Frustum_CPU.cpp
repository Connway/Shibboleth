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

#include "Gleam_Frustum_CPU.h"
//#include "Gleam_Matrix4x4_CPU.h"
#include <Gaff_IncludeAssert.h>
#include <cmath>

NS_GLEAM

FrustumCPU::FrustumCPU(float fov, float aspect_ratio, float z_near, float z_far)
{
	construct(fov, aspect_ratio, z_near, z_far);
}

//FrustumCPU::FrustumCPU(const Matrix4x4CPU& matrix)
//{
//	construct(matrix);
//}

FrustumCPU::FrustumCPU(const FrustumCPU& frustum)
{
	*this = frustum;
}

FrustumCPU::FrustumCPU(void)
{
}

FrustumCPU::~FrustumCPU(void)
{
}

const FrustumCPU& FrustumCPU::operator=(const FrustumCPU& rhs)
{
	_planes[0] = rhs._planes[0];
	_planes[1] = rhs._planes[1];
	_planes[2] = rhs._planes[2];
	_planes[3] = rhs._planes[3];
	_planes[4] = rhs._planes[4];
	_planes[5] = rhs._planes[5];

	return *this;
}

const PlaneCPU& FrustumCPU::getPlane(unsigned int index) const
{
	assert(index >= 0 && index < 6);
	return _planes[index];
}

const PlaneCPU* FrustumCPU::getPlanes(void) const
{
	return _planes;
}

void FrustumCPU::construct(float fov, float aspect_ratio, float z_near, float z_far)
{
	fov *= 0.5f;
	float tan_fov = tanf(fov);
	float near_height = tan_fov * z_near;
	float far_height = tan_fov * z_far;
	float near_width = near_height * aspect_ratio;
	float far_width = far_height * aspect_ratio;

	Vector4CPU nlt(-near_width, near_height, z_near, 1.0f);
	Vector4CPU nlb(-near_width, -near_height, z_near, 1.0f);
	Vector4CPU nrt(near_width, near_height, z_near, 1.0f);
	Vector4CPU nrb(near_width, -near_height, z_near, 1.0f);
	Vector4CPU flt(-far_width, far_height, z_far, 1.0f);
	Vector4CPU flb(-far_width, -far_height, z_far, 1.0f);
	Vector4CPU frt(far_width, far_height, z_far, 1.0f);
	Vector4CPU frb(far_width, -far_height, z_far, 1.0f);

	_planes[0] = PlaneCPU(nlb, flb, nlt);
	_planes[1] = PlaneCPU(nrb, nrt, frb);
	_planes[2] = PlaneCPU(nrb, frb, nlb);
	_planes[3] = PlaneCPU(nrt, nlt, frt);
	_planes[4] = PlaneCPU(-Vector4CPU::Z_Axis, -z_near);
	_planes[5] = PlaneCPU(Vector4CPU::Z_Axis, z_far);
}

//void FrustumCPU::construct(const Matrix4x4CPU& matrix)
//{
//	Vector4CPU column_0 = matrix.getColumn(0);
//	Vector4CPU column_1 = matrix.getColumn(1);
//	Vector4CPU column_2 = matrix.getColumn(2);
//	Vector4CPU column_3 = matrix.getColumn(3);
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

bool FrustumCPU::contains(const AABBCPU& aabb) const
{
	// Assuming compiler will unroll this loop
	for (unsigned int i = 0; i < 6; ++i) {
		// Plane normals are pointing outside. Therefore, if we are in front of a plane, we are outside the frustum.
		if (_planes[i].contains(aabb) == PlaneCPU::FRONT) {
			return false;
		}
	}

	return true;
}

bool FrustumCPU::contains(const OBBCPU& obb) const
{
	// Assuming compiler will unroll this loop
	for (unsigned int i = 0; i < 6; ++i) {
		// Plane normals are pointing outside. Therefore, if we are in front of a plane, we are outside the frustum.
		if (_planes[i].contains(obb) == PlaneCPU::FRONT) {
			return false;
		}
	}

	return true;
}

void FrustumCPU::transform(const TransformCPU& transform)
{
	_planes[0].transform(transform);
	_planes[1].transform(transform);
	_planes[2].transform(transform);
	_planes[3].transform(transform);
	_planes[4].transform(transform);
	_planes[5].transform(transform);
}

void FrustumCPU::transform(const Matrix4x4CPU& matrix)
{
	_planes[0].transform(matrix);
	_planes[1].transform(matrix);
	_planes[2].transform(matrix);
	_planes[3].transform(matrix);
	_planes[4].transform(matrix);
	_planes[5].transform(matrix);
}

NS_END
