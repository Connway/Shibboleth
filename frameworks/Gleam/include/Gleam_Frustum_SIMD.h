/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#pragma once

#include "Gleam_Plane_SIMD.h"

WARNING("SIMD data structures are potentially going to be deprecated.")

NS_GLEAM

//class Matrix4x4SIMD;
class AABBSIMD;
class OBBSIMD;

class FrustumSIMD
{
public:
	FrustumSIMD(float fov, float aspect_ratio, float z_near, float z_far);
	//FrustumSIMD(const Matrix4x4SIMD& matrix);
	FrustumSIMD(const FrustumSIMD& frustum);
	FrustumSIMD(void);
	~FrustumSIMD(void);

	const FrustumSIMD& operator=(const FrustumSIMD& rhs);
	const PlaneSIMD& getPlane(unsigned int index) const;
	const PlaneSIMD* getPlanes(void) const;

	void construct(float fov, float aspect_ratio, float z_near, float z_far);
	//void construct(const Matrix4x4SIMD& matrix);

	bool contains(const AABBSIMD& aabb) const;
	bool contains(const OBBSIMD& obb) const;

private:
	PlaneSIMD _planes[6];
};

NS_END
