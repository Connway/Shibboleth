/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_Component.h"

NS_SHIBBOLETH

Component::Component(void):
	_owner(nullptr), _comp_index(0),
	_active(true)
{
}

Component::~Component(void)
{
}

//const Gaff::JSON& Component::getSchema(void) const
//{
//	static Gaff::JSON empty_schema;
//	return empty_schema;
//}
//
//bool Component::validate(const Gaff::JSON& json)
//{
//	const Gaff::JSON& schema = getSchema();
//
//	if (!schema.isNull() && !json.validate(schema)) {
//		// log error
//		return false;
//	}
//
//	return true;
//}

void Component::load(const Gaff::ISerializeReader& reader)
{
	getReflectionDefinition().load(reader, getBasePointer());
}

bool Component::save(Gaff::ISerializeWriter& /*writer*/)
{
	return true;
}

void Component::onAllComponentsLoaded(void)
{
}

void Component::onAddToWorld(void)
{
}

void Component::onRemoveFromWorld(void)
{
}

void Component::setActive(bool active)
{
	_active = active;
}

bool Component::isActive(void) const
{
	return _active;
}

const U8String& Component::getName(void) const
{
	return _name;
}

void Component::setName(const char* name)
{
	GAFF_ASSERT(name && strlen(name));
	_name = name;
}

const Object* Component::getOwner(void) const
{
	return _owner;
}

Object* Component::getOwner(void)
{
	return _owner;
}

void Component::setOwner(Object* owner)
{
	_owner = owner;
}

size_t Component::getIndex(void) const
{
	return _comp_index;
}

void Component::setIndex(size_t index)
{
	_comp_index = index;
}

NS_END
