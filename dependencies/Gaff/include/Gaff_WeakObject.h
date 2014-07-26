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

#pragma once

#include "Gaff_RefCounted.h"
#include "Gaff_RefPtr.h"

NS_GAFF

template <class Allocator = DefaultAllocator>
class WeakObject
{
public:
	typedef WeakObject<Allocator> ObjType;

	class WeakReference : public RefCounted<Allocator>
	{
	public:
		// Didn't want to make this public, but no other way to get my allocators
		// working without getting "cannot access private method" errors.
		// We can trust that no one will do stupid stuff by making their
		// own WeakReference objects. :)
		WeakReference(ObjType* object, const Allocator& allocator):
			RefCounted<Allocator>(allocator), _object(object)
		{
		}

		const ObjType* getObject(void) const
		{
			return _object;
		}

		ObjType* getObject(void)
		{
			return _object;
		}

	private:
		void weakObjectDestroyed(void)
		{
			_object = nullptr;
		}

		ObjType* _object;

		friend class WeakObject<Allocator>;
	};

	WeakObject(const Allocator& allocator = Allocator()):
		_allocator(allocator)
	{
	}

	virtual ~WeakObject(void)
	{
		if (_weak_ref) {
			_weak_ref->weakObjectDestroyed();
		}
	}

	const RefPtr<WeakReference>& getWeakRef(void) const
	{
		if (!_weak_ref) {
			_weak_ref = _allocator.template allocT<WeakReference>((ObjType*)this, _allocator);
		}

		return _weak_ref;
	}

private:
	mutable RefPtr<WeakReference> _weak_ref;
	mutable Allocator _allocator;
};

NS_END
