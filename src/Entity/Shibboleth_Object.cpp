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

#include "Shibboleth_Object.h"
//#include <Shibboleth_IComponentManager.h>
//#include <Shibboleth_IObjectManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>

#define OBJ_DIRTY (1 << 0)
#define OBJ_IN_WORLD (1 << 1)

NS_SHIBBOLETH

Object::Object(uint32_t id):
	//_comp_mgr(GetApp().getManagerT<IComponentManager>()),
	//_obj_mgr(GetApp().getManagerT<IObjectManager>()),
	_parent(nullptr), _id(id), _flags(0)
{
}

Object::~Object(void)
{
	destroy();
}

bool Object::init(const Gaff::JSON& json)
{
	Gaff::JSON name = json["name"];

	if (!name.isString()) {
		// log error
		return false;
	}

	_name = name.getString();

	Gaff::JSON components = json["components"];

	if (!components.isObject()) {
		// log error
		return false;
	}

	if (!createComponents(components)) {
		return false;
	}

	int32_t size = static_cast<int32_t>(_components.size());

	for (int32_t i = 0; i < size; ++i) {
		_components[i]->allComponentsLoaded();
	}

	// Start off object as dirty
	Gaff::SetBits<uint8_t>(_flags, OBJ_DIRTY);
	//_obj_mgr.addNewObject(this);

	return true;
}

bool Object::init(IFileSystem* fs, const char* file_name)
{
	GAFF_ASSERT(file_name && strlen(file_name));
	GAFF_ASSERT(!_name.size());

	IFile* file = fs->openFile(file_name);

	if (!file) {
		// log error
		return false;
	}

	Gaff::JSON object_json;

	if (!object_json.parse(file->getBuffer())) {
		// log error
		fs->closeFile(file);
		return false;
	}

	fs->closeFile(file);

	return init(object_json);
}

void Object::destroy(void)
{
	for (auto it = _components.begin(); it != _components.end(); ++it) {
		//_comp_mgr.destroyComponent(*it);
	}

	removeChildren();
}

const HashString64& Object::getName(void) const
{
	return _name;
}

uint32_t Object::getID(void) const
{
	return _id;
}

void Object::setID(uint32_t id)
{
	_id = id;
}

const Gleam::Transform& Object::getLocalTransform(void) const
{
	return _local_transform;
}

const Gleam::Transform& Object::getWorldTransform(void) const
{
	return _world_transform;
}

void Object::setLocalTransform(const Gleam::Transform& transform)
{
	_local_transform = transform;
	markDirty();
}

void Object::setWorldTransform(const Gleam::Transform& transform)
{
	if (_parent) {
		Gleam::Transform parent_world_transform = _parent->getWorldTransform().inverse();
		_local_transform = transform + parent_world_transform;

	} else {
		_local_transform = transform;
	}

	_world_transform = transform;
	markDirty();
}

const glm::quat& Object::getLocalRotation(void) const
{
	return _local_transform.getRotation();
}

const glm::quat& Object::getWorldRotation(void) const
{
	return _world_transform.getRotation();
}

void Object::setLocalRotation(const glm::quat& rot)
{
	_local_transform.setRotation(rot);
	markDirty();
}

void Object::setWorldRotation(const glm::quat& rot)
{
	if (_parent) {
		glm::quat inv_rot = glm::inverse(_parent->getWorldRotation());
		_local_transform.setRotation(rot * inv_rot);

	} else {
		_local_transform.setRotation(rot);
	}

	_world_transform.setRotation(rot);
	markDirty();
}

const glm::vec3& Object::getLocalPosition(void) const
{
	return _local_transform.getTranslation();
}

const glm::vec3& Object::getWorldPosition(void) const
{
	return _world_transform.getTranslation();
}

void Object::setLocalPosition(const glm::vec3& pos)
{
	_local_transform.setTranslation(pos);
	markDirty();
}

void Object::setWorldPosition(const glm::vec3& pos)
{
	if (_parent) {
		_local_transform.setTranslation(pos - _parent->getWorldPosition());
	} else {
		_local_transform.setTranslation(pos);
	}

	_world_transform.setTranslation(pos);
	markDirty();
}

const glm::vec3& Object::getLocalScale(void) const
{
	return _local_transform.getScale();
}

const glm::vec3& Object::getWorldScale(void) const
{
	return _local_transform.getScale();
}

void Object::setLocalScale(const glm::vec3& scale)
{
	_local_transform.setScale(scale);
	markDirty();
}

void Object::setWorldScale(const glm::vec3& scale)
{
	if (_parent) {
		glm::vec3 inv_scale = glm::vec3(1.0f) / _parent->getWorldScale();
		_local_transform.setScale(scale * inv_scale);

	} else {
		_local_transform.setScale(scale);
	}

	_world_transform.setScale(scale);
	markDirty();
}

const Gleam::AABB& Object::getLocalAABB(void) const
{
	return _local_aabb;
}

const Gleam::AABB& Object::getWorldAABB(void) const
{
	return _world_aabb;
}

void Object::setLocalAABB(const Gleam::AABB& aabb)
{
	_local_aabb = aabb;
	markDirty();
}

int32_t Object::getNumComponents(void) const
{
	return static_cast<int32_t>(_components.size());
}

const Component* Object::getComponent(int32_t index) const
{
	GAFF_ASSERT(index < _components.size());
	return _components[index];
}

Component* Object::getComponent(int32_t index)
{
	GAFF_ASSERT(index < _components.size());
	return _components[index];
}

const Vector<Component*>& Object::getComponents(void) const
{
	return _components;
}

Vector<Component*>& Object::getComponents(void)
{
	return _components;
}

const void* Object::getFirstComponentWithInterface(Gaff::Hash64 /*class_id*/) const
{
	//for (auto it = _components.begin(); it != _components.end(); ++it) {
	//	const void* component = (*it)->rawRequestInterface(class_id);

	//	if (component) {
	//		return component;
	//	}
	//}

	return nullptr;
}


void* Object::getFirstComponentWithInterface(Gaff::Hash64 /*class_id*/)
{
	//for (auto it = _components.begin(); it != _components.end(); ++it) {
	//	void* component = (*it)->rawRequestInterface(class_id);

	//	if (component) {
	//		return component;
	//	}
	//}

	return nullptr;
}

void Object::addChild(Object* object)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_children_lock);
	_children.emplace_back(object);
	object->_parent = this;
}

void Object::removeFromParent(void)
{
	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_parent->_children_lock);
		auto it = eastl::find(_parent->_children.begin(), _parent->_children.end(), this);

		if (it != _parent->_children.end()) {
			_parent->_children.erase_unsorted(it);
		}
	}

	_parent = nullptr;
}

void Object::removeChildren(void)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_children_lock);

	 for (auto it = _children.begin(); it != _children.end(); ++it) {
		 (*it)->_parent = nullptr;
	 }

	 _children.clear();
}

void Object::updateTransforms(void)
{
	if (_parent) {
		_world_transform = _local_transform;
		_world_transform += _parent->_world_transform;
		_world_aabb = _local_aabb;

	} else {
		_world_transform = _local_transform;
		_world_aabb = _local_aabb;
	}

	_world_aabb.transform(_world_transform);

	for (auto it = _children.begin(); it != _children.end(); ++it) {
		(*it)->updateTransforms();
	}
}

void Object::registerForLocalDirtyCallback(const DirtyCallback& callback, uint64_t user_data)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_local_cb_lock);
	_local_callbacks.emplace_back(callback, user_data);
}

void Object::unregisterForLocalDirtyCallback(const DirtyCallback& callback)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_local_cb_lock);

	auto it = eastl::find(
		_local_callbacks.begin(),
		_local_callbacks.end(),
		callback,
		[](const eastl::pair<DirtyCallback, uint64_t>& left, const DirtyCallback& right) -> bool
		{
			return left.first == right;
		}
	);

	if (it != _local_callbacks.end()) {
		_local_callbacks.erase_unsorted(it);
	}
}

// This should occur in a non-thread contention situation.
void Object::notifyLocalDirtyCallbacks(void)
{
	for (auto it = _local_callbacks.begin(); it != _local_callbacks.end(); ++it) {
		it->first(this, it->second);
	}

	// If we are marked as not-dirty, then we are a child of another object that has been updated.
	if (isDirty()) {
		for (auto it = _children.begin(); it != _children.end(); ++it) {
			(*it)->notifyWorldDirtyCallbacks();
		}
	}

	clearDirty();
}

void Object::registerForWorldDirtyCallback(const DirtyCallback& callback, uint64_t user_data)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_world_cb_lock);
	_world_callbacks.emplace_back(callback, user_data);
}

void Object::unregisterForWorldDirtyCallback(const DirtyCallback& callback)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_world_cb_lock);

	auto it = eastl::find(
		_world_callbacks.begin(),
		_world_callbacks.end(),
		callback,
		[](const eastl::pair<DirtyCallback, uint64_t>& left, const DirtyCallback& right) -> bool
		{
			return left.first == right;
		}
	);

	if (it != _world_callbacks.end()) {
		_world_callbacks.erase_unsorted(it);
	}
}

// This should occur in a non-thread contention situation.
void Object::notifyWorldDirtyCallbacks(void)
{
	for (auto it = _world_callbacks.begin(); it != _world_callbacks.end(); ++it) {
		it->first(this, it->second);
	}

	clearDirty();
}

bool Object::isDirty(void) const
{
	return Gaff::IsAnyBitSet<char>(_flags, OBJ_DIRTY);
}

void Object::clearDirty(void)
{
	Gaff::ClearBits<uint8_t>(_flags, OBJ_DIRTY);
}

bool Object::isInWorld(void) const
{
	return Gaff::IsAnyBitSet<char>(_flags, OBJ_IN_WORLD);
}

void Object::addToWorld(void)
{
	for (auto it = _components.begin(); it != _components.end(); ++it) {
		(*it)->addToWorld();
	}

	Gaff::SetBits<uint8_t>(_flags, OBJ_IN_WORLD);
}

void Object::removeFromWorld(void)
{
	for (auto it = _components.begin(); it != _components.end(); ++it) {
		(*it)->removeFromWorld();
	}

	Gaff::ClearBits<uint8_t>(_flags, OBJ_IN_WORLD);
}

bool Object::createComponents(const Gaff::JSON& json)
{
	bool error = false;

	_components.reserve(json.size());

	json.forEachInObject([&](const char* /*key*/, const Gaff::JSON& value) -> bool
	{
		if (!value.isObject()) {
			// log error
			error = true;
			return true;
		}

		const Gaff::JSON type = value["Type"];

		if (!type.isString()) {
			// log error
			error = true;
			return true;
		}

		//Component* component = _comp_mgr.createComponent(type.getString());

		//if (!component) {
		//	// log error
		//	error = true;
		//	return true;
		//}

		//component->setOwner(this);
		//component->setName(key);

		//if (!component->validate(value) || !component->load(value)) {
		//	error = true;
		//	return true;
		//}

		//_components.emplace_back(component);

		return false;
	});

	return !error;
}

void Object::markDirty(void)
{
	if (!isDirty()) {
		Gaff::SetBits<uint8_t>(_flags, OBJ_DIRTY);
		//_obj_mgr.addDirtyObject(this);
	}
}

NS_END