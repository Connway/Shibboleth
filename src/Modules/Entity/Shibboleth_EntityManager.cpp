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
		const EntityID id = allocateID();
		entity->setID(id);

		if (id >= static_cast<int32_t>(_entities.size())) {
			constexpr int32_t k_entity_padding = 128;
			_entities.resize(id + k_entity_padding);
		}

		UpdateNode* const node = SHIB_ALLOCT(UpdateNode, s_allocator);
		GAFF_ASSERT(node);

		// Default to DuringPhysics update phase.
		node->parent = &_update_roots[static_cast<size_t>(UpdatePhase::DuringPhysics)];
		node->updater = entity;

		_entities[id].reset(node);

	} else {
		// $TODO: Log error.
	}

	return entity;
}

Entity* EntityManager::createEntity(void)
{
	return createEntity(Refl::Reflection<Entity>::GetReflectionDefinition());
}

void EntityManager::destroyEntity(Entity& entity)
{
	destroyEntity(entity.getID());
}

void EntityManager::destroyEntity(EntityID id)
{
	GAFF_ASSERT(Gaff::UpperBound(_free_ids, id) == _free_ids.end() || *Gaff::UpperBound(_free_ids, id) != id);
	GAFF_ASSERT(_entities[id]);

	UpdateNode* const node = _entities[id].get();

	SHIB_FREET(node->updater->getBasePointer(), s_allocator);

	removeFromGraph(*node);

	_entities[id] = nullptr;
	returnID(id);
}

void EntityManager::updateEntitiesAndComponents(UpdatePhase update_phase)
{
	GAFF_REF(update_phase);
}

void EntityManager::changeDefaultUpdatePhase(const Entity& entity, UpdatePhase update_phase)
{
	// Ensure we have not already set dependencies.
	GAFF_ASSERT(entity._update_after.empty() && entity._entities_dependent_on_me.empty());
	GAFF_ASSERT(_entities[entity.getID()]);

	UpdateNode* const node = _entities[entity.getID()].get();
	GAFF_ASSERT(!node->parent->parent); // Make sure that we are attached to a root node.
	GAFF_ASSERT(node->parent->last_child == node); // We should be doing this during initialization, so we are the last node in the list.

	removeFromGraph(*node);

	UpdateNode& root = _update_roots[static_cast<size_t>(update_phase)];
	root.last_child->next_sibling = node;
	node->prev_sibling = root.last_child;
	root.last_child = node;
}

void EntityManager::markDirty(Entity& entity)
{
	GAFF_ASSERT(_entities[entity.getID()]);

	// Already added to the dirty list.
	if (entity._flags.testAll(Entity::Flag::UpdateNodeDirty)) {
		return;
	}

	entity._flags.set(Entity::Flag::UpdateNodeDirty);

	UpdateNode* const node = _entities[entity.getID()].get();
	const auto it = Gaff::LowerBound(_dirty_nodes, node);

	GAFF_ASSERT(it == _dirty_nodes.end() || *it != node);
	_dirty_nodes.emplace(it, node);
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
}

void EntityManager::returnID(EntityID id)
{
	// Sort array from highest to lowest.
	const auto it = Gaff::UpperBound(_free_ids, id);
	GAFF_ASSERT(it == _free_ids.end() || (*it) != id);
	_free_ids.emplace(it, id);
}

EntityID EntityManager::allocateID(void)
{
	if (!_free_ids.empty()) {
		const EntityID id = _free_ids.back();
		_free_ids.pop_back();

		return id;
	}

	return _next_id++;
}

NS_END
