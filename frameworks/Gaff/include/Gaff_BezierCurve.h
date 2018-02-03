/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gaff_DefaultAllocator.h"
#include "Gaff_ICurve.h"
#include "Gaff_Vector.h"
#include "Gaff_Math.h"

NS_GAFF

template <class PointType, class Allocator = DefaultAllocator>
class BezierCurve : public ICurve<PointType>
{
public:
	struct Key
	{
		PointType point;
		float t;
	};

	BezierCurve(const BezierCurve<PointType, Allocator>& curve);
	BezierCurve(BezierCurve<PointType, Allocator>&& curve);
	BezierCurve(const Allocator& allocator = Allocator());
	~BezierCurve(void);

	PointType sample(float t) const;

	void addKey(float t, const PointType& point);
	void removeKey(int32_t index);

	const Key& getKey(int32_t index) const;
	int32_t getNumKeys(void) const;

private:
	Vector<Key, Allocator> _points;
};

#include "Gaff_BezierCurve.inl"

NS_END
