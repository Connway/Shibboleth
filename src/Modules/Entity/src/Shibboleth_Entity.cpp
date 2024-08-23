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

#include "Shibboleth_Entity.h"
#include "Shibboleth_EntitySceneComponent.h"
#include "Shibboleth_EntityManager.h"
#include "Shibboleth_EntityDefines.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EntityFlag)
SHIB_REFLECTION_DEFINE_END(Shibboleth::EntityFlag)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::Entity)
	.template ctor<Shibboleth::EntityManager&>()

	.var("flags", &Shibboleth::Entity::_flags)
	.var("name", &Shibboleth::Entity::_name)
SHIB_REFLECTION_DEFINE_END(Shibboleth::Entity)


namespace
{
	static Shibboleth::ProxyAllocator s_allocator{ ENTITY_ALLOCATOR };
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(Entity)

Entity::Entity(EntityManager& entity_mgr):
	_entity_mgr(entity_mgr)
{
}

bool Entity::init(void)
{
	bool success = true;

	for (auto& comp : _components) {
		if (!comp->init()) {
			// $TODO: Log error.
			success = false;
		}
	}

	return success;
}

bool Entity::clone(Entity*& new_entity, const ISerializeReader* overrides)
{
	const Refl::IReflectionDefinition& ref_def = getReflectionDefinition();

	new_entity = ref_def.template createT<Entity>(s_allocator);

	if (!new_entity) {
		// $TODO: Log error.
		return false;
	}

	// $TODO: Offer a path for calling a copy function instead.

	// Copy all reflected variables.
	for (int32_t i = 0; i < ref_def.getNumVars(); ++i) {
		Refl::IReflectionVar* const ref_var = ref_def.getVar(i);

		if (ref_var->isNoCopy()) {
			continue;
		}

		const void* const orig_data = ref_var->getData(getBasePointer());
		ref_var->setData(new_entity->getBasePointer(), orig_data);
	}

	bool success = true;

	new_entity->_components.reserve(_components.size());

	// Clone all components
	for (auto& comp : _components) {
		if (overrides) {
			overrides->enterElement(comp->getName().data());
		}

		EntityComponent* component = nullptr;

		if (!comp->clone(component, (overrides && overrides->isObject()) ? overrides : nullptr)) {
			// $TODO: Log error.
			success = false;
		}

		new_entity->_components.emplace_back(component);

		if (overrides) {
			overrides->exitElement();
		}
	}

	// Set up scene components.
	for (int32_t i = 0; i < static_cast<int32_t>(_components.size()); ++i) {
		auto& comp = _components[i];

		EntitySceneComponent* const scene_comp = comp->getReflectionDefinition().getInterface<EntitySceneComponent>(comp->getBasePointer());

		if (!scene_comp) {
			continue;
		}

		EntitySceneComponent* const new_scene_comp = static_cast<EntitySceneComponent*>(new_entity->_components[i].get());

		if (scene_comp->_parent) {
			EntitySceneComponent* const new_parent = static_cast<EntitySceneComponent*>(new_entity->findComponent(scene_comp->_parent->getName()));
			new_scene_comp->_parent = new_parent;

		// We are the root scene component.
		} else {
			if (_root_scene_comp) {
				// $TODO: Log error.
			} else {
				new_entity->_root_scene_comp = static_cast<EntitySceneComponent*>(new_entity->_components[i].get());
			}
		}
	}

	if (new_entity->_root_scene_comp) {
		new_entity->_root_scene_comp->updateToWorld();
	}

	return success;
}

void Entity::addToWorld(void)
{
	// _world = world;
	// broadcast event
}

void Entity::removeFromWorld(void)
{
	// broadcast event
}

void Entity::addComponent(const Vector<const Refl::IReflectionDefinition*>& ref_defs)
{
	for (const Refl::IReflectionDefinition* ref_def : ref_defs) {
		addComponent(*ref_def);
	}
}

EntityComponent* Entity::addComponent(const Refl::IReflectionDefinition& ref_def)
{
	GAFF_ASSERT(ref_def.hasInterface(CLASS_HASH(Shibboleth::EntityComponent)));
	GAFF_ASSERT(ref_def.getFactory<>());

	static ProxyAllocator s_allocator("Entity");
	EntityComponent* const comp = ref_def.createT<EntityComponent>(CLASS_HASH(Shibboleth::EntityComponent), s_allocator);

	if (comp) {
		addComponent(*comp);
	} else {
		// $TODO: Log error.
	}

	return comp;
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
	GAFF_ASSERT(Gaff::ValidIndex(index, static_cast<int32_t>(_components.size())));
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
	GAFF_ASSERT(Gaff::ValidIndex(index, static_cast<int32_t>(_components.size())));
	return *_components[index];
}

const EntityComponent* Entity::findComponent(const U8String& name) const
{
	return const_cast<Entity*>(this)->findComponent(name);
}

EntityComponent* Entity::findComponent(const U8String& name)
{
	for (auto& comp : _components) {
		if (comp->getName() == name) {
			return comp.get();
		}
	}

	return nullptr;
}

bool Entity::hasComponent(const Refl::IReflectionDefinition& ref_def) const
{
	return getComponent(ref_def);
}

int32_t Entity::getNumComponents(void) const
{
	return static_cast<int32_t>(_components.size());
}

const EntitySceneComponent* Entity::getRootComponent(void) const
{
	return _root_scene_comp;
}

EntitySceneComponent* Entity::getRootComponent(void)
{
	return _root_scene_comp;
}

const U8String& Entity::getName(void) const
{
	return _name;
}

void Entity::setName(const U8String& name)
{
	_name = name;
}

void Entity::setName(U8String&& name)
{
	_name = std::move(name);
}

const EntityUpdater& Entity::getUpdater(void) const
{
	return _updater;
}

EntityUpdater& Entity::getUpdater(void)
{
	return _updater;
}

NS_END
