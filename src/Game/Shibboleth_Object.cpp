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
#include "Shibboleth_App.h"
#include <Gaff_JSON.h>

NS_SHIBBOLETH

Object::Object(App& app, unsigned int id):
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
}

const char* Object::getName(void) const
{
	return _name;
}

unsigned int Object::getID(void) const
{
	return _id;
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

Gaff::WatchReceipt Object::watchAABB(const Watcher<Gleam::AABB>::Callback& callback)
{
	return _aabb.addCallback(callback);
}

Gaff::WatchReceipt Object::watchPos(const Watcher<Gleam::Vec4>::Callback& callback)
{
	return _pos.addCallback(callback);
}

const Gleam::AABB& Object::getAABB(void) const
{
	return _aabb.get();
}

void Object::setAABB(const Gleam::AABB& aabb)
{
	_aabb = aabb;
}

const Gleam::Vec4& Object::getPos(void) const
{
	return _pos.get();
}

void Object::setPos(const Gleam::Vec4& pos)
{
	_pos = pos;
}

bool Object::createComponents(const Gaff::JSON& json)
{
	ComponentManager& component_manager = _app.getManager<ComponentManager>("Component Manager");
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

		IComponent* component = component_manager.createComponent(type.getString());

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

		_components.push(ComponentPtr(component));

		return false;
	});

	return !error;
}

NS_END
