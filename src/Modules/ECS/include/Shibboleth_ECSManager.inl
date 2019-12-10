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

template <class... Components>
ArchetypeReference* ECSManager::removeSharedComponentsInternal(Gaff::Hash64 archetype_hash)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
	const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

	ECSArchetype archetype;
	archetype.copy(old_archetype);
	RemoveSharedComponentHelper<Components...>(archetype);
	archetype.finalize(old_archetype);

	const auto archetype_ref = addArchetype(std::move(archetype));
	migrate(id, archetype.getHash());

	return archetype_ref;
}

template <class... Components>
ArchetypeReference* ECSManager::removeComponentsInternal(Gaff::Hash64 archetype_hash)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
	const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

	ECSArchetype archetype;
	archetype.copy(old_archetype, true);
	RemoveComponentHelper<Components...>(archetype);
	archetype.finalize();

	const auto archetype_ref = addArchetype(std::move(archetype));
	migrate(id, archetype.getHash());

	return archetype_ref;
}

template <class... Components>
ArchetypeReference* ECSManager::addSharedComponentsInternal(Gaff::Hash64 archetype_hash)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
	const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

	ECSArchetype archetype;
	archetype.copy(old_archetype);
	AddSharedComponentHelper<Components...>(archetype);
	archetype.finalize(old_archetype);

	const auto archetype_ref = addArchetype(std::move(archetype));
	migrate(id, archetype.getHash());

	return archetype_ref;
}

template <class... Components>
ArchetypeReference* ECSManager::addComponentsInternal(Gaff::Hash64 archetype_hash)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
	const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

	ECSArchetype archetype;
	archetype.copy(old_archetype, true);
	AddComponentHelper<Components...>(archetype);
	archetype.finalize();

	const auto archetype_ref = addArchetype(std::move(archetype));
	migrate(id, archetype.getHash());

	return archetype_ref;
}

template <class... Components>
ArchetypeReference* ECSManager::removeSharedComponentsInternal(EntityID id)
{
	const ECSArchetype& base_archetype = getArchetype(id);
	ECSArchetype archetype;

	archetype.copy(base_archetype);
	RemoveSharedComponentHelper<Components...>(archetype);
	archetype.finalize(base_archetype);

	const auto archetype_ref = addArchetype(std::move(archetype));
	migrate(id, archetype.getHash());

	return archetype_ref;
}

template <class... Components>
ArchetypeReference* ECSManager::removeComponentsInternal(EntityID id)
{
	ECSArchetype archetype;
	archetype.copy(getArchetype(id), true);
	RemoveComponentHelper<Components...>(archetype);
	archetype.finalize();

	const auto arch_ref = addArchetypeInternal(std::move(archetype));
	migrate(id, archetype.getHash());

	return arch_ref;
}

template <class... Components>
ArchetypeReference* ECSManager::addSharedComponentsInternal(EntityID id)
{
	const ECSArchetype& base_archetype = getArchetype(id);
	ECSArchetype archetype;

	archetype.copy(base_archetype);
	AddSharedComponentHelper<Components...>(archetype);
	archetype.finalize(base_archetype);

	ArchetypeReference* const arch_ref = addArchetypeInternal(std::move(archetype));
	migrate(id, archetype.getHash());

	return arch_ref;
}

template <class... Components>
ArchetypeReference* ECSManager::addComponentsInternal(EntityID id)
{
	ECSArchetype archetype;
	archetype.copy(getArchetype(id), true);
	AddComponentHelper<Components...>(archetype);
	archetype.finalize();

	ArchetypeReference* const arch_ref = addArchetypeInternal(std::move(archetype));
	migrate(id, archetype.getHash());

	return arch_ref;
}

template <class Component>
Component ECSManager::get(const ECSQueryResult& query_result, int32_t entity_index)
{
	return Component{ Component::Get(*this, query_result, entity_index) };
}

template <class... QueryResults>
ECSManager::EntityData* ECSManager::getEntityData(const ECSQueryResult& query_result, const QueryResults&...)
{
	// Assumes all the query results are from the same query and index. So they should all share the same entity data.
	return reinterpret_cast<EntityData*>(query_result.entity_data);
}
