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
#include <Attributes/Shibboleth_EngineAttributesCommon.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::Entity)
	// $TODO: Custom serialization for setting up scene component hierarchy.

	.template ctor<>()

	.var("root_scene_component", &Shibboleth::Entity::_root_scene_comp, Shibboleth::ReadOnlyAttribute{})
	.var("components", &Shibboleth::Entity::_components, Shibboleth::OptionalAttribute{})
SHIB_REFLECTION_DEFINE_END(Shibboleth::Entity)


namespace
{
	static Shibboleth::ProxyAllocator s_allocator{ ENTITY_ALLOCATOR };
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(Entity)

bool Entity::init(void)
{
	bool success = true;

	for (auto& comp : _components) {
		if (!comp.init()) {
			// $TODO: Log error.
			success = false;
		}
	}

	return success;
}

bool Entity::clone(Entity& new_entity, const ISerializeReader* overrides) const
{
	const Refl::IReflectionDefinition& ref_def = getReflectionDefinition();
	GAFF_ASSERT(new_entity.getReflectionDefinition().hasInterface(ref_def));

	bool success = true;


	// Copy all reflected variables.
	for (int32_t i = 0; i < ref_def.getNumVars(); ++i) {
		Refl::IReflectionVar* const ref_var = ref_def.getVar(i);

		if (ref_var->isNoCopy()) {
			continue;
		}

		const void* const orig_data = ref_var->getData(getBasePointer());
		ref_var->setData(new_entity.getBasePointer(), orig_data);
	}

	if (overrides) {
		overrides->enterElement(u8"modify_entity");

		if (!overrides->isObject()) {
			// $TODO: Log error.
			success = false;
		}

		for (int32_t i = 0; i < ref_def.getNumVars(); ++i) {
			Refl::IReflectionVar* const ref_var = ref_def.getVar(i);
			const HashStringView32<> var_name = ref_def.getVarName(i);

			overrides->enterElement(var_name.getBuffer());

			if (overrides->isNull()) {
				continue;
			}

			void* const new_data = ref_var->getData(new_entity.getBasePointer());

			if (!ref_var->getReflection().getReflectionDefinition().load(*overrides, new_data, Refl::IReflectionDefinition::LoadFlags::SparseData)) {
				success = false;
			}

			overrides->exitElement();
		}

		overrides->exitElement();
	}


	// Clone all components.
	Vector<Gaff::Hash64> remove_components{ ENTITY_ALLOCATOR };

	if (overrides) {
		overrides->enterElement(u8"remove_components");

		if (overrides->isArray()) {
			remove_components.reserve(static_cast<size_t>(overrides->size()));

			overrides->forEachInArray([&](int32_t) -> bool
			{
				if (!overrides->isString()) {
					// $TODO: Log error.
					return false;
				}

				const char8_t* const component_name = overrides->readString();

				remove_components.emplace_back(Gaff::FNV1aHash64String(component_name));

				overrides->freeString(component_name);

				return false;
			});

		} else {
			// $TODO: Log error.
			success = false;
		}

		overrides->exitElement();

		overrides->enterElement(u8"modify_components");
	}

	for (auto& comp : _components) {
		if (Gaff::Contains(remove_components, comp.getName().getHash())) {
			continue;
		}

		if (overrides) {
			overrides->enterElement(comp.getName().getBuffer());
		}

		EntityComponent& component = new_entity._components.push(comp.getReflectionDefinition());
		comp.clone(component, (overrides && !overrides->isNull()) ? overrides : nullptr);

		if (overrides) {
			overrides->exitElement();
		}
	}

	if (overrides) {
		overrides->exitElement();
	}


	// Set up scene components.
	for (int32_t i = 0; i < _components.size(); ++i) {
		const EntityComponent* const comp = _components[i];
		const EntitySceneComponent* const scene_comp = comp->getReflectionDefinition().getInterface<EntitySceneComponent>(comp->getBasePointer());

		if (!scene_comp) {
			continue;
		}

		EntitySceneComponent* const new_scene_comp = static_cast<EntitySceneComponent*>(new_entity._components[i]);

		if (scene_comp->_parent) {
			EntitySceneComponent* const new_parent = static_cast<EntitySceneComponent*>(new_entity.findComponent(scene_comp->_parent->getName()));
			new_scene_comp->_parent = new_parent;

		// We are the root scene component.
		} else {
			if (_root_scene_comp) {
				// $TODO: Log error.
			} else {
				new_entity._root_scene_comp = static_cast<EntitySceneComponent*>(new_entity._components[i]);
			}
		}
	}

	if (new_entity._root_scene_comp) {
		new_entity._root_scene_comp->updateToWorld();
	}

	return success;
}

bool Entity::clone(Entity*& new_entity, const ISerializeReader* overrides) const
{
	new_entity = getReflectionDefinition().template createT<Entity>(s_allocator);

	if (!new_entity) {
		// $TODO: Log error.
		return false;
	}

	return clone(*new_entity, overrides);
}

void Entity::start(void)
{
}

void Entity::end(void)
{
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

	EntityComponent& comp = _components.push(ref_def);

	comp._owner = this;
	comp.init();

	return &comp;
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
	for (int32_t i = 0; i < _components.size(); ++i) {
		EntityComponent* const comp = _components[i];

		if (&comp->getReflectionDefinition() == &ref_def) {
			removeComponent(i);
			return true;
		}
	}

	return false;
}

void Entity::removeComponent(int32_t index)
{
	GAFF_ASSERT(Gaff::ValidIndex(index, _components.size()));
	_components[index]->destroy();
	_components.erase(index);
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

	for (int32_t i = 0; i < _components.size();) {
		if (&_components[i]->getReflectionDefinition() == &ref_def) {
			_components[i]->destroy();
			_components.erase(i);
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
		if (&comp.getReflectionDefinition() == &ref_def) {
			comps.emplace_back(&comp);
		}
	}

	comps.shrink_to_fit();
	return comps;
}

Vector<EntityComponent*> Entity::getComponents(const Refl::IReflectionDefinition& ref_def)
{
	Vector<EntityComponent*> comps;

	for (auto& comp : _components) {
		if (&comp.getReflectionDefinition() == &ref_def) {
			comps.emplace_back(&comp);
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
	for (auto& comp : _components) {
		if (&comp.getReflectionDefinition() == &ref_def) {
			return &comp;
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
	GAFF_ASSERT(Gaff::ValidIndex(index, _components.size()));
	return *_components[index];
}

const EntityComponent* Entity::findComponent(const HashString64<>& name) const
{
	return const_cast<Entity*>(this)->findComponent(name);
}

EntityComponent* Entity::findComponent(const HashString64<>& name)
{
	return findComponent(name.getHash());
}

const EntityComponent* Entity::findComponent(const Gaff::Hash64& name) const
{
	return const_cast<Entity*>(this)->findComponent(name);
}

EntityComponent* Entity::findComponent(const Gaff::Hash64& name)
{
	for (auto& comp : _components) {
		if (comp.getName().getHash() == name) {
			return &comp;
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
	return _components.size();
}

const EntitySceneComponent* Entity::getRootComponent(void) const
{
	return _root_scene_comp;
}

EntitySceneComponent* Entity::getRootComponent(void)
{
	return _root_scene_comp;
}

const HashString64<>& Entity::getName(void) const
{
	return _name;
}

void Entity::setName(const HashStringView64<>& name)
{
	_name = name;
}

void Entity::setName(HashString64<>&& name)
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
