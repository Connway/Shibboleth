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
ECSManager::ArchetypeReference* ECSManager::removeSharedComponentsInternal(Gaff::Hash64 archetype_hash)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
	const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

	ECSArchetype archetype;
	archetype.copy(old_archetype);
	RemoveSharedComponentHelper<Components...>(archetype);
	archetype.finalize();

	const auto archetype_ref = addArchetype(std::move(archetype));
	migrate(id, archetype.getHash());

	return archetype_ref;
}

template <class... Components>
ECSManager::ArchetypeReference* ECSManager::removeComponentsInternal(Gaff::Hash64 archetype_hash)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
	const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

	ECSArchetype archetype;
	archetype.copy(old_archetype);
	RemoveComponentHelper<Components...>(archetype);
	archetype.finalize();

	const auto archetype_ref = addArchetype(std::move(archetype));
	migrate(id, archetype.getHash());

	return archetype_ref;
}

template <class... Components>
ECSManager::ArchetypeReference* ECSManager::addSharedComponentsInternal(Gaff::Hash64 archetype_hash)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
	const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

	ECSArchetype archetype;
	archetype.copy(old_archetype);
	AddSharedComponentHelper<Components...>(archetype);
	archetype.finalize();

	const auto archetype_ref = addArchetype(std::move(archetype));
	migrate(id, archetype.getHash());

	return archetype_ref;
}

template <class... Components>
ECSManager::ArchetypeReference* ECSManager::addComponentsInternal(Gaff::Hash64 archetype_hash)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
	const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

	ECSArchetype archetype;
	archetype.copy(old_archetype);
	AddComponentHelper<Components...>(archetype);
	archetype.finalize();

	const auto archetype_ref = addArchetype(std::move(archetype));
	migrate(id, archetype.getHash());

	return archetype_ref;
}

template <class... Components>
ECSManager::ArchetypeReference* ECSManager::removeSharedComponentsInternal(EntityID id)
{
	ECSArchetype archetype;
	archetype.copy(getArchetype(id));
	RemoveSharedComponentHelper<Components...>(archetype);
	archetype.finalize();

	const auto archetype_ref = addArchetype(std::move(archetype));
	migrate(id, archetype.getHash());

	return archetype_ref;
}

template <class... Components>
ECSManager::ArchetypeReference* ECSManager::removeComponentsInternal(EntityID id)
{
	ECSArchetype archetype;
	archetype.copy(getArchetype(id));
	RemoveComponentHelper<Components...>(archetype);
	archetype.finalize();

	ECSManager::ArchetypeReference* const arch_ref = addArchetypeInternal(std::move(archetype));
	migrate(id, archetype.getHash());

	return arch_ref;
}

template <class... Components>
ECSManager::ArchetypeReference* ECSManager::addSharedComponentsInternal(EntityID id)
{
	ECSArchetype archetype;
	archetype.copy(getArchetype(id));
	AddSharedComponentHelper<Components...>(archetype);
	archetype.finalize();

	ECSManager::ArchetypeReference* const arch_ref = addArchetypeInternal(std::move(archetype));
	migrate(id, archetype.getHash());

	return arch_ref;
}

template <class... Components>
ECSManager::ArchetypeReference* ECSManager::addComponentsInternal(EntityID id)
{
	ECSArchetype archetype;
	archetype.copy(getArchetype(id));
	AddComponentHelper<Components...>(archetype);
	archetype.finalize();

	ECSManager::ArchetypeReference* const arch_ref = addArchetypeInternal(std::move(archetype));
	migrate(id, archetype.getHash());

	return arch_ref;
}