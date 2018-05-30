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

template <class Message>
MessageBroadcaster::MessageFunctor<Message>::MessageFunctor(const Gaff::FunctionBinder<void, const Message&>& callback):
	_callback(callback)
{
}

template <class Message>
MessageBroadcaster::MessageFunctor<Message>::~MessageFunctor(void)
{
}

template <class Message>
void MessageBroadcaster::MessageFunctor<Message>::call(const IMessage& message)
{
	_callback(reinterpret_cast<const Message&>(message));
}



template <class Message>
BroadcastID MessageBroadcaster::listen(const Gaff::FunctionBinder<void, const Message&>& callback)
{
	BroadcastID id(Message::GetReflectionHash(), AtomicIncrement(&_next_id));
	IMessageFunctor* cb = SHIB_ALLOCT(MessageFunctor<Message>, GetAllocator(), callback);

	_listener_add_lock.lock();
	_listener_add_queue.emplacePush(id, cb);
	_listener_add_lock.unlock();

	return id;
}

template <class Message>
void MessageBroadcaster::broadcastNow(const Message& message)
{
	// Stop the listener map from being modified.
	_listener_lock.readLock();

	auto it = _listeners.findElementWithKey(Message::GetReflectionHash());

	if (it != _listeners.end()) {
		// Stop listeners list from being modified
		it->lock.readLock();

		for (auto it_listeners = it->listeners.begin(); it_listeners != it->listeners.end(); ++it_listeners) {
			it_listeners->call(message);
		}

		it->lock.readUnlock();
	}

	_listener_lock.readUnlock();
}

template <class Message>
void MessageBroadcaster::broadcast(const Message& message)
{
	Message* message = SHIB_ALLOCT(Message, GetAllocator(), message);

	_message_add_lock.lock();
	_message_queues[1 - _curr_queue].emplacePush(Message::GetReflectionHash(), message);
	_message_add_lock.unlock();
}
