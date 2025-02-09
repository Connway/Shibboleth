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

#include "StateMachine/Shibboleth_StateMachine.h"
#include "Gaff_Assert.h"
#include "Gaff_Math.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::StateMachine)
	.template ctor<>()

	.var("data", &Shibboleth::StateMachine::_data)

	.func("getParent", static_cast<const Shibboleth::StateMachine* (Shibboleth::StateMachine::*)(void) const>(&Shibboleth::StateMachine::getParent))
	.func("getParent", static_cast<Shibboleth::StateMachine* (Shibboleth::StateMachine::*)(void)>(&Shibboleth::StateMachine::getParent))

	/*.func("getData", static_cast<const Shibboleth::InstancedArrayAny& (Shibboleth::StateMachine::*)(void) const>(&Shibboleth::StateMachine::getData))*/
	/*.func("getData", static_cast<Shibboleth::InstancedArrayAny& (Shibboleth::StateMachine::*)(void)>(&Shibboleth::StateMachine::getData))*/
SHIB_REFLECTION_DEFINE_END(Shibboleth::StateMachine)

namespace
{
	enum class SpecialStates
	{
		StartState,
		EndState,
		AnyState,

		Count
	};

	constexpr const char8_t* g_special_state_names[static_cast<size_t>(SpecialStates::Count)] =
	{
		u8"Start",
		u8"End",
		u8"Any"
	};
}

NS_SHIBBOLETH

StateMachine::StateMachine(const ProxyAllocator& allocator):
	_data(allocator),
	_states(allocator)
{
	// Always have special states.
	_states.resize(static_cast<size_t>(SpecialStates::Count));

	for (int32_t i = 0; i < static_cast<int32_t>(SpecialStates::Count); ++i) {
		_states[i].name = g_special_state_names[i];
	}
}

bool StateMachine::isActive(const Instance& instance) const
{
	return instance.current_state >= static_cast<int32_t>(SpecialStates::Count);
}

void StateMachine::update(Instance& instance)
{
	if (!Gaff::ValidIndex(instance.current_state, static_cast<int32_t>(_states.size()))) {
		// $TODO: Log error.
		return;
	}

	if (!doState(_states[static_cast<int32_t>(SpecialStates::AnyState)], instance)) {
		doState(_states[instance.current_state], instance);
	}
}

bool StateMachine::finalize(void)
{
	for (State& state : _states) {
		for (IProcess& process : state.processes) {
			if (!process.init(*this)) {
				// $TODO: Log error.
				return false;
			}
		}

		for (Edge& edge : state.edges) {
			for (ICondition& condition : edge.conditions) {
				if (!condition.init(*this)) {
					// $TODO: Log error.
					return false;
				}
			}
		}
	}

	return true;
}

const StateMachine* StateMachine::getParent(void) const
{
	return _parent;
}

StateMachine* StateMachine::getParent(void)
{
	return _parent;
}

StateMachine::Instance StateMachine::createInstance(void) const
{
	Instance instance;

	instance.current_state = static_cast<int32_t>(SpecialStates::StartState);
	_data.clone(instance.data);

	return instance;
}

void StateMachine::destroyInstance(Instance& instance)
{
	for (State& state : _states) {
		for (IProcess& process : state.processes) {
			process.deinitInstance(*this, instance.data);
		}

		for (Edge& edge : state.edges) {
			for (ICondition& condition : edge.conditions) {
				condition.deinitInstance(*this, instance.data);
			}
		}
	}
}

bool StateMachine::finalize(Instance& instance)
{
	for (State& state : _states) {
		for (IProcess& process : state.processes) {
			if (!process.initInstance(*this, instance.data)) {
				// $TODO: Log error.
				return false;
			}
		}

		for (Edge& edge : state.edges) {
			for (ICondition& condition : edge.conditions) {
				if (!condition.initInstance(*this, instance.data)) {
					// $TODO: Log error.
					return false;
				}
			}
		}
	}

	return true;
}

const InstancedArrayAny& StateMachine::getData(void) const
{
	return const_cast<StateMachine*>(this)->getData();
}

InstancedArrayAny& StateMachine::getData(void)
{
	return _data;
}

int32_t StateMachine::getStateIndex(const HashStringView32<>& name) const
{
	return findStateIndex(name);
}

int32_t StateMachine::addState(const HashStringView32<>& name)
{
	const int32_t index = findStateIndex(name);

	if (index > -1) {
		return -1;
	}

	State& state = _states.emplace_back();
	state.name = name;

	return static_cast<int32_t>(_states.size() - 1);
}

bool StateMachine::removeState(const HashStringView32<>& name)
{
	const int32_t index = findStateIndex(name);

	// Can't remove special states.
	if (index < static_cast<int32_t>(SpecialStates::Count)) {
		return false;
	}

	if (index > -1) {
		_states.erase(_states.begin() + index);
		return true;
	}

	return false;
}

int32_t StateMachine::addProcess(const HashStringView32<>& state_name, const Refl::IReflectionDefinition& ref_def)
{
	GAFF_ASSERT(ref_def.hasInterface<IProcess>());

	const int32_t index = findStateIndex(state_name);

	// Special states, except Any State, can't have processes.
	if (index < static_cast<int32_t>(SpecialStates::Count) && index != static_cast<int32_t>(SpecialStates::AnyState)) {
		return -1;
	}

	return addProcess(index, ref_def);
}

int32_t StateMachine::addProcess(int32_t state_index, const Refl::IReflectionDefinition& ref_def)
{
	GAFF_ASSERT(ref_def.hasInterface<IProcess>());

	if (!Gaff::ValidIndex(state_index, static_cast<int32_t>(_states.size()))) {
		return -1;
	}

	// Special states, except Any State, can't have processes.
	if (state_index < static_cast<int32_t>(SpecialStates::Count) && state_index != static_cast<int32_t>(SpecialStates::AnyState)) {
		return -1;
	}

	_states[state_index].processes.push(ref_def);
	return _states[state_index].processes.size() - 1;
}

bool StateMachine::removeProcess(const HashStringView32<>& state_name, const Refl::IReflectionDefinition& ref_def)
{
	const int32_t index = findStateIndex(state_name);

	// Special states, except Any State, can't have processes.
	if (index < static_cast<int32_t>(SpecialStates::Count) && index != static_cast<int32_t>(SpecialStates::AnyState)) {
		return false;
	}

	return removeProcess(index, ref_def);
}

bool StateMachine::removeProcess(int32_t state_index, const Refl::IReflectionDefinition& ref_def)
{
	if (!Gaff::ValidIndex(state_index, static_cast<int32_t>(_states.size()))) {
		return false;
	}

	// Special states, except Any State, can't have processes.
	if (state_index < static_cast<int32_t>(SpecialStates::Count) && state_index != static_cast<int32_t>(SpecialStates::AnyState)) {
		return false;
	}

	auto& processes = _states[state_index].processes;
	const auto it = Gaff::Find(processes, ref_def, [](const IProcess& lhs, const Refl::IReflectionDefinition& ref_def) -> bool { return &lhs.getReflectionDefinition() == &ref_def; });

	if (it == processes.end()) {
		return false;
	}

	processes.erase(it);
	return true;
}

bool StateMachine::removeProcess(const HashStringView32<>& state_name, IProcess* process)
{
	const int32_t index = findStateIndex(state_name);

	// Special states, except Any State, can't have processes.
	if (index < static_cast<int32_t>(SpecialStates::Count) && index != static_cast<int32_t>(SpecialStates::AnyState)) {
		return false;
	}

	return removeProcess(index, process);
}

bool StateMachine::removeProcess(int32_t state_index, IProcess* process)
{
	if (!Gaff::ValidIndex(state_index, static_cast<int32_t>(_states.size()))) {
		return false;
	}

	// Special states, except Any State, can't have processes.
	if (state_index < static_cast<int32_t>(SpecialStates::Count) && state_index != static_cast<int32_t>(SpecialStates::AnyState)) {
		return false;
	}

	auto& processes = _states[state_index].processes;
	const auto it = Gaff::Find(processes, process, [](const IProcess& lhs, const IProcess* rhs) -> bool { return &lhs == rhs; });

	if (it == processes.end()) {
		return false;
	}

	processes.erase(it);
	return true;
}

bool StateMachine::removeProcess(const HashStringView32<>& state_name, int32_t process_index)
{
	const int32_t index = findStateIndex(state_name);

	// Special states, except Any State, can't have processes.
	if (index < static_cast<int32_t>(SpecialStates::Count) && index != static_cast<int32_t>(SpecialStates::AnyState)) {
		return false;
	}

	return removeProcess(index, process_index);
}

bool StateMachine::removeProcess(int32_t state_index, int32_t process_index)
{
	if (!Gaff::ValidIndex(state_index, static_cast<int32_t>(_states.size()))) {
		return false;
	}

	// Special states, except Any State, can't have processes.
	if (state_index < static_cast<int32_t>(SpecialStates::Count) && state_index != static_cast<int32_t>(SpecialStates::AnyState)) {
		return false;
	}

	auto& processes = _states[state_index].processes;

	if (!Gaff::ValidIndex(process_index, static_cast<int32_t>(processes.size()))) {
		return false;
	}

	processes.erase(processes.begin() + process_index);
	return true;
}

const IProcess* StateMachine::getProcess(const HashStringView32<>& state_name, const Refl::IReflectionDefinition& ref_def) const
{
	return const_cast<StateMachine*>(this)->getProcess(state_name, ref_def);
}

const IProcess* StateMachine::getProcess(const HashStringView32<>& state_name, int32_t process_index) const
{
	return const_cast<StateMachine*>(this)->getProcess(state_name, process_index);
}

const IProcess* StateMachine::getProcess(int32_t state_index, const Refl::IReflectionDefinition& ref_def) const
{
	return const_cast<StateMachine*>(this)->getProcess(state_index, ref_def);
}

const IProcess* StateMachine::getProcess(int32_t state_index, int32_t process_index) const
{
	return const_cast<StateMachine*>(this)->getProcess(state_index, process_index);
}

IProcess* StateMachine::getProcess(const HashStringView32<>& state_name, const Refl::IReflectionDefinition& ref_def)
{
	const int32_t index = findStateIndex(state_name);
	return getProcess(index, ref_def);
}

IProcess* StateMachine::getProcess(const HashStringView32<>& state_name, int32_t process_index)
{
	const int32_t index = findStateIndex(state_name);
	return getProcess(index, process_index);
}

IProcess* StateMachine::getProcess(int32_t state_index, const Refl::IReflectionDefinition& ref_def)
{
	if (!Gaff::ValidIndex(state_index, _states)) {
		// $TODO: Log error.
		return nullptr;
	}

	const auto it = Gaff::Find(_states[state_index].processes, ref_def, [](const IProcess& lhs, const Refl::IReflectionDefinition& ref_def) -> bool { return &lhs.getReflectionDefinition() == &ref_def; });
	return (it == _states[state_index].processes.end()) ? nullptr : &*it;
}

IProcess* StateMachine::getProcess(int32_t state_index, int32_t process_index)
{
	if (!Gaff::ValidIndex(state_index, _states)) {
		// $TODO: Log error.
		return nullptr;
	}

	if (!Gaff::ValidIndex(process_index, _states[state_index].processes)) {
		// $TODO: Log error.
		return nullptr;
	}

	return _states[state_index].processes[process_index];
}

int32_t StateMachine::addEdge(const HashStringView32<>& start_state_name, const HashStringView32<>& end_state_name)
{
	const auto start_index = findStateIndex(start_state_name);
	const auto end_index = findStateIndex(end_state_name);

	if (start_index < 0) {
		return -1;
	}

	if (end_index < 0) {
		return -1;
	}

	return addEdge(start_index, end_index);
}

int32_t StateMachine::addEdge(int32_t start_state_index, int32_t end_state_index)
{
	if (!Gaff::ValidIndex(start_state_index, static_cast<int32_t>(_states.size()))) {
		return -1;
	}

	if (!Gaff::ValidIndex(end_state_index, static_cast<int32_t>(_states.size()))) {
		return -1;
	}

	// Can't add an edge coming from the End state.
	if (start_state_index == static_cast<int32_t>(SpecialStates::EndState)) {
		return -1;
	}

	// Can't add an edge back to the Start state.
	if (end_state_index == static_cast<int32_t>(SpecialStates::StartState)) {
		return -1;
	}

	// Can't add an edge to the Any state.
	if (end_state_index == static_cast<int32_t>(SpecialStates::AnyState)) {
		return -1;
	}

	Edge& edge = _states[start_state_index].edges.emplace_back();
	edge.destination = end_state_index;

	return static_cast<int32_t>(_states[start_state_index].edges.size() - 1);
}

bool StateMachine::removeEdge(const HashStringView32<>& start_state_name, const HashStringView32<>& end_state_name)
{
	const auto start_index = findStateIndex(start_state_name);
	const auto end_index = findStateIndex(end_state_name);

	if (start_index < 0) {
		return false;
	}

	if (end_index < 0) {
		return false;
	}

	return removeEdge(start_index, end_index);
}

bool StateMachine::removeEdge(int32_t start_state_index, int32_t end_state_index)
{
	if (!Gaff::ValidIndex(start_state_index, static_cast<int32_t>(_states.size()))) {
		return false;
	}

	if (!Gaff::ValidIndex(end_state_index, static_cast<int32_t>(_states.size()))) {
		return false;
	}

	const auto it = Gaff::Find(_states[start_state_index].edges, end_state_index, [](const Edge& lhs, int32_t rhs) -> bool
	{
		return lhs.destination == rhs;
	});

	if (it != _states[start_state_index].edges.end()) {
		_states[start_state_index].edges.erase(it);
		return true;
	}

	return false;
}

int32_t StateMachine::addCondition(const HashStringView32<>& state_name, int32_t edge_index, const Refl::IReflectionDefinition& ref_def)
{
	const int32_t state_index = findStateIndex(state_name);

	if (state_index < 0) {
		return -1;
	}

	return addCondition(state_index, edge_index, ref_def);
}

int32_t StateMachine::addCondition(int32_t state_index, int32_t edge_index, const Refl::IReflectionDefinition& ref_def)
{
	if (!Gaff::ValidIndex(state_index, static_cast<int32_t>(_states.size()))) {
		// $TODO: Log error.
		return -1;
	}

	if (!Gaff::ValidIndex(edge_index, static_cast<int32_t>(_states[state_index].edges.size()))) {
		// $TODO: Log error.
		return -1;
	}

	_states[state_index].edges[edge_index].conditions.push(ref_def);
	return _states[state_index].edges[edge_index].conditions.size() - 1;
}

bool StateMachine::removeCondition(const HashStringView32<>& state_name, int32_t edge_index, ICondition* condition)
{
	const int32_t state_index = findStateIndex(state_name);

	if (state_index < 0) {
		// $TODO: Log error.
		return false;
	}

	return removeCondition(state_index, edge_index, condition);
}

bool StateMachine::removeCondition(int32_t state_index, int32_t edge_index, ICondition* condition)
{
	if (!Gaff::ValidIndex(state_index, static_cast<int32_t>(_states.size()))) {
		// $TODO: Log error.
		return false;
	}

	if (!Gaff::ValidIndex(edge_index, static_cast<int32_t>(_states[state_index].edges.size()))) {
		// $TODO: Log error.
		return false;
	}

	auto& conditions = _states[state_index].edges[edge_index].conditions;
	const auto it = Gaff::Find(conditions, condition, [](const ICondition& lhs, const ICondition* rhs) -> bool { return &lhs == rhs; });

	if (it == conditions.end()) {
		// $TODO: Log warning.
		return false;
	}

	conditions.erase(it);
	return true;
}

bool StateMachine::removeCondition(const HashStringView32<>& state_name, int32_t edge_index, int32_t condition_index)
{
	const int32_t state_index = findStateIndex(state_name);
	return removeCondition(state_index, edge_index, condition_index);
}

bool StateMachine::removeCondition(int32_t state_index, int32_t edge_index, int32_t condition_index)
{
	if (!Gaff::ValidIndex(state_index, static_cast<int32_t>(_states.size()))) {
		// $TODO: Log error.
		return false;
	}

	if (!Gaff::ValidIndex(edge_index, static_cast<int32_t>(_states[state_index].edges.size()))) {
		// $TODO: Log error.
		return false;
	}

	auto& conditions = _states[state_index].edges[edge_index].conditions;

	if (!Gaff::ValidIndex(edge_index, conditions)) {
		// $TODO: Log error.
		return false;
	}

	conditions.erase(condition_index);
	return true;
}

const ICondition* StateMachine::getCondition(const HashStringView32<>& state_name, int32_t edge_index, const Refl::IReflectionDefinition& ref_def) const
{
	return const_cast<StateMachine*>(this)->getCondition(state_name, edge_index, ref_def);
}

const ICondition* StateMachine::getCondition(const HashStringView32<>& state_name, int32_t edge_index, int32_t condition_index) const
{
	return const_cast<StateMachine*>(this)->getCondition(state_name, edge_index, condition_index);
}

const ICondition* StateMachine::getCondition(int32_t state_index, int32_t edge_index, const Refl::IReflectionDefinition& ref_def) const
{
	return const_cast<StateMachine*>(this)->getCondition(state_index, edge_index, ref_def);
}

const ICondition* StateMachine::getCondition(int32_t state_index, int32_t edge_index, int32_t condition_index) const
{
	return const_cast<StateMachine*>(this)->getCondition(state_index, edge_index, condition_index);
}

ICondition* StateMachine::getCondition(const HashStringView32<>& state_name, int32_t edge_index, const Refl::IReflectionDefinition& ref_def)
{
	const int32_t state_index = findStateIndex(state_name);
	return getCondition(state_index, edge_index, ref_def);
}

ICondition* StateMachine::getCondition(const HashStringView32<>& state_name, int32_t edge_index, int32_t condition_index)
{
	const int32_t state_index = findStateIndex(state_name);
	return getCondition(state_index, edge_index, condition_index);
}

ICondition* StateMachine::getCondition(int32_t state_index, int32_t edge_index, const Refl::IReflectionDefinition& ref_def)
{
	if (!Gaff::ValidIndex(state_index, static_cast<int32_t>(_states.size()))) {
		// $TODO: Log error.
		return nullptr;
	}

	if (!Gaff::ValidIndex(edge_index, static_cast<int32_t>(_states[state_index].edges.size()))) {
		// $TODO: Log error.
		return nullptr;
	}

	auto& conditions = _states[state_index].edges[edge_index].conditions;
	const auto it = Gaff::Find(conditions, ref_def, [](const ICondition& lhs, const Refl::IReflectionDefinition& rhs) -> bool { return &lhs.getReflectionDefinition() == &rhs; });

	return (it == conditions.end()) ? nullptr : &*it;
}

ICondition* StateMachine::getCondition(int32_t state_index, int32_t edge_index, int32_t condition_index)
{
	if (!Gaff::ValidIndex(state_index, static_cast<int32_t>(_states.size()))) {
		// $TODO: Log error.
		return nullptr;
	}

	if (!Gaff::ValidIndex(edge_index, static_cast<int32_t>(_states[state_index].edges.size()))) {
		// $TODO: Log error.
		return nullptr;
	}

	if (!Gaff::ValidIndex(condition_index, static_cast<int32_t>(_states[state_index].edges[edge_index].conditions.size()))) {
		// $TODO: Log error.
		return nullptr;
	}

	return _states[state_index].edges[edge_index].conditions[condition_index];
}

int32_t StateMachine::findStateIndex(const HashStringView32<>& state_name) const
{
	const auto it = Gaff::Find(_states, state_name, [](const State& lhs, const HashStringView32<>& rhs) -> bool
	{
		return lhs.name == rhs;
	});

	return (it == _states.end()) ? -1 : static_cast<int32_t>(eastl::distance(_states.begin(), it));
}

bool StateMachine::doState(State& state, Instance& instance)
{
	for (IProcess& process : state.processes) {
		process.update(*this, instance.data);
	}

	int32_t next_state = -1;

	for (const Edge& edge : state.edges) {
		bool can_transition = true;

		for (const ICondition& condition : edge.conditions) {
			if (!condition.evaluate(*this, instance.data)) {
				can_transition = false;
				break;
			}
		}

		if (can_transition) {
			next_state = edge.destination;
			break;
		}
	}

	if (next_state > -1) {
		instance.current_state = next_state;
		return true;
	}

	return false;
}

NS_END
