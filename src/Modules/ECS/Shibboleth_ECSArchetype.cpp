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
#include "Shibboleth_ECSManager.h"
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
	destroySharedData();
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

bool ECSArchetype::finalize(const Gaff::ISerializeReader& reader, const ECSArchetype& base_archetype)
{
	destroySharedData();

	bool success = finalize<true>(reader, &base_archetype);
	success = success && finalize<false>(reader, &base_archetype);
	calculateHash();

	return success;
}

bool ECSArchetype::finalize(const Gaff::ISerializeReader& reader)
{
	if (!reader.isObject()) {
		// $TODO: Log error.
		return false;
	}

	destroySharedData();

	bool success = finalize<true>(reader, nullptr);
	success = success && finalize<false>(reader, nullptr);
	calculateHash();

	return success;
}

bool ECSArchetype::finalize(const ECSArchetype& base_archetype)
{
	destroySharedData();
	initShared();
	copySharedInstanceData(base_archetype);
	calculateHash();

	return true;
}

bool ECSArchetype::finalize(void)
{
	destroySharedData();
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

void ECSArchetype::append(const ECSArchetype& base)
{
	for (const RefDefOffset& rdo : base._shared_vars) {
		if (!hasSharedComponent(rdo.ref_def->getReflectionInstance().getHash())) {
			addShared(*rdo.ref_def);
		}
	}

	for (const RefDefOffset& rdo : base._vars) {
		if (!hasSharedComponent(rdo.ref_def->getReflectionInstance().getHash())) {
			add(*rdo.ref_def);
		}
	}
}

void ECSArchetype::copy(const ECSArchetype& base, bool copy_shared_instance_data)
{
	_shared_alloc_size = base._shared_alloc_size;
	_alloc_size = base._alloc_size;

	_shared_vars = base._shared_vars;
	_vars = base._vars;

	_hash = Gaff::INIT_HASH64;

	if (copy_shared_instance_data) {
		copySharedInstanceData(base);
	}
}

void ECSArchetype::copy(
	const ECSArchetype& old_archetype,
	void* old_data,
	int32_t old_index,
	void* new_data,
	int32_t new_index
)
{
	copySharedInstanceData(old_archetype);

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

	return (it != _shared_vars.end()) ? it->offset : -1;
}

int32_t ECSArchetype::getComponentOffset(Gaff::Hash64 component) const
{
	const auto it = Gaff::Find(_vars, component, [](const RefDefOffset& lhs, Gaff::Hash64 rhs) -> bool {
		return lhs.ref_def->getReflectionInstance().getHash() == rhs;
	});

	return (it != _vars.end()) ? it->offset : -1;
}

bool ECSArchetype::hasSharedComponent(Gaff::Hash64 component) const
{
	const auto it = Gaff::Find(_shared_vars, component, [](const RefDefOffset& lhs, Gaff::Hash64 rhs) -> bool {
		return lhs.ref_def->getReflectionInstance().getHash() == rhs;
	});

	return it != _shared_vars.end();
}

bool ECSArchetype::hasComponent(Gaff::Hash64 component) const
{
	const auto it = Gaff::Find(_vars, component, [](const RefDefOffset& lhs, Gaff::Hash64 rhs) -> bool {
		return lhs.ref_def->getReflectionInstance().getHash() == rhs;
	});

	return it != _vars.end();
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

const void* ECSArchetype::getSharedData(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_shared_vars.size()));
	return reinterpret_cast<int8_t*>(_shared_instances) + _shared_vars[index].offset;
}

void* ECSArchetype::getSharedData(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_shared_vars.size()));
	return reinterpret_cast<int8_t*>(_shared_instances) + _shared_vars[index].offset;
}

const void* ECSArchetype::getSharedData(void) const
{
	return _shared_instances;
}

void* ECSArchetype::getSharedData(void)
{
	return _shared_instances;
}

const Gaff::IReflectionDefinition& ECSArchetype::getSharedComponentRefDef(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_shared_vars.size()));
	return *_shared_vars[index].ref_def;
}

int32_t ECSArchetype::getNumSharedComponents(void) const
{
	return static_cast<int32_t>(_shared_vars.size());
}

const Gaff::IReflectionDefinition& ECSArchetype::getComponentRefDef(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	return *_vars[index].ref_def;
}

int32_t ECSArchetype::getNumComponents(void) const
{
	return static_cast<int32_t>(_vars.size());
}

const void* ECSArchetype::getSharedComponent(Gaff::Hash64 component) const
{
	return const_cast<ECSArchetype*>(this)->getSharedComponent(component);
}

void* ECSArchetype::getSharedComponent(Gaff::Hash64 component)
{
	const int32_t offset = getComponentSharedOffset(component);
	return (offset >= 0) ? reinterpret_cast<int8_t*>(_shared_instances) + offset : nullptr;
}

bool ECSArchetype::loadComponent(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader, Gaff::Hash64 component) const
{
	GAFF_ASSERT(ValidEntityID(id));

	for (const RefDefOffset& rdo : _vars) {
		if (rdo.ref_def->getReflectionInstance().getHash() == component) {
			if (!rdo.load_func) {
				// $TODO: Log error
				return false;
			}

			return rdo.load_func(ecs_mgr, id, reader);
		}
	}

	// $TODO: Log error
	return false;
}

bool ECSArchetype::loadComponent(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader, int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	GAFF_ASSERT(ValidEntityID(id));

	const RefDefOffset& rdo = _vars[index];

	if (!rdo.load_func) {
		// $TODO: Log error
		return false;
	}

	return rdo.load_func(ecs_mgr, id, reader);
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
	int32_t& alloc_size = (shared) ? _shared_alloc_size : _alloc_size;

	const ECSClassAttribute* const attr = ref_def.getClassAttr<ECSClassAttribute>();

	if (!attr) {
		// $TODO: Log warning.
		return false;
	}

	RefDefOffset::CopySharedFunc copy_shared_func = ref_def.getStaticFunc<void, const void*, void*>(Gaff::FNV1aHash32Const("CopyShared"));
	RefDefOffset::CopyFunc copy_func = ref_def.getStaticFunc<void, const void*, int32_t, void*, int32_t>(Gaff::FNV1aHash32Const("Copy"));
	RefDefOffset::LoadFunc load_func = ref_def.getStaticFunc<bool, ECSManager&, EntityID, const Gaff::ISerializeReader&>(Gaff::FNV1aHash32Const("Load"));

	if (!copy_shared_func && shared) {
		// $TODO: Log warning.
		return false;
	}

	if (!copy_func && !shared) {
		// $TODO: Log warning.
		return false;
	}

	if (copy_func && !shared && !load_func)
	{
		// $TODO: Log error.
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

		vars.emplace_back(RefDefOffset{ &ref_def, base_offset + size_offset, copy_shared_func, copy_func, load_func });

	// Inserting, bump up all the offsets.
	} else {
		const int32_t offset = it->offset;

		for (auto begin = it; begin != vars.end(); ++begin) {
			begin->offset += attr->size() * size_scalar;
		}

		vars.emplace(it, RefDefOffset{ &ref_def, offset, copy_shared_func, copy_func, load_func });
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

template <bool shared>
bool ECSArchetype::finalize(const Gaff::ISerializeReader& reader, const ECSArchetype* base_archetype)
{
	// Not doing a move?
	const auto guard = (shared) ?
		reader.enterElementGuard("shared_components") :
		reader.enterElementGuard("components");

	GAFF_REF(base_archetype); // Silence warning in non-shared version.
	bool read_data = true;

	// Not gonna lie, this would probably be easier to read with a goto.
	if (!reader.isNull()) {
		if (!reader.isObject()) {
			// $TODO: Log error
			read_data = false;
		}
	} else {
		read_data = false;
	}

	bool success = read_data;

	if (read_data) {
		const ReflectionManager& refl_mgr = GetApp().getReflectionManager();

		reader.forEachInObject([&](const char* component_name) -> bool
		{
			if (!reader.isObject()) {
				// $TODO: Log error
				success = false;
				return false;
			}

			const Gaff::Hash64 component_hash = Gaff::FNV1aHash64String(component_name);
			const Gaff::IReflectionDefinition* const ref_def = refl_mgr.getReflection(component_hash);

			if (!ref_def) {
				// $TODO: Log error
				success = false;
				return false;
			}

			const ECSClassAttribute* const ecs = ref_def->getClassAttr<ECSClassAttribute>();

			if (!ecs) {
				// $TODO: Log error
				success = false;
				return false;
			}

			if constexpr (shared) {
				if (!hasSharedComponent(component_hash)) {
					addShared(*ref_def);
				}
			} else {
				if (!hasComponent(component_hash)) {
					add(*ref_def);
				}
			}

			return false;
		});

		if constexpr (shared) {
			initShared(reader, base_archetype);
		}
	}

	return success;
}

void ECSArchetype::initShared(const Gaff::ISerializeReader& reader, const ECSArchetype* base_archetype)
{
	if (!_shared_alloc_size) {
		return;
	}

	ProxyAllocator allocator("ECS");
	_shared_instances = SHIB_ALLOC(_shared_alloc_size, allocator);
	//std::memset(_shared_instances, 0, _shared_alloc_size);

	// Should we prefer memset over this?
	for (const RefDefOffset& data : _shared_vars) {
		const auto ctor = data.ref_def->getConstructor<>();

		if (ctor) {
			ctor(reinterpret_cast<int8_t*>(_shared_instances) + data.offset);
		}
	}

	// Copy base archetype shared data so that our overrides will not be stomped.
	if (base_archetype) {
		copySharedInstanceData(*base_archetype);
	}

	int32_t index = 0;

	reader.forEachInObject([&](const char* component) -> bool
	{
		const RefDefOffset& data = _shared_vars[index];

		// Class had an issue and was not added to the archetype.
		if (data.ref_def->getReflectionInstance().getHash() != Gaff::FNV1aHash64String(component)) {
			return false;
		}

		++index;

		const auto ctor = data.ref_def->getConstructor<>();
		void* const instance = reinterpret_cast<int8_t*>(_shared_instances) + data.offset;

		if (ctor) {
			ctor(instance);
		}

		data.ref_def->load(reader, instance);
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
	//std::memset(_shared_instances, 0, _shared_alloc_size);

	// Should we prefer memset over this?
	for (const RefDefOffset& data : _shared_vars) {
		const auto ctor = data.ref_def->getConstructor<>();

		if (ctor) {
			ctor(reinterpret_cast<int8_t*>(_shared_instances) + data.offset);
		}
	}
}

void ECSArchetype::copySharedInstanceData(const ECSArchetype& old_archetype)
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
}

void ECSArchetype::destroySharedData(void)
{
	if (_shared_instances) {
		for (const RefDefOffset& rdo : _shared_vars) {
			if (rdo.ref_def->size() > 0) {
				rdo.ref_def->destroyInstance(reinterpret_cast<int8_t*>(_shared_instances) + rdo.offset);
			}
		}

		SHIB_FREE(_shared_instances, GetAllocator());
		_shared_instances = nullptr;
	}
}

NS_END
