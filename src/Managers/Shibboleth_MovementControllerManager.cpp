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

#include "Shibboleth_MovementControllerManager.h"

NS_SHIBBOLETH

REF_IMPL_REQ(MovementControllerManager);
SHIB_REF_IMPL(MovementControllerManager)
.addBaseClassInterfaceOnly<MovementControllerManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IMovementControllerManager)
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

MovementControllerManager::MovementControllerManager(void)
{
}

MovementControllerManager::~MovementControllerManager(void)
{
	IAllocator* allocator = GetAllocator();

	for (auto it = _update_list.begin(); it != _update_list.end(); ++it) {
		SHIB_FREET(it->second.first, *allocator);
	}
}

const char* MovementControllerManager::getName(void) const
{
	return GetFriendlyName();
}

void MovementControllerManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(U8String("Movement Controller Manager: Update"), Gaff::Bind(this, &MovementControllerManager::update));
}

void MovementControllerManager::registerMovementComponent(Gaff::ReflectionHash controller_class_id, void* component)
{
	auto it = _update_list.findElementWithKey(controller_class_id);

	if (it == _update_list.end()) {
		// log error
		return;
	}

	it->second.second.emplacePush(component);
}

void MovementControllerManager::unregisterMovementComponent(Gaff::ReflectionHash controller_class_id, void* component)
{
	auto it = _update_list.findElementWithKey(controller_class_id);

	if (it == _update_list.end()) {
		// log error
		return;
	}

	auto& comps = it->second.second;
	auto it_cmp = comps.linearSearch(component);

	if (it_cmp == comps.end()) {
		// log error
		return;
	}

	comps.fastErase(it_cmp);
}

void MovementControllerManager::update(double dt, void*)
{
	for (auto it_list = _update_list.begin(); it_list != _update_list.end(); ++it_list) {
		IMovementController* controller = it_list->second.first;
		auto& components = it_list->second.second;

		for (auto it_cmp = components.begin(); it_cmp != components.end(); ++it_cmp) {
			controller->updateMovement(*it_cmp, dt);
		}
	}
}

NS_END
