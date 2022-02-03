/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_ECSManager.h"
#include "Shibboleth_ECSComponentCommon.h"
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Gaff_Function.h>
#include <Gaff_Math.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ECSManager)
	.base<Shibboleth::IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::ECSManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ECSManager)

ECSManager::~ECSManager(void)
{
	IAllocator& allocator = GetAllocator();

	_curr_scene = nullptr;

	for (auto& pages : _entity_pages) {
		SHIB_FREE(pages.second->arch_ref, allocator);
	}
}

bool ECSManager::initAllModulesLoaded(void)
{
	//const Vector<const Refl::IReflectionDefinition*>* const ecs_comps = GetApp().getReflectionManager().getAttributeBucket<ECSClassAttribute>();

	//if (ecs_comps) {
	//	// Cache all components.
	//}

	// Add empty base archetype.
	ECSArchetype default_archetype;
	
	if (!default_archetype.finalize(true)) {
		// $TODO: Log error. (somehow failed to initialize an empty archetype?)
		return false;
	}

	_empty_arch_res = GetApp().getManagerTFast<ResourceManager>().createResourceT<ECSArchetypeResource>(k_empty_archetype_res_name);
	addArchetype(std::move(default_archetype), _empty_arch_res->_archetype_ref);

	const Gaff::JSON starting_scene = GetApp().getConfigs()[u8"starting_scene"];

	if (!starting_scene.isNull() && !starting_scene.isString()) {
		// $TODO: Log error
		return false;

	} else if (starting_scene.isString()) {
		const char8_t* const scene = starting_scene.getString();
		_curr_scene = GetApp().getManagerTFast<ResourceManager>().requestResourceT<ECSSceneResource>(HashStringView64<>(scene, eastl::CharStrlen(scene)));
	}

	return true;
}

void ECSManager::addArchetype(ECSArchetype&& archetype, ArchetypeReferencePtr& out_ref)
{
	out_ref = addArchetypeInternal(std::move(archetype));
}

void ECSManager::addArchetype(ECSArchetype&& archetype)
{
	addArchetypeInternal(std::move(archetype));
}

void ECSManager::removeArchetype(Gaff::Hash64 archetype)
{
	const EA::Thread::AutoMutex lock(_entity_page_lock);
	const auto it = _entity_pages.find(archetype);

	if (it == _entity_pages.end()) {
		return;
	}

	// Someone is manually calling removeArchetype().
	if (!it->second->entity_ids.empty()) {
		SHIB_FREE(it->second->arch_ref, GetAllocator());

		for (EntityID id : it->second->entity_ids) {
			if (id != -1) {
				destroyEntityInternal(id, false);
			}
		}
	}

	for (int32_t query_index : it->second->queries) {
		_queries[query_index].removeArchetype(it->second.get());
	}

	_entity_pages.erase(it);
}

const ECSArchetype& ECSManager::getArchetype(Gaff::Hash64 archetype) const
{
	const EA::Thread::AutoMutex lock(_entity_page_lock);
	const auto it = _entity_pages.find(archetype);
	GAFF_ASSERT(it != _entity_pages.end() && it->first == archetype);
	return it->second->archetype;
}

const ECSArchetype& ECSManager::getArchetype(EntityID id) const
{
	GAFF_ASSERT(ValidEntityID(id) && id < _next_id && _entities[id].data);
	return _entities[id].data->archetype;
}

ArchetypeReferencePtr ECSManager::getArchetypeReference(Gaff::Hash64 archetype)
{
	const EA::Thread::AutoMutex lock(_entity_page_lock);
	const auto it = _entity_pages.find(archetype);
	GAFF_ASSERT(it != _entity_pages.end() && it->first == archetype);
	return ArchetypeReferencePtr(it->second->arch_ref);
}

ArchetypeReferencePtr ECSManager::getArchetypeReference(EntityID id)
{
	GAFF_ASSERT(id < _next_id && _entities[id].data);
	return ArchetypeReferencePtr(_entities[id].data->arch_ref);
}

EntityID ECSManager::createEntity(const ECSArchetype& archetype)
{
	return createEntity(archetype.getHash());
}

EntityID ECSManager::createEntity(Gaff::Hash64 archetype)
{
	const EA::Thread::AutoMutex lock(_entity_page_lock);
	const auto it = _entity_pages.find(archetype);
	GAFF_ASSERT(it != _entity_pages.end() && it->first == archetype);
	EntityData& data = *(it->second);

	EntityID id = EntityID_None;

	if (_free_ids.empty()) {
		id = _next_id++;
	} else {
		id = _free_ids.back();
		_free_ids.pop_back();
	}

	// _next_id should never be more than _entities.size() + 1.
	if (id >= static_cast<int32_t>(_entities.size())) {
		_entities.emplace_back();
	}

	Entity& entity = _entities[id];
	entity.data = &data;
	entity.index = allocateIndex(data, id);

	const int32_t page = entity.index / data.num_entities_per_page;

	entity.page = data.pages[page].get();
	entity.index -= page * data.num_entities_per_page;

	data.arch_ref->addRef();

	return id;
}

EntityID ECSManager::loadEntity(const ECSArchetype& archetype, const ISerializeReader& reader)
{
	if (!reader.isObject()) {
		// $TODO: Log error
		return EntityID_None;
	}

	const EntityID id = createEntity(archetype);

	if (ValidEntityID(id)) {
		archetype.loadDefaults(*this, id);

		reader.forEachInObject([&](const char8_t* component) -> bool {
			const Gaff::Hash64 comp_hash = Gaff::FNV1aHash64String(component);
			archetype.loadComponent(*this, id, reader, comp_hash);
			return false;
		});
	}

	return id;
}

EntityID ECSManager::loadEntity(Gaff::Hash64 archetype_hash, const ISerializeReader& reader)
{
	return loadEntity(getArchetype(archetype_hash), reader);
}

void ECSManager::destroyEntity(EntityID id)
{
	destroyEntityInternal(id, true);
}

const void* ECSManager::getComponentShared(Gaff::Hash64 archetype, const Refl::IReflectionDefinition& component) const
{
	return const_cast<ECSManager*>(this)->getComponentShared(archetype, component);
}

const void* ECSManager::getComponentShared(Gaff::Hash64 archetype, Gaff::Hash64 component) const
{
	return const_cast<ECSManager*>(this)->getComponentShared(archetype, component);
}

const void* ECSManager::getComponentShared(EntityID id, const Refl::IReflectionDefinition& component) const
{
	return const_cast<ECSManager*>(this)->getComponentShared(id, component);
}

const void* ECSManager::getComponentShared(EntityID id, Gaff::Hash64 component) const
{
	return const_cast<ECSManager*>(this)->getComponentShared(id, component);
}

void* ECSManager::getComponentShared(Gaff::Hash64 archetype, const Refl::IReflectionDefinition& component)
{
	return getComponentShared(archetype, component.getReflectionInstance().getHash());
}

void* ECSManager::getComponentShared(Gaff::Hash64 archetype, Gaff::Hash64 component)
{
	const EA::Thread::AutoMutex lock(_entity_page_lock);
	GAFF_ASSERT(_entity_pages.find(archetype) != _entity_pages.end());
	auto& archetype_instance = _entity_pages[archetype]->archetype;

	const int32_t component_offset = archetype_instance.getComponentSharedOffset(component);
	GAFF_ASSERT(component_offset > -1);

	return reinterpret_cast<int8_t*>(archetype_instance.getSharedData()) + component_offset;
}

void* ECSManager::getComponentShared(EntityID id, const Refl::IReflectionDefinition& component)
{
	return getComponentShared(id, component.getReflectionInstance().getHash());
}

void* ECSManager::getComponentShared(EntityID id, Gaff::Hash64 component)
{
	GAFF_ASSERT(id < _next_id && _entities[id].data);
	const Entity& entity = _entities[id];

	const int32_t component_offset = entity.data->archetype.getComponentSharedOffset(component);
	GAFF_ASSERT(component_offset > -1);

	return reinterpret_cast<int8_t*>(entity.data->archetype.getSharedData()) + component_offset;
}

const void* ECSManager::getComponent(EntityID id, const Refl::IReflectionDefinition& component) const
{
	return const_cast<ECSManager*>(this)->getComponent(id, component);
}

const void* ECSManager::getComponent(EntityID id, Gaff::Hash64 component) const
{
	return const_cast<ECSManager*>(this)->getComponent(id, component);
}

void* ECSManager::getComponent(EntityID id, const Refl::IReflectionDefinition& component)
{
	return getComponent(id, component.getReflectionInstance().getHash());
}

void* ECSManager::getComponent(EntityID id, Gaff::Hash64 component)
{
	GAFF_ASSERT(id < _next_id && _entities[id].data);
	const Entity& entity = _entities[id];

	const int32_t component_offset = entity.data->archetype.getComponentOffset(component);

	if (component_offset < 0) {
		return nullptr;
	}

	const int32_t entity_offset = (entity.index / 4) * entity.data->archetype.size() * 4;

	return reinterpret_cast<int8_t*>(entity.page) + sizeof(EntityPage) + entity_offset + component_offset;
}

bool ECSManager::hasComponent(Gaff::Hash64 archetype, const Refl::IReflectionDefinition& component) const
{
	return hasComponent(archetype, component.getReflectionInstance().getHash());
}

bool ECSManager::hasComponent(Gaff::Hash64 archetype, Gaff::Hash64 component) const
{
	const EA::Thread::AutoMutex lock(_entity_page_lock);
	const auto it = _entity_pages.find(archetype);

	GAFF_ASSERT(it != _entity_pages.end());

	auto& archetype_instance = it->second->archetype;
	return archetype_instance.hasComponent(component);
}

bool ECSManager::hasComponent(EntityID id, const Refl::IReflectionDefinition& component) const
{
	return getComponent(id, component) != nullptr;
}

bool ECSManager::hasComponent(EntityID id, Gaff::Hash64 component) const
{
	return getComponent(id, component) != nullptr;
}

int32_t ECSManager::getComponentIndex(const ECSQueryResult& query_result, int32_t entity_index) const
{
	EntityData* const data = reinterpret_cast<EntityData*>(query_result.entity_data);
	GAFF_ASSERT(entity_index < static_cast<int32_t>(data->entity_ids.size()));

	return getComponentIndex(data->entity_ids[entity_index]);
}

int32_t ECSManager::getComponentIndex(EntityID id) const
{
	GAFF_ASSERT(ValidEntityID(id) && id < _next_id&& _entities[id].data);
	return _entities[id].index % 4;
}

int32_t ECSManager::getPageIndex(const ECSQueryResult& query_result, int32_t entity_index) const
{
	EntityData* const data = reinterpret_cast<EntityData*>(query_result.entity_data);
	GAFF_ASSERT(entity_index < static_cast<int32_t>(data->entity_ids.size()));

	return getPageIndex(data->entity_ids[entity_index]);
}

int32_t ECSManager::getPageIndex(EntityID id) const
{
	GAFF_ASSERT(ValidEntityID(id) && id < _next_id && _entities[id].data);
	return _entities[id].index;
}

const void* ECSManager::getComponent(const ECSQueryResult& query_result, int32_t entity_index) const
{
	return const_cast<ECSManager*>(this)->getComponent(query_result, entity_index);
}

void* ECSManager::getComponent(const ECSQueryResult& query_result, int32_t entity_index)
{
	EntityData* const data = reinterpret_cast<EntityData*>(query_result.entity_data);
	GAFF_ASSERT(entity_index < static_cast<int32_t>(data->entity_ids.size()));

	const int32_t page_index = entity_index / data->num_entities_per_page;
	EntityPage* const page = data->pages[page_index].get();

	entity_index -= page_index * data->num_entities_per_page;
	const int32_t entity_offset = (entity_index / 4) * data->archetype.size() * 4;

	return reinterpret_cast<int8_t*>(page) + sizeof(EntityPage) + entity_offset + query_result.component_offset;
}

int32_t ECSManager::getNumEntities(const ECSQueryResult& query_result) const
{
	return reinterpret_cast<const EntityData*>(query_result.entity_data)->num_entities;
}

void ECSManager::registerQuery(ECSQuery&& query)
{
	ECSQuery& new_query = _queries.emplace_back(std::move(query));

	for (auto& data : _entity_pages) {
		EntityData* const entity_data = data.second.get();
		
		if (!entity_data->archetype.isBase() && new_query.filter(entity_data->archetype, entity_data)) {
			entity_data->queries.emplace_back(static_cast<int32_t>(_queries.size() - 1));
		}
	}
}

const ECSSceneResourcePtr& ECSManager::getCurrentScene(void) const
{
	return _curr_scene;
}

void ECSManager::switchScene(const ECSSceneResourcePtr scene)
{
	_curr_scene = scene;
}

bool ECSManager::isValid(EntityID id) const
{
	return ValidEntityID(id) && id < _next_id;
}

const ECSArchetype& ECSManager::getEmptyArchetype(void) const
{
	return getArchetype(Gaff::k_init_hash64);
}

void ECSManager::destroyEntityInternal(EntityID id, bool change_ref_count)
{
	const EA::Thread::AutoMutex lock(_entity_page_lock);
	GAFF_ASSERT(id < _next_id && _entities[id].data);
	Entity& entity = _entities[id];

	--entity.data->num_entities;
	--entity.page->num_entities;

	const auto it = Gaff::Find(entity.data->pages, entity.page, [](const auto& lhs, const EntityPage* rhs) -> bool { return lhs.get() == rhs; });
	const int32_t page_index = static_cast<int32_t>(eastl::distance(entity.data->pages.begin(), it));
	const int32_t page_ids_start = page_index * entity.data->num_entities_per_page;
	const int32_t global_index = entity.index + page_ids_start;

	const int32_t entity_offset = (entity.index / 4) * entity.data->archetype.size() * 4;
	void* const comp_data = reinterpret_cast<int8_t*>(entity.page) + sizeof(EntityPage) + entity_offset;

	entity.data->archetype.destroyEntity(id, comp_data, entity.index % 4);

	if (entity.page->num_entities > 0) {
		entity.data->free_indices.emplace_back(global_index);
		entity.data->entity_ids[global_index] = -1;

	} else {
		int32_t size = static_cast<int32_t>(entity.data->free_indices.size());

		for (int32_t i = 0; i < size;) {
			const int32_t free_index_page_index = entity.data->free_indices[i] / entity.data->num_entities_per_page;

			if (page_index == free_index_page_index) {
				entity.data->free_indices.erase(entity.data->free_indices.begin() + i);
				--size;
			} else {
				++i;
			}
		}

		const auto begin = entity.data->entity_ids.begin() + page_ids_start;

		entity.data->entity_ids.erase(begin, begin + entity.data->num_entities_per_page);
		entity.data->pages.erase(entity.data->pages.begin() + page_index);
	}

	EntityData& data = *entity.data;
	entity.page = nullptr;
	entity.data = nullptr;
	entity.index = -1;

	_free_ids.emplace_back(id);

	if (change_ref_count) {
		data.arch_ref->release();
	}
}

void ECSManager::migrate(EntityID id, Gaff::Hash64 new_archetype)
{
	const EA::Thread::AutoMutex lock(_entity_page_lock);
	GAFF_ASSERT(_entity_pages.find(new_archetype) != _entity_pages.end());
	EntityData& data = *_entity_pages[new_archetype];
	Entity& entity = _entities[id];

	const int32_t global_index = allocateIndex(data, id);
	const int32_t page = global_index / data.num_entities_per_page;
	const int32_t new_index = global_index - page * data.num_entities_per_page;

	const int32_t old_entity_offset = (entity.index / 4) * entity.data->archetype.size() * 4;
	const int32_t new_entity_offset = (new_index / 4) * data.archetype.size() * 4;

	EntityPage* const new_page = data.pages[page].get();
	void* const old_data = reinterpret_cast<int8_t*>(entity.page) + sizeof(EntityPage) + old_entity_offset;
	void* const new_data = reinterpret_cast<int8_t*>(new_page) + sizeof(EntityPage) + new_entity_offset;

	data.archetype.copy(entity.data->archetype, old_data, entity.index % 4, new_data, new_index % 4);

	// Remove the old entity from the old archetype.
	EntityData& old_entity_data = *entity.data;
	destroyEntityInternal(id, false);

	_free_ids.pop_back(); // We still want to use this ID.

	entity.data = &data;
	entity.page = new_page;
	entity.index = new_index;

	data.arch_ref->addRef();
	old_entity_data.arch_ref->release();
}

int32_t ECSManager::allocateIndex(EntityData& data, EntityID id)
{
	// Grab a free ID if available.
	if (!data.free_indices.empty()) {
		const int32_t index = data.free_indices.back();
		data.free_indices.pop_back();

		const int32_t page = index / data.num_entities_per_page;
		++data.pages[page]->num_entities;
		++data.num_entities;

		data.entity_ids[index] = id;

		return index;
	}

	// Try to allocate a new ID on an existing page.
	if (!data.pages.empty()) {
		auto& page = data.pages.back();

		if (page->next_index < data.num_entities_per_page) {
			const int32_t index = page->next_index++;
			++page->num_entities;
			++data.num_entities;

			const int32_t global_index = index + static_cast<int32_t>(data.pages.size() - 1) * data.num_entities_per_page;
			data.entity_ids[global_index] = id;

			return global_index;
		}
	}

	// Didn't find a free index. Need to allocate a new page.
	ProxyAllocator allocator("ECS");
	EntityPage* const page = reinterpret_cast<EntityPage*>(SHIB_ALLOC_ALIGNED(data.page_size, 16, allocator));

	memset(page, 1, data.page_size);

	data.archetype.constructPage(page + 1, data.num_entities_per_page);

	page->num_entities = 1;
	page->next_index = 1;

	data.pages.emplace_back(page);
	++data.num_entities;

	const int32_t global_index = static_cast<int32_t>(data.pages.size() - 1) * data.num_entities_per_page;
	data.entity_ids.resize(data.entity_ids.size() + static_cast<size_t>(data.num_entities_per_page), -1);
	data.entity_ids[global_index] = id;

	return global_index;
}

ArchetypeReference* ECSManager::modifyInternal(EntityID& id, ArchetypeModifier modifier)
{
	GAFF_ASSERT(id < _next_id && _entities[id].data);

	const ECSArchetype& base_archetype = getArchetype(id);
	ECSArchetype archetype;

	archetype.copy(base_archetype);

	if (modifier.removeShared) {
		modifier.removeShared(archetype);
	}

	if (modifier.remove) {
		modifier.remove(archetype);
	}

	if (modifier.addShared) {
		modifier.addShared(archetype);
	}

	if (modifier.add) {
		modifier.add(archetype);
	}

	archetype.finalize(base_archetype);

	ArchetypeReference* const arch_ref = addArchetypeInternal(std::move(archetype));
	migrate(id, archetype.getHash());

	return arch_ref;
}

ArchetypeReference* ECSManager::addArchetypeInternal(ECSArchetype&& archetype)
{
	const Gaff::Hash64 archetype_hash = archetype.getHash();
	const EA::Thread::AutoMutex lock(_entity_page_lock);

	if (const auto it = _entity_pages.find(archetype_hash); it != _entity_pages.end()) {
		return it->second->arch_ref;
	}

	const int32_t entity_size = archetype.size();
	int32_t page_size_bytes = EA_KIBIBYTE(64);

	if (const PageSize* const page_size = archetype.getSharedComponent<PageSize>()) {
		page_size_bytes = Gaff::Max(page_size->value, static_cast<int32_t>(sizeof(EntityPage)) + entity_size * 4);
	}

	ProxyAllocator allocator("ECS");

	EntityData* const data = SHIB_ALLOCT(EntityData, allocator);

	if (archetype.isBase()) {
		data->num_entities_per_page = 0;
		data->page_size = 0;

	} else {
		data->num_entities_per_page = (page_size_bytes - sizeof(EntityPage)) / entity_size;
		// Scale num_entities_per_page down to the nearest multiple of 4.
		data->num_entities_per_page = static_cast<int32_t>(floor(static_cast<float>(data->num_entities_per_page) / 4.0f) * 4.0f);

		// If even after 64kb of data we still have less than 4 entities, then our entities are seriously way too big.
		GAFF_ASSERT(data->num_entities_per_page >= 4);
		data->page_size = page_size_bytes;
	}

	data->arch_ref = SHIB_ALLOCT(ArchetypeReference, allocator, *this, archetype_hash);
	data->archetype = std::move(archetype);

	_entity_pages[archetype_hash].reset(data);

	if (!data->archetype.isBase()) {
		for (int32_t i = 0; i < static_cast<int32_t>(_queries.size()); ++i) {
			if (!data->archetype.isBase() && _queries[i].filter(data->archetype, data)) {
				data->queries.emplace_back(i);
			}
		}
	}

	return data->arch_ref;
}

NS_END
