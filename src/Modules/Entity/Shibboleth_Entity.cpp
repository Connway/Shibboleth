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

#include "Shibboleth_Entity.h"
#include "Shibboleth_EntityManager.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::Entity)
	.template ctor<Shibboleth::EntityManager&>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::Entity)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(Entity)

Entity::Entity(EntityManager& entity_mgr):
	_entity_mgr(entity_mgr)
{
}

bool Entity::init(void)
{
	for (auto& comp : _components) {
		if (!comp->init()) {
			// $TODO: Log error.
			return false;
		}
	}

	return true;
}

void Entity::update(float dt)
{
	GAFF_REF(dt);
}

void Entity::addComponent(const Vector<const Refl::IReflectionDefinition*>& ref_defs)
{
	for (const Refl::IReflectionDefinition* ref_def : ref_defs) {
		addComponent(*ref_def);
	}
}

void Entity::addComponent(const Refl::IReflectionDefinition& ref_def)
{
	GAFF_ASSERT(ref_def.hasInterface(CLASS_HASH(Shibboleth::EntityComponent)));
	GAFF_ASSERT(ref_def.getFactory<>());

	static ProxyAllocator s_allocator("Entity");
	EntityComponent* const comp = ref_def.createT<EntityComponent>(CLASS_HASH(Shibboleth::EntityComponent), s_allocator);

	addComponent(*comp);
}

void Entity::addComponent(const Vector<EntityComponent*>& components)
{
	for (EntityComponent* comp : components) {
		addComponent(*comp);
	}
}

void Entity::addComponent(EntityComponent& component)
{
	_components.emplace_back(&component);
	component._owner = this;
	component.init();
}

int32_t Entity::removeComponent(const Vector<const Refl::IReflectionDefinition*>& ref_defs)
{
	int32_t count = 0;

	for (const Refl::IReflectionDefinition* ref_def : ref_defs) {
		if (removeComponent(*ref_def)) {
			++count;
		}
	}

	return count;
}

bool Entity::removeComponent(const Refl::IReflectionDefinition& ref_def)
{
	for (auto& comp : _components) {
		if (&comp->getReflectionDefinition() == &ref_def) {
			const int32_t index = static_cast<int32_t>(eastl::distance(_components.begin(), &comp));
			removeComponent(index);

			return true;
		}
	}

	return false;
}

void Entity::removeComponent(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_components.size()));
	_components[index]->destroy();
	_components.erase_unsorted(_components.begin() + index);
}

int32_t Entity::removeComponents(const Vector<const Refl::IReflectionDefinition*>& ref_defs)
{
	int32_t count = 0;

	for (const Refl::IReflectionDefinition* ref_def : ref_defs) {
		count += removeComponents(*ref_def);
	}

	return count;
}

int32_t Entity::removeComponents(const Refl::IReflectionDefinition& ref_def)
{
	int32_t count = 0;

	for (int32_t i = 0; i < static_cast<int32_t>(_components.size());) {
		if (&_components[i]->getReflectionDefinition() == &ref_def) {
			_components[i]->destroy();
			_components.erase_unsorted(_components.begin() + i);
			++count;

		} else {
			++i;
		}
	}

	return count;
}

Vector<const EntityComponent*> Entity::getComponents(const Refl::IReflectionDefinition& ref_def) const
{
	Vector<const EntityComponent*> comps;

	for (const auto& comp : _components) {
		if (&comp->getReflectionDefinition() == &ref_def) {
			comps.emplace_back(comp.get());
		}
	}

	comps.shrink_to_fit();
	return comps;
}

Vector<EntityComponent*> Entity::getComponents(const Refl::IReflectionDefinition& ref_def)
{
	Vector<EntityComponent*> comps;

	for (const auto& comp : _components) {
		if (&comp->getReflectionDefinition() == &ref_def) {
			comps.emplace_back(comp.get());
		}
	}

	comps.shrink_to_fit();
	return comps;
}

const EntityComponent* Entity::getComponent(const Refl::IReflectionDefinition& ref_def) const
{
	return const_cast<Entity*>(this)->getComponent(ref_def);
}

EntityComponent* Entity::getComponent(const Refl::IReflectionDefinition& ref_def)
{
	for (const auto& comp : _components) {
		if (&comp->getReflectionDefinition() == &ref_def) {
			return comp.get();
		}
	}

	return nullptr;
}

const EntityComponent& Entity::getComponent(int32_t index) const
{
	return const_cast<Entity*>(this)->getComponent(index);
}

EntityComponent& Entity::getComponent(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_components.size()));
	return *_components[index];
}

bool Entity::hasComponent(const Refl::IReflectionDefinition& ref_def) const
{
	return getComponent(ref_def);
}

int32_t Entity::getNumComponents(void) const
{
	return static_cast<int32_t>(_components.size());
}

EntityID Entity::getID(void) const
{
	return _id;
}

void Entity::setID(EntityID id)
{
	_id = id;
}

void Entity::updateAfter(Entity& entity)
{
	const auto it_other = Gaff::LowerBound(entity._dependent_on_me_entities, _id);

	// We are already dependent on this entity.
	if (it_other != entity._dependent_on_me_entities.end() && *it_other == _id) {
		// $TODO: Log error.
		return;
	}

	const EntityID id = entity.getID();
	const auto it_dep = Gaff::LowerBound(_update_after_entities, id);

	// We are already in the dependency list.
	if (it_dep != _update_after_entities.end() && *it_dep == id) {
		// $TODO: Log warning.
		return;
	}

	entity._dependent_on_me_entities.emplace(it_other, _id);
	_update_after_entities.emplace(it_dep, id);

	_entity_mgr.updateAfter(*this, entity);
}

void Entity::setEnableUpdate(bool enabled)
{
	_flags.set(enabled, Flag::UpdateEnabled);
}

bool Entity::canUpdate(void) const
{
	return _flags.testAll(Flag::UpdateEnabled);
}

NS_END
