/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_BroadcasterManager.h"
#include <Shibboleth_MessageBroadcaster.h>

NS_SHIBBOLETH

REF_IMPL_REQ(BroadcasterManager);
SHIB_REF_IMPL(BroadcasterManager)
.addBaseClassInterfaceOnly<BroadcasterManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IBroadcasterManager)
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

BroadcasterManager::BroadcasterManager(void)
{
}

BroadcasterManager::~BroadcasterManager(void)
{
	clear();
}

const char* BroadcasterManager::getName(void) const
{
	return GetFriendlyName();
}

void BroadcasterManager::getUpdateEntries(Array<IUpdateQuery::UpdateEntry>& entries)
{
	entries.emplacePush(U8String("Broadcaster Manager: Update"), Gaff::Bind(this, &BroadcasterManager::update));
}

MessageBroadcaster* BroadcasterManager::getBroadcaster(unsigned int object_id, bool create_if_doesnt_exist)
{
	_broadcaster_lock.readLock();
	auto it = _object_broadcasters.findElementWithKey(object_id);

	// If the broadcaster doesn't exist, then make it
	if (it == _object_broadcasters.end()) {
		if (!create_if_doesnt_exist) {
			_broadcaster_lock.readUnlock();
			return nullptr;
		}

		_broadcaster_lock.readUnlock();
		MessageBroadcaster* broadcaster = SHIB_ALLOCT(MessageBroadcaster, *GetAllocator());

		_broadcaster_lock.writeLock();
		_object_broadcasters.insert(object_id, broadcaster);
		_broadcaster_lock.writeUnlock();

		return broadcaster;
	}

	_broadcaster_lock.readUnlock();
	return it->second;
}

void BroadcasterManager::clear(void)
{
	_broadcaster_lock.writeLock();

	for (auto it = _object_broadcasters.begin(); it != _object_broadcasters.end(); ++it) {
		SHIB_FREET(it->second, *GetAllocator());
	}

	_broadcaster_lock.writeUnlock();
}

void BroadcasterManager::update(double, void*)
{
	_broadcaster_lock.readLock();

	for (auto it = _object_broadcasters.begin(); it != _object_broadcasters.end(); ++it) {
		it->second->update();
	}

	_broadcaster_lock.readUnlock();
}

NS_END