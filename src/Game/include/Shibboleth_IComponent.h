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

#pragma once

#include <Shibboleth_String.h>
#include <Gaff_IRequestableInterface.h>
#include <Gaff_IVirtualDestructor.h>
#include <Gaff_IncludeAssert.h>
#include <cstring>

NS_GAFF
	class JSON;
NS_END

NS_SHIBBOLETH

#define COMP_REF_DEF_LOAD(Class, RefDefName) \
	bool Class::load(const Gaff::JSON& json) \
	{ \
		RefDefName.read(json, this); \
		return true; \
	}

#define COMP_REF_DEF_SAVE(Class, RefDefName) \
	bool Class::save(Gaff::JSON& json) \
	{ \
		RefDefName.write(json, this); \
		return true; \
	}

class Object;

class IComponent : public Gaff::IRequestableInterface
{
public:
	IComponent(void): _owner(nullptr), _comp_index(0) {}
	virtual ~IComponent(void) {}

	virtual bool validate(Gaff::JSON&) { return true; }
	virtual bool load(const Gaff::JSON&) { return true; }
	virtual bool save(Gaff::JSON&) { return true; }

	virtual void allComponentsLoaded(void) {}

	const AString& getName(void) const
	{
		return _name;
	}

	void setName(const char* name)
	{
		assert(name && strlen(name));
		_name = name;
	}

	const Object* getOwner(void) const
	{
		return _owner;
	}

	Object* getOwner(void)
	{
		return _owner;
	}

	void setOwner(Object* owner)
	{
		_owner = owner;
	}

	size_t getIndex(void) const
	{
		return _comp_index;
	}

	void setIndex(size_t index)
	{
		_comp_index = index;
	}

private:
	AString _name;
	Object* _owner;
	size_t _comp_index;

	GAFF_NO_COPY(IComponent);
	GAFF_NO_MOVE(IComponent);
};

NS_END
