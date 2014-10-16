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

#include "Shibboleth_Object.h"
#include "Shibboleth_ComponentManager.h"
#include "Shibboleth_IApp.h"
#include <Gaff_JSON.h>

NS_SHIBBOLETH

Object::Object(IApp& app, unsigned int id):
	_comp_mgr(app.getManagerT<ComponentManager>("Component Manager")),
	_app(app), _id(id)
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

	strncpy(_name, name.getString(), MAX_OBJ_NAME_LENGTH);

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
}

const char* Object::getName(void) const
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

Gaff::WatchReceipt Object::watchRotation(const Watcher<Gleam::Quaternion>::Callback& callback)
{
	return _rotation.addCallback(callback);
}

Gaff::WatchReceipt Object::watchPosition(const Watcher<Gleam::Vec4>::Callback& callback)
{
	return _position.addCallback(callback);
}

Gaff::WatchReceipt Object::watchScale(const Watcher<Gleam::Vec4>::Callback& callback)
{
	return _scale.addCallback(callback);
}

Gaff::WatchReceipt Object::watchAABB(const Watcher<Gleam::AABB>::Callback& callback)
{
	return _aabb.addCallback(callback);
}

const Gleam::Quaternion& Object::getRotation(void) const
{
	return _rotation.get();
}

void Object::setRotation(const Gleam::Quaternion& rot)
{
	_rotation = rot;
}

const Gleam::Vec4& Object::getPosition(void) const
{
	return _position.get();
}

void Object::setPosition(const Gleam::Vec4& pos)
{
	_position = pos;
}

const Gleam::Vec4& Object::getScale(void) const
{
	return _scale.get();
}

void Object::setScale(const Gleam::Vec4& scale)
{
	_scale = scale;
}

const Gleam::AABB& Object::getAABB(void) const
{
	return _aabb.get();
}

void Object::setAABB(const Gleam::AABB& aabb)
{
	_aabb = aabb;
}

bool Object::createComponents(const Gaff::JSON& json)
{
	bool error = false;

	_components.reserve(json.size());

	json.forEachInObject([&](const char* key, const Gaff::JSON& value) -> bool
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

		if (!component->load(value)) {
			// log error
			error = true;
			return true;
		}

		_components.push(component);

		return false;
	});

	return !error;
}

NS_END
