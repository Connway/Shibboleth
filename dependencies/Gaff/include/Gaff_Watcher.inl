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

template <class T, class Allocator>
bool Watcher<T, Allocator>::init(void)
{
	_remover = _allocator.template allocT<Remover>(this, _allocator);
	return _remover != nullptr;
}

template <class T, class Allocator>
const Watcher<T, Allocator>& Watcher<T, Allocator>::operator=(T&& rhs)
{
	ScopedLock<SpinLock> scoped_lock(_lock);
	_data = Move(rhs);

	for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it) {
		it->second(_data);
	}

	return *this;
}

template <class T, class Allocator>
const Watcher<T, Allocator>& Watcher<T, Allocator>::operator=(const T& rhs)
{
	ScopedLock<SpinLock> scoped_lock(_lock);
	_data = rhs;

	for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it) {
		it->second(_data);
	}

	return *this;
}

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
void Watcher<T, Allocator>::remove(unsigned int id)
{
	ScopedLock<SpinLock> scoped_lock(_lock);
	_callbacks.erase(id);
}
