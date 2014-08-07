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

#pragma once

#include "Shibboleth_IComponent.h"
#include "Shibboleth_Array.h"
#include <Gaff_INamedObject.h>
#include <Gaff_WeakObject.h>
#include <Gaff_Function.h>
#include <Gaff_SmartPtr.h>
#include <Gleam_AABB.h>

#define MAX_OBJ_NAME_LENGTH 64

NS_SHIBBOLETH

class App;

class Object : public Gaff::INamedObject, public Gaff::WeakObject<ProxyAllocator>
{
public:
	typedef Gaff::FunctionBinder<void, double> UpdateCallback;

	Object(App& app, unsigned int id);
	~Object(void);

	bool init(const Gaff::JSON& json);
	INLINE bool init(const char* file_name);
	void destroy(void);

	const char* getName(void) const;

	unsigned int getID(void) const;
	void setID(unsigned int);

	void registerForPrePhysicsUpdate(const UpdateCallback& callback);
	void registerForPostPhysicsUpdate(const UpdateCallback& callback);

	void prePhysicsUpdate(double dt);
	void postPhysicsUpdate(double dt);

private:
	char _name[MAX_OBJ_NAME_LENGTH];

	Gleam::AABB _aabb;
	Gleam::Vec4 _pos;

	typedef Gaff::SmartPtr<IComponent, ProxyAllocator> ComponentPtr;

	Array<ComponentPtr> _components;
	App& _app;

	unsigned int _id;

	bool createComponents(const Gaff::JSON& json);

	GAFF_NO_COPY(Object);
	GAFF_NO_MOVE(Object);
};

NS_END
