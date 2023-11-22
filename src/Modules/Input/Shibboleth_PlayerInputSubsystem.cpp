/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_PlayerInputSubsystem.h"
#include "Shibboleth_InputMappingResource.h"
#include "Shibboleth_InputManager.h"
#include <Shibboleth_AppUtils.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::PlayerInputSubsystem)
	.template base<Shibboleth::LocalPlayerSubsystem>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::PlayerInputSubsystem)


namespace
{
	Shibboleth::ProxyAllocator g_allocator("Input");
}


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(PlayerInputSubsystem)


void PlayerInputSubsystem::init(const SubsystemCollectorBase& /*collector*/)
{
}

void PlayerInputSubsystem::destroy(const SubsystemCollectorBase& /*collector*/)
{
}

void PlayerInputSubsystem::addInputMapping(const InputMappingResource& input_mapping, int32_t priority)
{
	BoundInputMapping bound_mapping;
	bound_mapping.instances.reserve(input_mapping.getMappings().size());
	bound_mapping.source = &input_mapping;
	bound_mapping.priority = priority;

	for (const InputMapping& mapping : input_mapping.getMappings()) {
		InputMapping instance;
		instance.activators.reserve(mapping.activators.size());
		//instance.modifiers.reserve(mapping.modifiers.size());
		instance.alias_name = mapping.alias_name;
		instance.flags = mapping.flags;

		_alias_values[mapping.alias_name]; // Ensure alias exists.

		for (const InputActivator* activator : mapping.activators) {
			const Refl::IReflectionDefinition& ref_def = activator->getReflectionDefinition();
			InputActivator* const copy = ref_def.template duplicateT<InputActivator>(activator->getBasePointer(), g_allocator);
			GAFF_ASSERT(copy);

			instance.activators.emplace_back(copy);
		}
	}

	const auto it = Gaff::UpperBound(_mappings, priority);
	_mappings.emplace(it, std::move(bound_mapping));
}

void PlayerInputSubsystem::removeInputMapping(const InputMappingResource& input_mapping)
{
	const auto it = Gaff::Find(_mappings, &input_mapping);

	if (it != _mappings.end()) {
		_mappings.erase(it);
	}
}

NS_END

