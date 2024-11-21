/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

template <class Allocator>
Curve<Allocator>::Curve(const Allocator& allocator):
	_bezier_data(allocator),
	_keys(allocator)
{
}

template <class Allocator>
float Curve<Allocator>::sample(float t) const
{
	if (_keys.empty()) {
		return 0.0f;
	}

	if (t <= _keys.front().t) {
		return _keys.front().value;
	} else if (t >= _keys.back().t) {
		return _keys.back().value;
	}

	const int32_t num_keys = static_cast<int32_t>(_keys.size());
	int32_t index = 1;

	// t is in range of the curve, so we don't need to worry about bounds checking.
	while (_keys[index].t > t) {
		++index;
	}

	const Key& left = _keys[index - 1];
	float value = 0.0f;

	switch (left.type) {
		case SegmentType::Constant:
			value = left.value;
			break;

		case SegmentType::Bezier: {
			GAFF_ASSERT(left.data_index > -1);

			const BezierData& data = _bezier_data[left.data_index];
			const Key& right = _keys[index];

			const float x1 = left.t;
			const float x2 = Lerp(left.t, right.t, 0.5f);
			const float x3 = x2;
			const float x4 = right.t;

			const float y1 = left.value;
			const float y2 = left.value + data.left_slope * (x2 - x1);
			const float y3 = right.value + data.right_slope * (x4 - x3);
			const float y4 = right.value;

			t = (t - left.t) / (right.t - left.t); // Normalize.
			const float inv_t = 1.0f - t;

			value = inv_t * inv_t * inv_t * y1 +
				3.0f * inv_t * inv_t * t * y2 +
				3.0f * inv_t * t * t * y3 +
				t * t * t * y4;
		} break;

		case SegmentType::Linear: {
			const Key& right = _keys[index];
			t = (t - left.t) / (right.t - left.t); // Normalize.
			value = Lerp(left.value, right.value, t);
		} break;
	}

	return value;
}

template <class Allocator>
const typename Curve<Allocator>::Key& Curve<Allocator>::getKey(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < getKeyCount());
	return _keys[index];
}

template <class Allocator>
int32_t Curve<Allocator>::getKeyCount(void) const
{
	return static_cast<int32_t>(_keys.size());
}

template <class Allocator>
void Curve<Allocator>::setKeyType(int32_t index, const BezierData& data)
{
	GAFF_ASSERT(index >= 0 && index < getKeyCount());
	Key& key = _keys[index];

	if (key.type == SegmentType::Bezier) {
		GAFF_ASSERT(key.data_index > -1);
		_bezier_data[key.data_index] = data;
		return;
	}

	freeData(key);
	key.type = SegmentType::Bezier;
}

template <class Allocator>
void Curve<Allocator>::setKeyTypeConstant(int32_t index)
{
	GAFF_ASSERT(index >= 0 && index < getKeyCount());
	Key& key = _keys[index];

	if (key.type == SegmentType::Constant) {
		return;
	}

	freeData(key);
	key.type = SegmentType::Constant;
}

template <class Allocator>
void Curve<Allocator>::setKeyTypeLinear(int32_t index)
{
	GAFF_ASSERT(index >= 0 && index < getKeyCount());
	Key& key = _keys[index];

	if (key.type == SegmentType::Linear) {
		return;
	}

	freeData(key);
	key.type = SegmentType::Linear;
}

template <class Allocator>
void Curve<Allocator>::setKeyValue(int32_t index, float value)
{
	GAFF_ASSERT(index >= 0 && index < getKeyCount());
	_keys[index].value = value;
}

template <class Allocator>
void Curve<Allocator>::setKeyT(int32_t index, float t)
{
	GAFF_ASSERT(index >= 0 && index < getKeyCount());

	const int32_t new_index = getInsertPosition(t);
	GAFF_ASSERT(new_index > -1);

	// Do nothing.
	if (new_index == index) {
		return;
	}

	// Swap the keys.
	if (abs(new_index - index) == 1) {
		eastl::swap(_keys[index], _keys[new_index]);
	}

	// Copy the key and assign new t value.
	Key key = _keys[index];
	key.t = t;

	_keys.erase(_keys.begin() + index);
	_keys.insert(_keys.begin() + new_index, key);
}

template <class Allocator>
void Curve<Allocator>::addKey(float t, float value, const BezierData& data)
{
	const int32_t index = getInsertPosition(t);
	GAFF_ASSERT(index > -1);

	Key key = createKey(SegmentType::Bezier, value, t);
	key.data_index = allocateData(_bezier_data, data);
	_keys.insert(_keys.begin() + index, key);
}

template <class Allocator>
void Curve<Allocator>::addKeyConstant(float t, float value)
{
	const int32_t index = getInsertPosition(t);
	GAFF_ASSERT(index > -1);

	Key key = createKey(SegmentType::Constant, value, t);
	_keys.insert(_keys.begin() + index, key);
}

template <class Allocator>
void Curve<Allocator>::addKeyLinear(float t, float value)
{
	const int32_t index = getInsertPosition(t);
	GAFF_ASSERT(index > -1);

	const Key key = createKey(SegmentType::Linear, value, t);
	_keys.insert(_keys.begin() + index, key);
}

template <class Allocator>
template <class T>
int32_t Curve<Allocator>::allocateData(Vector<T, Allocator>& data, const T& value)
{
	const int32_t size = static_cast<int32_t>(data.size());

	for (int32_t i = 0; i < size; ++i) {
		if (!data[i].free) {
			data[i] = value;
			data[i].free = false;
			return i;
		}
	}

	data.emplace_back(value);
	data.back().free = false;
	return size;
}

template <class Allocator>
typename Curve<Allocator>::Key Curve<Allocator>::createKey(SegmentType type, float value, float t)
{
	Key key;
	key.type = type;
	key.data_index = -1;
	key.value = value;
	key.t = t;

	return key;
}

template <class Allocator>
int32_t Curve<Allocator>::getInsertPosition(float& t)
{
	const int32_t num_keys = getKeyCount();

	for (int32_t i = 0; i < num_keys; ++i) {
		if (_keys[i].t > t) {
			return i;

		// Can't have two keys with the same value. Add an epsilon and check the next key.
		} else if (_keys[i].t == t) {
			t += Epsilon;
		}
	}

	return -1;
}

template <class Allocator>
void Curve<Allocator>::freeData(Key& key)
{
	switch (key.type) {
		case SegmentType::Bezier:
			GAFF_ASSERT(key.data_index > -1);
			_bezier_data[key.data_index].free = true;
			break;

		case SegmentType::Constant:
		case SegmentType::Linear:
			break;
	}
}
