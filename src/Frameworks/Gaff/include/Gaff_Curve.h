/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Gaff_Assert.h"
#include "Gaff_Vector.h"
#include "Gaff_Math.h"

NS_GAFF

template <class Allocator = DefaultAllocator>
class Curve final
{
public:
	enum class SegmentType
	{
		Constant,
		Linear,
		Bezier
	};

	struct Key final
	{
		SegmentType type;
		int32_t data_index;
		float value;
		float t;
	};

	struct BezierData final
	{
		float left_slope;
		float right_slope;
		bool free;
	};

	Curve(const Allocator& allocator = Allocator());

	float sample(float t) const;

	const Key& getKey(int32_t index) const;
	int32_t getKeyCount(void) const;

	void setKeyType(int32_t index, const BezierData& data);
	void setKeyTypeConstant(int32_t index);
	void setKeyTypeLinear(int32_t index);

	void setKeyValue(int32_t index, float value);
	void setKeyT(int32_t index, float t);

	void addKey(float t, float value, const BezierData& data);
	void addKeyConstant(float t, float value);
	void addKeyLinear(float t, float value);

private:
	Vector<BezierData, Allocator> _bezier_data;
	Vector<Key, Allocator> _keys;

	template <class T>
	int32_t allocateData(Vector<T, Allocator>& data, const T& value);

	Key createKey(SegmentType type, float value, float t);
	int32_t getInsertPosition(float& t);
	void freeData(Key& key);
};

#include "Gaff_Curve.inl"

NS_END
