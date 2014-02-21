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

template <class Allocator>
TimerCallbackManager<Allocator>::TimerCallbackManager(const Allocator& allocator):
	_callbacks(allocator)
{
}

template <class Allocator>
TimerCallbackManager<Allocator>::~TimerCallbackManager(void)
{
}

template <class Allocator>
void TimerCallbackManager<Allocator>::update(double dt)
{
	Array<CallbackData, Allocator>::Iterator it = _callbacks.begin();

	while (it != _callbacks.end()) {
		it->elapsed += dt;

		if (it->elapsed > it->period) {
			it->callback();

			if (cbd.once) {
				it = _callbacks.erase(it);
			} else {
				it->elapsed -= it->period;
				++it;
			}
		}
	}
}

template <class Allocator>
unsigned int TimerCallbackManager<Allocator>::registerForCallback(void (*function)(unsigned int), double period, bool periodic)
{
	return registerCallback(Bind(function), period, periodic);
}

template <class Allocator>
template <class T>
unsigned int TimerCallbackManager<Allocator>::registerForCallback(T* object, void (T::*function)(unsigned int), double period, bool periodic)
{
	return registerCallback(Bind(object, function), period, periodic);
}

template <class Allocator>
template <class T>
unsigned int TimerCallbackManager<Allocator>::registerForCallback(const T& object, double period, bool periodic)
{
	return registerCallback(Bind<T, void, unsigned int>(object), period, periodic);
}

template <class Allocator>
unsigned int TimerCallbackManager<Allocator>::registerCallback(const FunctionBinder<void, unsigned int>& callback, double period, bool periodic)
{
	CallbackData data = {
		callback,
		0.0,
		period,
		++_nextID,
		!periodic
	};

	_callbacks.push(data);

	return data.id;
}

template <class Allocator>
void TimerCallbackManager<Allocator>::removeCallback(unsigned int id)
{
	Array<CallbackData, Allocator>::Iterator it = _callbacks.binarySearch(_callbacks.begin(), _callbacks.end(), id);

	if (it != _callbacks.end() && it->id == id) {
		_callbacks.erase(it);
	}
}
