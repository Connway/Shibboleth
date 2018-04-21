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
//#include <Shibboleth_IObjectManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Shibboleth_Math.h>
#include <Gaff_SerializeInterfaces.h>
#include <Gaff_Utils.h>

NS_SHIBBOLETH

enum ObjectFlags
{
	OF_DIRTY = (1 << 0),
	OF_IN_WORLD = (1 << 1)
};

Object::Object(int32_t id):
	//_obj_mgr(GetApp().getManagerT<IObjectManager>()),
	_parent(nullptr), _id(id), _flags(0)
{
}

Object::~Object(void)
{
	destroy();
}

bool Object::load(const Gaff::ISerializeReader& reader)
{
	// Load the object name.
	{
		Gaff::ScopeGuard guard = reader.enterElementGuard("name");
		GAFF_REF(guard);

		if (!reader.isString()) {
			// TODO: log error
			return false;
		}

		char name[256] = {};
		_name = reader.readString(name, ARRAY_SIZE(name));
	}

	// Create and load all the components.
	{
		Gaff::ScopeGuard guard = reader.enterElementGuard("components");
		GAFF_REF(guard);

		if (!reader.isObject()) {
			// TODO: log error
			return false;
		}

		if (!createComponents(reader)) {
			return false;
		}

		for (Component* comp : _components) {
			comp->onAllComponentsLoaded();
		}

		// Start off object as dirty
		Gaff::SetBits<int32_t>(_flags, OF_DIRTY);
	}

	return true;
}

bool Object::save(Gaff::ISerializeWriter& writer)
{
	writer.startObject(4);

	writer.writeString("name", _name.getString().data());

	//Reflection<Gleam::Transform>::Save(writer, _local_transform);
	//Reflection<Gleam::AABB>::Save(writer, _local_aabb);

	writer.writeKey("components");
	writer.startArray(static_cast<uint32_t>(_components.size()));

	for (Component* comp : _components) {
		comp->save(writer);
	}

	writer.endArray();
	writer.endObject();

	return true;
}

void Object::destroy(void)
{
	for (Component* const component : _components) {
		SHIB_FREET(component, *GetAllocator());
	}

	removeChildren();
}

const HashString64& Object::getName(void) const
{
	return _name;
}

int32_t Object::getID(void) const
{
	return _id;
}

void Object::setID(int32_t id)
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

const void* Object::getComponent(Gaff::Hash64 class_id) const
{
	for (const Component* const component : _components) {
		const void* const interface = component->getReflectionDefinition().getInterface(class_id, component->getBasePointer());

		if (interface) {
			return interface;
		}
	}

	return nullptr;
}

void* Object::getComponent(Gaff::Hash64 class_id)
{
	const void* const interface = const_cast<const Object*>(this)->getComponent(class_id);
	return const_cast<void*>(interface);
}

Vector<const void*> Object::getComponents(Gaff::Hash64 class_id) const
{
	Vector<const void*> components;
	getComponents(class_id, components);
	return components;
}

Vector<void*> Object::getComponents(Gaff::Hash64 class_id)
{
	Vector<void*> components;
	getComponents(class_id, components);
	return components;
}

Vector<const void*>& Object::getComponents(Gaff::Hash64 class_id, Vector<const void*>& components) const
{
	for (const Component* const component : _components) {
		const void* const interface = component->getReflectionDefinition().getInterface(class_id, component->getBasePointer());

		if (interface) {
			components.push_back(interface);
		}
	}

	return components;
}

Vector<void*>& Object::getComponents(Gaff::Hash64 class_id, Vector<void*>& components)
{
	for (Component* const component : _components) {
		void* const interface = component->getReflectionDefinition().getInterface(class_id, component->getBasePointer());

		if (interface) {
			components.push_back(interface);
		}
	}

	return components;
}

const Vector<Component*>& Object::getComponents(void) const
{
	return _components;
}

Vector<Component*>& Object::getComponents(void)
{
	return _components;
}

void Object::addComponent(Component* component)
{
	component->setOwner(this);
	component->setIndex(_components.size());
	_components.emplace_back(component);

	if (isInWorld()) {
		component->onAddToWorld();
	}
}

void Object::removeComponent(Component* component, bool destroy)
{
	component->setOwner(nullptr);
	component->setIndex(static_cast<size_t>(-1));
	_components.erase_unsorted(Gaff::Find(_components, component));

	if (isInWorld()) {
		component->onRemoveFromWorld();
	}

	if (destroy) {
		SHIB_FREET(component, *GetAllocator());
	}
}

void Object::addChild(Object* object)
{
	//std::lock_guard<std::mutex> lock(_children_lock);
	_children.emplace_back(object);
	object->_parent = this;
}

void Object::removeFromParent(void)
{
	//std::lock_guard<std::mutex> lock(_parent->_children_lock);
	auto it = Gaff::Find(_parent->_children, this);

	if (it != _parent->_children.end()) {
		_parent->_children.erase_unsorted(it);
	}

	_parent = nullptr;
}

void Object::removeChildren(void)
{
	//std::lock_guard<std::mutex> lock(_children_lock);

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
	std::lock_guard<std::mutex> lock(_local_cb_lock);
	_local_callbacks.emplace_back(callback, user_data);
}

void Object::unregisterForLocalDirtyCallback(const DirtyCallback& callback)
{
	std::lock_guard<std::mutex> lock(_local_cb_lock);

	auto it = Gaff::Find(
		_local_callbacks,
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
	std::lock_guard<std::mutex> lock(_world_cb_lock);
	_world_callbacks.emplace_back(callback, user_data);
}

void Object::unregisterForWorldDirtyCallback(const DirtyCallback& callback)
{
	std::lock_guard<std::mutex> lock(_world_cb_lock);

	auto it = Gaff::Find(
		_world_callbacks,
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
	return Gaff::IsAnyBitSet<int32_t>(_flags, OF_DIRTY);
}

void Object::clearDirty(void)
{
	Gaff::ClearBits<int32_t>(_flags, OF_DIRTY);
}

bool Object::isInWorld(void) const
{
	return Gaff::IsAnyBitSet<int32_t>(_flags, OF_IN_WORLD);
}

void Object::addToWorld(void)
{
	for (auto it = _components.begin(); it != _components.end(); ++it) {
		(*it)->onAddToWorld();
	}

	Gaff::SetBits<int32_t>(_flags, OF_IN_WORLD);
}

void Object::removeFromWorld(void)
{
	for (auto it = _components.begin(); it != _components.end(); ++it) {
		(*it)->onRemoveFromWorld();
	}

	Gaff::ClearBits<int32_t>(_flags, OF_IN_WORLD);
}

bool Object::createComponents(const Gaff::ISerializeReader& reader)
{
	_components.reserve(reader.size());

	const bool error = reader.forEachInObject([&](const char* key) -> bool
	{
		if (!reader.isObject()) {
			// TODO: log error
			return true;
		}

		Gaff::Hash64 name;

		{
			Gaff::ScopeGuard guard = reader.enterElementGuard("Type");
			char type[128] = {};

			if (!reader.isString()) {
				// TODO: log error
				return true;
			}

			reader.readString(type, ARRAY_SIZE(type));

			name = Gaff::FNV1aHash64String(type);
		}

		const Gaff::IReflectionDefinition* const ref_def = Shibboleth::GetApp().getReflectionManager().getReflection(name);

		if (!ref_def) {
			// TODO: log error
			return false;
		}

		ProxyAllocator allocator("Components");
		Component* const component = ref_def->CREATEALLOCT(Component, allocator);

		if (!component) {
			// TODO: log error
			return true;
		}

		component->setOwner(this);
		component->setName(key);
		component->setIndex(_components.size());

		//if (!component->validate(value)) {
		//	// TODO: log error
		//	return false;
		//}

		component->load(reader);
		_components.emplace_back(component);

		return false;
	});

	return !error;
}

void Object::markDirty(void)
{
	if (!isDirty()) {
		Gaff::SetBits<int32_t>(_flags, OF_DIRTY);
		//_obj_mgr.addDirtyObject(this);
	}
}

NS_END
