/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#define SHIB_REFL_IMPL
#include "Shibboleth_EntityManager.h"
#include <Shibboleth_GameTime.h>
#include <Shibboleth_AppUtils.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EntityManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::EntityManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(EntityManager)


bool EntityManager::initAllModulesLoaded(void)
{
	_game_time_mgr = &GetManagerT<GameTimeManager>();
	_job_pool = &GetApp().getJobPool();

	_job_data.reserve(static_cast<size_t>(_job_pool->getNumTotalThreads()));
	_job_data.emplace_back(Gaff::JobData{ UpdateEntities, this });

	return true;
}

void EntityManager::update(uintptr_t thread_id_int, EntityUpdatePhase update_phase)
{
	if (_update_levels[static_cast<size_t>(update_phase)].empty()) {
		return;
	}

	_nodes_to_update = _update_levels[static_cast<size_t>(update_phase)].front().update_nodes;
	_current_update_phase = update_phase;
	_current_level = 0;

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	_job_pool->addJobs(_job_data.data(), static_cast<int32_t>(_job_data.size()), _count);
	_job_pool->helpWhileWaiting(thread_id, _count);
}

void EntityManager::updateDisabledNodes(void)
{
	for (int32_t index : _disable_nodes) {
		UpdateNode& node = _update_nodes[index];
		disableNode(node);
	}

	_disable_nodes.clear();
}

void EntityManager::updateDirtyNodes(void)
{
	for (int32_t index : _dirty_nodes) {
		UpdateNode& node = _update_nodes[index];

		// This node might have already been processed.
		if (!node.flags.testAny(UpdateNode::Flag::Dirty)) {
			continue;
		}

		updateDirtyNode(node);
	}

	_dirty_nodes.clear();
}

void EntityManager::disable(EntityUpdater& updater)
{
	GAFF_ASSERT(updater._node_id > -1);

	EA::Thread::AutoSpinLock lock{ _disable_nodes_lock };

	Gaff::PushBackUnique(_disable_nodes, updater._node_id);
}

void EntityManager::enable(EntityUpdater& updater)
{
	GAFF_ASSERT(updater._node_id <= -1);

	EA::Thread::AutoMutex lock{ _update_nodes_lock };

	const int32_t index = _update_nodes.emplace();
	UpdateNode& node = _update_nodes[index];

	node.updater = &updater;
	node.index = index;

	updater._node_id = index;

	EA::Thread::AutoMutex dirty_lock{ _dirty_nodes_lock };
	markDirty(node);
}

void EntityManager::markDirty(EntityUpdater& updater)
{
	EA::Thread::AutoMutex nodes_lock{ _update_nodes_lock };
	EA::Thread::AutoMutex dirty_lock{ _dirty_nodes_lock };

	UpdateNode& node = _update_nodes[updater._node_id];

	markDirty(node);
}

void EntityManager::markDirty(UpdateNode& node)
{
	node.flags.set(UpdateNode::Flag::Dirty);

	Gaff::PushBackUnique(_dirty_nodes, node.index);

	// If this cache misses too much, we could store a copy of the indices locally.
	for (const EntityUpdater* const updater : node.updater->_depends_on_me) {
		UpdateNode& dependent_node = _update_nodes[updater->_node_id];
		markDirty(dependent_node);
	}
}

void EntityManager::updateDirtyNode(UpdateNode& node)
{
	EntityUpdatePhase update_phase = node.updater->_update_phase;
	int32_t level = -1;

	if (node.level > -1) {
		_update_levels[static_cast<size_t>(node.update_phase)][node.level].update_nodes.erase_first(node.index);
	}

	for (const EntityUpdater* updater : node.updater->_update_after) {
		UpdateNode& prereq_node = _update_nodes[updater->_node_id];

		if (prereq_node.flags.testAny(UpdateNode::Flag::Dirty)) {
			updateDirtyNode(prereq_node);
		}

		// We are moving up an update phase.
		if (update_phase < prereq_node.update_phase) {
			update_phase = prereq_node.update_phase;
			level = prereq_node.level;

		// We are staying on the same update phase, pick the largest level.
		} else if (update_phase == prereq_node.update_phase) {
			level = Gaff::Max(prereq_node.level, level);
		}
		// Ignore levels less than ours.
	}

	node.update_phase = update_phase;
	node.level = level + 1;

	Vector<UpdateNodeLevel>& levels = _update_levels[static_cast<size_t>(update_phase)];
	levels.resize(node.level);

	levels[node.level].update_nodes.emplace_back(node.index);
}

void EntityManager::disableNode(UpdateNode& node)
{
	// Remove our node from the level list.
	if (node.update_phase != EntityUpdatePhase::Count && node.level > -1) {
		GAFF_ASSERT(node.level < static_cast<int32_t>(_update_levels[static_cast<size_t>(node.update_phase)].size()));
		_update_levels[static_cast<size_t>(node.update_phase)][node.level].update_nodes.erase_first(node.index);
	}

	// Remove our node from the dirty list if we are dirty.
	if (node.flags.testAny(UpdateNode::Flag::Dirty)) {
		_dirty_nodes.erase_first(node.index);
	}

	// Mark all dependent nodes as dirty. Their phase and level needs to be recalculated.
	for (const EntityUpdater* const dependent : node.updater->_depends_on_me) {
		UpdateNode& dependent_node = _update_nodes[dependent->_node_id];
		markDirty(dependent_node);
	}

	node.updater->_node_id = -1;

	_update_nodes.removeAt(node.index);
}

void EntityManager::UpdateEntities(uintptr_t, void* data)
{
	EntityManager* const entity_mgr = reinterpret_cast<EntityManager*>(data);
	const Vector<UpdateNodeLevel>& levels = entity_mgr->_update_levels[static_cast<size_t>(entity_mgr->_current_update_phase)];

	const float dt = entity_mgr->_game_time_mgr->getGameTime().getDeltaFloat();
	int32_t index = -1;

	while (true) {
		{
			EA::Thread::AutoSpinLock lock(entity_mgr->_nodes_to_update_lock);

			if (entity_mgr->_nodes_to_update.empty()) {
				++entity_mgr->_current_level;

				if (entity_mgr->_current_level >= static_cast<int32_t>(levels.size())) {
	 				break;
				}

				entity_mgr->_nodes_to_update = levels[entity_mgr->_current_level].update_nodes;
			}

			index = entity_mgr->_nodes_to_update.back();
			entity_mgr->_nodes_to_update.pop_back();
		}

		UpdateNode& node = entity_mgr->_update_nodes[index];
		node.updater->_update_function(dt);
	}
}

NS_END
