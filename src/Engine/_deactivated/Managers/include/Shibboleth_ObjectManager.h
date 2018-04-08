/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_IObjectManager.h"
#include <Shibboleth_IUpdateQuery.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Array.h>
#include <Gaff_SpinLock.h>

NS_SHIBBOLETH

class ObjectManager : public IManager, public IUpdateQuery, public IObjectManager
{
public:
	template <class Callback>
	bool forEachObject(Callback&& callback) const
	{
		for (auto it = _objects.begin(); it != _objects.end(); ++it) {
			if (callback(*it)) {
				return true;
			}
		}

		return false;
	}

	ObjectManager(void);
	~ObjectManager(void);

	const char* getName(void) const override;
	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	Object* createObject(void) override;
	void removeObject(Object* object) override;
	void removeObject(unsigned int id) override;
	const Object* getObject(unsigned int id) const override;
	Object* getObject(unsigned int id) override;
	bool doesObjectExist(const Object* object) const override;
	bool doesObjectExist(unsigned int id) const override;

	const Object* findObject(const char* name) const override;
	Object* findObject(const char* name) override;
	const Object* findObject(uint32_t name_hash) const override;
	Object* findObject(uint32_t name_hash) override;

	void addDirtyObject(Object* object) override;
	void addNewObject(Object* object) override;

private:
	Array<Object*> _objects;
	Gaff::SpinLock _objects_lock;

	Array<Object*> _dirty_objects;
	Gaff::SpinLock _dirty_objects_lock;

	Array<Object*> _new_objects;
	Gaff::SpinLock _new_objects_lock;

	volatile unsigned int _next_id;

	void updateDirtyObjects(double, void*);
	void updateNewObjects(double, void*);
	Object* findObjectHelper(uint32_t name_hash) const;

	GAFF_NO_COPY(ObjectManager);
	GAFF_NO_MOVE(ObjectManager);

	SHIB_REF_DEF(ObjectManager);
	REF_DEF_REQ;
};

NS_END
