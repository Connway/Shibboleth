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

#include "Shibboleth_Broadcaster.h"
#include "Shibboleth_JobPool.h"
#include "Shibboleth_IApp.h"
#include <Gaff_Assert.h>

NS_SHIBBOLETH

ProxyAllocator Broadcaster::s_allocator("Broadcaster");

void Broadcaster::init(void)
{
	_job_pool = &GetApp().getJobPool();
}

void Broadcaster::broadcastSync(Gaff::Hash64 msg_hash, const void* message)
{
	const EA::Thread::AutoMutex lock(_listener_lock);
	const auto it_listeners = _listeners.find(msg_hash);

	if (it_listeners == _listeners.end()) {
		return;
	}

	for (const auto& entry : it_listeners->second) {
		entry.second(message);
	}
}

bool Broadcaster::remove(ID id)
{
	const EA::Thread::AutoMutex lock(_listener_lock);

	const auto it = _listeners.find(id.msg_hash);

	if (it == _listeners.end()) {
		// $TODO: Log error.
		return false;
	}

	if (!_listeners.erase(id.cb_hash)) {
		// $TODO: Log error.
		return false;
	}

	return true;
}

Broadcaster::ID Broadcaster::listenInternal(Gaff::Hash64 msg_hash, Listener&& callback)
{
	const EA::Thread::AutoMutex lock(_listener_lock);

	const Broadcaster::ID id = { msg_hash, Gaff::FNV1aHash64T(callback) };
	auto& listener_data = _listeners[msg_hash];

	// Should we make this a vector? Not a common scenario to register the same function for a message multiple times,
	// but still a possible scenario.
	listener_data[id.cb_hash] = std::move(callback);

	return id;
}

void Broadcaster::broadcastAsyncInternal(BroadcastData& broadcast_data)
{
	Gaff::JobData data = { BroadcastJobFunc, &broadcast_data };
	_job_pool->addJobs(&data, 1);
}

void Broadcaster::BroadcastJobFunc(uintptr_t /*thread_id*/, void* data)
{
	const BroadcastData* const broadcast_data = static_cast<BroadcastData*>(data);
	void* const message = static_cast<uint8_t*>(data) + broadcast_data->msg_size;

	broadcast_data->broadcaster->broadcastSync(broadcast_data->msg_hash, message);

	if (broadcast_data->deconstructor) {
		broadcast_data->deconstructor(message);
	}

	SHIB_FREE(data, s_allocator);
}



BroadcastRemover::BroadcastRemover(BroadcastRemover&& remover):
	_id(remover._id), _broadcaster(remover._broadcaster)
{
	remover._broadcaster = nullptr;
}

BroadcastRemover::BroadcastRemover(Broadcaster::ID id, Broadcaster& broadcaster):
	_id(id), _broadcaster(&broadcaster)
{
}

BroadcastRemover::BroadcastRemover(void)
{
}

BroadcastRemover::~BroadcastRemover(void)
{
	remove();
}

const BroadcastRemover& BroadcastRemover::operator=(BroadcastRemover&& rhs)
{
	remove();

	_id = rhs._id;
	_broadcaster = rhs._broadcaster;

	rhs._broadcaster = nullptr;

	return *this;
}

bool BroadcastRemover::operator==(const BroadcastRemover& rhs) const
{
	return _broadcaster == rhs._broadcaster && _id.msg_hash == rhs._id.msg_hash && _id.cb_hash == rhs._id.cb_hash;
}

bool BroadcastRemover::operator!=(const BroadcastRemover& rhs) const
{
	return !(*this == rhs);
}

bool BroadcastRemover::remove(void)
{
	bool result = false;

	if (_broadcaster) {
		result = _broadcaster->remove(_id);
		_broadcaster = nullptr;
	}

	return result;
}

NS_END
