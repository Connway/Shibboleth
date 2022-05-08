/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_Entity.h"
#include <Shibboleth_IManager.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_Vector.h>
//#include <eathread/eathread_mutex.h>

NS_SHIBBOLETH

class EntityManager final : public IManager
{
public:
	enum class UpdatePhase
	{
		PrePhysics,
		DuringPhysics,
		PostPhysics,

		Count
	};

	template <class T>
	T* createEntity(void)
	{
		static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity.");

		Entity* const entity = createEntity(Refl::Reflection<T>::GetReflectionDefinition());
		return (entity) ? static_cast<T*>(entity) : nullptr;
	}

	//~EntityManager(void);

	//bool initAllModulesLoaded(void) override;

	Entity* createEntity(const Refl::IReflectionDefinition& ref_def);
	Entity* createEntity(void);

	void destroyEntity(Entity& entity);
	void destroyEntity(EntityID id);

	void updateEntitiesAndComponents(UpdatePhase update_phase);

	void changeDefaultUpdatePhase(const Entity& entity, UpdatePhase update_phase);
	void markDirty(Entity& entity);

private:
	struct UpdateNode final
	{
		IEntityUpdateable* updater = nullptr;
		UpdateNode* parent = nullptr;
		UpdateNode* first_child = nullptr;
		UpdateNode* last_child = nullptr;
		UpdateNode* prev_sibling = nullptr;
		UpdateNode* next_sibling = nullptr;
	};

	UpdateNode _update_roots[static_cast<size_t>(UpdatePhase::Count)];
	// $TODO: Change this to using a page system?
	Vector<UpdateNode*> _dirty_nodes{ ProxyAllocator("Entity") };
	Vector< UniquePtr<UpdateNode> > _entities{ ProxyAllocator("Entity") };
	Vector<EntityID> _free_ids{ ProxyAllocator("Entity") };
	EntityID _next_id = 0;

	void removeFromGraph(UpdateNode& node);
	void returnID(EntityID id);
	EntityID allocateID(void);

	SHIB_REFLECTION_CLASS_DECLARE(EntityManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::EntityManager)
