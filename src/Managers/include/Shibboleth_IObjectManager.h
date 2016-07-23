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

#include <Shibboleth_ReflectionDefinitions.h>

NS_SHIBBOLETH

class Object;

class IObjectManager
{
public:
	IObjectManager(void) {}
	virtual ~IObjectManager(void) {}

	virtual Object* createObject(void) = 0;
	virtual void removeObject(Object* object) = 0;
	virtual void removeObject(unsigned int id) = 0;
	virtual const Object* getObject(unsigned int id) const = 0;
	virtual Object* getObject(unsigned int id) = 0;
	virtual bool doesObjectExist(const Object* object) const = 0;
	virtual bool doesObjectExist(unsigned int id) const = 0;

	virtual const Object* findObject(const char* name) const = 0;
	virtual Object* findObject(const char* name) = 0;
	virtual const Object* findObject(uint32_t name_hash) const = 0;
	virtual Object* findObject(uint32_t name_hash) = 0;

	virtual void addDirtyObject(Object* object) = 0;
	virtual void addNewObject(Object* object) = 0;

	SHIB_INTERFACE_REFLECTION(IObjectManager)
	SHIB_INTERFACE_MANAGER("Object Manager")
};

NS_END
