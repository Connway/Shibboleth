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

template <class PointType, class Allocator>
BezierCurve<PointType, Allocator>::BezierCurve(const BezierCurve<PointType, Allocator>& curve):
	_points(curve._points)
{
}

template <class PointType, class Allocator>
BezierCurve<PointType, Allocator>::BezierCurve(BezierCurve<PointType, Allocator>&& curve):
	_points(std::move(curve._points))
{
}

template <class PointType, class Allocator>
BezierCurve<PointType, Allocator>::BezierCurve(const Allocator& allocator):
	_points(allocator)
{
}

template <class PointType, class Allocator>
BezierCurve<PointType, Allocator>::~BezierCurve(void)
{
}

template <class PointType, class Allocator>
PointType BezierCurve<PointType, Allocator>::sample(float t) const
{
	GAFF_ASSERT(!_points.empty());

	// Clamp to range and normalize
	t = Clamp(t, _points.first().first, _points.last().first) / (_points.last().first - _points.first().first);

	PointType point = PointType::zero;

	for (unsigned int i = 0; i < _points.size(); ++i) {
		point += _points[i].second * BernsteinPolynomial(t, (float)(_points.size() - 1), (float)i);
	}

	return point;
}

template <class PointType, class Allocator>
void BezierCurve<PointType, Allocator>::addKey(float t, const PointType& point)
{
	unsigned int i = 0;

	for (; i < _points.size(); ++i) {
		GAFF_ASSERT(_points[i].first != t);

		if (t < _points[i].first) {
			break;
		}
	}

	_points.insert(MakePair(t, point), i);
}

template <class PointType, class Allocator>
void BezierCurve<PointType, Allocator>::removeKey(unsigned int index)
{
	GAFF_ASSERT(index < _points.size());
	_points.erase(index);
}

template <class PointType, class Allocator>
unsigned int BezierCurve<PointType, Allocator>::getNumKeys(void) const
{
	return _points.size();
}

template <class PointType, class Allocator>
const typename BezierCurve<PointType, Allocator>::Key& BezierCurve<PointType, Allocator>::getKey(unsigned int index)
{
	GAFF_ASSERT(index < _points.size());
	return Key(_points[i].first, _points[i].second);
}
