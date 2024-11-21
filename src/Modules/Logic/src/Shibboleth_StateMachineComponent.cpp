/************************************************************************************
Copyright (C) by Nicholas LaCroix

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
#include "Shibboleth_StateMachineComponent.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::StateMachineComponent)
	.template base<Shibboleth::EntityComponent>()
	// .serialize(Shibboleth::StateMachine::Load)

	.var("resource", &Type::_resource)
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::StateMachine)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(StateMachineComponent);

bool StateMachineComponent::init()
{
	if (!EntityComponent::init()) {
		return false;
	}

	if (!_resource) {
		return true;
	}

	if (_resource->hasFailed()) {
		return false;
	}

	_instance = _resource->getStateMachine()->getDefaultInstanceData();

	return true;
}

bool StateMachineComponent::clone(EntityComponent& new_component, const ISerializeReader* overrides) const
{
	if (!EntityComponent::clone(new_component, overrides)) {
		return false;
	}

	StateMachineComponent& sm_comp = static_cast<StateMachineComponent&>(new_component);

	sm_comp._instance = _instance;

	if (overrides) {
		const auto guard = overrides->enterElementGuard(u8"variables");
		sm_comp._resource->readValues(*overrides, sm_comp._instance.variables);
	}

	return true;
}

/*void StateMachineComponent::update(float dt)
{
	if (!_resource) {
		// $TODO: Log error
		return;
	}

	const Esprit::StateMachine* const sm = _resource->getStateMachine();

	if (!sm) {
		// $TODO: Log error
		return;
	}

	if (!_instance) {
		// $TODO: Log error
		return;
	}

	//const Esprit::VariableSet& vars = sm->getVariables();
	//const int32_t var_index = vars.getVariableIndex(HashStringView32<>(u8"entity_id"), Esprit::VariableSet::VariableType::Integer);

	//if (var_index > -1) {
	//	state_machine.instance->variables.integers[var_index] = id;
	//}

	sm->update(*_instance);
}*/

NS_END
