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

void Broadcaster::init(void)
{
	_job_pool = &GetApp().getJobPool();
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
