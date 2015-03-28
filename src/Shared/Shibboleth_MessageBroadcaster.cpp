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

#include "Shibboleth_MessageBroadcaster.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_IApp.h"
#include <Gaff_ScopedLock.h>

NS_SHIBBOLETH

MessageBroadcaster::MessageBroadcaster(void)
{
}

MessageBroadcaster::~MessageBroadcaster(void)
{
	for (auto it = _message_queue.begin(); it != _message_queue.end(); ++it) {
		GetAllocator()->freeT(it->second);
	}

	for (auto it1 = _listeners.begin(); it1 != _listeners.end(); ++it1) {
		for (auto it2 = it1->second.listeners.begin(); it2 != it1->second.listeners.end(); ++it2) {
			GetAllocator()->freeT(it2->second);
		}
	}
}

void MessageBroadcaster::remove(BroadcastID id)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_remove_lock);
	_remove_queue.emplacePush(id);
}

void MessageBroadcaster::update(bool wait)
{
	addListeners();
	removeListeners();
	spawnBroadcastTasks(wait);
}

void MessageBroadcaster::addListeners(void)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_add_lock);

	if (!_add_queue.empty()) {
		_listener_lock.readLock();

		for (auto it_add = _add_queue.begin(); it_add != _add_queue.end(); ++it_add) {
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

			assert(listeners);

			listeners->lock.writeLock();

			auto it = listeners->listeners.binarySearch(it_add->first.second, [](const Listener& lhs, size_t rhs) -> bool
			{
				return lhs.first < rhs;
			});

			listeners->listeners.emplaceInsert(it, it_add->first.second, it_add->second);
			listeners->lock.writeUnlock();
		}

		_listener_lock.readUnlock();
		_add_queue.clear();
	}
}

void MessageBroadcaster::removeListeners(void)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_remove_lock);

	if (!_remove_queue.empty()) {
		Gaff::ScopedReadLock<Gaff::ReadWriteSpinLock> read_lock(_listener_lock);

		for (auto it_rem = _remove_queue.begin(); it_rem != _remove_queue.end(); ++it_rem) {
			assert(_listeners.hasElementWithKey(it_rem->first));
			ListenerData& listeners = _listeners[it_rem->first];

			Gaff::ScopedWriteLock<Gaff::ReadWriteSpinLock> write_lock(listeners.lock);
			auto it = listeners.listeners.binarySearch(it_rem->second, [](const Listener& lhs, size_t rhs) -> bool
			{
				return lhs.first < rhs;
			});

			assert(it != listeners.listeners.end() && it->first == it_rem->second);
			listeners.listeners.erase(it);
		}
	}
}

void MessageBroadcaster::spawnBroadcastTasks(bool wait)
{
	Gaff::ScopedReadLock<Gaff::ReadWriteSpinLock> listener_lock(_listener_lock);
	Gaff::ScopedWriteLock<Gaff::ReadWriteSpinLock> msg_lock(_message_lock);

	Array<BroadcastTask*> tasks;

	if (wait) {
		tasks.reserve(_message_queue.size());
	}

	for (auto it_msg = _message_queue.begin(); it_msg != _message_queue.end(); ++it_msg) {
		// Only broadcast the message if there are people actually listening for it.
		if (_listeners.findElementWithKey(it_msg->first) != _listeners.end()) {
			BroadcastTask* task = GetAllocator()->allocT<BroadcastTask>(*it_msg, *this);

			if (wait) {
				task->addRef();
				tasks.push(task);
			}

			GetApp().addTask(task);
		}
	}

	_message_queue.clearNoFree();

	if (wait) {
		for (auto it = tasks.begin(); it != tasks.end(); ++it) {
			(*it)->spinWait();
			(*it)->release();
		}
	}
}


MessageBroadcaster::BroadcastTask::BroadcastTask(Gaff::Pair<unsigned int, IMessage*> message_data, MessageBroadcaster& broadcaster):
	_message_data(message_data), _broadcaster(broadcaster)
{
}

MessageBroadcaster::BroadcastTask::~BroadcastTask(void)
{
}

void MessageBroadcaster::BroadcastTask::doTask(void)
{
	Gaff::ScopedReadLock<Gaff::ReadWriteSpinLock> listener_lock(_broadcaster._listener_lock);

	MessageBroadcaster::ListenerData& listener_data = _broadcaster._listeners[_message_data.first];

	Gaff::ScopedReadLock<Gaff::ReadWriteSpinLock> ld_lock(listener_data.lock);

	for (auto it = listener_data.listeners.begin(); it != listener_data.listeners.end(); ++it) {
		it->second->call(*_message_data.second);
	}

	GetAllocator()->freeT(_message_data.second);
}


BroadcastRemover::BroadcastRemover(MessageBroadcaster::BroadcastID id, MessageBroadcaster& broadcaster):
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
