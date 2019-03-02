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

#pragma once

#include "Shibboleth_ECSArchetype.h"
#include "Shibboleth_ECSEntity.h"
#include "Shibboleth_ECSQuery.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class ECSQuery;
class IFile;

template <class First, class... Rest>
void RemoveSharedComponentHelper(ECSArchetype& archetype)
{
	archetype.removeShared<First>();

	if constexpr (sizeof...(Rest) > 0) {
		RemoveSharedComponentHelper<Rest...>(archetype);
	}
}

template <class First, class... Rest>
void RemoveComponentHelper(ECSArchetype& archetype)
{
	archetype.remove<First>();

	if constexpr (sizeof...(Rest) > 0) {
		RemoveComponentHelper<Rest...>(archetype);
	}
}

template <class First, class... Rest>
void AddSharedComponentHelper(ECSArchetype& archetype)
{
	archetype.addShared<First>();

	if constexpr (sizeof...(Rest) > 0) {
		AddSharedComponentHelper<Rest...>(archetype);
	}
}

template <class First, class... Rest>
void AddComponentHelper(ECSArchetype& archetype)
{
	archetype.add<First>();

	if constexpr (sizeof...(Rest) > 0) {
		AddComponentHelper<Rest...>(archetype);
	}
}


class ECSManager final : public IManager
{
public:
	struct ArchetypeModifier final
	{
		void (*removeShared)(ECSArchetype&);
		void (*addShared)(ECSArchetype&);
		void (*remove)(ECSArchetype&);
		void (*add)(ECSArchetype&);
	};

	template <class T>
	typename T::SharedData* getComponentShared(Gaff::Hash64 archetype)
	{
		return reinterpret_cast<T::SharedData*>(getComponentShared(archetype, Reflection<T>::GetHash()));
	}

	template <class T>
	typename T::SharedData* getComponentShared(EntityID id)
	{
		return reinterpret_cast<T::SharedData*>(getComponentShared(id, Reflection<T>::GetHash()));
	}

	template <class T>
	void* getComponent(EntityID id)
	{
		return getComponent(id, Reflection<T>::GetHash());
	}

	template <class... Components>
	void removeSharedComponents(Gaff::Hash64 archetype_hash)
	{
		GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
		const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

		ECSArchetype archetype;
		archetype.copy(old_archetype);
		RemoveSharedComponentHelper<Components...>(archetype);
		archetype.finalize();

		addArchetype(std::move(archetype));
		migrate(id, archetype.getHash());
	}

	template <class... Components>
	void removeComponents(Gaff::Hash64 archetype_hash)
	{
		GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
		const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

		ECSArchetype archetype;
		archetype.copy(old_archetype);
		RemoveComponentHelper<Components...>(archetype);
		archetype.finalize();

		addArchetype(std::move(archetype));
		migrate(id, archetype.getHash());
	}

	template <class... Components>
	void addSharedComponents(Gaff::Hash64 archetype_hash)
	{
		GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
		const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

		ECSArchetype archetype;
		archetype.copy(old_archetype);
		AddSharedComponentHelper<Components...>(archetype);
		archetype.finalize();

		addArchetype(std::move(archetype));
		migrate(id, archetype.getHash());
	}

	template <class... Components>
	void addComponents(Gaff::Hash64 archetype_hash)
	{
		GAFF_ASSERT(_entity_pages.find(archetype.getHash()) != _entity_pages.end());
		const ECSArchetype& old_archetype = _entity_pages[archetype_hash]->archetype;

		ECSArchetype archetype;
		archetype.copy(old_archetype);
		AddComponentHelper<Components...>(archetype);
		archetype.finalize();

		addArchetype(std::move(archetype));
		migrate(id, archetype.getHash());
	}

	template <class... Components>
	void removeSharedComponents(EntityID id)
	{
		ECSArchetype archetype;
		archetype.copy(getArchetype(id));
		RemoveSharedComponentHelper<Components...>(archetype);
		archetype.finalize();

		addArchetype(std::move(archetype));
		migrate(id, archetype.getHash());
	}

	template <class... Components>
	void removeComponents(EntityID id)
	{
		ECSArchetype archetype;
		archetype.copy(getArchetype(id));
		RemoveComponentHelper<Components...>(archetype);
		archetype.finalize();

		addArchetype(std::move(archetype));
		migrate(id, archetype.getHash());
	}

	template <class... Components>
	void addSharedComponents(EntityID id)
	{
		ECSArchetype archetype;
		archetype.copy(getArchetype(id));
		AddSharedComponentHelper<Components...>(archetype);
		archetype.finalize();

		addArchetype(std::move(archetype));
		migrate(id, archetype.getHash());
	}

	template <class... Components>
	void addComponents(EntityID id)
	{
		ECSArchetype archetype;
		archetype.copy(getArchetype(id));
		AddComponentHelper<Components...>(archetype);
		archetype.finalize();

		addArchetype(std::move(archetype));
		migrate(id, archetype.getHash());
	}

	void modify(EntityID& id, ArchetypeModifier modifier)
	{
		GAFF_ASSERT(id < _next_id && _entities[id].data);

		ECSArchetype archetype;
		archetype.copy(getArchetype(id));

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

		archetype.finalize();

		addArchetype(std::move(archetype));
		migrate(id, archetype.getHash());
	}

	bool init(void) override;

	void addArchetype(ECSArchetype&& archetype, const char* name);
	void addArchetype(ECSArchetype&& archetype);
	void removeArchetype(Gaff::Hash64 archetype);
	void removeArchetype(const char* name);

	const ECSArchetype& getArchetype(Gaff::Hash64 name) const;
	const ECSArchetype& getArchetype(const char* name) const;
	const ECSArchetype& getArchetype(EntityID id) const;

	EntityID createEntityByName(Gaff::Hash64 name);
	EntityID createEntityByName(const char* name);

	EntityID createEntity(const ECSArchetype& archetype);
	EntityID createEntity(Gaff::Hash64 archetype);

	void destroyEntity(EntityID id);

	void* getComponentShared(Gaff::Hash64 archetype, Gaff::Hash64 component);
	void* getComponentShared(EntityID id, Gaff::Hash64 component);
	void* getComponent(EntityID id, Gaff::Hash64 component);
	int32_t getPageIndex(EntityID id) const;

	void* getComponent(ECSQueryResult& query_result, int32_t entity_index);
	int32_t getNumEntities(const ECSQueryResult& query_result) const;

	void registerQuery(ECSQuery&& query);

private:
	struct EntityData;

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable : 4324)
#endif

	struct alignas(16) EntityPage final
	{
		int32_t num_entities;
		int32_t next_index;
		// Data after this.
	};

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif

	struct EntityData final
	{
		ECSArchetype archetype;

		int32_t num_entities_per_page = 0;
		int32_t num_entities = 0;
		int32_t next_index = 0;

		Vector< UniquePtr<EntityPage> > pages;
		Vector<EntityID> entity_ids;
		Vector<int32_t> free_indices;
	};

	struct Entity final
	{
		EntityData* data = nullptr;
		EntityPage* page = nullptr;
		int32_t index = -1;
	};

	VectorMap< Gaff::Hash64, UniquePtr<EntityData> > _entity_pages;
	VectorMap<Gaff::Hash64, Gaff::Hash64> _archtypes;

	Vector<ECSQuery> _queries;

	Vector<Entity> _entities;
	Vector<int32_t> _free_ids;
	int32_t _next_id = 0;

	//bool loadFile(const char* file_name, IFile* file);
	void migrate(EntityID id, Gaff::Hash64 new_archetype);
	int32_t allocateIndex(EntityData& data, EntityID id);

	SHIB_REFLECTION_CLASS_DECLARE(ECSManager);
};

NS_END

SHIB_REFLECTION_DECLARE(ECSManager)
