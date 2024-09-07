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

#pragma once

#include "Esprit_ICondition.h"
#include "Esprit_SmartPtrs.h"
#include "Esprit_IProcess.h"
#include "Esprit_Vector.h"

NS_ESPRIT

class StateMachine final
{
public:
	struct Instance final
	{
		VariableSet::Instance variables;
		int32_t current_state = 0;
	};

	StateMachine(void);

	Instance getDefaultInstanceData(void) const;

	bool isActive(const Instance& instance) const;

	void update(Instance& instance) const;
	bool finalize(void);

	const StateMachine* getParent(void) const;
	StateMachine* getParent(void);
	void setParent(StateMachine* parent);

	const VariableSet& getVariables(void) const;
	VariableSet& getVariables(void);

	// Add states first.
	int32_t getStateIndex(const HashStringView32<>& name) const;
	int32_t addState(const HashStringView32<>& name);
	bool removeState(const HashStringView32<>& name);

	// Then processes, edges, and conditions.
	int32_t addProcess(const HashStringView32<>& name, IProcess* process);
	int32_t addProcess(int32_t state_index, IProcess* process);
	bool removeProcess(const HashStringView32<>& name, IProcess* process);
	bool removeProcess(int32_t state_index, IProcess* process);
	bool removeProcess(const HashStringView32<>& name, int32_t process_index);
	bool removeProcess(int32_t state_index, int32_t process_index);

	int32_t addEdge(const HashStringView32<>& start_state_name, const HashStringView32<>& end_state_name);
	int32_t addEdge(int32_t start_state_index, int32_t end_state_index);

	bool removeEdge(const HashStringView32<>& start_state_name, const HashStringView32<>& end_state_name);
	bool removeEdge(int32_t start_state_index, int32_t end_state_index);

	int32_t addCondition(const HashStringView32<>& state_name, int32_t edge_index, ICondition* condition);
	int32_t addCondition(int32_t state_index, int32_t edge_index, ICondition* condition);

	bool removeCondition(const HashStringView32<>& state_name, int32_t edge_index, ICondition* condition);
	bool removeCondition(int32_t state_index, int32_t edge_index, ICondition* condition);
	bool removeCondition(const HashStringView32<>& state_name, int32_t edge_index, int32_t condition_index);
	bool removeCondition(int32_t state_index, int32_t edge_index, int32_t condition_index);

#ifdef _DEBUG
	// Add debug query functions here.
#endif

private:
	struct Edge final
	{
		Vector< UniquePtr<ICondition> > conditions;
		int32_t destination;
	};

	struct State final
	{
		Vector< UniquePtr<IProcess> > processes;
		Vector<Edge> edges;
		OptimizedHashString32<> name;
	};

	VariableSet _variables;
	Vector<State> _states;

	StateMachine* _parent = nullptr;

	int32_t findStateIndex(const HashStringView32<>& state_name) const;
	bool doState(const State& state, Instance& instance) const;
};

NS_END
