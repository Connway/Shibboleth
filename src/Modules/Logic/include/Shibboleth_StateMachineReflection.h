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

#include <Shibboleth_Reflection.h>
#include <Esprit_StateMachineCommonConditions.h>
#include <Esprit_StateMachine.h>

SHIB_REFLECTION_DECLARE(Esprit::U8String)
SHIB_REFLECTION_DECLARE(Esprit::HashString32<>)
SHIB_REFLECTION_DECLARE(Esprit::HashString64<>)
SHIB_REFLECTION_DECLARE(Esprit::HashStringNoString32<>)
SHIB_REFLECTION_DECLARE(Esprit::HashStringNoString64<>)

SHIB_REFLECTION_DECLARE(Esprit::CheckVariableCondition::Operation)
SHIB_REFLECTION_DECLARE(Esprit::VariableSet::VariableType)

SHIB_REFLECTION_DECLARE(Esprit::CheckVariableCondition)

SHIB_REFLECTION_DECLARE(Esprit::VariableSet::Instance)
SHIB_REFLECTION_DECLARE(Esprit::VariableSet)
SHIB_REFLECTION_DECLARE(Esprit::StateMachine)
