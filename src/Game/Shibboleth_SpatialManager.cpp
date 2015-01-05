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

#include "Shibboleth_SpatialManager.h"
#include "Shibboleth_Object.h"
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

REF_IMPL_REQ(SpatialManager);
REF_IMPL_ASSIGN_SHIB(SpatialManager)
.addBaseClassInterfaceOnly<SpatialManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

SpatialManager::WatchUpdater::WatchUpdater(SpatialManager* spatial_mgr, Node* node):
	_spatial_mgr(spatial_mgr), _node(node)
{
}

SpatialManager::WatchUpdater::~WatchUpdater(void)
{
}

void SpatialManager::WatchUpdater::operator()(const Gleam::Quaternion&) const
{
	addDirtyNode();
}

void SpatialManager::WatchUpdater::operator()(const Gleam::AABB&) const
{
	addDirtyNode();
}

void SpatialManager::WatchUpdater::operator()(const Gleam::Vec4&) const
{
	addDirtyNode();
}

void SpatialManager::WatchUpdater::addDirtyNode(void) const
{
	if (!_node->dirty) {
		_node->_lock.lock();

		// Check again in-case another thread changed the value before we acquired the lock
		if (!_node->dirty) {
			_node->dirty = true;
			_node->_lock.unlock(); // We've set the flag, unlock so that other threads don't have to wait for the rest of the function to finish.

			Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_spatial_mgr->_dirty_lock);
			_spatial_mgr->_dirty_nodes.push(_node);
		}
	}
}




SpatialManager::SpatialManager(IApp& app):
	_dirty_nodes(100, (Node*)nullptr), _bvh(nullptr), _app(app), _next_id(0)
{
}

SpatialManager::~SpatialManager(void)
{
}

void SpatialManager::requestUpdateEntries(Array<UpdateEntry>& entries)
{
	//entries.movePush(UpdateEntry(AString("Spatial Manager: Update"), Gaff::Bind(this, &SpatialManager::update)));
}

const char* SpatialManager::getName(void) const
{
	return "Spatial Manager";
}

unsigned int SpatialManager::addObject(Object* object)
{
	Node* node = _app.getAllocator().template allocT<Node>();

	if (!node) {
		return 0;
	}

	// register for position and aabb changes
	WatchUpdater updater(this, node);

	// Not watching scale because if the scale changes, so will the AABB.
	node->receipts[0] = object->watchAABB(Gaff::Bind<WatchUpdater, void, const Gleam::AABB&>(updater));
	node->receipts[1] = object->watchRotation(Gaff::Bind<WatchUpdater, void, const Gleam::Quaternion&>(updater));
	node->receipts[2] = object->watchPosition(Gaff::Bind<WatchUpdater, void, const Gleam::Vec4&>(updater));
	node->object = object;
	node->dirty = false;

	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_bvh_lock);
	node->id = _next_id;
	_node_map[node->id] = node;
	insertNode(node);
	++_next_id;

	return node->id;
}

void SpatialManager::removeObject(unsigned int id)
{
	Gaff::ScopedLock<Gaff::SpinLock> bvh_lock(_bvh_lock);

	// Split into two because VS2013 is retarded and thinks it's an uninitialized variable ...
	Node* node = nullptr;
	node = _node_map[node->id];

	removeNode(node);
	_node_map.erase(id);

	Gaff::ScopedLock<Gaff::SpinLock> dirty_lock(_dirty_lock);
	auto it = _dirty_nodes.linearSearch(node);

	if (it != _dirty_nodes.end()) {
		_dirty_nodes.erase(it);
	}
}

void SpatialManager::update(double)
{
	_dirty_nodes.clearNoFree();
}

void SpatialManager::removeNode(Node* node)
{
	// implement me!
}

void SpatialManager::insertNode(Node* node)
{
	// implement me!
}

NS_END
