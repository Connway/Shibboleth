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

#include "Shibboleth_ObjectManager.h"
#include "Shibboleth_Object.h"
#include <Shibboleth_IApp.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_Atomic.h>

NS_SHIBBOLETH

REF_IMPL_REQ(ObjectManager);
SHIB_REF_IMPL(ObjectManager)
.addBaseClassInterfaceOnly<ObjectManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

ObjectManager::ObjectManager(void):
	_next_id(0)
{
}

ObjectManager::~ObjectManager(void)
{
}

Object* ObjectManager::createObject(void)
{
	unsigned int id = AtomicUAddFetchOrig(&_next_id, 1);
	Object* object = GetAllocator()->template allocT<Object>(id);

	if (object) {
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_objects_lock);

		auto it = _objects.binarySearch(id, [](const Object* lhs, unsigned int rhs)
		{
			return lhs->getID() < rhs;
		});

		// If we generated a duplicate ID, then something is borked
		assert(it == _objects.end() || (*it)->getID() != id);

		_objects.insert(object, it);
	}

	return object;
}

void ObjectManager::removeObject(Object* object)
{
	assert(object);
	removeObject(object->getID());
}

void ObjectManager::removeObject(unsigned int id)
{
	assert(id < _next_id);
	Object* object = nullptr;

	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_objects_lock);

		auto it = _objects.binarySearch(id, [](const Object* lhs, unsigned int rhs)
		{
			return lhs->getID() < rhs;
		});

		if (it != _objects.end() && (*it)->getID() == id) {
			object = *it;
			_objects.erase(it);
		}
	}

	if (object) {
		GetAllocator()->freeT(object);
	}
}

bool ObjectManager::doesObjectExist(const Object* object) const
{
	auto it = _objects.linearSearch(object);
	return it != _objects.end() && *it == object;
}

bool ObjectManager::doesObjectExist(unsigned int id) const
{
	auto it = _objects.binarySearch(id, [](const Object* lhs, unsigned int rhs)
	{
		return lhs->getID() < rhs;
	});

	return it != _objects.end() && (*it)->getID() == id;
}

void ObjectManager::addDirtyObject(Object* object)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_dirty_objects_lock);
	_dirty_objects.push(object);
}

const char* ObjectManager::getName(void) const
{
	return "Object Manager";
}

void ObjectManager::requestUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.movePush(UpdateEntry(AString("Object Manager: Pre-Physics Update"), Gaff::Bind(this, &ObjectManager::prePhysicsUpdate)));
	entries.movePush(UpdateEntry(AString("Object Manager: Post-Physics Update"), Gaff::Bind(this, &ObjectManager::prePhysicsUpdate)));
}

void ObjectManager::prePhysicsUpdate(double dt)
{
	// generate update jobs
	//for (auto it = _objects.begin(); it != _objects.end(); ++it) {
	//}
}

void ObjectManager::postPhysicsUpdate(double dt)
{
	// generate update jobs
	// wait for update jobs to finish

	for (auto it = _dirty_objects.begin(); it != _dirty_objects.end(); ++it) {
		(*it)->notifyWorldDirtyCallbacks();
		(*it)->notifyLocalDirtyCallbacks();
	}

	_dirty_objects.clearNoFree();
}

NS_END
