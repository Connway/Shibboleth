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

#include "Shibboleth_VariableSet.h"
#include "Shibboleth_StateMachineReflection.h"

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::VariableSet::Instance)
SHIB_REFLECTION_DEFINE_END(Esprit::VariableSet::Instance)

SHIB_REFLECTION_DEFINE_BEGIN(Esprit::VariableSet)
	.func(u8"getVariableIndex", &Esprit::VariableSet::getVariableIndex)
	.func(u8"getReference", Shibboleth::GetReference)
	.func(u8"getString", &Esprit::VariableSet::getString)
	.func(u8"getFloat", &Esprit::VariableSet::getFloat)
	.func(u8"getInteger", &Esprit::VariableSet::getInteger)
	.func(u8"getBool", &Esprit::VariableSet::getBool)
SHIB_REFLECTION_DEFINE_END(Esprit::VariableSet)

NS_SHIBBOLETH

void* GetReference(
	const Esprit::VariableSet& variables,
	const Esprit::VariableSet::Instance& instance,
	int32_t index,
	const Refl::IReflectionDefinition& ref_def)
{
	void* const object = variables.getReference(instance, index * 2);
	const Refl::IReflectionDefinition* const object_ref_def = reinterpret_cast<Refl::IReflectionDefinition*>(variables.getReference(instance, index * 2 + 1));

	GAFF_ASSERT(object);
	GAFF_ASSERT(object_ref_def);

	if (!object_ref_def->hasInterface(ref_def)) {
		return nullptr;
	}

	return object_ref_def->getInterface(ref_def, object);
}

NS_END
