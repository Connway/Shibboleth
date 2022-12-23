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
#include <eathread/eathread_rwspinlock.h>
#include <eathread/eathread_rwmutex.h>

NS_SHIBBOLETH

class GameTimeManager;

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
	T* createUpdateable(void)
	{
		static_assert(std::is_base_of<IEntityUpdateable, T>::value, "T must be derived from IEntityUpdateable.");

		IEntityUpdateable* const updateable = createUpdateable(Refl::Reflection<T>::GetReflectionDefinition());
		return (updateable) ? static_cast<T*>(updateable) : nullptr;
	}

	template <class T>
	T* createEntity(void)
	{
		static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity.");

		Entity* const entity = createEntity(Refl::Reflection<T>::GetReflectionDefinition());
		return (entity) ? static_cast<T*>(entity) : nullptr;
	}

	//~EntityManager(void);

	bool initAllModulesLoaded(void) override;

	IEntityUpdateable* createUpdateable(const Refl::IReflectionDefinition& ref_def);

	Entity* createEntity(const Refl::IReflectionDefinition& ref_def);
	Entity* createEntity(void);

	void destroy(IEntityUpdateable& updateable);

	void updateEntitiesAndComponents(UpdatePhase update_phase);

	void changeDefaultUpdatePhase(IEntityUpdateable& entity, UpdatePhase update_phase);

	void updateAfter(IEntityUpdateable& updateable, IEntityUpdateable& after);

private:
	struct UpdateNode final
	{
		enum class Flag
		{
			Component,

			Removed,
			Destroy,
			Dirty,

			Count
		};

		IEntityUpdateable* updater = nullptr;
		UpdateNode* root = nullptr;
		UpdateNode* parent = nullptr;
		UpdateNode* prev_sibling = nullptr;
		UpdateNode* next_sibling = nullptr;
		UpdateNode* first_child = nullptr;
		UpdateNode* last_child = nullptr;

		int16_t depth = 1;
		Gaff::Flags<Flag> flags;
	};

	UpdateNode _update_roots[static_cast<size_t>(UpdatePhase::Count)];
	mutable EA::Thread::RWMutex _graph_locks[static_cast<size_t>(UpdatePhase::Count)]; // Mutable so const functions can take a lock.

	EA::Thread::Mutex _dirty_nodes_locks[static_cast<size_t>(UpdatePhase::Count)];
	Vector<UpdateNode*> _dirty_nodes[static_cast<size_t>(UpdatePhase::Count)] =
	{
		Vector<UpdateNode*>{ ProxyAllocator("Entity") },
		Vector<UpdateNode*>{ ProxyAllocator("Entity") },
		Vector<UpdateNode*>{ ProxyAllocator("Entity") }
	};

	const GameTimeManager* _game_time_mgr = nullptr;

	void markDirty(UpdateNode& node);

	UpdatePhase getUpdatePhase(const UpdateNode& node) const;

	void addToGraph(UpdateNode& node);
	void removeFromGraph(UpdateNode& node);

	SHIB_REFLECTION_CLASS_DECLARE(EntityManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::EntityManager)
