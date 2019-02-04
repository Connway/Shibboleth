/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

template <class PointType, class Allocator>
LinearCurve<PointType, Allocator>::LinearCurve(const LinearCurve<PointType, Allocator>& curve):
	_points(curve._points)
{
}

template <class PointType, class Allocator>
LinearCurve<PointType, Allocator>::LinearCurve(BezierCurve<PointType, Allocator>&& curve):
	_points(std::move(curve._points))
{
}

template <class PointType, class Allocator>
LinearCurve<PointType, Allocator>::LinearCurve(const Allocator& allocator):
	_points(allocator)
{
}

template <class PointType, class Allocator>
LinearCurve<PointType, Allocator>::~LinearCurve(void)
{
}

template <class PointType, class Allocator>
PointType LinearCurve<PointType, Allocator>::sample(float t) const
{
	GAFF_ASSERT(!_points.empty());

	t = Clamp(t, _points.front().t, _points.back().t);

	// We clamped, so just use the extreme values
	if (t <= _points.front().t) {
		return _points.front().point;

	} else if (t >= _points.back().t) {
		return _points.back().point;
	}

	int32_t i = 1; // Is never going to terminate on i == 0, so start at 1

	for (; i < static_cast<int32_t>(_points.size()) - 1; ++i) {
		if (t > _points[i].first) {
			break;
		}
	}

	const Pair<float, PointType>& k1 = _points[i - 1];
	const Pair<float, PointType>& k2 = _points[i];

	t = (t - k1.t) / (k2.t - k1.t); // Normalize
	return Lerp(k1.point, k2.point, t);
}

template <class PointType, class Allocator>
void LinearCurve<PointType, Allocator>::addKey(float t, const PointType& point)
{
	int32_t i = 0;

	for (; i < static_cast<int32_t>(_points.size()); ++i) {
		GAFF_ASSERT(_points[i].t != t);

		if (t < _points[i].t) {
			break;
		}
	}

	_points.insert({ point, t }), i);
}

template <class PointType, class Allocator>
void LinearCurve<PointType, Allocator>::removeKey(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_points.size()));
	_points.erase(index);
}

template <class PointType, class Allocator>
const typename LinearCurve<PointType, Allocator>::Key& LinearCurve<PointType, Allocator>::getKey(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_points.size()));
	return _points[i];
}

template <class PointType, class Allocator>
int32_t LinearCurve<PointType, Allocator>::getNumKeys(void) const
{
	return static_cast<int32_t>(_points.size());
}
