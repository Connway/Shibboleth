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

#include "Gleam_Plane_CPU.h"

NS_GLEAM

class TransformCPU;
class Matrix4x4CPU;
class AABBCPU;
class OBBCPU;

class FrustumCPU
{
public:
	FrustumCPU(float fov, float aspect_ratio, float z_near, float z_far);
	//FrustumCPU(const Matrix4x4CPU& matrix);
	FrustumCPU(const FrustumCPU& frustum);
	FrustumCPU(void);
	~FrustumCPU(void);

	const FrustumCPU& operator=(const FrustumCPU& rhs);
	const PlaneCPU& getPlane(unsigned int index) const;
	const PlaneCPU* getPlanes(void) const;

	void construct(float fov, float aspect_ratio, float z_near, float z_far);
	//void construct(const Matrix4x4CPU& matrix);

	bool contains(const AABBCPU& aabb) const;
	bool contains(const OBBCPU& obb) const;

	void transform(const TransformCPU& transform);
	void transform(const Matrix4x4CPU& matrix);

private:
	PlaneCPU _planes[6];
};

NS_END
