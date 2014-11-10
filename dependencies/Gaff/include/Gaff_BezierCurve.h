/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

/*! \file */

#pragma once

#include "Gaff_DefaultAllocator.h"
#include "Gaff_ICurve.h"
#include "Gaff_Array.h"
#include "Gaff_Pair.h"
#include "Gaff_Math.h"

NS_GAFF

/*!
	\brief Bezier Curve sampling implementation.

	\tparam PointType The type that represents our point data.
	\tparam Allocator The allocator we will use to allocate memory.
*/
template <class PointType, class Allocator = DefaultAllocator>
class BezierCurve : public ICurve<PointType>
{
public:
	typedef Pair<float, PointType> Key;

	BezierCurve(const BezierCurve<PointType, Allocator>& curve);
	BezierCurve(BezierCurve<PointType, Allocator>&& curve);
	BezierCurve(const Allocator& allocator = Allocator());
	~BezierCurve(void);

	PointType sample(float t) const;

	void addKey(float t, const PointType& point);
	void removeKey(unsigned int index);

	unsigned int getNumKeys(void) const;
	const Key& getKey(unsigned int index);

private:
	Array<Key, Allocator> _points;
};

#include "Gaff_BezierCurve.inl"

NS_END
