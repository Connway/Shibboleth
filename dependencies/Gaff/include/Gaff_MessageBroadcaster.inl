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
MessageBroadcaster<Allocator>::Remover::Remover(MessageBroadcaster<Allocator>* broadcaster,
												const AHashString<Allocator>* message_type,
												const IFunction* listener):
	_broadcaster(broadcaster), _message_type(message_type), _listener(listener), _ref_count(0)
{
}

template <class Allocator>
MessageBroadcaster<Allocator>::Remover::~Remover(void)
{
}

template <class Allocator>
void MessageBroadcaster<Allocator>::Remover::addRef(void) const
{
	++_ref_count;
}

template <class Allocator>
void MessageBroadcaster<Allocator>::Remover::release(void) const
{
	--_ref_count;

	if (!_ref_count) {
		_broadcaster->removeListener(*_message_type, _listener);
		_broadcaster->_allocator.freeT(this);
	}
}

template <class Allocator>
unsigned int MessageBroadcaster<Allocator>::Remover::getRefCount(void) const
{
	return _ref_count;
}

///////////////////////////////
//    MESSAGE BROADCASTER    //
///////////////////////////////
template <class Allocator>
MessageBroadcaster<Allocator>::MessageBroadcaster(const Allocator& allocator):
	_allocator(allocator)
{
}

template <class Allocator>
MessageBroadcaster<Allocator>::~MessageBroadcaster(void)
{
}

template <class Allocator>
template <class Message, class Object>
MessageReceipt MessageBroadcaster<Allocator>::listen(Object* object, void (Object::*function)(const Message&))
{
	MemberFunction<Message, Object>*  function_binder = (MemberFunction<Message, Object>*)_allocator.alloc(sizeof(MemberFunction<Message, Object>));
	construct(function_binder, object, function);

	IFuncPtr ptr = function_binder;
	ListenerList& listeners = _listeners[Message::g_Hash];
	listeners.push(ptr);

	Remover* temp = _allocator.template allocT<Remover>(this, &Message::g_Hash, function_binder);
	return MessageReceipt(temp);
}

template <class Allocator>
template <class Message>
MessageReceipt MessageBroadcaster<Allocator>::listen(void (*function)(const Message&))
{
	Function<Message>*  function_binder = (Function<Message>*)_allocator.alloc(sizeof(Function<Message>));
	construct(function_binder, function);

	IFuncPtr ptr = function_binder;
	ListenerList& listeners = _listeners[Message::g_Hash];
	listeners.push(ptr);

	Remover* temp = _allocator.template allocT<Remover>(this, &Message::g_Hash, function_binder);
	return MessageReceipt(temp);
}

template <class Allocator>
template <class Message, class FunctorT>
MessageReceipt MessageBroadcaster<Allocator>::listen(const FunctorT& functor)
{
	Functor<Message, FunctorT>*  function_binder = (Functor<Message, FunctorT>*)_allocator.alloc(sizeof(Functor<Message, FunctorT>));
	construct(function_binder, functor);

	IFuncPtr ptr = function_binder;
	ListenerList& listeners = _listeners[Message::g_Hash];
	listeners.push(ptr);

	Remover* temp = _allocator.template allocT<Remover>(this, &Message::g_Hash, function_binder);
	return MessageReceipt(temp);
}

template <class Allocator>
template <class Message>
void MessageBroadcaster<Allocator>::broadcastNow(const Message& message)
{
	ListenerList& listeners = _listeners[Message::g_Hash];

	for (unsigned int i = 0; i < listeners.size(); ++i) {
		listeners[i]->call((void*)&message);
	}
}

template <class Allocator>
template <class Message>
void MessageBroadcaster<Allocator>::broadcast(const Message& message)
{
	void* msg = _allocator.alloc(sizeof(Message));
	memcpy(msg, &message, sizeof(Message));

	_message_queue.push(MakePair(Message::g_Hash, msg));
}

template <class Allocator>
void MessageBroadcaster<Allocator>::update(void)
{
	// possibility for optimization, if we sort this list by message type, we can do all similar messages at once
	for (unsigned int i = 0; i < _message_queue.size(); ++i) {
		const Pair< AHashString<Allocator>, void*>& msg = _message_queue[i];
		ListenerList& listeners = _listeners[msg.first];

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
	assert(message_type.size() && _listeners[message_type].size());
	ListenerList& listeners = _listeners[message_type];

	int index = listeners.linearSearch(0, listeners.size(), listener);

	if (index > -1) {
		listeners.erase(index);
	}
}

