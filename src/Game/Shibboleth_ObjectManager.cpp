/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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
#include <Shibboleth_App.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_Atomic.h>

NS_SHIBBOLETH

ObjectManager::ObjectManager(App& app):
	_app(app)
{
}

ObjectManager::~ObjectManager(void)
{
}

void ObjectManager::requestUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.push(UpdateEntry("Object Manager: Pre-Physics Update", Gaff::Bind(this, &ObjectManager::prePhysicsUpdate)));
	entries.push(UpdateEntry("Object Manager: Post-Physics Update", Gaff::Bind(this, &ObjectManager::prePhysicsUpdate)));
	//entries.push(UpdateEntry("Object Manager: Update", Gaff::Bind(this, &ObjectManager::update)));
}

void ObjectManager::prePhysicsUpdate(double dt)
{
	// Add new objects
	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_add_lock);

		for (auto it = _add_queue.begin(); it != _add_queue.end(); ++it) {
			auto location =_objects.binarySearch((*it)->getID(), [](const Object* lhs, unsigned int rhs)
			{
				return lhs->getID() < rhs;
			});

			// If we generated a duplicate ID, then something is borked
			assert(location == _objects.end() || (*location)->getID() != (*it)->getID());

			_objects.insert(*it, location);
		}

		_add_queue.clearNoFree();
	}

	// Remove objects
	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_remove_lock);

		for (auto it = _remove_queue.begin(); it != _remove_queue.end(); ++it) {
		}

		_remove_queue.clearNoFree();
	}

	// generate update jobs
	for (auto it = _objects.begin(); it != _objects.end(); ++it) {
	}
}

void ObjectManager::postPhysicsUpdate(double dt)
{
}

//void ObjectManager::update(double dt)
//{
//}

NS_END
