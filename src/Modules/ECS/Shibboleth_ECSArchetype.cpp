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
#include <Gaff_SerializeInterfaces.h>

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

bool ECSArchetype::addShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	return add<true>(ref_defs);
}

bool ECSArchetype::addShared(const Gaff::IReflectionDefinition& ref_def)
{
	return add<true>(ref_def);
}

bool ECSArchetype::removeShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	return remove<true>(ref_defs);
}

bool ECSArchetype::removeShared(const Gaff::IReflectionDefinition& ref_def)
{
	return remove<true>(ref_def);
}

bool ECSArchetype::removeShared(int32_t index)
{
	return remove<true>(index);
}

bool ECSArchetype::finalize(const Gaff::ISerializeReader& reader)
{
	if (!reader.isObject()) {
		// $TODO: Log error.
		return false;
	}

	const ReflectionManager& refl_mgr = GetApp().getReflectionManager();
	//const Gaff::JSON shared_components = json["shared_components"];
	//const Gaff::JSON components = json["components"];

	{
		const auto guard = reader.enterElementGuard("shared_components");

		reader.forEachInObject([&](const char* component) -> bool
		{
			if (!reader.isArray()) {
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
	}

	{
		const auto guard = reader.enterElementGuard("components");

		reader.forEachInArray([&](int32_t) -> bool
		{
			if (!reader.isString()) {
				// $TODO: Log error.
				return false;
			}

			const Gaff::IReflectionDefinition* const ref_def = refl_mgr.getReflection(CLASS_HASH(reader.readString()));

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
	}

	{
		const auto guard = reader.enterElementGuard("shared_components");
		initShared(reader);
		calculateHash();
	}

	return true;
}

bool ECSArchetype::finalize(void)
{
	initShared();
	calculateHash();
	return true;
}

bool ECSArchetype::add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	return add<false>(ref_defs);
}

bool ECSArchetype::add(const Gaff::IReflectionDefinition& ref_def)
{
	return add<false>(ref_def);
}

bool ECSArchetype::remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	return remove<false>(ref_defs);
}

bool ECSArchetype::remove(const Gaff::IReflectionDefinition& ref_def)
{
	return remove<false>(ref_def);
}

bool ECSArchetype::remove(int32_t index)
{
	return remove<false>(index);
}

void ECSArchetype::copy(const ECSArchetype& base)
{
	_shared_alloc_size = base._shared_alloc_size;
	_alloc_size = base._alloc_size;

	_shared_vars = base._shared_vars;
	_vars = base._vars;

	_hash = Gaff::INIT_HASH64;
}

void ECSArchetype::copy(
	const ECSArchetype& old_archetype,
	void* old_data,
	int32_t old_index,
	void* new_data,
	int32_t new_index
)
{
	const auto end_shared = _shared_vars.end();
	auto it_shared = _shared_vars.begin();

	for (const RefDefOffset& rdo : old_archetype._shared_vars) {
		const auto it_pos = eastl::find(it_shared, end_shared, rdo.ref_def, [](const auto& lhs, const auto* rhs) -> bool { return lhs.ref_def == rhs; });

		// Component was deleted.
		if (it_pos == end_shared) {
			continue;
		}

		it_shared = it_pos;

		const void* const old_component = reinterpret_cast<int8_t*>(old_archetype._shared_instances) + rdo.offset;
		void* const new_component = reinterpret_cast<int8_t*>(_shared_instances) + it_shared->offset;

		rdo.copy_shared_func(old_component, new_component);
	}

	const auto end = _vars.end();
	auto it = _vars.begin();

	for (const RefDefOffset& rdo : old_archetype._vars) {
		const auto it_pos = eastl::find(it, end, rdo.ref_def, [](const auto& lhs, const auto* rhs) -> bool { return lhs.ref_def == rhs; });

		// Component was deleted.
		if (it_pos == end) {
			continue;
		}

		it = it_pos;

		const void* const old_component = reinterpret_cast<int8_t*>(old_data) + rdo.offset;
		void* const new_component = reinterpret_cast<int8_t*>(new_data) + it->offset;

		rdo.copy_func(old_component, old_index, new_component, new_index);
	}
}

int32_t ECSArchetype::getComponentSharedOffset(Gaff::Hash64 component) const
{
	const auto it = Gaff::Find(_shared_vars, component, [](const RefDefOffset& lhs, Gaff::Hash64 rhs) -> bool
	{
		return lhs.ref_def->getReflectionInstance().getHash() == rhs;
	});

	return (it != _vars.end()) ? it->offset : -1;
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
	return _hash;
}

const void* ECSArchetype::getSharedData(void) const
{
	return _shared_instances;
}

void* ECSArchetype::getSharedData(void)
{
	return _shared_instances;
}

template <bool shared>
bool ECSArchetype::add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	bool success = true;

	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		success = success && add<shared>(*ref_def);
	}

	return success;
}

template <bool shared>
bool ECSArchetype::add(const Gaff::IReflectionDefinition& ref_def)
{
	Vector<RefDefOffset>& vars = (shared) ? _shared_vars : _vars;
	Vector<RefDefOffset>& other_vars = (shared) ? _vars : _shared_vars;
	int32_t& alloc_size = (shared) ? _shared_alloc_size : _alloc_size;

	const auto it_other = Gaff::LowerBound(other_vars, ref_def, [](const RefDefOffset& lhs, const Gaff::IReflectionDefinition& rhs) -> bool
	{
		return lhs.ref_def->getReflectionInstance().getHash() < rhs.getReflectionInstance().getHash();
	});

	GAFF_ASSERT(it_other == other_vars.end() || it_other->ref_def->getReflectionInstance().getHash() != ref_def.getReflectionInstance().getHash());

	const ECSClassAttribute* const attr = ref_def.getClassAttr<ECSClassAttribute>();

	if (!attr) {
		// $TODO: Log warning.
		return false;
	}

	RefDefOffset::CopySharedFunc copy_shared_func = ref_def.getStaticFunc<void, const void*, void*>(Gaff::FNV1aHash32Const("CopyShared"));
	RefDefOffset::CopyFunc copy_func = ref_def.getStaticFunc<void, const void*, int32_t, void*, int32_t>(Gaff::FNV1aHash32Const("Copy"));

	if (!copy_shared_func && shared) {
		// $TODO: Log warning.
		return false;
	}

	if (!copy_func && !shared) {
		// $TODO: Log warning.
		return false;
	}

	const auto it = Gaff::LowerBound(vars, ref_def, [](const RefDefOffset& lhs, const Gaff::IReflectionDefinition& rhs) -> bool
	{
		return lhs.ref_def->getReflectionInstance().getHash() < rhs.getReflectionInstance().getHash();
	});

	GAFF_ASSERT(it == vars.end() || it->ref_def->getReflectionInstance().getHash() != ref_def.getReflectionInstance().getHash());
	constexpr int32_t size_scalar = (shared) ? 1 : 4;

	// Simple append.
	if (it == vars.end()) {
		const int32_t size_offset = (vars.empty()) ? 0 : vars.back().ref_def->getClassAttr<ECSClassAttribute>()->size() * size_scalar;
		const int32_t base_offset = (vars.empty()) ? 0 : vars.back().offset;

		vars.emplace_back(RefDefOffset{ &ref_def, base_offset + size_offset, copy_shared_func, copy_func });

	// Inserting, bump up all the offsets.
	} else {
		const int32_t offset = it->offset;

		for (auto begin = it; begin != vars.end(); ++begin) {
			begin->offset += attr->size() * size_scalar;
		}

		vars.emplace(it, RefDefOffset{ &ref_def, offset, copy_shared_func, copy_func });
	}

	alloc_size += attr->size();
	return true;
}

template <bool shared>
bool ECSArchetype::remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	bool success = true;

	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		success = success && remove<shared>(*ref_def);
	}

	return success;
}

template <bool shared>
bool ECSArchetype::remove(const Gaff::IReflectionDefinition& ref_def)
{
	Vector<RefDefOffset>& vars = (shared) ? _shared_vars : _vars;

	const auto it = Gaff::Find(vars, &ref_def, [](const RefDefOffset& lhs, const Gaff::IReflectionDefinition* rhs) -> bool
	{
		return lhs.ref_def == rhs;
	});

	if (it == vars.end()) {
		// $TODO: Log error.
		return false;
	}

	return remove<shared>(static_cast<int32_t>(it - vars.begin()));
}

template <bool shared>
bool ECSArchetype::remove(int32_t index)
{
	Vector<RefDefOffset>& vars = (shared) ? _shared_vars : _vars;
	int32_t& alloc_size = (shared) ? _shared_alloc_size : _alloc_size;

	const int32_t num_vars  = static_cast<int32_t>(vars.size());
	GAFF_ASSERT(index < num_vars );

	constexpr int32_t size_scalar = (shared) ? 1 : 4;
	const int32_t data_size = vars[index].ref_def->getClassAttr<ECSClassAttribute>()->size();
	alloc_size -= data_size;

	// Reduce all the offsets.
	for (int32_t i = index + 1; i < num_vars ; ++i) {
		vars[i].offset -= data_size * size_scalar;
	}

	vars.erase(vars.begin() + index);
	return true;
}

void ECSArchetype::initShared(const Gaff::ISerializeReader& reader)
{
	if (!_shared_alloc_size) {
		return;
	}

	ProxyAllocator allocator("ECS");
	_shared_instances = SHIB_ALLOC(_shared_alloc_size, allocator);

	int32_t index = 0;

	reader.forEachInObject([&](const char* component) -> bool
	{
		const RefDefOffset& data = _shared_vars[index];

		// Class had an issue and was not added to the archetype.
		if (data.ref_def->getReflectionInstance().getHash() != Gaff::FNV1aHash64String(component)) {
			return false;
		}

		const auto vars = data.ref_def->GET_CLASS_ATTRS(IECSVarAttribute, ProxyAllocator);
		int32_t offset = 0;
		++index;

		reader.forEachInArray([&](int32_t var_index) -> bool
		{
			const Gaff::IReflectionDefinition& ref_def = vars[var_index]->getType();
			const auto ctor = ref_def.getConstructor<>();

			if (ctor) {
				ctor(reinterpret_cast<int8_t*>(_shared_instances) + data.offset + offset);
			}

			ref_def.load(reader, reinterpret_cast<int8_t*>(_shared_instances) + data.offset + offset);

			offset += ref_def.size();
			return false;
		});

		return false;
	});
}

void ECSArchetype::initShared(void)
{
	if (!_shared_alloc_size) {
		return;
	}

	ProxyAllocator allocator("ECS");
	_shared_instances = SHIB_ALLOC(_shared_alloc_size, allocator);

	for (const RefDefOffset& data : _shared_vars) {
		const auto vars = data.ref_def->GET_CLASS_ATTRS(IECSVarAttribute, ProxyAllocator);
		int32_t offset = 0;

		for (const IECSVarAttribute* ecs_var : vars) {
			const Gaff::IReflectionDefinition& ref_def = ecs_var->getType();
			const auto ctor = data.ref_def->getConstructor<>();

			if (ctor) {
				ctor(reinterpret_cast<int8_t*>(_shared_instances) + data.offset + offset);
			}

			offset += ref_def.size();
		}
	}
}

void ECSArchetype::calculateHash(void)
{
	_hash = Gaff::INIT_HASH64;

	if (_shared_instances) {
		_hash = Gaff::FNV1aHash64(reinterpret_cast<char*>(_shared_instances), static_cast<size_t>(_shared_alloc_size), _hash);
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

NS_END
