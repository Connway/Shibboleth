/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_Reflection.h"
#include "Shibboleth_VectorMap.h"
#include "Shibboleth_JobPool.h"
#include "Shibboleth_Vector.h"
#include <Shibboleth_Memory.h>
#include <Gaff_Function.h>
#include <eathread/eathread_mutex.h>

NS_SHIBBOLETH

// Message Hash and Listener ID
using BroadcastID = std::pair<Gaff::Hash64, size_t>;

class Broadcaster
{
public:
	void init(void);

	template <class Message>
	BroadcastID listen(const eastl::function<void (const Message&)>& callback);

	template <class Message>
	void broadcastSync(const Message& message);

	template <class Message>
	void broadcast(const Message& message);

	void remove(BroadcastID id);

private:
	struct ListenerData
	{
		using Listener = eastl::function<void (const void*)>;

		Vector<Listener> listeners;
		Vector<size_t> unused_ids;
	};

	VectorMap<Gaff::Hash64, ListenerData> _listeners;
	EA::Thread::Mutex _listener_lock;

	JobPool* _job_pool = nullptr;
	Gaff::Counter* _counter = nullptr;
};

class BroadcastRemover
{
public:
	BroadcastRemover(BroadcastID id, Broadcaster& broadcaster);
	BroadcastRemover(const BroadcastRemover& remover);
	BroadcastRemover(void);
	~BroadcastRemover(void);

	// Takes ownership
	const BroadcastRemover& operator=(const BroadcastRemover& rhs);

	bool operator==(const BroadcastRemover& rhs) const;
	bool operator!=(const BroadcastRemover& rhs) const;

	void remove(void);

private:
	BroadcastID _id;
	Broadcaster* _broadcaster = nullptr;
	bool _valid = false;
};

#include "Shibboleth_Broadcaster.inl"

NS_END
