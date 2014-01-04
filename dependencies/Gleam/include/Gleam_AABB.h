/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gleam_Matrix4x4.h"
#include "Gleam_Vector4.h"
#include "Gleam_Array.h"

NS_GLEAM

class AABB
{
public:
	AABB(const Vec4& min, const Vec4& max);
	AABB(const AABB& aabb);
	AABB(void);
	~AABB(void);

	const Vec4& getMin(void) const;
	const Vec4& getMax(void) const;
	Vec4 getCenter(void) const;
	void setMin(const Vec4& min);
	void setMax(const Vec4& max);

	void addPoint(float x, float y, float z);
	void addPoint(const float* point);
	void addPoint(const Vec4& point);

	void addPoints(const float* points, unsigned int num_points, unsigned int stride = 3);
	void addPoints(const Vec4* points, unsigned int num_points);
	void addPoints(const GleamArray(Vec4)& points);

	void addAABB(const AABB& aabb);

	void reset(void);

	const GleamArray(Vec4)& generatePoints(GleamArray(Vec4)& out) const;
	GleamArray(Vec4) generatePoints(void) const;
	const Vec4* generatePoints(Vec4* out) const;

	void transform(const Mtx4x4& transform);
	bool contains(const Vec4& point) const;

private:
	Vec4 _min;
	Vec4 _max;
};

NS_END
