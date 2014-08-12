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

///////////////////////////
//    MEMBER FUNCTION    //
///////////////////////////
template <class Allocator>
template <class Message, class Object>
MessageBroadcaster<Allocator>::MemberFunction<Message, Object>::MemberFunction(Object* object, FunctionType function):
	_function(function), _object(object)
{
}

template <class Allocator>
template <class Message, class Object>
void MessageBroadcaster<Allocator>::MemberFunction<Message, Object>::call(void* message)
{
	Message* msg = (Message*)message;
	(_object->*_function)(*msg);
}

///////////////.////
//    FUNCTION    //
//////////////./////
template <class Allocator>
template <class Message>
MessageBroadcaster<Allocator>::Function<Message>::Function(FunctionType function):
	_function(function)
{
}

template <class Allocator>
template <class Message>
void MessageBroadcaster<Allocator>::Function<Message>::call(void* message)
{
	Message* msg = (Message*)message;
	_function(*msg);
}

///////////////////
//    FUNCTOR    //
///////////////////
template <class Allocator>
template <class Message, class FunctorT>
MessageBroadcaster<Allocator>::Functor<Message, FunctorT>::Functor(const FunctorT& functor):
	_functor(functor)
{
}

template <class Allocator>
template <class Message, class FunctorT>
void MessageBroadcaster<Allocator>::Functor<Message, FunctorT>::call(void* message)
{
	Message* msg = (Message*)message;
	_functor(*msg);
}

///////////////////
//    REMOVER    //
///////////////////
template <class Allocator>
MessageBroadcaster<Allocator>::Remover::Remover(MessageBroadcaster* broadcaster, const Allocator& allocator):
	RefCounted<Allocator>(allocator), _broadcaster(broadcaster)
{
}

template <class Allocator>
MessageBroadcaster<Allocator>::Remover::~Remover(void)
{
}

template <class Allocator>
void MessageBroadcaster<Allocator>::Remover::removeListener(const AHashString<Allocator>& message_type, const IFunction* listener)
{
	ScopedLock<SpinLock> scoped_lock(_lock);

	if (_broadcaster) {
		_broadcaster->removeListener(message_type, listener);
	}
}

template <class Allocator>
void MessageBroadcaster<Allocator>::Remover::broadcasterDeleted(void)
{
	ScopedLock<SpinLock> scoped_lock(_lock);
	_broadcaster = nullptr;
}

///////////////////
//    RECEIPT    //
///////////////////
template <class Allocator>
MessageBroadcaster<Allocator>::Receipt::Receipt(const RefPtr<Remover>& remover,
												const AHashString<Allocator>* message_type,
												const IFunction* listener,
												const Allocator& allocator):
	_allocator(allocator), _remover(remover),
	_message_type(message_type), _listener(listener),
	_ref_count(0)
{
}

template <class Allocator>
MessageBroadcaster<Allocator>::Receipt::~Receipt(void)
{
}

template <class Allocator>
void MessageBroadcaster<Allocator>::Receipt::addRef(void) const
{
	AtomicIncrement(&_ref_count);
}

template <class Allocator>
void MessageBroadcaster<Allocator>::Receipt::release(void) const
{
	unsigned int new_count = AtomicDecrement(&_ref_count);

	if (!new_count) {
		_remover->removeListener(*_message_type, _listener);

		// When the allocator resides in the object we are deleting,
		// we need to make a copy, otherwise there will be crashes.
		Allocator allocator = _allocator;
		allocator.freeT(this);
	}
}

template <class Allocator>
unsigned int MessageBroadcaster<Allocator>::Receipt::getRefCount(void) const
{
	return _ref_count;
}

//////////////////////////
//    BROADCAST TASK    //
//////////////////////////
template <class Allocator>
MessageBroadcaster<Allocator>::BroadcastTask::BroadcastTask(MessageBroadcaster<Allocator>& broadcaster, const AHashString<Allocator>& msg_type, void* msg):
	_listeners(broadcaster._listeners[msg_type]), _broadcaster(broadcaster), _msg(msg)
{
}

template <class Allocator>
void MessageBroadcaster<Allocator>::BroadcastTask::doTask(void)
{
	ScopedReadLock<ReadWriteSpinLock> scope_lock(_listeners.first);

	for (auto it = _listeners.second.begin(); it != _listeners.second.end(); ++it) {
		(*it)->call(_msg);
	}

	_broadcaster._allocator.free(_msg);
}

///////////////////////////////
//    MESSAGE BROADCASTER    //
///////////////////////////////
template <class Allocator>
MessageBroadcaster<Allocator>::MessageBroadcaster(const Allocator& allocator):
	_listeners(allocator), _message_queue(allocator), _allocator(allocator)
{
}

template <class Allocator>
MessageBroadcaster<Allocator>::~MessageBroadcaster(void)
{
	destroy();
}

template <class Allocator>
template <class Message, class Object>
MessageReceipt MessageBroadcaster<Allocator>::listen(Object* object, void (Object::*function)(const Message&))
{
	MemberFunction<Message, Object>*  function_binder = (MemberFunction<Message, Object>*)_allocator.alloc(sizeof(MemberFunction<Message, Object>));
	construct(function_binder, object, function);

	IFuncPtr ptr(function_binder);

	_listener_lock.lock();
	ScopedWriteLock<ReadWriteSpinLock> scoped_lock(_listeners[Message::g_Hash].first);
	_listener_lock.unlock();

	ListenerList& listeners = _listeners[Message::g_Hash].second;
	listeners.push(ptr);

	Receipt* temp = _allocator.template allocT<Receipt>(_remover, &Message::g_Hash, function_binder, _allocator);
	return MessageReceipt(temp);
}

template <class Allocator>
template <class Message>
MessageReceipt MessageBroadcaster<Allocator>::listen(void (*function)(const Message&))
{
	Function<Message>*  function_binder = (Function<Message>*)_allocator.alloc(sizeof(Function<Message>));
	construct(function_binder, function);

	IFuncPtr ptr(function_binder);

	_listener_lock.lock();
	ScopedWriteLock<ReadWriteSpinLock> scoped_lock(_listeners[Message::g_Hash].first);
	_listener_lock.unlock();

	ListenerList& listeners = _listeners[Message::g_Hash].second;
	listeners.push(ptr);

	Receipt* temp = _allocator.template allocT<Receipt>(_remover, &Message::g_Hash, function_binder, _allocator);
	return MessageReceipt(temp);
}

template <class Allocator>
template <class Message, class FunctorT>
MessageReceipt MessageBroadcaster<Allocator>::listen(const FunctorT& functor)
{
	Functor<Message, FunctorT>*  function_binder = (Functor<Message, FunctorT>*)_allocator.alloc(sizeof(Functor<Message, FunctorT>));
	construct(function_binder, functor);

	IFuncPtr ptr(function_binder);

	_listener_lock.lock();
	ScopedWriteLock<ReadWriteSpinLock> scoped_lock(_listeners[Message::g_Hash].first);
	_listener_lock.unlock();

	ListenerList& listeners = _listeners[Message::g_Hash].second;
	listeners.push(ptr);

	Receipt* temp = _allocator.template allocT<Receipt>(_remover, &Message::g_Hash, function_binder, _allocator);
	return MessageReceipt(temp);
}

template <class Allocator>
template <class Message>
void MessageBroadcaster<Allocator>::broadcast(const Message& message)
{
	Message* msg = _allocator.template allocT<Message>(message);

	ScopedLock<SpinLock> scoped_lock(_message_lock);
	_message_queue.push(MakePair(Message::g_Hash, (void*)msg));
}

template <class Allocator>
void MessageBroadcaster<Allocator>::update(ThreadPool<Allocator>& thread_pool)
{
	ScopedLock<SpinLock> scoped_lock(_message_lock);

	// Possibility for optimization, if we sort this list by message type, we can do all similar messages at once
	for (unsigned int i = 0; i < _message_queue.size(); ++i) {
		const Pair< AHashString<Allocator>, void*>& msg = _message_queue[i];

		// We lock here because we are potentially creating a new entry in the HashMap.
		_listener_lock.lock();
		BroadcastTask* task = _allocator.template allocT<BroadcastTask>(*this, msg.first, msg.second);
		_listener_lock.unlock();

		TaskPtr<Allocator> task_ptr(task);

		if (task) {
			thread_pool.addTask(task_ptr);
		}
	}

	_message_queue.clear();
}

template <class Allocator>
void MessageBroadcaster<Allocator>::update(void)
{
	// Possibility for optimization, if we sort this list by message type, we can do all similar messages at once
	for (unsigned int i = 0; i < _message_queue.size(); ++i) {
		const Pair< AHashString<Allocator>, void*>& msg = _message_queue[i];
		ListenerList& listeners = _listeners[msg.first].second;

		for (unsigned int j = 0; j < listeners.size(); ++j) {
			listeners[j]->call(msg.second);
		}

		_allocator.free(msg.second);
	}

	_message_queue.clear();
}

template <class Allocator>
void MessageBroadcaster<Allocator>::removeListener(const AHashString<Allocator>& message_type, const IFunction* listener)
{
#ifdef _DEBUG
	_listener_lock.lock();
	assert(message_type.size() && _listeners[message_type].second.size());
	_listener_lock.unlock();
#endif

	ScopedWriteLock<ReadWriteSpinLock> scoped_lock(_listeners[message_type].first);
	ListenerList& listeners = _listeners[message_type].second;

	int index = listeners.linearSearch(0, listeners.size(), listener);

	if (index > -1) {
		listeners.erase(index);
	}
}

template <class Allocator>
bool MessageBroadcaster<Allocator>::init(void)
{
	_remover = _allocator.template allocT<Remover>(this, _allocator);
	return _remover != nullptr;
}

template <class Allocator>
void MessageBroadcaster<Allocator>::destroy(void)
{
	_remover->broadcasterDeleted();

	ScopedLock<SpinLock> scoped_lock(_message_lock);

	for (auto it = _message_queue.begin(); it != _message_queue.end(); ++it) {
		_allocator.free(it->second);
	}

	_message_queue.clear();
	_listeners.clear();
}
