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

#include "Shibboleth_ECSArchetype.h"
#include "Shibboleth_ECSAttributes.h"
#include "Shibboleth_ECSManager.h"
#include <Gaff_SerializeInterfaces.h>

NS_SHIBBOLETH

template <class Ret, class... Args>
static Gaff::IReflectionDefinition::TemplateFunc<Ret, Args...> GetStaticFunc(const Gaff::IReflectionDefinition& ref_def, Gaff::Hash32 name)
{
	const auto* const func = ref_def.getStaticFunc<Ret, Args...>(name);
	return (func) ? static_cast<const Gaff::IReflectionStaticFunction<Ret, Args...>*>(func)->getFunc() : nullptr;
}

ECSArchetype::ECSArchetype(ECSArchetype&& archetype):
	_shared_vars(std::move(archetype._shared_vars)),
	_vars(std::move(archetype._vars)),
	_vars_defaults(std::move(archetype._vars_defaults)),
	_hash(archetype._hash),
	_shared_alloc_size(archetype._shared_alloc_size),
	_alloc_size(archetype._alloc_size),
	_shared_instances(archetype._shared_instances),
	_default_data(archetype._default_data),
	_is_base(archetype._is_base)
{
	archetype._shared_instances = nullptr;
	archetype._default_data = nullptr;
}

ECSArchetype& ECSArchetype::operator=(ECSArchetype&& rhs)
{
	_shared_vars = std::move(rhs._shared_vars);
	_vars = std::move(rhs._vars);
	_vars_defaults = std::move(rhs._vars_defaults);
	_hash = rhs._hash;
	_shared_alloc_size = rhs._shared_alloc_size;
	_alloc_size = rhs._alloc_size;
	_shared_instances = rhs._shared_instances;
	_default_data = rhs._default_data;
	_is_base = rhs._is_base;

	rhs._shared_instances = nullptr;
	rhs._default_data = nullptr;

	return *this;
}

ECSArchetype::~ECSArchetype(void)
{
	destroyDefaultData();
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

bool ECSArchetype::finalize(const Gaff::ISerializeReader& reader, const ECSArchetype& base_archetype, bool read_default_overrides)
{
	destroyDefaultData();
	destroySharedData();

	bool success = finalize<true>(reader, &base_archetype, read_default_overrides);
	success = success && finalize<false>(reader, &base_archetype, read_default_overrides);
	calculateHash();

	_is_base = false;

	return success;
}

bool ECSArchetype::finalize(const Gaff::ISerializeReader& reader)
{
	if (!reader.isObject()) {
		// $TODO: Log error.
		return false;
	}

	destroyDefaultData();
	destroySharedData();

	bool success = finalize<true>(reader, nullptr, true);
	success = success && finalize<false>(reader, nullptr, true);
	calculateHash();

	const Gaff::ScopeGuard guard = reader.enterElementGuard("is_base");
	_is_base = reader.readBool(false);

	return success;
}

bool ECSArchetype::finalize(const ECSArchetype& base_archetype)
{
	destroyDefaultData();
	destroySharedData();
	initShared();
	copySharedInstanceData(base_archetype);
	initDefaultData();
	copyDefaultData(base_archetype);
	calculateHash();

	_is_base = false;

	return true;
}

bool ECSArchetype::finalize(bool is_base)
{
	destroyDefaultData();
	destroySharedData();
	initDefaultData();
	initShared();
	calculateHash();
	_is_base = is_base;

	return true;
}

bool ECSArchetype::add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs, bool has_default_values)
{
	return add<false>(ref_defs, has_default_values);
}

bool ECSArchetype::add(const Gaff::IReflectionDefinition& ref_def, bool has_default_value)
{
	return add<false>(ref_def, has_default_value);
}

bool ECSArchetype::remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	return remove<false>(ref_defs);
}

bool ECSArchetype::remove(const Gaff::IReflectionDefinition& ref_def)
{
	return remove<false>(ref_def);
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

void ECSArchetype::copy(const ECSArchetype& base, bool copy_shared_instance_data, bool copy_default_data)
{
	destroyDefaultData();
	destroySharedData();

	_shared_alloc_size = base._shared_alloc_size;
	_alloc_size = base._alloc_size;

	_shared_vars = base._shared_vars;
	_vars = base._vars;

	_vars_defaults = base._vars_defaults;

	_hash = Gaff::INIT_HASH64;

	if (copy_shared_instance_data) {
		initShared();
		copySharedInstanceData(base);
	}

	if (copy_default_data) {
		initDefaultData();
		copyDefaultData(base);
	}
}

void ECSArchetype::copy(
	const ECSArchetype& old_archetype,
	void* old_data,
	int32_t old_index,
	void* new_data,
	int32_t new_index)
{
	copySharedInstanceData(old_archetype);
	copyDefaultData(old_archetype);

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
	const auto it = Gaff::LowerBound(_shared_vars, component, SearchPredicate);
	return (it == _shared_vars.end() || it->ref_def->getReflectionInstance().getHash() != component) ? -1 : it->offset;
}

int32_t ECSArchetype::getComponentOffset(Gaff::Hash64 component) const
{
	const auto it = Gaff::LowerBound(_vars, component, SearchPredicate);
	return (it == _vars.end() || it->ref_def->getReflectionInstance().getHash() != component) ? -1 : it->offset;
}

bool ECSArchetype::hasSharedComponent(Gaff::Hash64 component) const
{
	const auto it = Gaff::LowerBound(_shared_vars, component, SearchPredicate);
	return (it != _shared_vars.end() && it->ref_def->getReflectionInstance().getHash() == component);
}

bool ECSArchetype::hasComponent(Gaff::Hash64 component) const
{
	const auto it = Gaff::LowerBound(_vars, component, SearchPredicate);
	return (it != _vars.end() && it->ref_def->getReflectionInstance().getHash() == component);
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

	//if (_shared_instances) {
	//	_hash = Gaff::FNV1aHash64(reinterpret_cast<char*>(_shared_instances), static_cast<size_t>(_shared_alloc_size), _hash);
	//}

	for (const RefDefOffset& data : _shared_vars) {
		const Gaff::Hash64 hash = data.ref_def->getReflectionInstance().getHash();
		_hash = Gaff::FNV1aHash64T(hash, _hash);
		_hash = data.ref_def->getInstanceHash(reinterpret_cast<const int8_t*>(_shared_instances) + data.offset, _hash);
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

	const auto it = Gaff::LowerBound(_vars, component, SearchPredicate);

	if (it != _vars.end() && it->ref_def->getReflectionInstance().getHash() == component && it->load_func) {
		return it->load_func(ecs_mgr, id, reader);
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

void ECSArchetype::loadDefaults(ECSManager& ecs_mgr, EntityID id) const
{
	for (const RefDefOffset& rdo : _vars_defaults) {
		rdo.copy_default_to_non_shared_func(ecs_mgr, id, static_cast<int8_t*>(_default_data) + rdo.offset);
	}
}

bool ECSArchetype::isBase(void) const
{
	return _is_base;
}

void ECSArchetype::destroyEntity(EntityID id, void* entity, int32_t entity_index) const
{
	for (const RefDefOffset& rdo : _vars) {
		if (rdo.destructor_func) {
			rdo.destructor_func(id, reinterpret_cast<int8_t*>(entity) + rdo.offset, entity_index);
		}

		if (rdo.constructor_func) {
			rdo.constructor_func(EntityID_None, reinterpret_cast<int8_t*>(entity) + rdo.offset, entity_index);
		}
	}
}

void ECSArchetype::constructPage(void* page, int32_t num_entities) const
{
	int8_t* entity_start = reinterpret_cast<int8_t*>(page);
	const int32_t entity_size = size();

	for (int32_t i = 0; i < num_entities; i += 4) {
		for (const RefDefOffset& rdo : _vars) {
			if (rdo.constructor_func) {
				rdo.constructor_func(EntityID_None, entity_start + rdo.offset, 0);
				rdo.constructor_func(EntityID_None, entity_start + rdo.offset, 1);
				rdo.constructor_func(EntityID_None, entity_start + rdo.offset, 2);
				rdo.constructor_func(EntityID_None, entity_start + rdo.offset, 3);
			}
		}

		entity_start += entity_size * 4;
	}
}

template <bool shared>
bool ECSArchetype::add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs, bool has_default_values)
{
	bool success = true;

	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		success = success && add<shared>(*ref_def, has_default_values);
	}

	return success;
}

template <bool shared>
bool ECSArchetype::add(const Gaff::IReflectionDefinition& ref_def, bool has_default_value)
{
	if constexpr (shared) {
		const auto is_shared = ref_def.getStaticFunc<bool>(Gaff::FNV1aHash32Const("IsShared"));

		if (!is_shared || !is_shared->call()) {
			// $TODO: Log error.
			return false;
		}

	} else {
		GAFF_ASSERT(!ref_def.isPolymorphic());

		const auto is_non_shared = ref_def.getStaticFunc<bool>(Gaff::FNV1aHash32Const("IsNonShared"));

		if (!is_non_shared || !is_non_shared->call()) {
			// $TODO: Log error.
			return false;
		}
	}

	Vector<RefDefOffset>& vars = (shared) ? _shared_vars : _vars;
	int32_t& alloc_size = (shared) ? _shared_alloc_size : _alloc_size;

	const auto it = Gaff::LowerBound(vars, ref_def.getReflectionInstance().getHash(), SearchPredicate);

	// Already have this component, do nothing.
	if (it != vars.end() && it->ref_def == &ref_def) {
		return true;
	}

	const ECSClassAttribute* const attr = ref_def.getClassAttr<ECSClassAttribute>();

	if (!attr) {
		// $TODO: Log error.
		return false;
	}

	RefDefOffset::CopyDefaultToNonSharedFunc copy_default_to_non_shared_func = GetStaticFunc<void, ECSManager&, EntityID, const void*>(ref_def, Gaff::FNV1aHash32Const("CopyDefaultToNonShared"));
	RefDefOffset::CopySharedFunc copy_shared_func = GetStaticFunc<void, const void*, void*>(ref_def, Gaff::FNV1aHash32Const("CopyShared"));
	RefDefOffset::CopyFunc copy_func = GetStaticFunc<void, const void*, int32_t, void*, int32_t>(ref_def, Gaff::FNV1aHash32Const("Copy"));
	RefDefOffset::LoadFunc load_func = GetStaticFunc<bool, ECSManager&, EntityID, const Gaff::ISerializeReader&>(ref_def, Gaff::FNV1aHash32Const("Load"));
	RefDefOffset::ConstructorFunc constructor_func = GetStaticFunc<void, EntityID, void*, int32_t>(ref_def, Gaff::FNV1aHash32Const("Constructor"));
	RefDefOffset::DestructorFunc destructor_func = GetStaticFunc<void, EntityID, void*, int32_t>(ref_def, Gaff::FNV1aHash32Const("Destructor"));

	if (!copy_shared_func && shared) {
		// $TODO: Log error.
		return false;
	}

	if (!copy_func && !shared) {
		// $TODO: Log error.
		return false;
	}

	if (copy_func && !shared && !load_func) {
		// $TODO: Log error.
		return false;
	}

	if (copy_shared_func && copy_func && !copy_default_to_non_shared_func) {
		// $TODO: Log error.
		return false;
	}

	constexpr int32_t size_scalar = (shared) ? 1 : 4;

	// Simple append.
	if (it == vars.end()) {
		const int32_t size_offset = (vars.empty()) ? 0 : (vars.back().ref_def->size() * size_scalar);
		const int32_t base_offset = (vars.empty()) ? 0 : vars.back().offset;

		vars.emplace_back(RefDefOffset{ &ref_def, base_offset + size_offset, copy_default_to_non_shared_func, copy_shared_func, copy_func, load_func, constructor_func, destructor_func });

	// Inserting, bump up all the offsets.
	} else {
		const int32_t offset = it->offset;

		for (auto begin = it; begin != vars.end(); ++begin) {
			begin->offset += ref_def.size() * size_scalar;
		}

		vars.emplace(it, RefDefOffset{ &ref_def, offset, copy_default_to_non_shared_func, copy_shared_func, copy_func, load_func, constructor_func, destructor_func });
	}

	// If not shared, add to the default values list if applicable.
	if constexpr (shared) {
		GAFF_REF(has_default_value);

	} else {
		// Simple append.
		if (has_default_value && ref_def.size() > 0) {
			const auto it_default = Gaff::LowerBound(_vars_defaults, ref_def.getReflectionInstance().getHash(), SearchPredicate);

			if (it_default == _vars_defaults.end()) {
				const int32_t size_offset = (_vars_defaults.empty()) ? 0 : _vars_defaults.back().ref_def->size();
				const int32_t base_offset = (_vars_defaults.empty()) ? 0 : _vars_defaults.back().offset;

				_vars_defaults.emplace_back(RefDefOffset{ &ref_def, base_offset + size_offset, copy_default_to_non_shared_func, copy_shared_func, copy_func, load_func, constructor_func, destructor_func });

			// Inserting, bump up all the offsets.
			} else {
				const int32_t offset = it_default->offset;

				for (auto begin = it_default; begin != _vars_defaults.end(); ++begin) {
					begin->offset += ref_def.size();
				}

				_vars_defaults.emplace(it_default, RefDefOffset{ &ref_def, offset, copy_default_to_non_shared_func, copy_shared_func, copy_func, load_func, constructor_func, destructor_func });
			}
		}
	}

	alloc_size += ref_def.size();
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
	int32_t& alloc_size = (shared) ? _shared_alloc_size : _alloc_size;
	constexpr int32_t size_scalar = (shared) ? 1 : 4;

	const Gaff::Hash64 component_hash = ref_def.getReflectionInstance().getHash();
	const auto it = Gaff::LowerBound(vars, component_hash, SearchPredicate);

	if (it == vars.end() || it->ref_def != &ref_def) {
		// $TODO: Log error.
		return false;
	}

	const int32_t data_size = ref_def.size();
	alloc_size -= data_size;

	const auto it_end = vars.end();

	// Reduce all the offsets.
	for (auto it_2 = it + 1; it_2 != it_end; ++it_2) {
		it_2->offset -= data_size * size_scalar;
	}

	vars.erase(it);

	if constexpr (!shared) {
		const auto it_default = Gaff::LowerBound(_vars_defaults, component_hash, SearchPredicate);

		if (it_default != _vars_defaults.end() && it_default->ref_def == &ref_def) {
			const int32_t default_data_size = it_default->ref_def->size();
			const auto it_default_end = _vars_defaults.end();

			// Reduce all the offsets.
			for (auto it_default_2 = it_default + 1; it_default_2 != it_default_end; ++it_default_2) {
				it_default_2->offset -= default_data_size;
			}

			_vars_defaults.erase(it_default);
		}
	}

	return true;
}

template <bool shared>
bool ECSArchetype::finalize(const Gaff::ISerializeReader& reader, const ECSArchetype* base_archetype, bool read_default_overrides)
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
				addShared(*ref_def);
			} else {
				add(*ref_def, !reader.isNull());
			}

			return false;
		});

		if constexpr (shared) {
			initShared(reader, base_archetype);
			GAFF_REF(read_default_overrides);
		} else {
			initDefaultData(reader, base_archetype, read_default_overrides);
		}
	}

	return success;
}

void ECSArchetype::initShared(const Gaff::ISerializeReader& reader, const ECSArchetype* base_archetype)
{
	initShared();

	if (!_shared_instances) {
		return;
	}

	// Copy base archetype shared data so that our overrides will not be stomped.
	if (base_archetype) {
		copySharedInstanceData(*base_archetype);
	}

	reader.forEachInObject([&](const char* component) -> bool
	{
		const Gaff::Hash64 component_hash = Gaff::FNV1aHash64String(component);

		const auto it = Gaff::LowerBound(_shared_vars, component_hash, SearchPredicate);

		if (it == _shared_vars.end() || it->ref_def->getReflectionInstance().getHash() != component_hash)
		{
			return false;
		}

		void* const instance = reinterpret_cast<int8_t*>(_shared_instances) + it->offset;
		it->ref_def->load(reader, instance);

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
		const auto ctor = data.ref_def->getConstructor<>();

		if (ctor) {
			ctor(reinterpret_cast<int8_t*>(_shared_instances) + data.offset);
		}
	}
}

void ECSArchetype::copySharedInstanceData(const ECSArchetype& old_archetype)
{
	const auto it_end = _shared_vars.end();

	for (const RefDefOffset& rdo : old_archetype._shared_vars) {
		const auto it = Gaff::LowerBound(_shared_vars, rdo.ref_def->getReflectionInstance().getHash(), SearchPredicate);

		// Component was deleted.
		if (it == it_end || it->ref_def != rdo.ref_def) {
			continue;
		}

		const void* const old_component = reinterpret_cast<int8_t*>(old_archetype._shared_instances) + rdo.offset;
		void* const new_component = reinterpret_cast<int8_t*>(_shared_instances) + it->offset;

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

void ECSArchetype::initDefaultData(const Gaff::ISerializeReader& reader, const ECSArchetype* base_archetype, bool read_default_overrides)
{
	initDefaultData();

	if (!_default_data) {
		return;
	}

	// Copy base archetype shared data so that our overrides will not be stomped.
	if (base_archetype) {
		copyDefaultData(*base_archetype);
	}

	if (!read_default_overrides) {
		return;
	}

	reader.forEachInObject([&](const char* component) -> bool
	{
		const Gaff::Hash64 component_hash = Gaff::FNV1aHash64String(component);

		const auto it = Gaff::LowerBound(_vars_defaults, component_hash, SearchPredicate);

		if (it == _vars_defaults.end() || it->ref_def->getReflectionInstance().getHash() != component_hash)
		{
			return false;
		}

		const RefDefOffset& data = *it;

		void* const instance = reinterpret_cast<int8_t*>(_default_data) + data.offset;

		if (!reader.isNull() && reader.size() > 0) {
			data.ref_def->load(reader, instance);
		}

		return false;
	});
}

void ECSArchetype::initDefaultData(void)
{
	if (_vars_defaults.empty()) {
		return;
	}

	int32_t alloc_size = 0;

	for (const RefDefOffset& rdo : _vars_defaults) {
		alloc_size += rdo.ref_def->size();
	}

	ProxyAllocator allocator("ECS");
	_default_data = SHIB_ALLOC(alloc_size, allocator);

	for (const RefDefOffset& data : _vars_defaults) {
		const auto ctor = data.ref_def->getConstructor<>();

		if (ctor) {
			ctor(reinterpret_cast<int8_t*>(_default_data) + data.offset);
		}
	}
}

void ECSArchetype::copyDefaultData(const ECSArchetype& old_archetype)
{
	const auto it_end = _vars_defaults.end();

	for (const RefDefOffset& rdo : old_archetype._vars_defaults) {
		const auto it = Gaff::LowerBound(_vars_defaults, rdo.ref_def->getReflectionInstance().getHash(), SearchPredicate);

		// Component was deleted.
		if (it == it_end || it->ref_def != rdo.ref_def) {
			continue;
		}

		const void* const old_component = reinterpret_cast<int8_t*>(old_archetype._default_data) + rdo.offset;
		void* const new_component = reinterpret_cast<int8_t*>(_default_data) + it->offset;

		rdo.copy_shared_func(old_component, new_component);
	}
}

void ECSArchetype::destroyDefaultData(void)
{
	if (_default_data) {
		SHIB_FREE(_default_data, GetAllocator());
		_default_data = nullptr;
	}
}

bool ECSArchetype::SearchPredicate(const RefDefOffset& lhs, Gaff::Hash64 rhs)
{
	return lhs.ref_def->getReflectionInstance().getHash() < rhs;
}


ArchetypeReference::ArchetypeReference(ECSManager& ecs_mgr, Gaff::Hash64 archetype):
	_ecs_mgr(ecs_mgr),
	_archetype(archetype)
{
}

ArchetypeReference::~ArchetypeReference(void)
{
	_ecs_mgr.removeArchetype(_archetype);
}

const ECSArchetype& ArchetypeReference::getArchetype(void) const
{
	return _ecs_mgr.getArchetype(_archetype);
}

Gaff::Hash64 ArchetypeReference::getArchetypeHash(void) const
{
	return _archetype;
}

NS_END
