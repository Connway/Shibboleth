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

#include "Shibboleth_EntityManager.h"
#include <Shibboleth_GameTime.h>
#include <Shibboleth_AppUtils.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EntityManager)
	.base<Shibboleth::IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::EntityManager)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(EntityManager)

static ProxyAllocator s_allocator("Entity");

//EntityManager::~EntityManager(void);

bool EntityManager::initAllModulesLoaded(void)
{
	_game_time_mgr = &GetManagerT<GameTimeManager>();
	return true;
}

Entity* EntityManager::createEntity(const Refl::IReflectionDefinition& ref_def)
{
	GAFF_ASSERT(ref_def.getFactory<EntityManager&>());
	GAFF_ASSERT(ref_def.hasInterface<Entity>());

	Entity* const entity = ref_def.createT<Entity>(s_allocator, *this);

	if (entity) {
		// $TODO: Add to world and manage newly created entity.

		UpdateNode* const node = SHIB_ALLOCT(UpdateNode, s_allocator);

		if (!node) {
			// $TODO: Log error.
			SHIB_FREET(entity, s_allocator);
			return nullptr;
		}

		// Default to DuringPhysics update phase.
		node->root = &_update_roots[static_cast<size_t>(UpdatePhase::DuringPhysics)];
		node->parent = node->root;
		node->updater = entity;

		Gaff::Flags extra_flags(UpdateNode::Flag::Removed);

		if (!entity->init()) {
			// $TODO: Log error.

			extra_flags.set(UpdateNode::Flag::Destroy);
		}

		// Mark dirty so it gets inserted in to the graph appropriately.
		markDirty(*node, extra_flags);

	} else {
		// $TODO: Log error.
	}

	return entity;
}

Entity* EntityManager::createEntity(void)
{
	return createEntity(Refl::Reflection<Entity>::GetReflectionDefinition());
}

void EntityManager::destroy(IEntityUpdateable& updateable)
{
	UpdateNode* const node = static_cast<UpdateNode*>(updateable._update_node);

	markDirty(*node, Gaff::Flags(UpdateNode::Flag::Destroy));

	for (const void* dep_node : updateable._dependent_on_me) {
		UpdateNode* const dn = static_cast<UpdateNode*>(const_cast<void*>(dep_node));
		markDirty(*dn);
	}
}

void EntityManager::updateEntitiesAndComponents(UpdatePhase update_phase)
{
	// Handle all dirty nodes for this update phase.
	auto& dirty_nodes = _dirty_nodes[static_cast<size_t>(update_phase)];
	const EA::Thread::AutoMutex lock_dirty(_dirty_nodes_locks[static_cast<size_t>(update_phase)]);

	for (UpdateNode* node : dirty_nodes) {
		const EA::Thread::AutoRWMutex lock_node(
			_graph_locks[static_cast<size_t>(update_phase)],
			EA::Thread::RWMutex::kLockTypeWrite
		);

		if (!node->flags.testAll(UpdateNode::Flag::Removed)) {
			removeFromGraph(*node);
		}

		if (node->flags.testAll(UpdateNode::Flag::Destroy)) {
			SHIB_FREET(node, s_allocator);

		} else {
			addToGraph(*node);
		}

		node->flags.clear(UpdateNode::Flag::Dirty);
	}

	dirty_nodes.clear();

	// Update all nodes for this phase.
	const EA::Thread::AutoRWMutex lock_node(
		_graph_locks[static_cast<size_t>(update_phase)],
		EA::Thread::RWMutex::kLockTypeRead
	);

	UpdateNode* root = &_update_roots[static_cast<size_t>(update_phase)];
	const float dt = _game_time_mgr->getGameTime().getDeltaFloat();

	if (root->updater) {
		root->updater->update(dt);
	}

	// Iterate over all pages.
	for (UpdateNode* node_page = root->first_child; node_page; node_page = node_page->first_child) {
		// $TODO: Have each node update in a job so this gets multithreaded.
		// Update each node in the page.
		for (UpdateNode* node = node_page; node; node = node->next_sibling) {
			// If there isn't a valid updater, then we should have never made the node.
			GAFF_ASSERT(node->updater);

			node->updater->update(dt);
		}
	}
}

void EntityManager::changeDefaultUpdatePhase(IEntityUpdateable& updateable, UpdatePhase update_phase)
{
	// Ensure we have not already set dependencies.
	GAFF_ASSERT(updateable._update_after.empty() && updateable._dependent_on_me.empty());

	UpdateNode* const node = static_cast<UpdateNode*>(updateable._update_node);

	GAFF_ASSERT(node->flags.testAll(UpdateNode::Flag::Removed, UpdateNode::Flag::Dirty));
	GAFF_ASSERT(!node->parent->parent); // Make sure that we are attached to a root node.
	GAFF_ASSERT(node->root == node->parent);
	GAFF_ASSERT(!node->prev_sibling);
	GAFF_ASSERT(!node->next_sibling);
	GAFF_ASSERT(!node->first_child);
	GAFF_ASSERT(!node->last_child);

	UpdateNode& root = _update_roots[static_cast<size_t>(update_phase)];
	node->root = &root;
	node->parent = &root;
}

void EntityManager::updateAfter(IEntityUpdateable& updateable, IEntityUpdateable& after)
{
	// A component or some user introduced type that doesn't get the update node created with the object.
	if (!updateable._update_node) {
		UpdateNode* const node = SHIB_ALLOCT(UpdateNode, s_allocator);

		if (!node) {
			// $TODO: Log error.
			return;
		}

		// Default to DuringPhysics update phase.
		node->root = &_update_roots[static_cast<size_t>(UpdatePhase::DuringPhysics)];
		node->parent = node->root;
		node->updater = &updateable;

		updateable._update_node = node;
	}

	// A component or some user introduced type that doesn't get the update node created with the object.
	if (!after._update_node) {
		UpdateNode* const node = SHIB_ALLOCT(UpdateNode, s_allocator);

		if (!node) {
			// $TODO: Log error.
			return;
		}

		// Default to DuringPhysics update phase.
		node->root = &_update_roots[static_cast<size_t>(UpdatePhase::DuringPhysics)];
		node->parent = node->root;
		node->updater = &after;

		after._update_node = node;
	}

	// Check if we already have dependencies set up.
	const auto it_other = Gaff::LowerBound(after._dependent_on_me, updateable._update_node);

	// We are already dependent on this entity.
	if (it_other != after._dependent_on_me.end() && *it_other == updateable._update_node) {
		// $TODO: Log warning.
		return;
	}

	const auto it_dep = Gaff::LowerBound(updateable._update_after, after._update_node);

	// We are already in the dependency list.
	if (it_dep != updateable._update_after.end() && *it_dep == after._update_node) {
		// $TODO: Log warning.
		return;
	}

	after._dependent_on_me.emplace(it_other, updateable._update_node);
	updateable._update_after.emplace(it_dep, after._update_node);

	// Find largest update depth.
	UpdateNode* const node_after = static_cast<UpdateNode*>(after._update_node);
	UpdateNode* const node = static_cast<UpdateNode*>(updateable._update_node);
	int16_t largest_depth = node_after->depth;

	// Not part of the same update phase.
	if (node->root != node_after->root) {
		// $TODO: Log error.
		return;
	}

	for (const void* dep_node : updateable._update_after) {
		UpdateNode* const dn = static_cast<UpdateNode*>(const_cast<void*>(dep_node));
		const int16_t depth = dn->depth;

		largest_depth = (depth > largest_depth) ? depth : largest_depth;
	}

	// We have a new depth.
	if ((largest_depth + 1) != node->depth) {
		node->depth = largest_depth + 1;
		markDirty(*node);
	}
}

void EntityManager::markDirty(UpdateNode& node, Gaff::Flags<UpdateNode::Flag> extra_flags)
{
	if (!node.flags.testAll(UpdateNode::Flag::Dirty)) {
		const int32_t update_phase_index = static_cast<int32_t>(getUpdatePhase(node));
		auto& dirty_nodes = _dirty_nodes[update_phase_index];

		const EA::Thread::AutoMutex lock(_dirty_nodes_locks[update_phase_index]);

		const auto it = Gaff::LowerBound(dirty_nodes, &node);
		GAFF_ASSERT(it == dirty_nodes.end() || *it != &node);

		node.flags.set(UpdateNode::Flag::Dirty);
		dirty_nodes.emplace(it, &node);
	}

	node.flags |= extra_flags;
}

EntityManager::UpdatePhase EntityManager::getUpdatePhase(const UpdateNode& node) const
{
	const UpdateNode* const root_begin = _update_roots;
	const UpdateNode* const node_root = node.root;

	const int32_t index = static_cast<int32_t>(eastl::distance(root_begin, node_root));
	return static_cast<UpdatePhase>(index);
}

void EntityManager::addToGraph(UpdateNode& node)
{
	GAFF_ASSERT(!node.prev_sibling);
	GAFF_ASSERT(!node.next_sibling);
	GAFF_ASSERT(!node.first_child);
	GAFF_ASSERT(!node.last_child);
	GAFF_ASSERT(!node.parent->parent);
	GAFF_ASSERT(node.root == node.parent);

	UpdateNode* new_parent = node.root;
	int16_t new_depth = 0;

	for (; new_parent->first_child && (new_depth < node.depth); ++new_depth) {
		new_parent = new_parent->first_child;
	}

	// We requested too deep a depth.
	if (new_depth < (node.depth - 1)) {
		// $TODO: Log warning.

		// Create a new row.
		node.depth = new_depth + 1;
	}

	node.parent = new_parent;

	// We are creating a new row.
	if (new_depth == (node.depth - 1)) {
		if (!new_parent->first_child) {
			new_parent->first_child = &node;
			new_parent->last_child = &node;
		}

	// We are inserting into an existing row.
	} else {
		node.prev_sibling = new_parent->last_child;
		new_parent->last_child->next_sibling = &node;
		new_parent->last_child = &node;
	}

	node.flags.clear(UpdateNode::Flag::Removed);
}

void EntityManager::removeFromGraph(UpdateNode& node)
{
	// We are the only element in our row.
	if (!node.prev_sibling && !node.next_sibling) {
		GAFF_ASSERT(node.parent->first_child == &node && node.parent->last_child == &node);

		node.parent->first_child = node.first_child;
		node.parent->last_child = node.last_child;

		// Set children to our parent.
		for (UpdateNode* child_node = node.first_child; child_node; child_node = child_node->next_sibling) {
			GAFF_ASSERT(child_node->parent == &node);
			child_node->parent = node.parent;
		}

	// We are the first child in the row.
	} else if (!node.prev_sibling) {
		GAFF_ASSERT(node.parent->first_child == &node);
		node.parent->first_child = node.next_sibling;

		// Next sibling is now the new parent for our row.
		GAFF_ASSERT(node.next_sibling->prev_sibling == &node);
		node.next_sibling->first_child = node.first_child;
		node.next_sibling->last_child = node.last_child;

		// Set children to use our sibling as their parent.
		for (UpdateNode* child_node = node.first_child; child_node; child_node = child_node->next_sibling) {
			GAFF_ASSERT(child_node->parent == &node);
			child_node->parent = node.next_sibling;
		}

	// We are the last child in the row.
	} else if (!node.next_sibling) {
		node.parent->last_child = node.prev_sibling;
		node.prev_sibling->next_sibling = node.next_sibling;

	// We're in the middle of a row.
	} else {
		node.prev_sibling->next_sibling = node.next_sibling;
		node.next_sibling->prev_sibling = node.prev_sibling;
	}

	node.prev_sibling = nullptr;
	node.next_sibling = nullptr;
	node.first_child = nullptr;
	node.last_child = nullptr;
	node.parent = node.root;

	node.flags.set(UpdateNode::Flag::Removed);
}

NS_END
