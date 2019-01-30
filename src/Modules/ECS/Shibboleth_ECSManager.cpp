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

#include "Shibboleth_ECSManager.h"
#include "Shibboleth_ECSAttributes.h"
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
	return true;
}

//void ECSManager::addArchetype(const ECSArchetype& archetype, const char* name)
//{
//	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) == _entity_pages.end());
//	EntityData& data = _entity_pages[archetype.getHash()];
//	data.num_entities_per_page = (EA_KIBIBYTE(64) - sizeof(EntityPage*)) / archetype.size();
//	data.archetype = archetype;
//
//	ProxyAllocator allocator("ECS");
//	data.shared_components = SHIB_ALLOC_ALIGNED(archetype.sharedSize(), 16, allocator);
//
//	_archtypes[Gaff::FNV1aHash64String(name)] = archetype.getHash();
//}

void ECSManager::addArchetype(ECSArchetype&& archetype, const char* name)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) == _entity_pages.end());
	ProxyAllocator allocator("ECS");

	EntityData* const data = SHIB_ALLOCT(EntityData, allocator);
	data->num_entities_per_page = (EA_KIBIBYTE(64) - sizeof(EntityPage*)) / archetype.size();
	//data.shared_components = SHIB_ALLOC_ALIGNED(data.archetype.sharedSize(), 16, allocator);
	//data->shared_components = SHIB_ALLOC(archetype.sharedSize(), allocator);
	data->archetype = std::move(archetype);

	_archtypes[Gaff::FNV1aHash64String(name)] = archetype.getHash();
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

	EntityID id;

	// Look for free indices.
	// $TODO: Try and optimize this.
	for (auto& page : data.pages) {
		if (page->free_indices.empty()) {
			continue;
		}

		id._entity_index = page->free_indices.back();
		id._entity_page = page.get();
		page->free_indices.pop_back();
	}

	// Didn't find a free index.
	if (!id._entity_page) {
		auto& page = data.pages.back();

		// Take next index on last page.
		if (page->num_entities < data.num_entities_per_page) {
			id._entity_index = page->next_index++;
			id._entity_page = page.get();

		// All pages are full, allocate another page.
		} else {
			ProxyAllocator allocator("ECS");
			EntityPage* const id_page = SHIB_ALLOCT(EntityPage, allocator);

			id._entity_page = id_page;
			id._entity_index = 0;

			id_page->num_entities = 1;
			id_page->next_index = 1;

			id_page->data = SHIB_ALLOC_ALIGNED(EA_KIBIBYTE(64), 16, allocator);
			data.pages.emplace_back(id_page);
		}
	}

	++data.num_entities;
	return id;
}

void* ECSManager::getComponent(EntityID id, Gaff::Hash64 component)
{
	GAFF_ASSERT(id._entity_page);
	EntityPage* const page = reinterpret_cast<EntityPage*>(id._entity_page);
	const int32_t entity_offset = (id._entity_index / 4) * page->owner->archetype.size() * 4;
	const int32_t component_offset = page->owner->archetype.getComponentOffset(component);

	GAFF_ASSERT(component_offset > -1);

	return reinterpret_cast<int8_t*>(page->data) + entity_offset + component_offset;
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

	if (!archetype.fromJSON(json)) {
		// $TODO: Log error.
		return false;
	}

	addArchetype(std::move(archetype), name.getString());
	return false;
}

NS_END
