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

#pragma once

#include "Gleam_Vector4_CPU.h"
#include "Gleam_Array.h"

NS_GLEAM

class TransformCPU;
class Matrix4x4CPU;
class AABBCPU;

class OBBCPU
{
public:
	OBBCPU(const Vector4CPU& center, const Vector4CPU& right, const Vector4CPU& up, const Vector4CPU& forward);
	OBBCPU(const AABBCPU& aabb, const Matrix4x4CPU& transform);
	OBBCPU(const AABBCPU& aabb);
	OBBCPU(const OBBCPU& obb);
	OBBCPU(void);
	~OBBCPU(void);

	INLINE const Vector4CPU& getCenter(void) const;
	INLINE Vector4CPU getExtent(void) const;

	INLINE void setAxis(int axis, const Vector4CPU& vec);
	INLINE const Vector4CPU& getAxis(int axis) const;
	INLINE const Vector4CPU* getAxes(void) const;

	INLINE const GleamArray<Vector4CPU>& generatePoints(GleamArray<Vector4CPU>& out) const;
	INLINE GleamArray<Vector4CPU> generatePoints(void) const;
	const Vector4CPU* generatePoints(Vector4CPU* out) const;

	INLINE void transform(const TransformCPU& transform);
	INLINE void transform(const Matrix4x4CPU& transform);
	bool contains(const Vector4CPU& point) const;

private:
	Vector4CPU _center;
	Vector4CPU _axes[3];
};

NS_END
