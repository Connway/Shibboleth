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

	.base< ECSComponentBaseBoth<StateMachine> >()
	.base< ECSComponentDestructable<StateMachine> >()

	.var("value", &StateMachine::value)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(StateMachine)

NS_SHIBBOLETH

void StateMachine::CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const StateMachine* const old_comp = reinterpret_cast<const StateMachine*>(old_begin) + old_index;
	StateMachine* const new_comp = reinterpret_cast<StateMachine*>(new_begin) + new_index;

	*new_comp = *old_comp;
}

void StateMachine::SetInternal(void* component, int32_t page_index, const StateMachine& value)
{
	*(reinterpret_cast<StateMachine*>(component) + page_index) = value;
}

StateMachine StateMachine::GetInternal(const void* component, int32_t page_index)
{
	return *(reinterpret_cast<const StateMachine*>(component) + page_index);
}

NS_END
