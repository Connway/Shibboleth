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

#include "Shibboleth_EntityComponent.h"
#include "Shibboleth_EntityManager.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EntityComponentFlag)
	.entry("Update Enabled", Shibboleth::EntityComponentFlag::UpdateEnabled)
SHIB_REFLECTION_DEFINE_END(Shibboleth::EntityComponentFlag)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EntityComponent)
	.BASE(Shibboleth::IEntityUpdateable)

	.var("flags", &Shibboleth::EntityComponent::_flags)
	.var("name", &Shibboleth::EntityComponent::_name)

	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::EntityComponent)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(EntityComponent)

bool EntityComponent::init(void)
{
	return true;
}

bool EntityComponent::clone(EntityComponent*& new_component, const ISerializeReader* overrides)
{
	const Refl::IReflectionDefinition& ref_def = getReflectionDefinition();

	new_component = static_cast<EntityComponent*>(_owner->_entity_mgr.createUpdateable(ref_def));

	if (!new_component) {
		// $TODO: Log error.
		return false;
	}

	// Copy all reflected variables.
	for (int32_t i = 0; i < ref_def.getNumVars(); ++i) {
		Refl::IReflectionVar* const ref_var = ref_def.getVar(i);

		if (ref_var->isNoCopy()) {
			continue;
		}

		const void* const orig_data = ref_var->getData(getBasePointer());
		ref_var->setData(new_component->getBasePointer(), orig_data);
	}

	if (overrides) {
		bool success = true;

		if (!ref_def.load(*overrides, getBasePointer())) {
			// $TODO: Log error.
			success = false;
		}

		if (!success) {
			return false;
		}
	}

	return new_component != nullptr;
}

void EntityComponent::destroy(void)
{
}

Entity* EntityComponent::getOwner(void) const
{
	return _owner;
}

void EntityComponent::updateAfter(IEntityUpdateable& after)
{
	GAFF_ASSERT(_owner);
	_owner->_entity_mgr.updateAfter(*this, after);
}

void EntityComponent::setEnableUpdate(bool enabled)
{
	_flags.set(enabled, EntityComponentFlag::UpdateEnabled);
}

bool EntityComponent::canUpdate(void) const
{
	return _flags.testAll(EntityComponentFlag::UpdateEnabled);
}

const U8String& EntityComponent::getName(void) const
{
	return _name;
}

void EntityComponent::setName(const U8String& name)
{
	_name = name;
}

void EntityComponent::setName(U8String&& name)
{
	_name = std::move(name);
}

NS_END
