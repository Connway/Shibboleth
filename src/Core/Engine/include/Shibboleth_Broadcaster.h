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

#include "Reflection/Shibboleth_Reflection.h"
#include "Shibboleth_VectorMap.h"
#include "Shibboleth_JobPool.h"
#include "Shibboleth_Vector.h"
#include <Shibboleth_Memory.h>
#include <Gaff_Function.h>
#include <eathread/eathread_mutex.h>

NS_SHIBBOLETH

class Broadcaster final
{
public:
	struct ID final
	{
		Gaff::Hash64 msg_hash;
		Gaff::Hash64 cb_hash;
	};

	void init(void);

	template <class Message>
	ID listen(const eastl::function<void (const Message&)>& callback);

	template <class Message>
	ID listen(eastl::function<void (const Message&)>&& callback);

	template <class Message>
	bool remove(const eastl::function<void (const Message&)>& callback);

	template <class Message>
	void broadcastAsync(const Message& message);

	template <class Message>
	void broadcastSync(const Message& message);

	void broadcastSync(Gaff::Hash64 msg_hash, const void* message);

	bool remove(ID id);

private:
	struct BroadcastData final
	{
		void (*deconstructor)(void*) = nullptr;
		Broadcaster* broadcaster = nullptr;
		Gaff::Hash64 msg_hash;
		size_t msg_size = 0;
	};

	using Listener = eastl::function<void (const void*)>;
	using ListenerData = VectorMap<Gaff::Hash64, Listener>;

	VectorMap<Gaff::Hash64, ListenerData> _listeners;
	EA::Thread::Mutex _listener_lock;

	JobPool* _job_pool = nullptr;

	static ProxyAllocator s_allocator;


	ID listenInternal(Gaff::Hash64 msg_hash, Listener&& callback);
	void broadcastAsyncInternal(BroadcastData& broadcast_data);

	static void BroadcastJobFunc(uintptr_t thread_id, void* data);
};

class BroadcastRemover final
{
public:
	BroadcastRemover(Broadcaster::ID id, Broadcaster& broadcaster);
	BroadcastRemover(const BroadcastRemover& remover) = delete;
	BroadcastRemover(BroadcastRemover&& remover);
	BroadcastRemover(void);
	~BroadcastRemover(void);

	// Takes ownership
	const BroadcastRemover& operator=(const BroadcastRemover& rhs) = delete;
	const BroadcastRemover& operator=(BroadcastRemover&& rhs);

	bool operator==(const BroadcastRemover& rhs) const;
	bool operator!=(const BroadcastRemover& rhs) const;

	bool remove(void);

private:
	Broadcaster::ID _id;
	Broadcaster* _broadcaster = nullptr;
};

#include "Shibboleth_Broadcaster.inl"

NS_END
