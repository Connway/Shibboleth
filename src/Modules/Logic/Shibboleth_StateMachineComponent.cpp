/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#pragma once

#include "Shibboleth_StateMachineComponent.h"
#include <Shibboleth_ResourceManager.h>

SHIB_REFLECTION_DEFINE_BEGIN(StateMachine)
	.classAttrs(
		ECSClassAttribute(nullptr, "Logic")
	)

	.base< ECSComponentBaseBoth<StateMachine, StateMachine&> >()
	.serialize(StateMachine::Load)

	.staticFunc("Constructor", StateMachine::Constructor)
	.staticFunc("Destructor", StateMachine::Destructor)

	.staticFunc("Load", static_cast<bool (*)(ECSManager&, EntityID, const Gaff::ISerializeReader&)>(StateMachine::Load))

	.var("resource", &StateMachine::resource)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(StateMachine)

NS_SHIBBOLETH

void StateMachine::CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const auto* const old_sm = reinterpret_cast<const StateMachine*>(old_begin) + old_index;
	auto* const new_sm = reinterpret_cast<StateMachine*>(new_begin) + new_index;

	*new_sm = *old_sm;
}

void StateMachine::SetInternal(void* component, int32_t comp_index, const StateMachine& value)
{
	GetInternal(component, comp_index) = value;
}

StateMachine& StateMachine::GetInternal(const void* component, int32_t comp_index)
{
	return *(reinterpret_cast<StateMachine*>(const_cast<void*>(component)) + comp_index);
}

void StateMachine::Constructor(EntityID, void* component, int32_t comp_index)
{
	new(&GetInternal(component, comp_index)) StateMachine();
}

void StateMachine::Destructor(EntityID, void* component, int32_t comp_index)
{
	GetInternal(component, comp_index).~StateMachine();
}

bool StateMachine::Load(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader)
{
	StateMachine& sm = GetInternal(ecs_mgr.getComponent<StateMachine>(id), ecs_mgr.getComponentIndex(id));

	if (!sm.resource || sm.resource->hasFailed()) {
		return false;
	}

	sm.resource->readValues(reader, sm.instance->variables);

	return true;
}

bool StateMachine::Load(const Gaff::ISerializeReader& reader, StateMachine& out)
{
	if (!Reflection<StateMachine>::Load(reader, out, true)) {
		return false;
	}

	while (out.resource->getState() == ResourceState::Pending) {
		// $TODO: Help out?
		EA::Thread::ThreadSleep();
	}

	if (out.resource->hasFailed()) {
		return false;
	}

	const Esprit::StateMachine& sm = *out.resource->getStateMachine();
	out.instance.reset(sm.createInstanceData());

	out.resource->readValues(reader, out.instance->variables);
	return true;
}

StateMachine& StateMachine::operator=(const StateMachine& rhs)
{
	resource = rhs.resource;

	if (rhs.instance) {
		instance.reset(rhs.instance->clone());
	} else if (resource && resource->isLoaded() && resource->getStateMachine()) {
		instance.reset(resource->getStateMachine()->createInstanceData());
	}

	return *this;
}

NS_END
