/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_StateMachineReflection.h"
#include <Shibboleth_EngineAttributesCommon.h>

#ifdef LoadString
	#undef LoadString
#endif

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::U8String)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(Shibboleth::ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Shibboleth::HashStringInstance<Esprit::ProxyAllocator>)
	.serialize(Shibboleth::LoadString<Esprit::ProxyAllocator>, Shibboleth::SaveString<Esprit::ProxyAllocator>)
SHIB_REFLECTION_DEFINE_END(Esprit::U8String)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::HashString32<>)
	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Esprit::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Esprit::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Esprit::ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(Esprit::HashString32<>)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::HashString64<>)
	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Esprit::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Esprit::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Esprit::ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(Esprit::HashString64<>)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::HashStringNoString32<>)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(Shibboleth::ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Esprit::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Esprit::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Esprit::ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(Esprit::HashStringNoString32<>)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::HashStringNoString64<>)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(Shibboleth::ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Esprit::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Esprit::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Esprit::ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(Esprit::HashStringNoString64<>)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::CheckVariableCondition::Operation)
	.entry("IsFalse", Esprit::CheckVariableCondition::Operation::IsFalse)
	.entry("IsTrue", Esprit::CheckVariableCondition::Operation::IsTrue)
	.entry("NotEqualTo", Esprit::CheckVariableCondition::Operation::NotEqualTo)
	.entry("EqualTo", Esprit::CheckVariableCondition::Operation::EqualTo)
	.entry("LessThan", Esprit::CheckVariableCondition::Operation::LessThan)
	.entry("GreaterThan", Esprit::CheckVariableCondition::Operation::GreaterThan)
	.entry("LessThanOrEqualTo", Esprit::CheckVariableCondition::Operation::LessThanOrEqualTo)
	.entry("GreaterThanOrEqualTo", Esprit::CheckVariableCondition::Operation::GreaterThanOrEqualTo)
SHIB_REFLECTION_DEFINE_END(Esprit::CheckVariableCondition::Operation)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::VariableSet::VariableType)
	.entry("Reference", Esprit::VariableSet::VariableType::Reference)
	.entry("String", Esprit::VariableSet::VariableType::String)
	.entry("Float", Esprit::VariableSet::VariableType::Float)
	.entry("Integer", Esprit::VariableSet::VariableType::Integer)
	.entry("Bool", Esprit::VariableSet::VariableType::Bool)
SHIB_REFLECTION_DEFINE_END(Esprit::VariableSet::VariableType)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::CheckVariableCondition)
	.ctor<>()

	.var("var_name", &Esprit::CheckVariableCondition::getVariableName, &Esprit::CheckVariableCondition::setVariableName)
	.var("var_type", &Esprit::CheckVariableCondition::getVariableType, &Esprit::CheckVariableCondition::setVariableType)
	.var("operation", &Esprit::CheckVariableCondition::getOperation, &Esprit::CheckVariableCondition::setOperation)

	.var("string", &Esprit::CheckVariableCondition::getString, &Esprit::CheckVariableCondition::setString)
	.var("float", &Esprit::CheckVariableCondition::getFloat, &Esprit::CheckVariableCondition::setFloat)
	.var("integer", &Esprit::CheckVariableCondition::getInteger, &Esprit::CheckVariableCondition::setInteger)
	.var("bool", &Esprit::CheckVariableCondition::getBool, &Esprit::CheckVariableCondition::setBool)
SHIB_REFLECTION_DEFINE_END(Esprit::CheckVariableCondition)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::VariableSet::Instance)
SHIB_REFLECTION_DEFINE_END(Esprit::VariableSet::Instance)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::VariableSet)
	.func("getVariableIndex", &Esprit::VariableSet::getVariableIndex)
	.func("getString", &Esprit::VariableSet::getString)
	.func("getFloat", &Esprit::VariableSet::getFloat)
	.func("getInteger", &Esprit::VariableSet::getInteger)
	.func("getBool", &Esprit::VariableSet::getBool)
SHIB_REFLECTION_DEFINE_END(Esprit::VariableSet)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::StateMachine)
	.func("getVariables", static_cast<const Esprit::VariableSet& (Esprit::StateMachine::*)(void) const>(&Esprit::StateMachine::getVariables))
SHIB_REFLECTION_DEFINE_END(Esprit::StateMachine)
