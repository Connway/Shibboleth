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

#include "Shibboleth_ObjectManager.h"
#include "Shibboleth_Object.h"
#include <Gaff_ScopedLock.h>
#include <Gaff_Atomic.h>

NS_SHIBBOLETH

REF_IMPL_REQ(ObjectManager);
SHIB_REF_IMPL(ObjectManager)
.addBaseClassInterfaceOnly<ObjectManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

const char* ObjectManager::GetFriendlyName(void)
{
	return "Object Manager";
}

ObjectManager::ObjectManager(void):
	_next_id(0)
{
}

ObjectManager::~ObjectManager(void)
{
	for (auto it = _objects.begin(); it != _objects.end(); ++it) {
		SHIB_FREET(*it, *GetAllocator());
	}

	_objects.clear();
}

const char* ObjectManager::getName(void) const
{
	return GetFriendlyName();
}

void ObjectManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.push(UpdateEntry(AString("Object Manager: Update Dirty Objects"), Gaff::Bind(this, &ObjectManager::updateDirtyObjects)));
}

Object* ObjectManager::createObject(void)
{
	unsigned int id = AtomicUAddFetchOrig(&_next_id, 1);
	Object* object = SHIB_ALLOCT(Object, *GetAllocator(), id);

	if (object) {
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_objects_lock);

		auto it = _objects.binarySearch(id, [](const Object* lhs, unsigned int rhs)
		{
			return lhs->getID() < rhs;
		});

		// If we generated a duplicate ID, then something is borked
		GAFF_ASSERT(it == _objects.end() || (*it)->getID() != id);

		_objects.insert(it, object);
	}

	return object;
}

void ObjectManager::removeObject(Object* object)
{
	GAFF_ASSERT(object);
	removeObject(object->getID());
}

void ObjectManager::removeObject(unsigned int id)
{
	GAFF_ASSERT(id < _next_id);
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
		SHIB_FREET(object, *GetAllocator());
	}
}

const Object* ObjectManager::getObject(unsigned int id) const
{
	auto it = _objects.binarySearch(id, [](const Object* lhs, unsigned int rhs)
	{
		return lhs->getID() < rhs;
	});

	return (it != _objects.end() && (*it)->getID() == id) ? *it : nullptr;
}

Object* ObjectManager::getObject(unsigned int id)
{
	auto it = _objects.binarySearch(id, [](const Object* lhs, unsigned int rhs)
	{
		return lhs->getID() < rhs;
	});

	return (it != _objects.end() && (*it)->getID() == id) ? *it : nullptr;
}

bool ObjectManager::doesObjectExist(const Object* object) const
{
	auto it = _objects.linearSearch(object);
	return it != _objects.end() && *it == object;
}

bool ObjectManager::doesObjectExist(unsigned int id) const
{
	return getObject(id) != nullptr;
}

void ObjectManager::addDirtyObject(Object* object)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_dirty_objects_lock);
	_dirty_objects.push(object);
}

void ObjectManager::updateDirtyObjects(double, void*)
{
	// generate update jobs
	// wait for update jobs to finish

	for (auto it = _dirty_objects.begin(); it != _dirty_objects.end(); ++it) {
		if ((*it)->isDirty()) {
			(*it)->updateTransforms();
			(*it)->notifyLocalDirtyCallbacks();
			(*it)->notifyWorldDirtyCallbacks();

		} else {
			(*it)->notifyLocalDirtyCallbacks();
		}
	}

	_dirty_objects.clearNoFree();
}

NS_END
