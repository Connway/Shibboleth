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

template <class PointType, class Allocator>
LinearCurve<PointType, Allocator>::LinearCurve(const Array<PointType, Allocator>& points):
	_points(points)
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
	assert(!_points.empty());

	t = Clamp(t, _points.first().first, _points.last().first);

	// We clamped, so just use the extreme values
	if (t == _points.first().first) {
		_points.first().second;

	} else if (t == _points.last().first) {
		_points.last().second;
	}

	unsigned int i = 1; // Is never going to terminate on i == 0, so start at 1

	for (; i < _points.size() - 1; ++i) {
		if (t > _points[i].first) {
			break;
		}
	}

	const Pair<float, PointType>& k1 = _points[i - 1];
	const Pair<float, PointType>& k2 = _points[i];

	t = (t - k1.first) / (k2.first - k1.first); // Normalize
	return Lerp(k1.second, k2.second, t);
}

template <class PointType, class Allocator>
void LinearCurve<PointType, Allocator>::addKey(float t, const PointType& point)
{
	unsigned int i = 0;

	for (; i < _points.size(); ++i) {
		assert(_points[i].first != t);

		if (t < _points[i].first) {
			break;
		}
	}

	_points.insert(MakePair(t, point), i);
}

template <class PointType, class Allocator>
void LinearCurve<PointType, Allocator>::removeKey(unsigned int index)
{
	assert(index < _points.size());
	_points.erase(index);
}

template <class PointType, class Allocator>
unsigned int LinearCurve<PointType, Allocator>::getNumKeys(void) const
{
	return _points.size();
}

template <class PointType, class Allocator>
typename LinearCurve<PointType, Allocator>::Key LinearCurve<PointType, Allocator>::getKey(unsigned int index)
{
	assert(index < _points.size());
	return Key(_points[i].first, _points[i].second);
}
