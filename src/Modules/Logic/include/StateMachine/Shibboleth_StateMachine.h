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

#pragma once

#include "StateMachine/Shibboleth_ICondition.h"
#include "StateMachine/Shibboleth_IProcess.h"

NS_SHIBBOLETH

// $TODO: Add support for transitions.
class StateMachine final
{
public:
	struct Instance final
	{
		InstancedArrayAny data;
		int32_t current_state = 0;
	};

	explicit StateMachine(const ProxyAllocator& allocator = ProxyAllocator{});

	bool isActive(const Instance& instance) const;

	void update(Instance& instance);
	bool finalize(void);

	const StateMachine* getParent(void) const;
	StateMachine* getParent(void);

	Instance createInstance(void) const;
	void destroyInstance(Instance& instance);
	bool finalize(Instance& instance);

	const InstancedArrayAny& getData(void) const;
	InstancedArrayAny& getData(void);

	// Add states first.
	int32_t getStateIndex(const HashStringView32<>& name) const;
	int32_t addState(const HashStringView32<>& name);
	bool removeState(const HashStringView32<>& name);

	// Then processes, edges, and conditions.
	int32_t addProcess(const HashStringView32<>& state_name, const Refl::IReflectionDefinition& ref_def);
	int32_t addProcess(int32_t state_index, const Refl::IReflectionDefinition& ref_def);
	bool removeProcess(const HashStringView32<>& state_name, const Refl::IReflectionDefinition& ref_def);
	bool removeProcess(int32_t state_index, const Refl::IReflectionDefinition& ref_def);
	bool removeProcess(const HashStringView32<>& state_name, IProcess* process);
	bool removeProcess(int32_t state_index, IProcess* process);
	bool removeProcess(const HashStringView32<>& state_name, int32_t process_index);
	bool removeProcess(int32_t state_index, int32_t process_index);

	const IProcess* getProcess(const HashStringView32<>& state_name, const Refl::IReflectionDefinition& ref_def) const;
	const IProcess* getProcess(const HashStringView32<>& state_name, int32_t process_index) const;
	const IProcess* getProcess(int32_t state_index, const Refl::IReflectionDefinition& ref_def) const;
	const IProcess* getProcess(int32_t state_index, int32_t process_index) const;
	IProcess* getProcess(const HashStringView32<>& state_name, const Refl::IReflectionDefinition& ref_def);
	IProcess* getProcess(const HashStringView32<>& state_name, int32_t process_index);
	IProcess* getProcess(int32_t state_index, const Refl::IReflectionDefinition& ref_def);
	IProcess* getProcess(int32_t state_index, int32_t process_index);

	int32_t addEdge(const HashStringView32<>& start_state_name, const HashStringView32<>& end_state_name);
	int32_t addEdge(int32_t start_state_index, int32_t end_state_index);

	bool removeEdge(const HashStringView32<>& start_state_name, const HashStringView32<>& end_state_name);
	bool removeEdge(int32_t start_state_index, int32_t end_state_index);

	int32_t addCondition(const HashStringView32<>& state_name, int32_t edge_index, const Refl::IReflectionDefinition& ref_def);
	int32_t addCondition(int32_t state_index, int32_t edge_index, const Refl::IReflectionDefinition& ref_def);

	bool removeCondition(const HashStringView32<>& state_name, int32_t edge_index, ICondition* condition);
	bool removeCondition(int32_t state_index, int32_t edge_index, ICondition* condition);
	bool removeCondition(const HashStringView32<>& state_name, int32_t edge_index, int32_t condition_index);
	bool removeCondition(int32_t state_index, int32_t edge_index, int32_t condition_index);

	const ICondition* getCondition(const HashStringView32<>& state_name, int32_t edge_index, const Refl::IReflectionDefinition& ref_def) const;
	const ICondition* getCondition(const HashStringView32<>& state_name, int32_t edge_index, int32_t condition_index) const;
	const ICondition* getCondition(int32_t state_index, int32_t edge_index, const Refl::IReflectionDefinition& ref_def) const;
	const ICondition* getCondition(int32_t state_index, int32_t edge_index, int32_t condition_index) const;
	ICondition* getCondition(const HashStringView32<>& state_name, int32_t edge_index, const Refl::IReflectionDefinition& ref_def);
	ICondition* getCondition(const HashStringView32<>& state_name, int32_t edge_index, int32_t condition_index);
	ICondition* getCondition(int32_t state_index, int32_t edge_index, const Refl::IReflectionDefinition& ref_def);
	ICondition* getCondition(int32_t state_index, int32_t edge_index, int32_t condition_index);

private:
	struct Edge final
	{
		InstancedArray<ICondition> conditions;
		int32_t destination;
	};

	struct State final
	{
		InstancedArray<IProcess> processes;
		Vector<Edge> edges;
		HashString32<> name;
	};

	InstancedArrayAny _data;
	Vector<State> _states;

	StateMachine* _parent = nullptr;

	int32_t findStateIndex(const HashStringView32<>& state_name) const;
	bool doState(State& state, Instance& instance);
};

NS_END
