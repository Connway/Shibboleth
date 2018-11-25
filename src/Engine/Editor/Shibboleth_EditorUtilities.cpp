/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_EditorUtilities.h"
#include <Shibboleth_ProxyAllocator.h>
#include <Shibboleth_Reflection.h>

NS_SHIBBOLETH

const Gaff::IReflectionDefinition* GetInspectorForType(const Gaff::IReflectionDefinition& ref_def)
{
	// Search through bucket with inspector attribute.
	GAFF_REF(ref_def);
	return nullptr;
}

Gaff::IReflectionObject* CreateInspector(
	const char* name,
	const Gaff::IReflectionDefinition& ref_def_type,
	const Gaff::IReflectionDefinition& ref_def_inspector)
{
	ProxyAllocator allocator;

	return ref_def_inspector.createT<Gaff::IReflectionObject>(
		CLASS_HASH(Gaff::IReflectionObject),
		ARG_HASH(const Gaff::IReflectionDefinition&, const char*),
		allocator,
		ref_def_type,
		name
	);
}

Gaff::IReflectionObject* CreateInspectorForType(const char* name, const Gaff::IReflectionDefinition& ref_def)
{
	const Gaff::IReflectionDefinition* const ref_def_inspector = GetInspectorForType(ref_def);

	if (!ref_def_inspector) {
		// TODO: Log error
		return nullptr;
	}

	return CreateInspector(name, ref_def, *ref_def_inspector);
}

NS_END
