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

#include "Shibboleth_Broadcaster.h"
#include "Shibboleth_JobPool.h"
#include "Shibboleth_IApp.h"
#include <Gaff_Assert.h>

NS_SHIBBOLETH

void Broadcaster::init(void)
{
	_job_pool = &GetApp().getJobPool();
}

void Broadcaster::remove(BroadcastID id)
{
	std::lock_guard lock(_listener_lock);

	const auto it = _listeners.find(id.first);
	GAFF_ASSERT(it != _listeners.end() && it->second.listeners.size() > id.second);

	it->second.unused_ids.emplace_back(id.second);
	it->second.listeners[id.second] = nullptr;
}

BroadcastRemover::BroadcastRemover(BroadcastID id, Broadcaster& broadcaster):
	_id(id), _broadcaster(&broadcaster), _valid(true)
{
}

BroadcastRemover::BroadcastRemover(void)
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
