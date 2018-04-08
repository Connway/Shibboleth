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

template <class T, class Allocator>
Watcher<T, Allocator>::Watcher(T&& data, const Allocator& allocator):
	_callbacks(allocator), _data(std::move(data)),
	_allocator(allocator)
{
}

template <class T, class Allocator>
Watcher<T, Allocator>::Watcher(const T& data, const Allocator& allocator):
	_callbacks(allocator), _data(data),
	_allocator(allocator)
{
}

template <class T, class Allocator>
Watcher<T, Allocator>::Watcher(const Allocator& allocator):
	_callbacks(allocator), _allocator(allocator)
{
}

template <class T, class Allocator>
Watcher<T, Allocator>::~Watcher(void)
{
}

template <class T, class Allocator>
const Watcher<T, Allocator>& Watcher<T, Allocator>::operator=(T&& rhs)
{
	_data = std::move(rhs);
	notifyCallbacks();
	return *this;
}

template <class T, class Allocator>
const Watcher<T, Allocator>& Watcher<T, Allocator>::operator=(const T& rhs)
{
	_data = rhs;
	notifyCallbacks();
	return *this;
}

template <class T, class Allocator>
void Watcher<T, Allocator>::addCallback(const typename Watcher<T, Allocator>::Callback& callback)
{
	std::lock_guard<std::mutex> lock(_lock);
	_callbacks.emplacePush(callback);
}

template <class T, class Allocator>
void Watcher<T, Allocator>::removeCallback(const typename Watcher<T, Allocator>::Callback& callback)
{
	std::lock_guard<std::mutex> lock(_lock);

	auto it = _callbacks.linearSearch(callback);
	GAFF_ASSERT(it == _callbacks.end());

	_callbacks.fastErase(it);
}

template <class T, class Allocator>
void Watcher<T, Allocator>::notifyCallbacks(void)
{
	std::lock_guard<std::mutex> lock(_lock);

	for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it) {
		(*it)(_data);
	}
}

template <class T, class Allocator>
const T* Watcher<T, Allocator>::getPtr(void) const
{
	return &_data;
}

template <class T, class Allocator>
const T& Watcher<T, Allocator>::get(void) const
{
	return _data;
}

template <class T, class Allocator>
T* Watcher<T, Allocator>::getPtr(void)
{
	return &_data;
}

template <class T, class Allocator>
T& Watcher<T, Allocator>::get(void)
{
	return _data;
}
