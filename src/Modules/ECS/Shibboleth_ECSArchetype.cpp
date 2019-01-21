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
#include <Shibboleth_SerializeReader.h>
#include <Gaff_JSON.h>
#include <EASTL/algorithm.h>

NS_SHIBBOLETH

void ECSArchetype::setSharedName(Gaff::Hash64 name)
{
	_shared_name = name;
	_hash = Gaff::INIT_HASH64;
}

void ECSArchetype::setSharedName(const char* name)
{
	setSharedName(Gaff::FNV1aHash32String(name));
}

void ECSArchetype::addShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		addShared(*ref_def);
	}
}

void ECSArchetype::addShared(const Gaff::IReflectionDefinition& ref_def)
{
	Vector<const IECSVarAttribute*> attrs;
	ref_def.getClassAttrs(CLASS_HASH(IECSVarAttribute), attrs);

	if (attrs.empty()) {
		// $TODO: Log warning.
		return;
	}

	_shared_vars.emplace_back(SharedVarData{ ref_def, Vector<VarData>() });
	SharedVarData& shared_var_data = _shared_vars.back();

	for (const IECSVarAttribute* attr : attrs) {
		shared_var_data.var_attrs.emplace_back(VarData{ *attr, _shared_alloc_size });
		_shared_alloc_size += attr->getType().getReflectionInstance().size();
	}

	_hash = Gaff::INIT_HASH64;
}

void ECSArchetype::removeShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		removeShared(*ref_def);
	}
}

void ECSArchetype::removeShared(const Gaff::IReflectionDefinition& ref_def)
{
	const auto it = Gaff::Find(_shared_vars, ref_def, [](const SharedVarData& lhs, const Gaff::IReflectionDefinition& rhs) -> bool
	{
		return &lhs.ref_def == &rhs;
	});

	if (it == _shared_vars.end()) {
		return;
	}

	removeShared(static_cast<int32_t>(it - _shared_vars.begin()));
}

void ECSArchetype::removeShared(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_shared_vars.size()));
	const auto attrs = _shared_vars[index].ref_def.getClassAttrs<IECSVarAttribute, ProxyAllocator>(CLASS_HASH(IECSVarAttribute));

	for (const IECSVarAttribute* attr : attrs) {
		_shared_alloc_size -= attr->getType().getReflectionInstance().size();
	}

	_shared_vars.erase(_shared_vars.begin() + index);
	_hash = Gaff::INIT_HASH64;
}

void ECSArchetype::initShared(const Gaff::JSON& json)
{
	ProxyAllocator allocator("ECS");
	_shared_instances = SHIB_ALLOC(_shared_alloc_size, allocator);

	int32_t offset = 0;
	int32_t index = 0;

	json.forEachInObject([&](const char* component, const Gaff::JSON& value) -> bool
	{
		const SharedVarData& shared_var_data = _shared_vars[index];

		// Class had an issue and was not added to the archetype.
		if (shared_var_data.ref_def.getReflectionInstance().getHash() != Gaff::FNV1aHash64String(component)) {
			return false;
		}

		++index;

		value.forEachInArray([&](int32_t, const Gaff::JSON& ecs_var) -> bool
		{
			SerializeReader<Gaff::JSON> reader(ecs_var, allocator);
			shared_var_data.ref_def.load(reader, reinterpret_cast<int8_t*>(_shared_instances) + offset);

			offset += shared_var_data.ref_def.getReflectionInstance().size();
			return false;
		});

		return false;
	});
}

void ECSArchetype::add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	add(_vars, _alloc_size, ref_defs);
}

void ECSArchetype::add(const Gaff::IReflectionDefinition* ref_def)
{
	add(_vars, _alloc_size, ref_def);
}

void ECSArchetype::remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	remove(_vars, _alloc_size, ref_defs);
}

void ECSArchetype::remove(const Gaff::IReflectionDefinition* ref_def)
{
	remove(_vars, _alloc_size, ref_def);

}

void ECSArchetype::remove(int32_t index)
{
	remove(_vars, _alloc_size, index);
}

int32_t ECSArchetype::sharedSize(void) const
{
	return _shared_alloc_size;
}

int32_t ECSArchetype::size(void) const
{
	return _alloc_size;
}

Gaff::Hash64 ECSArchetype::getHash(void) const
{
	if (_hash == Gaff::INIT_HASH64) {
		_hash = Gaff::FNV1aHash64T(_shared_name, _hash);

		for (const SharedVarData& var_data : _shared_vars) {
			const Gaff::Hash64 hash = var_data.ref_def.getReflectionInstance().getHash();
			_hash = Gaff::FNV1aHash64T(hash, _hash);
		}

		for (const Gaff::IReflectionDefinition* ref_def : _vars) {
			const Gaff::Hash64 hash = ref_def->getReflectionInstance().getHash();
			_hash = Gaff::FNV1aHash64T(hash, _hash);
		}
	}

	return _hash;
}

bool ECSArchetype::fromJSON(const Gaff::JSON& json)
{
	if (!json.isObject()) {
		return false;
	}

	const ReflectionManager& refl_mgr = GetApp().getReflectionManager();

	const Gaff::JSON shared_components = json["SharedComponents"];
	const Gaff::JSON components = json["Components"];

	shared_components.forEachInObject([&](const char* component, const Gaff::JSON& value) -> bool
	{
		if (!value.isArray()) {
			// $TODO: Log error
			return false;
		}

		const Gaff::IReflectionDefinition* const ref_def = refl_mgr.getReflection(Gaff::FNV1aHash64String(component));

		if (!ref_def) {
			// $TODO: Log error
			return false;
		}

		const ECSClassAttribute* const ecs = ref_def->getClassAttr<ECSClassAttribute>();

		if (!ecs) {
			// $TODO: Log error
			return false;
		}

		addShared(*ref_def);

		return false;
	});

	initShared(shared_components);


	components.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		if (!value.isString()) {
			// $TODO: Log error.
			return false;
		}

		const Gaff::IReflectionDefinition* const ref_def = refl_mgr.getReflection(CLASS_HASH(value.getString()));

		if (!ref_def) {
			// $TODO: Log error
			return false;
		}

		const ECSClassAttribute* const ecs = ref_def->getClassAttr<ECSClassAttribute>();

		if (!ecs) {
			// $TODO: Log error
			return false;
		}

		add(ref_def);
		return false;
	});

	return true;
}

void ECSArchetype::add(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		add(vars, alloc_size, ref_def);
	}
}

void ECSArchetype::add(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Gaff::IReflectionDefinition* ref_def)
{
	Vector<const IECSVarAttribute*> attrs;
	ref_def->getClassAttrs(CLASS_HASH(IECSVarAttribute), attrs);

	if (attrs.empty()) {
		// $TODO: Log warning.
		return;
	}

	for (const IECSVarAttribute* attr : attrs) {
		alloc_size += attr->getType().getReflectionInstance().size();
		vars.emplace_back(ref_def);
	}

	_hash = Gaff::INIT_HASH64;
}

void ECSArchetype::remove(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		remove(vars, alloc_size, ref_def);
	}
}

void ECSArchetype::remove(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Gaff::IReflectionDefinition* ref_def)
{
	const auto it = Gaff::Find(vars, ref_def);

	if (it == vars.end()) {
		return;
	}

	vars.erase_unsorted(it);

	const auto attrs = ref_def->getClassAttrs<IECSVarAttribute, ProxyAllocator>(CLASS_HASH(IECSVarAttribute));

	for (const IECSVarAttribute* attr : attrs) {
		alloc_size -= attr->getType().getReflectionInstance().size();
	}

	_hash = Gaff::INIT_HASH64;
}

void ECSArchetype::remove(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(vars.size()));
	const auto attrs = vars[index]->getClassAttrs<IECSVarAttribute, ProxyAllocator>(CLASS_HASH(IECSVarAttribute));

	for (const IECSVarAttribute* attr : attrs) {
		alloc_size -= attr->getType().getReflectionInstance().size();
	}

	vars.erase(vars.begin() + index);
	_hash = Gaff::INIT_HASH64;
}

NS_END
