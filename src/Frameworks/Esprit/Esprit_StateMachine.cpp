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

#include "Esprit_StateMachine.h"
#include <Gaff_Vector.h>

NS_ESPRIT

enum class SpecialStates
{
	StartState,
	EndState,

	Count
};


StateMachine::StateMachine(void)
{
	// Always have special states.
	_states.resize(static_cast<size_t>(SpecialStates::Count));
	_states[0].name = HashString32<>("Start");
	_states[0].name = HashString32<>("End");
}

bool StateMachine::isActive(void) const
{
	return _current_state >= static_cast<int32_t>(SpecialStates::Count);
}

const StateMachine* StateMachine::getParent(void) const
{
	return _parent;
}

StateMachine* StateMachine::getParent(void)
{
	return _parent;
}

void StateMachine::setParent(StateMachine* parent)
{
	_parent = parent;
}

int32_t StateMachine::addState(const HashStringTemp32<>& name)
{
	const int32_t index = findStateIndex(name);

	if (index > -1) {
		return -1;
	}

	State& state = _states.emplace_back();
	state.name = name;

	return static_cast<int32_t>(_states.size() - 1);
}

int32_t StateMachine::addState(HashString32<>&& name)
{
	const int32_t index = findStateIndex(name);

	if (index > -1) {
		return -1;
	}

	State& state = _states.emplace_back();
	state.name = std::move(name);

	return static_cast<int32_t>(_states.size() - 1);
}

int32_t StateMachine::addState(const char* name)
{
	return addState(HashStringTemp32<>(name));
}

bool StateMachine::removeState(const HashStringTemp32<>& name)
{
	const int32_t index = findStateIndex(name);

	if (index > -1) {
		_states.erase(_states.begin() + index);
		return true;
	}

	return false;
}

bool StateMachine::removeState(const char* name)
{
	return removeState(HashStringTemp32<>(name));
}

int32_t StateMachine::addEdge(const HashStringTemp32<>& start_state_name, const HashStringTemp32<>& end_state_name)
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

int32_t StateMachine::addEdge(const char* start_state_name, const char* end_state_name)
{
	return addEdge(HashStringTemp32<>(start_state_name), HashStringTemp32<>(end_state_name));
}

int32_t StateMachine::addEdge(int32_t start_state_index, int32_t end_state_index)
{
	if (!Gaff::Between(start_state_index, 0, static_cast<int32_t>(_states.size() - 1))) {
		return -1;
	}

	if (!Gaff::Between(end_state_index, 0, static_cast<int32_t>(_states.size() - 1))) {
		return -1;
	}

	Edge& edge = _states[start_state_index].edges.emplace_back();
	edge.destination = end_state_index;

	return static_cast<int32_t>(_states[start_state_index].edges.size() - 1);
}

bool StateMachine::removeEdge(const HashStringTemp32<>& start_state_name, const HashStringTemp32<>& end_state_name)
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

bool StateMachine::removeEdge(const char* start_state_name, const char* end_state_name)
{
	return removeEdge(HashStringTemp32<>(start_state_name), HashStringTemp32<>(end_state_name));
}

bool StateMachine::removeEdge(int32_t start_state_index, int32_t end_state_index)
{
	if (!Gaff::Between(start_state_index, 0, static_cast<int32_t>(_states.size() - 1))) {
		return false;
	}

	if (!Gaff::Between(end_state_index, 0, static_cast<int32_t>(_states.size() - 1))) {
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

int32_t StateMachine::addCondition(const HashStringTemp32<>& state_name, int32_t edge_index, ICondition* condition)
{
	const int32_t state_index = findStateIndex(state_name);

	if (state_index < 0) {
		return -1;
	}

	return addCondition(state_index, edge_index, condition);
}

int32_t StateMachine::addCondition(const char* state_name, int32_t edge_index, ICondition* condition)
{
	return addCondition(HashStringTemp32<>(state_name), edge_index, condition);
}

int32_t StateMachine::addCondition(int32_t state_index, int32_t edge_index, ICondition* condition)
{
	if (!Gaff::Between(state_index, 0, static_cast<int32_t>(_states.size() - 1))) {
		return -1;
	}

	if (!Gaff::Between(edge_index, 0, static_cast<int32_t>(_states[state_index].edges.size() - 1))) {
		return -1;
	}

	_states[state_index].edges[edge_index].conditions.emplace_back(condition);
	return static_cast<int32_t>(_states[state_index].edges[edge_index].conditions.size() - 1);
}

bool StateMachine::removeCondition(const HashStringTemp32<>& state_name, int32_t edge_index, ICondition* condition)
{
	const int32_t state_index = findStateIndex(state_name);

	if (state_index < 0) {
		return false;
	}

	return removeCondition(state_index, edge_index, condition);
}

bool StateMachine::removeCondition(const char* state_name, int32_t edge_index, ICondition* condition)
{
	return removeCondition(HashStringTemp32<>(state_name), edge_index, condition);
}

bool StateMachine::removeCondition(int32_t state_index, int32_t edge_index, ICondition* condition)
{
	if (!Gaff::Between(state_index, 0, static_cast<int32_t>(_states.size() - 1))) {
		return false;
	}

	if (!Gaff::Between(edge_index, 0, static_cast<int32_t>(_states[state_index].edges.size() - 1))) {
		return false;
	}

	auto& conditions = _states[state_index].edges[edge_index].conditions;
	const auto it = Gaff::Find(conditions, condition, [](const UniquePtr<ICondition>& lhs, const ICondition* rhs) -> bool
	{
		return lhs.get() == rhs;
	});

	if (it != conditions.end()) {
		conditions.erase(it);
		return true;
	}

	return false;
}

bool StateMachine::removeCondition(const HashStringTemp32<>& state_name, int32_t edge_index, int32_t condition_index)
{
	const int32_t state_index = findStateIndex(state_name);

	if (state_index < 0) {
		return false;
	}

	return removeCondition(state_index, edge_index, condition_index);
}

bool StateMachine::removeCondition(const char* state_name, int32_t edge_index, int32_t condition_index)
{
	return removeCondition(HashStringTemp32<>(state_name), edge_index, condition_index);
}

bool StateMachine::removeCondition(int32_t state_index, int32_t edge_index, int32_t condition_index)
{
	if (!Gaff::Between(state_index, 0, static_cast<int32_t>(_states.size() - 1))) {
		return false;
	}

	if (!Gaff::Between(edge_index, 0, static_cast<int32_t>(_states[state_index].edges.size() - 1))) {
		return false;
	}

	auto& conditions = _states[state_index].edges[edge_index].conditions;

	if (!Gaff::Between(edge_index, 0, static_cast<int32_t>(conditions.size() - 1))) {
		return false;
	}

	conditions.erase(conditions.begin() + condition_index);
	return true;
}

int32_t StateMachine::findStateIndex(const HashStringTemp32<>& state_name)
{
	const auto it = Gaff::Find(_states, state_name, [](const State& lhs, const HashStringTemp32<>& rhs) -> bool
	{
		return lhs.name == rhs;
	});

	return static_cast<int32_t>(eastl::distance(_states.begin(), it));
}

NS_END
