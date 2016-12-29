/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_Vector.h"
#include "Shibboleth_VectorMap.h"
#include "Shibboleth_SmartPtrs.h"
#include <Shibboleth_Memory.h>
#include <Gaff_Function.h>
#include <Gaff_SpinLock.h>
#include <atomic>

NS_GAFF
	struct Counter;
NS_END

NS_SHIBBOLETH

// Do not use messages for triggering things that need to happen this frame.

class IMessage
{
public:
	virtual ~IMessage(void) {}
};


// Message Hash and Listener ID
using BroadcastID = std::pair<int32_t, int32_t>;

class MessageBroadcaster
{
public:
	MessageBroadcaster(void);
	~MessageBroadcaster(void);

	template <class Message>
	BroadcastID listen(const Gaff::FunctionBinder<void, const Message&>& callback);

	template <class Message>
	void broadcastNow(const Message& message);

	template <class Message>
	void broadcast(const Message& message);

	void remove(BroadcastID id);

	void update(bool wait = false);

private:
	class IMessageFunctor
	{
	public:
		IMessageFunctor(void) {}
		virtual ~IMessageFunctor(void) {}

		virtual void call(const IMessage& message) = 0;
	};

	template <class Message>
	class MessageFunctor : public IMessageFunctor
	{
	public:
		MessageFunctor(const Gaff::FunctionBinder<void, const Message&>& callback);
		~MessageFunctor(void);

		void call(const IMessage& message);

	private:
		Gaff::FunctionBinder<void, const Message&> _callback;
	};

	using Listener = std::pair<size_t, IMessageFunctor*>;

	struct ListenerData
	{
		ListenerData(void) = default;
		GAFF_COPY_DEFAULT(ListenerData);
		GAFF_MOVE_DEFAULT(ListenerData);

		Vector<Listener> listeners;
		UniquePtr<Gaff::ReadWriteSpinLock> lock = UniquePtr<Gaff::ReadWriteSpinLock>(SHIB_ALLOCT(Gaff::ReadWriteSpinLock, *GetAllocator()));
	};

	Vector< std::pair<int32_t, IMessage*> > _message_queues[2];
	Gaff::SpinLock _message_add_lock;

	VectorMap<int32_t, ListenerData> _listeners;
	Gaff::ReadWriteSpinLock _listener_lock;

	Vector< std::pair<BroadcastID, IMessageFunctor*> > _listener_add_queue;
	Gaff::SpinLock _listener_add_lock;

	Vector<BroadcastID> _listener_remove_queue;
	Gaff::SpinLock _listener_remove_lock;

	Gaff::Counter* _counter;

	std::atomic_int32_t _next_message;
	std::atomic_int32_t _next_id;
	int32_t _curr_queue;

	void addListeners(void);
	void removeListeners(void);
	void swapMessageQueues(void);
	void spawnBroadcastTasks(bool wait);
	void waitForCounter(void);

	friend void BroadcastJob(void*);
};

class BroadcastRemover
{
public:
	BroadcastRemover(BroadcastID id, MessageBroadcaster& broadcaster);
	BroadcastRemover(void);
	~BroadcastRemover(void);

	// Takes ownership
	const BroadcastRemover& operator=(const BroadcastRemover& rhs);

	bool operator==(const BroadcastRemover& rhs) const;
	bool operator!=(const BroadcastRemover& rhs) const;

	void remove(void);

private:
	BroadcastID _id;
	MessageBroadcaster* _broadcaster;
	bool _valid;
};

#include "Shibboleth_MessageBroadcaster.inl"

NS_END
