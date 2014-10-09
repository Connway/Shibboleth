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

#include "Shibboleth_IUpdateQuery.h"
#include "Shibboleth_IManager.h"
#include "Shibboleth_Object.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_Array.h>
#include <Shibboleth_Map.h>
#include <Gaff_SpinLock.h>

NS_SHIBBOLETH

class IApp;

class ObjectManager : public IManager, public IUpdateQuery
{
public:
	template <class Callback>
	void forEachObject(Callback&& callback) const
	{
		for (auto it = _objects.begin(); it != _objects.end(); ++it) {
			if (callback(*it)) {
				break;
			}
		}
	}

	ObjectManager(IApp& app);
	~ObjectManager(void);

	const char* getName(void) const;

	void requestUpdateEntries(Array<UpdateEntry>& entries);
	void* rawRequestInterface(unsigned int class_id) const;

	static void InitReflectionDefinition(void);

private:
	Array<unsigned int> _remove_queue;
	Array<Object*> _add_queue;
	Array<Object*> _objects;

	IApp& _app;

	Gaff::SpinLock _remove_lock;
	Gaff::SpinLock _add_lock;

	volatile unsigned int _next_id;

	void prePhysicsUpdate(double dt);
	void postPhysicsUpdate(double dt);

	GAFF_NO_COPY(ObjectManager);
	GAFF_NO_MOVE(ObjectManager);

	REF_DEF_SHIB(ObjectManager);
};

NS_END
