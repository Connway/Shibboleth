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

#include "Shibboleth_MessageBroadcaster.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_JobPool.h"
#include "Shibboleth_IApp.h"
#include <Gaff_ScopedLock.h>

NS_SHIBBOLETH

void BroadcastJob(void* data)
{
	MessageBroadcaster* broadcaster = reinterpret_cast<MessageBroadcaster*>(data);

	size_t index = AtomicIncrement(&broadcaster->_next_id) - 1;
	Gaff::Pair<unsigned int, IMessage*>& msg_data = broadcaster->_message_queues[broadcaster->_curr_queue][index];

	Gaff::ScopedReadLock<Gaff::ReadWriteSpinLock> listener_lock(broadcaster->_listener_lock);

	MessageBroadcaster::ListenerData& listener_data = broadcaster->_listeners[msg_data.first];
	Gaff::ScopedReadLock<Gaff::ReadWriteSpinLock> ld_lock(listener_data.lock);

	for (auto it = listener_data.listeners.begin(); it != listener_data.listeners.end(); ++it) {
		it->second->call(*msg_data.second);
	}

	SHIB_FREET(msg_data.second, *GetAllocator());
}

MessageBroadcaster::MessageBroadcaster(void)
{
}

MessageBroadcaster::~MessageBroadcaster(void)
{
	for (unsigned int i = 0; i < 2; ++i) {
		for (auto it = _message_queues[i].begin(); it != _message_queues[i].end(); ++it) {
			SHIB_FREET(it->second, *GetAllocator());
		}
	}

	for (auto it1 = _listeners.begin(); it1 != _listeners.end(); ++it1) {
		for (auto it2 = it1->second.listeners.begin(); it2 != it1->second.listeners.end(); ++it2) {
			SHIB_FREET(it2->second, *GetAllocator());
		}
	}

	if (_counter) {
		GetApp().getJobPool().freeCounter(_counter);
	}
}

void MessageBroadcaster::remove(BroadcastID id)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_listener_remove_lock);
	_listener_remove_queue.emplacePush(id);
}

void MessageBroadcaster::update(bool wait)
{
	waitForCounter();
	addListeners();
	removeListeners();
	swapMessageQueues();
	spawnBroadcastTasks(wait);
}

void MessageBroadcaster::addListeners(void)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_listener_add_lock);

	if (!_listener_add_queue.empty()) {
		_listener_lock.readLock();

		for (auto it_add = _listener_add_queue.begin(); it_add != _listener_add_queue.end(); ++it_add) {
			auto it_listeners = _listeners.findElementWithKey(it_add->first.first);
			ListenerData* listeners = nullptr;

			if (it_listeners == _listeners.end()) {
				_listener_lock.readUnlock();

				_listener_lock.writeLock();
				listeners = &_listeners[it_add->first.first];
				_listener_lock.writeUnlock();

				_listener_lock.readLock();

			} else {
				listeners = &it_listeners->second;
			}

			GAFF_ASSERT(listeners);

			listeners->lock.writeLock();

			auto it = listeners->listeners.binarySearch(it_add->first.second, [](const Listener& lhs, size_t rhs) -> bool
			{
				return lhs.first < rhs;
			});

			listeners->listeners.emplace(it, it_add->first.second, it_add->second);
			listeners->lock.writeUnlock();
		}

		_listener_lock.readUnlock();
		_listener_add_queue.clear();
	}
}

void MessageBroadcaster::removeListeners(void)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_listener_remove_lock);

	if (!_listener_remove_queue.empty()) {
		Gaff::ScopedReadLock<Gaff::ReadWriteSpinLock> read_lock(_listener_lock);

		for (auto it_rem = _listener_remove_queue.begin(); it_rem != _listener_remove_queue.end(); ++it_rem) {
			GAFF_ASSERT(_listeners.hasElementWithKey(it_rem->first));
			ListenerData& listeners = _listeners[it_rem->first];

			Gaff::ScopedWriteLock<Gaff::ReadWriteSpinLock> write_lock(listeners.lock);
			auto it = listeners.listeners.binarySearch(it_rem->second, [](const Listener& lhs, size_t rhs) -> bool
			{
				return lhs.first < rhs;
			});

			GAFF_ASSERT(it != listeners.listeners.end() && it->first == it_rem->second);
			listeners.listeners.erase(it);
		}
	}
}

void MessageBroadcaster::swapMessageQueues(void)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_message_add_lock);
	_message_queues[_curr_queue].clearNoFree();
	_curr_queue = 1 - _curr_queue;
}

void MessageBroadcaster::spawnBroadcastTasks(bool wait)
{
	if (_message_queues[_curr_queue].empty()) {
		return;
	}

	// Might want to cache this array so we're not allocating all the time.
	Array<Gaff::JobData> jobs_data(_message_queues[_curr_queue].size());

	_listener_lock.readLock();

	for (auto it_msg = _message_queues[_curr_queue].begin(); it_msg != _message_queues[_curr_queue].end(); ++it_msg) {

		// Only broadcast the message if there are people actually listening for it.
		if (_listeners.findElementWithKey(it_msg->first) != _listeners.end()) {
			jobs_data.emplacePush(&BroadcastJob, this);
		}
	}

	_listener_lock.readUnlock();

	GetApp().getJobPool().addJobs(jobs_data.getArray(), jobs_data.size(), &_counter);

	if (wait) {
		waitForCounter();
	}
}

void MessageBroadcaster::waitForCounter(void)
{
	if (_counter) {
		auto& jp = GetApp().getJobPool();
		jp.helpWhileWaiting(_counter);
	}

	_next_id = 0;
}


BroadcastRemover::BroadcastRemover(BroadcastID id, MessageBroadcaster& broadcaster):
	_id(id), _broadcaster(&broadcaster), _valid(true)
{
}

BroadcastRemover::BroadcastRemover(void):
	_broadcaster(nullptr), _valid(false)
{
}

BroadcastRemover::~BroadcastRemover(void)
{
	remove();
}

const BroadcastRemover& BroadcastRemover::operator=(const BroadcastRemover& rhs)
{
	remove();

	_id = rhs._id;
	_broadcaster = rhs._broadcaster;
	_valid = true;

	const_cast<BroadcastRemover&>(rhs)._valid = false;
	
	return *this;
}

bool BroadcastRemover::operator==(const BroadcastRemover& rhs) const
{
	return (!_valid == !rhs._valid) || (_broadcaster == rhs._broadcaster && _id.first == rhs._id.first && _id.second == rhs._id.second);
}

bool BroadcastRemover::operator!=(const BroadcastRemover& rhs) const
{
	return !(*this == rhs);
}

void BroadcastRemover::remove(void)
{
	if (_valid) {
		_broadcaster->remove(_id);
		_valid = false;
	}
}

NS_END
