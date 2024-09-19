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

#define SHIB_REFL_IMPL
#include "Shibboleth_EntityComponent.h"
#include "Shibboleth_EntityManager.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EntityComponentFlag)
SHIB_REFLECTION_DEFINE_END(Shibboleth::EntityComponentFlag)

SHIB_REFLECTION_IMPL(Shibboleth::EntityComponent)

namespace
{
	static Shibboleth::ProxyAllocator s_allocator{ ENTITY_ALLOCATOR };
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(EntityComponent)

bool EntityComponent::init(void)
{
	return true;
}

void EntityComponent::destroy(void)
{
}

bool EntityComponent::clone(EntityComponent& new_component, const ISerializeReader* overrides) const
{
	const Refl::IReflectionDefinition& ref_def = getReflectionDefinition();
	GAFF_ASSERT(new_component.getReflectionDefinition().hasInterface(ref_def));

	bool success = true;

	// Copy all reflected variables.
	for (int32_t i = 0; i < ref_def.getNumVars(); ++i) {
		Refl::IReflectionVar* const ref_var = ref_def.getVar(i);

		if (ref_var->isNoCopy()) {
			continue;
		}

		const void* const orig_data = ref_var->getData(getBasePointer());
		ref_var->setData(new_component.getBasePointer(), orig_data);
	}

	if (overrides) {
		if (!ref_def.load(*overrides, new_component.getBasePointer(), Refl::IReflectionDefinition::LoadFlags::SparseData)) {
			// $TODO: Log error.
			success = false;
		}
	}

	return success;
}

bool EntityComponent::clone(EntityComponent*& new_component, const ISerializeReader* overrides) const
{
	new_component = getReflectionDefinition().template createT<EntityComponent>(s_allocator);

	if (!new_component) {
		// $TODO: Log error.
		return false;
	}

	return clone(*new_component, overrides);
}

Entity* EntityComponent::getOwner(void) const
{
	return _owner;
}

const HashString64<>& EntityComponent::getName(void) const
{
	return _name;
}

void EntityComponent::setName(const HashString64<>& name)
{
	_name = name;
}

void EntityComponent::setName(HashString64<>&& name)
{
	_name = std::move(name);
}

void EntityComponent::setName(const HashStringView64<>& name)
{
	_name = name;
}

void EntityComponent::setName(const U8String& name)
{
	_name = name;
}

NS_END
