/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

///////////////////
//    REMOVER    //
///////////////////
template <class T, class Allocator>
Watcher<T, Allocator>::Remover::Remover(Watcher* watcher, const Allocator& allocator):
	RefCounted<Allocator>(allocator), _watcher(watcher)
{
}

template <class T, class Allocator>
Watcher<T, Allocator>::Remover::~Remover(void)
{
}

template <class T, class Allocator>
void Watcher<T, Allocator>::Remover::removeCallback(unsigned int id)
{
	ScopedLock<SpinLock> scoped_lock(_lock);

	if (_watcher) {
		_watcher->remove(id);
	}
}

template <class T, class Allocator>
void Watcher<T, Allocator>::Remover::watcherDeleted(void)
{
	ScopedLock<SpinLock> scoped_lock(_lock);
	_watcher = nullptr;
}


///////////////////
//    RECEIPT    //
///////////////////
template <class T, class Allocator>
Watcher<T, Allocator>::Receipt::Receipt(unsigned int id, const RefPtr<Remover>& remover, const Allocator& allocator):
	_allocator(allocator), _remover(remover), _ref_count(0), _id(id)
{
}

template <class T, class Allocator>
Watcher<T, Allocator>::Receipt::~Receipt(void)
{
}

template <class T, class Allocator>
void Watcher<T, Allocator>::Receipt::addRef(void) const
{
	AtomicIncrement(&_ref_count);
}

template <class T, class Allocator>
void Watcher<T, Allocator>::Receipt::release(void) const
{
	unsigned int new_count = AtomicDecrement(&_ref_count);

	if (!new_count) {
		_remover->removeCallback(_id);

		// When the allocator resides in the object we are deleting,
		// we need to make a copy, otherwise there will be crashes.
		Allocator allocator = _allocator;
		allocator.freeT(this);
	}
}

template <class T, class Allocator>
unsigned int Watcher<T, Allocator>::Receipt::getRefCount(void) const
{
	return _ref_count;
}


///////////////////
//    WATCHER    //
///////////////////
template <class T, class Allocator>
Watcher<T, Allocator>::Watcher(T&& data, const Allocator& allocator):
	_callbacks(allocator), _data(Move(data)),
	_allocator(allocator), _next_id(0)
{
}

template <class T, class Allocator>
Watcher<T, Allocator>::Watcher(const T& data, const Allocator& allocator):
	_callbacks(allocator), _data(data),
	_allocator(allocator), _next_id(0)
{
}

template <class T, class Allocator>
Watcher<T, Allocator>::Watcher(const Allocator& allocator):
	_callbacks(allocator), _allocator(allocator), _next_id(0)
{
}

template <class T, class Allocator>
Watcher<T, Allocator>::~Watcher(void)
{
	if (_remover) {
		_remover->watcherDeleted();
	}
}

/*!
	\brief Initializes the watcher. This must be called before the Watcher is used.
*/
template <class T, class Allocator>
bool Watcher<T, Allocator>::init(void)
{
	_remover = _allocator.template allocT<Remover>(this, _allocator);
	return _remover != nullptr;
}

/*!
	\brief Does move assignment operation on the value and notifies all callbacks that the value has changed.
*/
template <class T, class Allocator>
const Watcher<T, Allocator>& Watcher<T, Allocator>::operator=(T&& rhs)
{
	ScopedLock<SpinLock> scoped_lock(_lock);
	_data = Move(rhs);

	notifyCallbacks();

	return *this;
}

/*!
	\brief Does normal assignment operation on the value and notifies all callbacks that the value has changed.
*/
template <class T, class Allocator>
const Watcher<T, Allocator>& Watcher<T, Allocator>::operator=(const T& rhs)
{
	ScopedLock<SpinLock> scoped_lock(_lock);
	_data = rhs;

	notifyCallbacks();

	return *this;
}

/*!
	\brief Adds a callback for when the value gets changed.
*/
template <class T, class Allocator>
WatchReceipt Watcher<T, Allocator>::addCallback(const typename Watcher<T, Allocator>::Callback& callback)
{
	_lock.lock();

	unsigned int id = _next_id;
	++_next_id;
	_callbacks.insert(id, callback);

	_lock.unlock();

	Receipt* receipt = _allocator.template allocT<Receipt>(id, _remover, _allocator);
	return WatchReceipt(receipt);
}

/*!
	\brief Notifies all callbacks that the value has changed.
*/
template <class T, class Allocator>
void Watcher<T, Allocator>::notifyCallbacks(void)
{
	for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it) {
		it->second(_data);
	}
}

/*!
	\brief Gets a pointer to the internal data.
*/
template <class T, class Allocator>
const T* Watcher<T, Allocator>::getPtr(void) const
{
	return &_data;
}

/*!
	\brief Gets a reference to the internal data.
*/
template <class T, class Allocator>
const T& Watcher<T, Allocator>::get(void) const
{
	return _data;
}

/*!
	\brief Gets a pointer to the internal data.
*/
template <class T, class Allocator>
T* Watcher<T, Allocator>::getPtr(void)
{
	return &_data;
}

/*!
	\brief Gets a reference to the internal data.
*/
template <class T, class Allocator>
T& Watcher<T, Allocator>::get(void)
{
	return _data;
}

template <class T, class Allocator>
void Watcher<T, Allocator>::remove(unsigned int id)
{
	ScopedLock<SpinLock> scoped_lock(_lock);
	_callbacks.erase(id);
}
