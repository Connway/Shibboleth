/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_EntityManager.h"
#include <Shibboleth_AppUtils.h>
#include <Shibboleth_ISystem.h>

NS_SHIBBOLETH

template <EntityManager::UpdatePhase update_phase>
class EntityUpdateSystem : public ISystem
{
public:
	bool init(void) override
	{
		_entity_mgr = &GetManagerTFast<EntityManager>();
		return true;
	}

	void update(uintptr_t /*thread_id_int*/) override
	{
		_entity_mgr->updateEntitiesAndComponents(update_phase);
	}

private:
	EntityManager* _entity_mgr = nullptr;
};

class EntityUpdatePrePhysicsSystem final : public EntityUpdateSystem<EntityManager::UpdatePhase::PrePhysics>
{
	SHIB_REFLECTION_CLASS_DECLARE(EntityUpdatePrePhysicsSystem);
};

class EntityUpdateDuringPhysicsSystem final : public EntityUpdateSystem<EntityManager::UpdatePhase::DuringPhysics>
{
	SHIB_REFLECTION_CLASS_DECLARE(EntityUpdateDuringPhysicsSystem);
};

class EntityUpdatePostPhysicsSystem final : public EntityUpdateSystem<EntityManager::UpdatePhase::PostPhysics>
{
	SHIB_REFLECTION_CLASS_DECLARE(EntityUpdatePostPhysicsSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::EntityUpdatePrePhysicsSystem)
SHIB_REFLECTION_DECLARE(Shibboleth::EntityUpdateDuringPhysicsSystem)
SHIB_REFLECTION_DECLARE(Shibboleth::EntityUpdatePostPhysicsSystem)
