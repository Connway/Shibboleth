/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_StateMachine.h"

NS_SHIBBOLETH

StateMachine::StateMachine(void):
	_curr_state(UINT_FAIL), _next_state(UINT_FAIL),
	_restart(false)
{
}

StateMachine::~StateMachine(void)
{
}

void StateMachine::clear(void)
{
	_curr_state = _next_state = UINT_FAIL;

	for (auto it = _states.begin(); it != _states.end(); ++it) {
		it->destroy_func(it->state, it->module_id);
	}

	_states.clear();
}

void StateMachine::update(void)
{
	if (_next_state != _curr_state || _restart) {
		if (_curr_state != UINT_FAIL) {
			_states[_curr_state].state->exit();
		}

		_states[_next_state].state->enter();
		_curr_state = _next_state;
		_restart = false;
	}

	_states[_curr_state].state->update();
}

unsigned int StateMachine::getCurrentState(void) const
{
	return _curr_state;
}

unsigned int StateMachine::getNextState(void) const
{
	return _next_state;
}

void StateMachine::addState(const StateEntry& state)
{
	_states.push(state);
}

void StateMachine::switchState(unsigned int state_id)
{
	GAFF_ASSERT(state_id < _states.size());
	_next_state = state_id;

	if (_next_state == _curr_state) {
		_restart = true;
	}
}

void StateMachine::switchState(const AString& name)
{
	GAFF_ASSERT(name.size());
	_next_state = getStateID(name.getBuffer());

	if (_next_state == _curr_state) {
		_restart = true;
	}
}

void StateMachine::switchState(const char* name)
{
	GAFF_ASSERT(name);
	_next_state = getStateID(name);

	if (_next_state == _curr_state) {
		_restart = true;
	}
}

const Array<unsigned int>& StateMachine::getTransitions(unsigned int state_id)
{
	GAFF_ASSERT(state_id < _states.size());
	return _states[state_id].state->getTransitions();
}

const AString& StateMachine::getName(unsigned int state)
{
	GAFF_ASSERT(state < _states.size());
	return _states[state].name;
}

unsigned int StateMachine::getStateID(const AString& name)
{
	GAFF_ASSERT(name.size());
	return getStateID(name.getBuffer());
}

unsigned int StateMachine::getStateID(const char* name)
{
	GAFF_ASSERT(name);
	auto it = _states.linearSearch(name);
	GAFF_ASSERT(it != _states.end());
	return static_cast<unsigned int>(it - _states.begin());
}

unsigned int StateMachine::getNumStates(void) const
{
	return static_cast<unsigned int>(_states.size());
}

NS_END
