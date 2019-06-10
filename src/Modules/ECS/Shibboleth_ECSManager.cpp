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

#include "Shibboleth_ECSManager.h"
#include "Shibboleth_ECSAttributes.h"
#include "Shibboleth_IECSSystem.h"
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Gaff_Function.h>

SHIB_REFLECTION_DEFINE(ECSManager)

NS_SHIBBOLETH

ECSManager::ArchetypeReference::ArchetypeReference(ECSManager& ecs_mgr, Gaff::Hash64 archetype):
	_ecs_mgr(ecs_mgr),
	_archetype(archetype)
{
}

ECSManager::ArchetypeReference::~ArchetypeReference(void)
{
	_ecs_mgr.removeArchetype(_archetype);
}

const ECSArchetype& ECSManager::ArchetypeReference::getArchetype(void) const
{
	return _ecs_mgr.getArchetype(_archetype);
}

Gaff::Hash64 ECSManager::ArchetypeReference::getArchetypeHash(void) const
{
	return _archetype;
}



SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ECSManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(ECSManager)

ECSManager::~ECSManager(void)
{
	IAllocator& allocator = GetAllocator();

	for (auto& pages : _entity_pages) {
		SHIB_FREE(pages.second->arch_ref, allocator);
	}
}

bool ECSManager::init(void)
{
	IApp& app = GetApp();
	const auto* const ecs_systems = app.getReflectionManager().getTypeBucket(CLASS_HASH(IECSSystem));

	if (!ecs_systems || ecs_systems->empty()) {
		return true;
	}

	ProxyAllocator allocator("ECS");
	SerializeReaderWrapper readerWrapper(allocator);
	
	if (!OpenJSONOrMPackFile(readerWrapper, "cfg/update_phases.cfg")) {
		LogErrorDefault("Failed to read cfg/update_phases.cfg[.bin].");
		return false;
	}

	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();

	// $TODO: Setup system operation order.
	GAFF_REF(reader);

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

	_entity_pages.erase(it);
}

const ECSArchetype& ECSManager::getArchetype(Gaff::Hash64 archetype) const
{
	const auto it = _entity_pages.find(archetype);
	GAFF_ASSERT(it != _entity_pages.end() && it->first == archetype);
	return it->second->archetype;
}

const ECSArchetype& ECSManager::getArchetype(EntityID id) const
{
	GAFF_ASSERT(id < _next_id && _entities[id].data);
	return _entities[id].data->archetype;
}

ECSManager::ArchetypeReferencePtr ECSManager::getArchetypeReference(Gaff::Hash64 archetype)
{
	const auto it = _entity_pages.find(archetype);
	GAFF_ASSERT(it != _entity_pages.end() && it->first == archetype);
	return ArchetypeReferencePtr(it->second->arch_ref);
}

ECSManager::ArchetypeReferencePtr ECSManager::getArchetypeReference(EntityID id)
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
	const auto it = _entity_pages.find(archetype);
	GAFF_ASSERT(it != _entity_pages.end() && it->first == archetype);
	EntityData& data = *(it->second);

	EntityID id = -1;

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

void ECSManager::destroyEntity(EntityID id)
{
	destroyEntityInternal(id, true);
}

void* ECSManager::getComponentShared(Gaff::Hash64 archetype, Gaff::Hash64 component)
{
	GAFF_ASSERT(_entity_pages.find(archetype) != _entity_pages.end());
	auto& archetype_instance = _entity_pages[archetype]->archetype;

	const int32_t component_offset = archetype_instance.getComponentSharedOffset(component);
	GAFF_ASSERT(component_offset > -1);

	return reinterpret_cast<int8_t*>(archetype_instance.getSharedData()) + component_offset;
}

void* ECSManager::getComponentShared(EntityID id, Gaff::Hash64 component)
{
	GAFF_ASSERT(id < _next_id && _entities[id].data);
	const Entity& entity = _entities[id];

	const int32_t component_offset = entity.data->archetype.getComponentSharedOffset(component);
	GAFF_ASSERT(component_offset > -1);

	return reinterpret_cast<int8_t*>(entity.data->archetype.getSharedData()) + component_offset;
}

void* ECSManager::getComponent(EntityID id, Gaff::Hash64 component)
{
	GAFF_ASSERT(id < _next_id && _entities[id].data);
	const Entity& entity = _entities[id];

	const int32_t entity_offset = (entity.index / 4) * entity.data->archetype.size() * 4;
	const int32_t component_offset = entity.data->archetype.getComponentOffset(component);

	GAFF_ASSERT(component_offset > -1);

	return reinterpret_cast<int8_t*>(entity.page) + sizeof(EntityPage) + entity_offset + component_offset;
}

int32_t ECSManager::getPageIndex(EntityID id) const
{
	GAFF_ASSERT(id < _next_id && _entities[id].data);
	return _entities[id].index;
}

void* ECSManager::getComponent(ECSQueryResult& query_result, int32_t entity_index)
{
	EntityData* const data = reinterpret_cast<EntityData*>(query_result.entity_data);
	GAFF_ASSERT(entity_index < data->num_entities);

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
		new_query.filter(entity_data->archetype, entity_data);
	}
}

//bool ECSManager::loadFile(const char* file_name, IFile* file)
//{
//	ProxyAllocator allocator("ECS");
//
//	if (Gaff::EndsWith(file_name, ".archetype.bin")) {
//		Gaff::MessagePackReader mpack;
//
//		if (!mpack.parse(file->getBuffer(), file->size())) {
//			// $TODO: Log error.
//			return false;
//		}
//
//		SerializeReader<Gaff::MessagePackNode> reader(mpack.getRoot(), allocator);
//		ECSArchetype archetype;
//
//		if (!archetype.finalize(reader)) {
//			return false;
//		}
//
//		{
//			const auto guard = reader.enterElementGuard("name");
//
//			if (!reader.isString()) {
//				// $TODO: Log error.
//				return false;
//			}
//
//			addArchetype(std::move(archetype), reader.readString());
//		}
//
//	} else {
//		Gaff::JSON json;
//
//		if (!json.parse(file->getBuffer())) {
//			// $TODO: Log error.
//			return false;
//		}
//
//		const Gaff::JSON name = json["name"];
//
//		if (!name.isString()) {
//			// $TODO: Log error.
//			return false;
//		}
//
//		SerializeReader<Gaff::JSON> reader(json, allocator);
//
//		ECSArchetype archetype;
//
//		if (!archetype.finalize(reader)) {
//			return false;
//		}
//
//		addArchetype(std::move(archetype), name.getString());
//	}
//
//	return true;
//}

void ECSManager::destroyEntityInternal(EntityID id, bool change_ref_count)
{
	GAFF_ASSERT(id < _next_id && _entities[id].data);
	Entity& entity = _entities[id];

	--entity.data->num_entities;
	--entity.page->num_entities;

	const auto it = Gaff::Find(entity.data->pages, entity.page, [](const auto& lhs, const EntityPage* rhs) -> bool { return lhs.get() == rhs; });
	const int32_t page_index = static_cast<int32_t>(eastl::distance(entity.data->pages.begin(), it));
	const int32_t page_ids_start = page_index * entity.data->num_entities_per_page;
	const int32_t global_index = entity.index + page_ids_start;

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
	EntityPage* const page = reinterpret_cast<EntityPage*>(SHIB_ALLOC_ALIGNED(EA_KIBIBYTE(64), 16, allocator));

	page->num_entities = 1;
	page->next_index = 1;

	data.pages.emplace_back(page);
	++data.num_entities;

	const int32_t global_index = static_cast<int32_t>(data.pages.size() - 1) * data.num_entities_per_page;
	data.entity_ids.resize(data.entity_ids.size() + static_cast<size_t>(data.num_entities_per_page), -1);
	data.entity_ids[global_index] = id;

	return global_index;
}

ECSManager::ArchetypeReference* ECSManager::modifyInternal(EntityID& id, ArchetypeModifier modifier)
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

ECSManager::ArchetypeReference* ECSManager::addArchetypeInternal(ECSArchetype&& archetype)
{
	const Gaff::Hash64 archetype_hash = archetype.getHash();

	if (const auto it = _entity_pages.find(archetype_hash); it != _entity_pages.end()) {
		return it->second->arch_ref;
	}

	ProxyAllocator allocator("ECS");

	EntityData* const data = SHIB_ALLOCT(EntityData, allocator);
	data->num_entities_per_page = (EA_KIBIBYTE(64) - sizeof(EntityPage)) / archetype.size();
	data->arch_ref = SHIB_ALLOCT(ArchetypeReference, allocator, *this, archetype_hash);
	data->archetype = std::move(archetype);

	_entity_pages[archetype_hash].reset(data);

	for (ECSQuery& query : _queries) {
		query.filter(data->archetype, data);
	}

	return data->arch_ref;
}

NS_END
