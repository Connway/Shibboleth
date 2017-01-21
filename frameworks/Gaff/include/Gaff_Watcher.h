/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gaff_ScopedLock.h"
#include "Gaff_SpinLock.h"
#include "Gaff_Function.h"
#include "Gaff_Array.h"

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class Watcher
{
public:
	typedef Gaff::FunctionBinder<void, const T&> Callback;

	Watcher(T&& data, const Allocator& allocator = Allocator());
	Watcher(const T& data, const Allocator& allocator = Allocator());
	Watcher(const Allocator& allocator = Allocator());
	~Watcher(void);

	const Watcher& operator=(T&& rhs);
	const Watcher& operator=(const T& rhs);

	void addCallback(const Callback& callback);
	void removeCallback(const Callback& callback);
	void notifyCallbacks(void);

	const T* getPtr(void) const;
	const T& get(void) const;
	T* getPtr(void);
	T& get(void);

private:
	Array<Callback, Allocator> _callbacks;
	T _data;

	Allocator _allocator;

	SpinLock _lock;

	GAFF_NO_COPY(Watcher);
	GAFF_NO_MOVE(Watcher);
};

#include "Gaff_Watcher.inl"

NS_END
