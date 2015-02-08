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

#include "Shibboleth_Object.h"
#include <Shibboleth_ComponentManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

Object::Object(unsigned int id):
	_comp_mgr(GetApp().getManagerT<ComponentManager>("Component Manager")),
	_parent(nullptr), _id(id)
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

	for (unsigned int i = 0; i < _components.size(); ++i) {
		_components[i]->allComponentsLoaded();
	}

	return true;
}

bool Object::init(const char* file_name)
{
	assert(file_name && strlen(file_name));

	Gaff::JSON object_json;

	if (!object_json.parseFile(file_name)) {
		return false;
	}

	return init(object_json);
}

void Object::destroy(void)
{
	for (auto it = _components.begin(); it != _components.end(); ++it) {
		_comp_mgr.destroyComponent(*it);
	}

	removeChildren();
}

const AString& Object::getName(void) const
{
	return _name;
}

unsigned int Object::getID(void) const
{
	return _id;
}

void Object::setID(unsigned int id)
{
	_id = id;
}

void Object::registerForPrePhysicsUpdate(const UpdateCallback& callback)
{
}

void Object::registerForPostPhysicsUpdate(const UpdateCallback& callback)
{
}

void Object::prePhysicsUpdate(double dt)
{
}

void Object::postPhysicsUpdate(double dt)
{
}

MessageBroadcaster& Object::getBroadcaster(void)
{
	return _broadcaster;
}

const Gleam::TransformCPU& Object::getLocalTransform(void) const
{
	return _local_transform;
}

const Gleam::TransformCPU& Object::getWorldTransform(void) const
{
	return _world_transform;
}

void Object::setLocalTransform(const Gleam::TransformCPU& transform)
{
	_local_transform = transform;
}

const Gleam::QuaternionCPU& Object::getLocalRotation(void) const
{
	return _local_transform.getRotation();
}

const Gleam::QuaternionCPU& Object::getWorldRotation(void) const
{
	return _world_transform.getRotation();
}

void Object::setLocalRotation(const Gleam::QuaternionCPU& rot)
{
	_local_transform.setRotation(rot);
}

const Gleam::Vector4CPU& Object::getLocalPosition(void) const
{
	return _local_transform.getTranslation();
}

const Gleam::Vector4CPU& Object::getWorldPosition(void) const
{
	return _world_transform.getTranslation();
}

void Object::setLocalPosition(const Gleam::Vector4CPU& pos)
{
	_local_transform.setTranslation(pos);
}

const Gleam::Vector4CPU& Object::getLocalScale(void) const
{
	return _local_transform.getScale();
}

const Gleam::Vector4CPU& Object::getWorldScale(void) const
{
	return _local_transform.getScale();
}

void Object::setLocalScale(const Gleam::Vector4CPU& scale)
{
	_local_transform.setScale(scale);
}

const Gleam::AABBCPU& Object::getLocalAABB(void) const
{
	return _local_aabb;
}

const Gleam::AABBCPU& Object::getWorldAABB(void) const
{
	return _world_aabb;
}

void Object::setLocalAABB(const Gleam::AABBCPU& aabb)
{
	_local_aabb = aabb;
}

size_t Object::getNumComponents(void) const
{
	return _components.size();
}

const IComponent* Object::getComponent(unsigned int index) const
{
	assert(index < _components.size());
	return _components[index];
}

IComponent* Object::getComponent(unsigned int index)
{
	assert(index < _components.size());
	return _components[index];
}

const Array<IComponent*>& Object::getComponents(void) const
{
	return _components;
}

Array<IComponent*>& Object::getComponents(void)
{
	return _components;
}

void Object::addChild(Object* object)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_children_lock);
	_children.push(object);
	object->_parent = this;
}

void Object::removeFromParent(void)
{
	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_parent->_children_lock);
		auto it = _parent->_children.linearSearch(this);

		if (it != _parent->_children.end()) {
			_parent->_children.fastErase(it);
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
		_world_aabb.transform(_parent->_world_transform);

	} else {
		_world_transform = _local_transform;
		_world_aabb = _local_aabb;
	}

	for (auto it = _children.begin(); it != _children.end(); ++it) {
		// Create jobs for updating children nodes.
		// We will be assuming that most objects won't have that many children to be worth splitting into multiple updates.
		(*it)->updateTransforms();
	}
}

void Object::registerForLocalDirtyCallback(const DirtyCallback& callback, unsigned long long user_data)
{
	_local_callbacks.emplacePush(callback, user_data);
}

void Object::unregisterForLocalDirtyCallback(const DirtyCallback& callback)
{
	auto it = _local_callbacks.linearSearch(callback,
	[](const Gaff::Pair<DirtyCallback, unsigned long long>& left, const DirtyCallback& right) -> bool
	{
		return left.first == right;
	});

	if (it != _local_callbacks.end()) {
		_local_callbacks.fastErase(it);
	}
}

void Object::notifyLocalDirtyCallbacks(void)
{
	for (auto it = _local_callbacks.begin(); it != _local_callbacks.end(); ++it) {
		it->first(this, it->second);
	}
}

void Object::registerForWorldDirtyCallback(const DirtyCallback& callback, unsigned long long user_data)
{
	_world_callbacks.emplacePush(callback, user_data);
}

void Object::unregisterForWorldDirtyCallback(const DirtyCallback& callback)
{
	auto it = _world_callbacks.linearSearch(callback,
	[](const Gaff::Pair<DirtyCallback, unsigned long long>& left, const DirtyCallback& right) -> bool
	{
		return left.first == right;
	});

	if (it != _world_callbacks.end()) {
		_world_callbacks.fastErase(it);
	}
}

void Object::notifyWorldDirtyCallbacks(void)
{
	for (auto it = _world_callbacks.begin(); it != _world_callbacks.end(); ++it) {
		it->first(this, it->second);
	}
}

bool Object::createComponents(const Gaff::JSON& json)
{
	bool error = false;

	_components.reserve(json.size());

	json.forEachInObject([&](const char* key, Gaff::JSON& value) -> bool
	{
		if (!value.isObject()) {
			// log error
			error = true;
			return true;
		}

		Gaff::JSON type = value["type"];

		if (!type.isString()) {
			// log error
			error = true;
			return true;
		}

		IComponent* component = _comp_mgr.createComponent(type.getString());

		if (!component) {
			// log error
			error = true;
			return true;
		}

		component->setOwner(this);
		component->setName(key);

		if (!component->validate(value) || !component->load(value)) {
			error = true;
			return true;
		}

		_components.push(component);

		return false;
	});

	return !error;
}

void Object::markDirty(void)
{
	if (!_dirty) {

		_dirty = true;
	}
}

NS_END
