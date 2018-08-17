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

#include "Shibboleth_ECSArchetype.h"
#include "Shibboleth_ECSAttributes.h"

NS_SHIBBOLETH

void ECSArchetype::add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		add(ref_def);
	}
}

void ECSArchetype::add(const Gaff::IReflectionDefinition* ref_def)
{
	Vector<const IECSVarAttribute*> attrs;
	ref_def->getClassAttrs(CLASS_HASH(IECSVarAttribute), attrs);

	if (attrs.empty()) {
		return;
	}

	for (const IECSVarAttribute* attr : attrs) {
		_alloc_size += attr->getType().getReflectionInstance().size();
		_vars.emplace_back(ref_def);
	}
}

void ECSArchetype::remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		remove(ref_def);
	}
}

void ECSArchetype::remove(const Gaff::IReflectionDefinition* ref_def)
{
	const auto it = Gaff::Find(_vars, ref_def);

	if (it == _vars.end()) {
		return;
	}

	_vars.erase_unsorted(it);

	Vector<const IECSVarAttribute*> attrs;
	ref_def->getClassAttrs(CLASS_HASH(IECSVarAttribute), attrs);

	for (const IECSVarAttribute* attr : attrs) {
		_alloc_size -= attr->getType().getReflectionInstance().size();
	}
}

void ECSArchetype::remove(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	_vars.erase(_vars.begin() + index);
}

int32_t ECSArchetype::size(void) const
{
	return _alloc_size;
}

Gaff::JSON ECSArchetype::toJSON(void) const
{
	Gaff::JSON json = Gaff::JSON::CreateArray();

	for (const Gaff::IReflectionDefinition* ref_def : _vars) {
		json.push(Gaff::JSON::CreateString(ref_def->getReflectionInstance().getName()));
	}

	return json;
}

NS_END
