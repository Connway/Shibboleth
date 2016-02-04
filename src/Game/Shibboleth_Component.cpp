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

#include "Shibboleth_Component.h"
#include <Gaff_Utils.h>

#define CMP_ACTIVE 1
#define CMP_IN_WORLD 2

NS_SHIBBOLETH

using ClearSetBitFunc = void (*)(char&, char);
ClearSetBitFunc gClear_set_funcs[] = { &Gaff::ClearBits<char>, &Gaff::SetBits<char> };

Component::Component(void):
	_owner(nullptr), _comp_index(0),
	_flags(CMP_ACTIVE)
{
}

Component::~Component(void)
{
}

const Gaff::JSON& Component::getSchema(void) const
{
	static Gaff::JSON empty_schema;
	return empty_schema;
}

bool Component::validate(const Gaff::JSON& json)
{
	const Gaff::JSON& schema = getSchema();
	return (schema) ? json.validate(schema) : true;
}

bool Component::load(const Gaff::JSON&)
{
	return true;
}

bool Component::save(Gaff::JSON&)
{
	return true;
}

void Component::allComponentsLoaded(void)
{
}

bool Component::isInWorld(void) const
{
	return Gaff::IsAnyBitSet<char>(_flags, CMP_IN_WORLD);
}

void Component::addToWorld(void)
{
	Gaff::SetBits<char>(_flags, CMP_IN_WORLD);
}

void Component::removeFromWorld(void)
{
	Gaff::ClearBits<char>(_flags, CMP_IN_WORLD);
}

void Component::setActive(bool active)
{
	gClear_set_funcs[active](_flags, CMP_ACTIVE);
}

bool Component::isActive(void) const
{
	return Gaff::IsAnyBitSet<char>(_flags, CMP_ACTIVE);
}

const AString& Component::getName(void) const
{
	return _name;
}

void Component::setName(const char* name)
{
	assert(name && strlen(name));
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
