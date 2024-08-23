/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Shibboleth_EntityUpdater.h"
#include "Shibboleth_EntityManager.h"
#include <Ptrs/Shibboleth_ManagerRef.h>

NS_SHIBBOLETH

EntityUpdater::~EntityUpdater(void)
{
	if (isEnabled()) {
		disable();
	}

	for (EntityUpdater* updater : _update_after) {
		updater->_depends_on_me.erase_first(this);
	}

	for (EntityUpdater* updater : _depends_on_me) {
		updater->_update_after.erase_first(updater);
	}
}

void EntityUpdater::updateAfter(EntityUpdater& updater)
{
	updater._depends_on_me.emplace_back(this);
	_update_after.emplace_back(&updater);

	if (isEnabled()) {
		markDirty();
	}
}

void EntityUpdater::setEnabled(bool value)
{
	if (value && !isEnabled()) {
		enable();
	} else if (!value && isEnabled()) {
		disable();
	}
}

bool EntityUpdater::isEnabled(void) const
{
	return _node_id != -1;
}

void EntityUpdater::disable(void)
{
	GAFF_ASSERT(isEnabled());

	// $TODO: Potentially cache this if it becomes a perf problem.
	ManagerRef<EntityManager> entity_mgr;
	entity_mgr->disable(*this);
}

void EntityUpdater::enable(void)
{
	GAFF_ASSERT(!isEnabled() && _update_function);

	// $TODO: Potentially cache this if it becomes a perf problem.
	ManagerRef<EntityManager> entity_mgr;
	entity_mgr->enable(*this);
}

void EntityUpdater::setUpdateFunction(const UpdateFunction& function)
{
	_update_function = function;
}

void EntityUpdater::setUpdateFunction(UpdateFunction&& function)
{
	_update_function = std::move(function);
}

void EntityUpdater::setUpdatePhase(EntityUpdatePhase update_phase)
{
	_update_phase = update_phase;

	if (isEnabled()) {
		markDirty();
	}
}

EntityUpdatePhase EntityUpdater::getUpdatePhase(void) const
{
	return _update_phase;
}

void EntityUpdater::markDirty(void)
{
	// $TODO: Potentially cache this if it becomes a perf problem.
	ManagerRef<EntityManager> entity_mgr;
	entity_mgr->markDirty(*this);
}

NS_END
