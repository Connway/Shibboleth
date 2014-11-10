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

#include "Gaff_RefCounted.h"
#include "Gaff_ScopedLock.h"
#include "Gaff_SpinLock.h"
#include "Gaff_Function.h"
#include "Gaff_RefPtr.h"
#include "Gaff_Map.h"

NS_GAFF

/*!
	\brief
		A receipt used to signify that you are watching for the Watcher value to change.
		If this receipt is released, you will be removed from the watch list.
*/
typedef RefPtr<IRefCounted> WatchReceipt;

/*!
	\brief A class that wraps the assignment operator and calls a list of callbacks when the value changes.
*/
template <class T, class Allocator = DefaultAllocator>
class Watcher
{
public:
	typedef Gaff::FunctionBinder<void, const T&> Callback;

	Watcher(T&& data, const Allocator& allocator = Allocator());
	Watcher(const T& data, const Allocator& allocator = Allocator());
	Watcher(const Allocator& allocator = Allocator());
	~Watcher(void);

	bool init(void);

	const Watcher& operator=(T&& rhs);
	const Watcher& operator=(const T& rhs);

	WatchReceipt addCallback(const Callback& callback);
	void notifyCallbacks(void);

	const T* getPtr(void) const;
	const T& get(void) const;

private:
	class Remover : public RefCounted<Allocator>
	{
	public:
		Remover(Watcher* watcher, const Allocator& allocator);
		~Remover(void);

		void removeCallback(unsigned int id);

	private:
		Watcher* _watcher;
		SpinLock _lock;

		void watcherDeleted(void);

		friend class Watcher;
	};

	class Receipt : public IRefCounted
	{
	public:
		Receipt(unsigned int id, const RefPtr<Remover>& remover, const Allocator& allocator);
		~Receipt(void);

		void addRef(void) const;
		void release(void) const;

		unsigned int getRefCount(void) const;

	private:
		mutable Allocator _allocator;
		mutable RefPtr<Remover> _remover;
		mutable volatile unsigned int _ref_count;
		unsigned int _id;
	};

	Map<unsigned int, Callback, Allocator> _callbacks;
	T _data;

	Allocator _allocator;

	RefPtr<Remover> _remover;
	SpinLock _lock;

	unsigned int _next_id;

	void remove(unsigned int id);

	GAFF_NO_COPY(Watcher);
	GAFF_NO_MOVE(Watcher);

	friend class Remover;
};

#include "Gaff_Watcher.inl"

NS_END
