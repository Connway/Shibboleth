/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Shibboleth_IState.h"
#include "Shibboleth_String.h"

NS_SHIBBOLETH

class App;

class StateMachine
{
public:
	struct StateEntry
	{
		typedef IState* (*CreateStateFunc)(ProxyAllocator&, App&);
		typedef void (*DestroyStateFunc)(ProxyAllocator&, IState*);

		Array<unsigned int> transitions;
		AString name;

		CreateStateFunc create_func;
		DestroyStateFunc destroy_func;
		IState* state;

		bool operator==(const char* rhs) const
		{
			return name == rhs;
		}
	};

	StateMachine(void);
	~StateMachine(void);

	void clear(void);

	void update(void);

	INLINE unsigned int getCurrentState(void) const;
	INLINE unsigned int getNextState(void) const;

	INLINE void addState(const StateEntry& state);
	INLINE void switchState(unsigned int state);
	INLINE void switchState(const AString& name);
	INLINE void switchState(const char* name);

	INLINE const Array<unsigned int>& getTransitions(unsigned int state);
	INLINE const AString& getName(unsigned int state);
	INLINE unsigned int getStateID(const AString& name);
	INLINE unsigned int getStateID(const char* name);

	INLINE unsigned int getNumStates(void) const;

private:
	Array<StateEntry> _states;
	unsigned int _curr_state;
	unsigned int _next_state;
	bool _restart;

	GAFF_NO_COPY(StateMachine);
	GAFF_NO_MOVE(StateMachine);
};

NS_END
