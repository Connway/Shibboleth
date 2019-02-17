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
#include <Shibboleth_IFileSystem.h>
#include <Gaff_Function.h>
#include <Gaff_JSON.h>

SHIB_REFLECTION_DEFINE(ECSManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ECSManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(ECSManager)

bool ECSManager::init(void)
{
	// Load all archetypes from config directory
	auto callback = Gaff::MemberFunc(this, &ECSManager::loadFile);
	IApp& app = GetApp();

	const char* const archetype_dir = app.getConfigs()["archetype_dir"].getString("Resources/Archetypes");
	app.getFileSystem().forEachFile(archetype_dir, callback);

	const auto ecs_systems = app.getReflectionManager().getTypeBucket(CLASS_HASH(IECSSystem));
	ProxyAllocator allocator("ECS");

	for (const Gaff::IReflectionDefinition* ref_def : *ecs_systems) {
		IECSSystem* const system = ref_def->createT<IECSSystem>(CLASS_HASH(IECSSystem), allocator);

		if (!system) {
			// $TODO: Log error.
			continue;
		}

		system->init(*this);
	}

	return true;
}

void ECSManager::addArchetype(ECSArchetype&& archetype, const char* name)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) == _entity_pages.end());
	ProxyAllocator allocator("ECS");

	EntityData* const data = SHIB_ALLOCT(EntityData, allocator);
	data->num_entities_per_page = (EA_KIBIBYTE(64) - sizeof(EntityPage)) / archetype.size();
	data->archetype = std::move(archetype);

	_archtypes[Gaff::FNV1aHash64String(name)] = archetype.getHash();
	_entity_pages[archetype.getHash()].reset(data);
}

void ECSManager::addArchetype(ECSArchetype&& archetype)
{
	if (_entity_pages.find(archetype.getHash()) != _entity_pages.end()) {
		return;
	}

	ProxyAllocator allocator("ECS");

	EntityData* const data = SHIB_ALLOCT(EntityData, allocator);
	data->num_entities_per_page = (EA_KIBIBYTE(64) - sizeof(EntityPage)) / archetype.size();
	data->archetype = std::move(archetype);

	_entity_pages[archetype.getHash()].reset(data);
}

const ECSArchetype& ECSManager::getArchetype(Gaff::Hash64 name) const
{
	const auto it = _archtypes.find(name);
	GAFF_ASSERT(it != _archtypes.end() && it->first == name);
	return _entity_pages.find(it->second)->second->archetype;
}

const ECSArchetype& ECSManager::getArchetype(const char* name) const
{
	return getArchetype(Gaff::FNV1aHash64String(name));
}

const ECSArchetype& ECSManager::getArchetype(EntityID id) const
{
	GAFF_ASSERT(id < _next_id && _entities[id].data);
	return _entities[id].data->archetype;
}

EntityID ECSManager::createEntityByName(Gaff::Hash64 name)
{
	const auto it = _archtypes.find(name);
	GAFF_ASSERT(it != _archtypes.end() && it->first == name);
	return createEntity(it->second);
}

EntityID ECSManager::createEntityByName(const char* name)
{
	return createEntityByName(Gaff::FNV1aHash64String(name));
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

	return id;
}

void ECSManager::destroyEntity(EntityID id)
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

	entity.page = nullptr;
	entity.data = nullptr;
	entity.index = -1;

	_free_ids.emplace_back(id);
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

bool ECSManager::loadFile(const char*, IFile* file)
{
	Gaff::JSON json;

	if (!json.parse(file->getBuffer())) {
		// $TODO: Log error.
		return false;
	}

	const Gaff::JSON name = json["name"];

	if (!name.isString()) {
		// $TODO: Log error.
		return false;
	}

	ECSArchetype archetype;

	if (!archetype.finalize(json)) {
		return false;
	}

	addArchetype(std::move(archetype), name.getString());
	return false;
}

void ECSManager::migrate(EntityID id, Gaff::Hash64 new_archetype)
{
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

	entity.data = &data;
	entity.page = new_page;
	entity.index = new_index;
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

NS_END
