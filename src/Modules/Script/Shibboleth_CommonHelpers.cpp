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

#include "Shibboleth_CommonHelpers.h"
#include <Shibboleth_ReflectionInterfaces.h>
#include <Shibboleth_HashString.h>
#include <Shibboleth_String.h>

NS_SHIBBOLETH

TableState::~TableState(void)
{
	for (auto& pair : array_entries) {
		if (pair.second.ref_def && !pair.second.ref_def->isBuiltIn() && !pair.second.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference)) {
			pair.second.ref_def->destroyInstance(pair.second.value.vp);
			SHIB_FREE(pair.second.value.vp, GetAllocator());
		}
	}

	for (auto& pair : key_values) {
		if (pair.second.ref_def && !pair.second.ref_def->isBuiltIn() && !pair.second.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference)) {
			pair.second.ref_def->destroyInstance(pair.second.value.vp);
			SHIB_FREE(pair.second.value.vp, GetAllocator());
		}
	}
}



void FreeDifferentType(Gaff::FunctionStackEntry& entry, const Gaff::IReflectionDefinition& new_ref_def, bool new_is_reference)
{
	const bool is_reference = entry.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference);

	if (entry.ref_def && ((entry.ref_def != &new_ref_def) || (is_reference != new_is_reference))) {
		if (!is_reference && !entry.ref_def->isBuiltIn()) {
			entry.ref_def->destroyInstance(entry.value.vp);
			SHIB_FREE(entry.value.vp, GetAllocator());
		}

		entry.value.vp = nullptr;
		entry.ref_def = nullptr;
		entry.flags.clear();
	}
}

void CopyUserType(const Gaff::IReflectionDefinition& ref_def, const void* value, void* dest, bool old_value_is_valid, ProxyAllocator allocator)
{
	U8String ctor_sig(allocator);
	ctor_sig.append_sprintf("const %s&", ref_def.getReflectionInstance().getName());

	const HashStringView64<> hash(ctor_sig);

	auto ctor = ref_def.getConstructor(hash.getHash());

	if (!ctor) {
		// $TODO: Log error.
		return;
	}

	// Deconstruct old value.
	if (old_value_is_valid) {
		ref_def.destroyInstance(dest);
	}

	// Construct new value.
	const auto cast_ctor = reinterpret_cast<void (*)(void*, const void*)>(ctor);
	cast_ctor(dest, value);
}

void CopyUserType(const Gaff::FunctionStackEntry& entry, void* dest, bool old_value_is_valid, ProxyAllocator allocator)
{
	CopyUserType(*entry.ref_def, entry.value.vp, dest, old_value_is_valid, allocator);
}

NS_END
