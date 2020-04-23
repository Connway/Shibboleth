/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

SHIB_REFLECTION_DEFINE_BEGIN(StateMachine)
	.classAttrs(
		ECSClassAttribute(nullptr, "Logic")
	)

	.base< ECSComponentBaseBoth<StateMachine, StateMachineView> >()
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
	const auto* const old_res = reinterpret_cast<const StateMachineResourcePtr*>(old_begin) + old_index;
	auto* const new_res = reinterpret_cast<StateMachineResourcePtr*>(new_begin) + new_index;

	const auto* const old_vars = reinterpret_cast<const UniquePtr<Esprit::StateMachine::Instance>*>(old_res - old_index + 4) + old_index;
	auto* const new_vars = reinterpret_cast<UniquePtr<Esprit::StateMachine::Instance>*>(new_res - new_index + 4) + new_index;

	*new_res= *old_res;
	new_vars->reset((*old_vars)->clone());
}

void StateMachine::SetInternal(void* component, int32_t entity_index, const StateMachine& value)
{
	auto* const res = reinterpret_cast<StateMachineResourcePtr*>(component) + entity_index;
	auto* const inst = reinterpret_cast<UniquePtr<Esprit::StateMachine::Instance>*>(res - entity_index + 4) + entity_index;

	*res = value.resource;

	if (value.instance) {
		inst->reset(value.instance->clone());
	} else if (value.resource && value.resource->isLoaded() && value.resource->getStateMachine()) {
		inst->reset(value.resource->getStateMachine()->createInstanceData());
	}
}

void StateMachine::SetInternal(void* component, int32_t entity_index, const StateMachineView& value)
{
	auto* const res = reinterpret_cast<StateMachineResourcePtr*>(component) + entity_index;
	auto* const inst = reinterpret_cast<UniquePtr<Esprit::StateMachine::Instance>*>(res - entity_index + 4) + entity_index;

	*res = value.resource;

	if (inst->get() != value.instance) {
		inst->reset(value.instance);
	}

	if (!*inst && value.resource && value.resource->isLoaded() && value.resource->getStateMachine()) {
		inst->reset(value.resource->getStateMachine()->createInstanceData());
	}
}

StateMachineView StateMachine::GetInternal(const void* component, int32_t entity_index)
{
	StateMachineView view;

	const auto* const res = reinterpret_cast<const StateMachineResourcePtr*>(component) + entity_index;
	const auto* const inst = reinterpret_cast<const UniquePtr<Esprit::StateMachine::Instance>*>(res - entity_index + 4) + entity_index;

	view.resource = res->get();
	view.instance = inst->get();

	return view;
}

void StateMachine::Constructor(EntityID, void* component, int32_t entity_index)
{
	auto* const res = reinterpret_cast<StateMachineResourcePtr*>(component) + entity_index;
	auto* const inst = reinterpret_cast<UniquePtr<Esprit::StateMachine::Instance>*>(res - entity_index + 4) + entity_index;

	new(res) StateMachineResourcePtr();
	new(inst) UniquePtr<Esprit::StateMachine::Instance>();
}

void StateMachine::Destructor(EntityID, void* component, int32_t entity_index)
{
	auto* const res = reinterpret_cast<StateMachineResourcePtr*>(component) + entity_index;
	auto* const inst = reinterpret_cast<UniquePtr<Esprit::StateMachine::Instance>*>(res - entity_index + 4) + entity_index;

	res->~StateMachineResourcePtr();
	inst->~UniquePtr<Esprit::StateMachine::Instance>();
}

bool StateMachine::Load(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader)
{
	void* const component = ecs_mgr.getComponent<StateMachine>(id);
	const int32_t entity_index = ecs_mgr.getPageIndex(id) % 4;

	auto* const res = reinterpret_cast<StateMachineResourcePtr*>(component) + entity_index;
	auto* const inst = reinterpret_cast<UniquePtr<Esprit::StateMachine::Instance>*>(res - entity_index + 4) + entity_index;

	if (!(*res) || (*res)->hasFailed()) {
		return false;
	}

	(*res)->readValues(reader, (*inst)->variables);

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

StateMachine& StateMachine::operator=(StateMachine&& rhs)
{
	resource = std::move(rhs.resource);
	instance = std::move(rhs.instance);
	return *this;
}

NS_END
