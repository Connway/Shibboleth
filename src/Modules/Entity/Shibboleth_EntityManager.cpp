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

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EntityManager)
	.base<Shibboleth::IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::EntityManager)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(EntityManager)

static ProxyAllocator s_allocator("Entity");

//EntityManager::~EntityManager(void);

//bool EntityManager::initAllModulesLoaded(void) override;

Entity* EntityManager::createEntity(const Refl::IReflectionDefinition& ref_def)
{
	GAFF_ASSERT(ref_def.getFactory<EntityManager&>());
	GAFF_ASSERT(ref_def.hasInterface<Entity>());

	Entity* const entity = ref_def.createT<Entity>(s_allocator, *this);

	if (entity) {
		// $TODO: Add to world and manage newly created entity.
		const EntityID id = allocateEntityID();
		entity->setID(id);

		if (id >= static_cast<int32_t>(_entities.size())) {
			constexpr int32_t k_entity_padding = 128;

			const EA::Thread::AutoRWSpinLock lock(_entities_lock, EA::Thread::AutoRWSpinLock::kLockTypeWrite);
			_entities.resize(id + k_entity_padding);
		}

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

		// Set lock to read, because we are not actually changing the size of _entities. We are only writing to our owned slot.
		const EA::Thread::AutoRWSpinLock lock(_entities_lock, EA::Thread::AutoRWSpinLock::kLockTypeRead);
		_entities[id].reset(node);

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

void EntityManager::destroyComponent(EntityComponent& comp)
{
	destroyComponent(comp.getID());
}

void EntityManager::destroyComponent(EntityComponentID id)
{
	// Set lock to read, because we are not actually changing the size of _components. We are only writing to owned slots.
	_components_lock.ReadLock();

	GAFF_ASSERT(_components[id]);
	UpdateNode* const node = _components[id].get();

	_components_lock.ReadUnlock();

	markDirty(*node, Gaff::Flags(UpdateNode::Flag::Destroy));

	EntityComponent* const comp = static_cast<EntityComponent*>(node->updater);
	markDependentsDirty(comp->_dependent_on_me_components, comp->_dependent_on_me_entities);
}

void EntityManager::destroyEntity(Entity& entity)
{
	destroyEntity(entity.getID());
}

void EntityManager::destroyEntity(EntityID id)
{
	// Set lock to read, because we are not actually changing the size of _entities. We are only writing to owned slots.
	_entities_lock.ReadLock();

	GAFF_ASSERT(_entities[id]);
	UpdateNode* const node = _entities[id].get();

	_entities_lock.ReadUnlock();

	markDirty(*node, Gaff::Flags(UpdateNode::Flag::Destroy));

	Entity* const entity = static_cast<Entity*>(node->updater);
	markDependentsDirty(entity->_dependent_on_me_components, entity->_dependent_on_me_entities);
}

void EntityManager::updateEntitiesAndComponents(UpdatePhase update_phase)
{
	// Handle all dirty nodes for this update phase.
	auto& dirty_nodes = _dirty_nodes[static_cast<size_t>(update_phase)];
	const EA::Thread::AutoMutex lock_dirty(_dirty_nodes_locks[static_cast<size_t>(update_phase)]);

	for (UpdateNode* node : dirty_nodes) {
		const EA::Thread::AutoRWMutex lock_node(
			_graph_locks[static_cast<size_t>(getUpdatePhase(*node))],
			EA::Thread::RWMutex::kLockTypeWrite
		);

		if (!node->flags.testAll(UpdateNode::Flag::Removed)) {
			removeFromGraph(*node);
		}

		if (node->flags.testAll(UpdateNode::Flag::Destroy)) {
			if (node->flags.testAll(UpdateNode::Flag::Component)) {
				const EntityComponentID id = static_cast<EntityComponent*>(node->updater)->getID();

				_components[id] = nullptr;
				returnComponentID(id);

			} else {
				const EntityID id = static_cast<Entity*>(node->updater)->getID();
				SHIB_FREET(node->updater->getBasePointer(), s_allocator);

				_entities[id] = nullptr;
				returnEntityID(id);
			}

		} else {
			addToGraph(*node);
		}

		node->flags.clear(UpdateNode::Flag::Dirty);
	}

	dirty_nodes.clear();

	// $TODO: Update all nodes for this phase.
}

void EntityManager::changeDefaultUpdatePhase(const Entity& entity, UpdatePhase update_phase)
{
	// Ensure we have not already set dependencies.
	GAFF_ASSERT(entity._update_after_components.empty() && entity._dependent_on_me_components.empty());
	GAFF_ASSERT(entity._update_after_entities.empty() && entity._dependent_on_me_entities.empty());

	_entities_lock.ReadLock();

	GAFF_ASSERT(_entities[entity.getID()]);
	UpdateNode* const node = _entities[entity.getID()].get();

	_entities_lock.ReadUnlock();

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

void EntityManager::markDependentsDirty(const Vector<EntityComponentID>& dep_comps, const Vector<EntityID>& dep_ents)
{
	Vector<UpdateNode*> dep_nodes;
	dep_nodes.reserve(dep_comps.size() + dep_ents.size());

	if (!dep_comps.empty()) {
		// Set lock to read, because we are not actually changing the size of _components. We are only writing to owned slots.
		const EA::Thread::AutoRWSpinLock lock(_components_lock, EA::Thread::AutoRWSpinLock::kLockTypeRead);

		for (EntityID dep_id : dep_comps) {
			GAFF_ASSERT(_components[dep_id]);
			dep_nodes.push_back(_components[dep_id].get());
		}
	}


	if (!dep_ents.empty()) {
		// Set lock to read, because we are not actually changing the size of _entities. We are only writing to owned slots.
		const EA::Thread::AutoRWSpinLock lock(_entities_lock, EA::Thread::AutoRWSpinLock::kLockTypeRead);

		for (EntityID dep_id : dep_ents) {
			GAFF_ASSERT(_entities[dep_id]);
			dep_nodes.push_back(_entities[dep_id].get());
		}
	}

	for (UpdateNode* node : dep_nodes) {
		markDirty(*node);
	}
}

void EntityManager::updateAfter(Entity& entity, const Entity& after)
{
	GAFF_ASSERT(_entities[entity.getID()]);
	GAFF_ASSERT(_entities[after.getID()]);

	_entities_lock.ReadLock();

	GAFF_ASSERT(_entities[after.getID()]);

	UpdateNode* const node = _entities[entity.getID()].get();
	int16_t largest_depth = _entities[after.getID()]->depth;

	// Check entities.
	for (EntityID id : entity._update_after_entities) {
		if (id == after.getID()) {
			continue;
		}

		GAFF_ASSERT(_entities[id]);

		const int16_t depth = _entities[id]->depth;
		largest_depth = (depth > largest_depth) ? depth : largest_depth;
	}

	_entities_lock.ReadUnlock();

	// Check components.
	if (!entity._update_after_components.empty()) {
		_components_lock.ReadLock();

		for (EntityComponentID id : entity._update_after_components) {
			GAFF_ASSERT(_components[id]);

			const int16_t depth = _components[id]->depth;
			largest_depth = (depth > largest_depth) ? depth : largest_depth;
		}

		_components_lock.ReadUnlock();
	}

	// We have a new depth.
	if ((largest_depth + 1) != node->depth) {
		node->depth = largest_depth + 1;
		markDirty(*node);
	}
}

EntityManager::UpdatePhase EntityManager::getUpdatePhase(const UpdateNode& node) const
{
	const UpdateNode* const root_begin = _update_roots;
	const UpdateNode* const node_root = node.root;

	const int32_t index = static_cast<int32_t>(eastl::distance(root_begin, node_root));
	return static_cast<UpdatePhase>(index);
}

int16_t EntityManager::getDepth(const Entity& entity) const
{
	const EA::Thread::AutoRWSpinLock lock(_entities_lock, EA::Thread::AutoRWSpinLock::kLockTypeRead);

	GAFF_ASSERT(_entities[entity.getID()]);
	return _entities[entity.getID()]->depth;
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

void EntityManager::returnComponentID(EntityComponentID id)
{
	const EA::Thread::AutoMutex lock(_free_ids_components_lock);

	// Sort array from highest to lowest.
	const auto it = Gaff::UpperBound(_free_ids_components, id);
	GAFF_ASSERT(it == _free_ids_components.end() || (*it) != id);
	_free_ids_components.emplace(it, id);
}

EntityComponentID EntityManager::allocateComponentID(void)
{
	const EA::Thread::AutoMutex lock(_free_ids_components_lock);

	if (!_free_ids_components.empty()) {
		const EntityComponentID id = _free_ids_components.back();
		_free_ids_components.pop_back();

		return id;
	}

	return _next_id_component++;
}

void EntityManager::returnEntityID(EntityID id)
{
	const EA::Thread::AutoMutex lock(_free_ids_entities_lock);

	// Sort array from highest to lowest.
	const auto it = Gaff::UpperBound(_free_ids_entities, id);
	GAFF_ASSERT(it == _free_ids_entities.end() || (*it) != id);
	_free_ids_entities.emplace(it, id);
}

EntityID EntityManager::allocateEntityID(void)
{
	const EA::Thread::AutoMutex lock(_free_ids_entities_lock);

	if (!_free_ids_entities.empty()) {
		const EntityID id = _free_ids_entities.back();
		_free_ids_entities.pop_back();

		return id;
	}

	return _next_id_entity++;
}

NS_END
