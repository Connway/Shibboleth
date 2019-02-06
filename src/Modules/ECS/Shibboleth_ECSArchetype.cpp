/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

NS_SHIBBOLETH

ECSArchetype::ECSArchetype(ECSArchetype&& archetype):
	_shared_vars(std::move(archetype._shared_vars)),
	_vars(std::move(archetype._vars)),
	_hash(archetype._hash),
	_shared_alloc_size(archetype._shared_alloc_size),
	_alloc_size(archetype._alloc_size),
	_shared_instances(archetype._shared_instances)
{
	archetype._shared_instances = nullptr;
}

ECSArchetype& ECSArchetype::operator=(ECSArchetype&& rhs)
{
	_shared_vars = std::move(rhs._shared_vars);
	_vars = std::move(rhs._vars);
	_hash = rhs._hash;
	_shared_alloc_size = rhs._shared_alloc_size;
	_alloc_size = rhs._alloc_size;
	_shared_instances = rhs._shared_instances;

	rhs._shared_instances = nullptr;

	return *this;
}

ECSArchetype::~ECSArchetype(void)
{
	if (_shared_instances) {
		SHIB_FREE(_shared_instances, GetAllocator());
	}
}

void ECSArchetype::addShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	add(_shared_vars, _shared_alloc_size, ref_defs, true);
}

void ECSArchetype::addShared(const Gaff::IReflectionDefinition& ref_def)
{
	add(_shared_vars, _shared_alloc_size, ref_def, true);
}

void ECSArchetype::removeShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	remove(_shared_vars, _shared_alloc_size, ref_defs, true);
}

void ECSArchetype::removeShared(const Gaff::IReflectionDefinition& ref_def)
{
	remove(_shared_vars, _shared_alloc_size, ref_def, true);
}

void ECSArchetype::removeShared(int32_t index)
{
	remove(_shared_vars, _shared_alloc_size, index, true);
}

void ECSArchetype::initShared(const Gaff::JSON& json)
{
	if (!_shared_alloc_size) {
		return;
	}

	ProxyAllocator allocator("ECS");
	_shared_instances = SHIB_ALLOC(_shared_alloc_size, allocator);

	int32_t offset = 0;
	int32_t index = 0;

	json.forEachInObject([&](const char* component, const Gaff::JSON& value) -> bool
	{
		const RefDefOffset& data = _shared_vars[index];

		// Class had an issue and was not added to the archetype.
		if (data.ref_def->getReflectionInstance().getHash() != Gaff::FNV1aHash64String(component)) {
			return false;
		}

		++index;

		value.forEachInArray([&](int32_t, const Gaff::JSON& ecs_var) -> bool
		{
			SerializeReader<Gaff::JSON> reader(ecs_var, allocator);
			data.ref_def->load(reader, reinterpret_cast<int8_t*>(_shared_instances) + offset);

			offset += data.ref_def->getReflectionInstance().size();
			return false;
		});

		return false;
	});
}

void ECSArchetype::add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	add(_vars, _alloc_size, ref_defs, false);
}

void ECSArchetype::add(const Gaff::IReflectionDefinition& ref_def)
{
	add(_vars, _alloc_size, ref_def, false);
}

void ECSArchetype::remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	remove(_vars, _alloc_size, ref_defs, false);
}

void ECSArchetype::remove(const Gaff::IReflectionDefinition& ref_def)
{
	remove(_vars, _alloc_size, ref_def, false);
}

void ECSArchetype::remove(int32_t index)
{
	remove(_vars, _alloc_size, index, false);
}

void ECSArchetype::copy(const ECSArchetype& base)
{
	_shared_alloc_size = base._shared_alloc_size;
	_alloc_size = base._alloc_size;

	_shared_vars = base._shared_vars;
	_vars = base._vars;

	_hash = Gaff::INIT_HASH64;
}

int32_t ECSArchetype::getComponentOffset(Gaff::Hash64 component) const
{
	const auto it = Gaff::Find(_vars, component, [](const RefDefOffset& lhs, Gaff::Hash64 rhs) -> bool {
		return lhs.ref_def->getReflectionInstance().getHash() == rhs;
	});

	return (it != _vars.end()) ? it->offset : -1;
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
		if (_shared_instances) {
			_hash = Gaff::FNV1aHash64(reinterpret_cast<char*>(_shared_instances), static_cast<size_t>(_shared_alloc_size));
		}

		for (const RefDefOffset& data : _shared_vars) {
			const Gaff::Hash64 hash = data.ref_def->getReflectionInstance().getHash();
			_hash = Gaff::FNV1aHash64T(hash, _hash);
		}

		for (const RefDefOffset& data : _vars) {
			const Gaff::Hash64 hash = data.ref_def->getReflectionInstance().getHash();
			_hash = Gaff::FNV1aHash64T(hash, _hash);
		}
	}

	return _hash;
}

void ECSArchetype::dirtyHash(void)
{
	_hash = Gaff::INIT_HASH64;
}

bool ECSArchetype::fromJSON(const Gaff::JSON& json)
{
	if (!json.isObject()) {
		// $TODO: Log error.
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

		add(*ref_def);
		return false;
	});

	return true;
}

void ECSArchetype::add(Vector<RefDefOffset>& vars, int32_t& alloc_size, const Vector<const Gaff::IReflectionDefinition*>& ref_defs, bool shared)
{
	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		add(vars, alloc_size, *ref_def, shared);
	}
}

void ECSArchetype::add(Vector<RefDefOffset>& vars, int32_t& alloc_size, const Gaff::IReflectionDefinition& ref_def, bool shared)
{
	const ECSClassAttribute* const attr = ref_def.getClassAttr<ECSClassAttribute>();

	if (!attr) {
		// $TODO: Log warning.
		return;
	}

	const int32_t size_scalar = (shared) ? 1 : 4;
	const int32_t size_offset = (vars.empty()) ? 0 : vars.back().ref_def->getClassAttr<ECSClassAttribute>()->size() * size_scalar;
	const int32_t base_offset = (vars.empty()) ? 0 : vars.back().offset;

	alloc_size += attr->size();

	vars.emplace_back(RefDefOffset{ &ref_def, base_offset + size_offset });

	dirtyHash();
}

void ECSArchetype::remove(Vector<RefDefOffset>& vars, int32_t& alloc_size, const Vector<const Gaff::IReflectionDefinition*>& ref_defs, bool shared)
{
	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		remove(vars, alloc_size, *ref_def, shared);
	}
}

void ECSArchetype::remove(Vector<RefDefOffset>& vars, int32_t& alloc_size, const Gaff::IReflectionDefinition& ref_def, bool shared)
{
	const auto it = Gaff::Find(vars, &ref_def, [](const RefDefOffset& lhs, const Gaff::IReflectionDefinition* rhs) -> bool
	{
		return lhs.ref_def == rhs;
	});

	if (it == vars.end()) {
		return;
	}

	remove(vars, alloc_size, static_cast<int32_t>(it - vars.begin()), shared);
}

void ECSArchetype::remove(Vector<RefDefOffset>& vars, int32_t& alloc_size, int32_t index, bool shared)
{
	const int32_t num_vars  = static_cast<int32_t>(vars.size());
	GAFF_ASSERT(index < num_vars );

	const int32_t size_scalar = (shared) ? 1 : 4;
	const int32_t data_size = vars[index].ref_def->getClassAttr<ECSClassAttribute>()->size() * size_scalar;
	alloc_size -= data_size;

	for (int32_t i = index + 1; i < num_vars ; ++i) {
		vars[i].offset -= data_size;
	}

	vars.erase(vars.begin() + index);
	dirtyHash();
}

NS_END
