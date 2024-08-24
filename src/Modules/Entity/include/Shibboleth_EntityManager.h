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

#pragma once

#include "Shibboleth_EntityUpdater.h"
#include <Containers/Shibboleth_SparseStack.h>
#include <Containers/Shibboleth_VectorMap.h>
#include <Containers/Shibboleth_Vector.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_JobPool.h>
#include <Gaff_IncludeEASTLAtomic.h>
#include <eathread/eathread_mutex.h>

NS_SHIBBOLETH

class GameTimeManager;

class EntityManager final : public IManager
{
public:
	bool initAllModulesLoaded(void) override;

	void update(uintptr_t thread_id_int, EntityUpdatePhase update_phase);
	void updateDirtyNodes(void);

private:
	struct UpdateNode final
	{
		enum class Flag
		{
			Dirty,

			Count
		};

		UpdateNode(const UpdateNode& node):
			updater(node.updater),
			prereq_count(static_cast<int32_t>(node.prereq_count)),
			level(node.level),
			update_phase(node.update_phase),
			flags(node.flags)
		{
		}

		UpdateNode(void) = default;

		UpdateNode& operator=(UpdateNode&& node)
		{
			updater = node.updater;
			prereq_count = static_cast<int32_t>(node.prereq_count);
			level = node.level;
			update_phase = node.update_phase;
			flags = node.flags;

			return *this;
		}

		EntityUpdater* updater = nullptr;
		eastl::atomic<int32_t> prereq_count = 0;
		int32_t level = -1;
		EntityUpdatePhase update_phase = EntityUpdatePhase::Count;

		Gaff::Flags<Flag> flags;
	};

	struct UpdateNodeLevel final
	{
		Vector<int32_t> update_nodes{ ENTITY_ALLOCATOR };
	};

	Vector<UpdateNodeLevel> _update_levels[static_cast<size_t>(EntityUpdatePhase::Count)] =
	{
		Vector<UpdateNodeLevel>{ ENTITY_ALLOCATOR },
		Vector<UpdateNodeLevel>{ ENTITY_ALLOCATOR },
		Vector<UpdateNodeLevel>{ ENTITY_ALLOCATOR }
	};

	SparseStack<UpdateNode> _update_nodes{ ENTITY_ALLOCATOR };

	Vector<int32_t> _dirty_nodes{ ENTITY_ALLOCATOR };

	// Job Data
	Vector<int32_t> _nodes_to_update{ ENTITY_ALLOCATOR };
	Vector<Gaff::JobData> _job_data{ ENTITY_ALLOCATOR };

	EA::Thread::Mutex _nodes_to_update_lock;
	EA::Thread::Mutex _update_nodes_lock;
	EA::Thread::Mutex _dirty_nodes_lock;

	const GameTimeManager* _game_time_mgr = nullptr;
	JobPool* _job_pool = nullptr;
	Gaff::Counter _count = 0;

	EntityUpdatePhase _current_update_phase = EntityUpdatePhase::Count;


	// "register" is a reserved keyword, so using enable/disable.
	void disable(EntityUpdater& updater);
	void enable(EntityUpdater& updater);

	void markDirty(EntityUpdater& updater);
	void markDirty(UpdateNode& node);

	void updateDirtyNode(UpdateNode& node);

	static void UpdateEntities(uintptr_t, void* data);

	friend class EntityUpdater;

	SHIB_REFLECTION_CLASS_DECLARE(EntityManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::EntityManager)
