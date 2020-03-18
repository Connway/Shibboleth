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

#include "Esprit_VariableSet.h"
#include "Esprit_ICondition.h"
#include "Esprit_SmartPtrs.h"
#include "Esprit_IProcess.h"
#include "Esprit_Vector.h"

NS_ESPRIT

class StateMachine final
{
public:
	StateMachine(void);

	bool isActive(void) const;

	const StateMachine* getParent(void) const;
	StateMachine* getParent(void);
	void setParent(StateMachine* parent);

	int32_t getStateIndex(const HashStringTemp32<>& name) const;
	int32_t addState(const HashStringTemp32<>& name);
	bool removeState(const HashStringTemp32<>& name);

	int32_t addEdge(const HashStringTemp32<>& start_state_name, const HashStringTemp32<>& end_state_name);
	int32_t addEdge(int32_t start_state_index, int32_t end_state_index);

	bool removeEdge(const HashStringTemp32<>& start_state_name, const HashStringTemp32<>& end_state_name);
	bool removeEdge(int32_t start_state_index, int32_t end_state_index);

	int32_t addCondition(const HashStringTemp32<>& state_name, int32_t edge_index, ICondition* condition);
	int32_t addCondition(int32_t state_index, int32_t edge_index, ICondition* condition);

	bool removeCondition(const HashStringTemp32<>& state_name, int32_t edge_index, ICondition* condition);
	bool removeCondition(int32_t state_index, int32_t edge_index, ICondition* condition);
	bool removeCondition(const HashStringTemp32<>& state_name, int32_t edge_index, int32_t condition_index);
	bool removeCondition(int32_t state_index, int32_t edge_index, int32_t condition_index);

#ifdef _DEBUG
	// Add debug query functions here.
#endif

private:
	struct Edge final
	{
		int32_t destination;
		Vector< UniquePtr<ICondition> >conditions;
	};

	struct State final
	{
		Vector< UniquePtr<IProcess> > processes;
		Vector<Edge> edges;
		OptimizedHashString32<> name;
	};

	Vector<State> _states;
	int32_t _current_state = 0;

	StateMachine* _parent = nullptr;

	int32_t findStateIndex(const HashStringTemp32<>& state_name) const;
};

NS_END
