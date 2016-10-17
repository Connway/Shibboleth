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

#pragma once

#include <Shibboleth_String.h>
#include <Gaff_IRequestableInterface.h>
#include <Gaff_JSON.h>

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

class Component : public Gaff::IRequestableInterface
{
public:
	Component(void);
	virtual ~Component(void);

	virtual const Gaff::JSON& getSchema(void) const;

	virtual bool validate(const Gaff::JSON& json);

	virtual bool load(const Gaff::JSON&);
	virtual bool save(Gaff::JSON&);

	virtual void allComponentsLoaded(void);

	virtual void addToWorld(void);
	virtual void removeFromWorld(void);

	virtual void setActive(bool active);
	virtual bool isActive(void) const;

	const U8String& getName(void) const;
	void setName(const char* name);

	const Object* getOwner(void) const;
	Object* getOwner(void);
	void setOwner(Object* owner);

	size_t getIndex(void) const;
	void setIndex(size_t index);

private:
	U8String _name;
	Object* _owner;
	size_t _comp_index;
	bool _active;

	GAFF_NO_COPY(Component);
	GAFF_NO_MOVE(Component);
};

NS_END
