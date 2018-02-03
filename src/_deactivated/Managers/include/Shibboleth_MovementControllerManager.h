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

#include "Shibboleth_IMovementControllerManager.h"
#include <Shibboleth_IUpdateQuery.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Map.h>

NS_SHIBBOLETH

class Object;

class IMovementController
{
public:
	IMovementController(void) {}
	virtual ~IMovementController(void) {}

	virtual void updateMovement(void* component, double dt) = 0;
};

class MovementControllerManager : public IManager, public IUpdateQuery, public IMovementControllerManager
{
public:
	template <class Controller>
	void registerMovementController(void)
	{
		auto it = _update_list.findElementWithKey(Controller::GetReflectionHash());

		if (it == _update_list.end()) {
			IMovementController* controller = SHIB_ALLOCT(Controller, *GetAllocator());
			_update_list.emplace(Controller::GetReflectionHash(), Gaff::MakePair(controller, Array<void*>()));
		}
	}

	template <class Controller>
	void registerMovementComponent(void* component)
	{
		registerMovementComponent(Controller::GetReflectionHash());
	}

	template <class Controller>
	void unregisterMovementComponent(void* component)
	{
		unregisterMovementComponent(Controller::GetReflectionHash());
	}

	MovementControllerManager(void);
	~MovementControllerManager(void);

	const char* getName(void) const override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	void registerMovementComponent(Gaff::ReflectionHash controller_class_id, void* component) override;
	void unregisterMovementComponent(Gaff::ReflectionHash controller_class_id, void* component) override;

private:
	using ControllerObjects = Gaff::Pair< IMovementController*, Array<void*> >;

	Map<Gaff::ReflectionHash, ControllerObjects> _update_list;

	void update(double dt, void*);

	SHIB_REF_DEF(MovementControllerManager);
	REF_DEF_REQ;
};

NS_END
