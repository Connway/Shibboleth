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

#include "Shibboleth_StateMachineSystem.h"
#include "Shibboleth_StateMachineComponent.h"
#include <Shibboleth_ECSManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

SHIB_REFLECTION_DEFINE_BEGIN(StateMachineSystem)
	.BASE(ISystem)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(StateMachineSystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(StateMachineSystem)

bool StateMachineSystem::init(void)
{
	_ecs_mgr = &GetApp().getManagerTFast<ECSManager>();

	ECSQuery query;
	query.add<StateMachine>(_state_machines);

	_ecs_mgr->registerQuery(std::move(query));

	return true;
}

void StateMachineSystem::update(uintptr_t /*thread_id_int*/)
{
	// $TODO: Jobify this loop.
	for (const auto& sm_arch : _state_machines) {
		_ecs_mgr->iterate<StateMachine>(
			sm_arch,
			[](EntityID id, StateMachine& state_machine) -> void
			{
				if (!state_machine.resource) {
					// $TODO: Log error
					return;
				}

				const Esprit::StateMachine* const sm = state_machine.resource->getStateMachine();

				if (!sm) {
					// $TODO: Log error
					return;
				}

				if (!state_machine.instance) {
					// $TODO: Log error
					return;
				}

				const Esprit::VariableSet& vars = sm->getVariables();
				const int32_t var_index = vars.getVariableIndex("entity_id", Esprit::VariableSet::VariableType::Integer);

				if (var_index > -1) {
					state_machine.instance->variables.integers[var_index] = id;
				}

				sm->update(*state_machine.instance);
			}
		);
	}
}

NS_END
