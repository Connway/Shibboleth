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

class AABB;

class OBB
{
public:
	OBB(const Vec4& center, const Vec4& right, const Vec4& up, const Vec4& forward,
		float right_length, float up_length, float dir_length);
	OBB(const AABB& aabb, const Mtx4x4& transform);
	OBB(const AABB& aabb);
	OBB(const OBB& obb);
	OBB(void);
	~OBB(void);

	INLINE void setLength(int axis, float length);
	INLINE float getLength(int axis) const;

	INLINE const Vec4& getAxis(int axis) const;

	const GleamArray(Vec4)& generatePoints(GleamArray(Vec4)& out) const;
	GleamArray(Vec4) generatePoints(void) const;
	const Vec4* generatePoints(Vec4* out) const;

	void transform(const Mtx4x4& transform);
	bool contains(const Vec4& point) const;

private:
	Vec4 _center;
	Vec4 _axes[3];
	float _lengths[3];
};

NS_END
