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

#include "Shibboleth_LayerResource.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_Utilities.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::LayerEntityData)
	.serialize(nullptr, &Shibboleth::LayerEntityData::Save)

	// .var("entity_definition", &Type::entity_definition)
	.var("entity_base", &Type::entity_base)
SHIB_REFLECTION_DEFINE_END(Shibboleth::LayerEntityData)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::LayerResource)
	.classAttrs(
		Shibboleth::ResourceExtensionAttribute(u8".layer.bin"),
		Shibboleth::ResourceExtensionAttribute(u8".layer")
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()

	.var("entities", &Type::_entities)
SHIB_REFLECTION_DEFINE_END(Shibboleth::LayerResource)


namespace
{
	static Shibboleth::ProxyAllocator s_allocator{ SCENE_ALLOCATOR };
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(LayerResource)

void LayerEntityData::Save(ISerializeWriter& writer, const LayerEntityData& instance)
{
	const auto& ref_def = Refl::Reflection<LayerEntityData>::GetReflectionDefinition();

	ref_def.save(writer, instance, Refl::IReflectionDefinition::SaveFlags::ReflectionSave);
	// $TODO: Write delta of entity_base and entity_definition in "overrides" field.
}

bool LayerEntityData::postLoad(const ISerializeReader& reader, InstancedArray<Entity>& instances)
{
	const auto overrides_guard = reader.enterElementGuard(u8"overrides");

	if (reader.isNull()) {
		return true;
	}

	if (!reader.isObject()) {
		// $TODO: Log error.
		return false;
	}

	const Entity* const base_entity_definition = entity_base->getDefinition();

	if (!base_entity_definition) {
		// $TODO: Log error.
		return false;
	}

	Entity& new_entity_definition = instances.push(base_entity_definition->getReflectionDefinition());

	if (!base_entity_definition->clone(new_entity_definition, &reader)) {
		instances.pop();
		return false;
	}

	return true;
}



const VectorMap<HashString64<>, LayerEntityData>& LayerResource::getEntityData(void) const
{
	return _entities;
}

const InstancedArray<Entity>& LayerResource::getEntityDefinitions(void) const
{
	return _entity_definitions;
}

InstancedArray<Entity>&& LayerResource::releaseEntityDefinitions(void)
{
	return std::move(_entity_definitions);
}

void LayerResource::dependenciesLoaded(const ISerializeReader& reader, uintptr_t thread_id_int)
{
	if (haveDependenciesSucceeded()) {
		const auto entity_guard = reader.enterElementGuard(u8"entities");
		bool success = true;
		int32_t index = 0;

		for (auto& entry : _entities) {
			const auto element_guard = reader.enterElementGuard(index);

			if (!entry.second.postLoad(reader, _entity_definitions)) {
				success = false;
			}

			++index;
		}

		if (!success) {
			failed();
		}
	}

	IResource::dependenciesLoaded(reader, thread_id_int);
}

NS_END
