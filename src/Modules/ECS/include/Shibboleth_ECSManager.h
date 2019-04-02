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
#include <Shibboleth_RefCounted.h>
#include <Shibboleth_IManager.h>
#include <Gaff_RefPtr.h>

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
	class ArchetypeReference final
	{
	public:
		ArchetypeReference(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
		~ArchetypeReference(void);

		Gaff::Hash64 getArchetype(void) const;

	private:
		Gaff::Hash64 _archetype;
		ECSManager& _ecs_mgr;

		GAFF_NO_COPY(ArchetypeReference);
		SHIB_REF_COUNTED();
	};

	using ArchetypeReferencePtr = Gaff::RefPtr<ArchetypeReference>;


	struct ArchetypeModifier final
	{
		void (*removeShared)(ECSArchetype&);
		void (*addShared)(ECSArchetype&);
		void (*remove)(ECSArchetype&);
		void (*add)(ECSArchetype&);
	};

	template <class T>
	typename T* getComponentShared(Gaff::Hash64 archetype)
	{
		return reinterpret_cast<T*>(getComponentShared(archetype, Reflection<T>::GetHash()));
	}

	template <class T>
	typename T* getComponentShared(EntityID id)
	{
		return reinterpret_cast<T*>(getComponentShared(id, Reflection<T>::GetHash()));
	}

	template <class T>
	void* getComponent(EntityID id)
	{
		return getComponent(id, Reflection<T>::GetHash());
	}

	template <class... Components>
	void removeSharedComponents(Gaff::Hash64 archetype_hash, ArchetypeReferencePtr& out_ref)
	{
		out_ref = removeSharedComponentsInternal<Components...>(archetype_hash);
	}

	template <class... Components>
	void removeSharedComponents(Gaff::Hash64 archetype_hash)
	{
		removeSharedComponentsInternal<Components...>(archetype_hash);
	}

	template <class... Components>
	void removeComponents(Gaff::Hash64 archetype_hash, ArchetypeReferencePtr& out_ref)
	{
		out_ref = removeComponentsInternal<Components...>(archetype_hash);
	}

	template <class... Components>
	void removeComponents(Gaff::Hash64 archetype_hash)
	{
		removeComponentsInternal<Components...>(archetype_hash);
	}

	template <class... Components>
	void addSharedComponents(Gaff::Hash64 archetype_hash, ArchetypeReferencePtr& out_ref)
	{
		out_ref = addSharedComponentsInternal<Components...>(archetype_hash);
	}

	template <class... Components>
	void addSharedComponents(Gaff::Hash64 archetype_hash)
	{
		addSharedComponentsInternal<Components...>(archetype_hash);
	}

	template <class... Components>
	void addComponents(Gaff::Hash64 archetype_hash, ArchetypeReferencePtr& out_ref)
	{
		out_ref = addComponentsInternal<Components...>(archetype_hash);
	}

	template <class... Components>
	void addComponents(Gaff::Hash64 archetype_hash)
	{
		addComponentsInternal<Components...>(archetype_hash);
	}

	template <class... Components>
	void removeSharedComponents(EntityID id, ArchetypeReferencePtr& out_ref)
	{
		out_ref = removeSharedComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void removeSharedComponents(EntityID id)
	{
		removeSharedComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void removeComponents(EntityID id, ArchetypeReferencePtr& out_ref)
	{
		out_ref = removeComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void removeComponents(EntityID id)
	{
		removeComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void addSharedComponents(EntityID id, ArchetypeReferencePtr& out_ref)
	{
		out_ref = addSharedComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void addSharedComponents(EntityID id)
	{
		addSharedComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void addComponents(EntityID id, ArchetypeReferencePtr& out_ref)
	{
		out_ref = addComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void addComponents(EntityID id)
	{
		addComponentsInternal<Components...>(id);
	}

	void modify(EntityID& id, ArchetypeModifier modifier, ArchetypeReferencePtr& out_ref)
	{
		out_ref = modifyInternal(id, modifier);
	}

	void modify(EntityID& id, ArchetypeModifier modifier)
	{
		modifyInternal(id, modifier);
	}

	~ECSManager(void);

	bool init(void) override;

	void addArchetype(ECSArchetype&& archetype, ArchetypeReferencePtr& out_ref);
	void addArchetype(ECSArchetype&& archetype);
	void removeArchetype(Gaff::Hash64 archetype);

	const ECSArchetype& getArchetype(Gaff::Hash64 archetype) const;
	const ECSArchetype& getArchetype(EntityID id) const;

	ArchetypeReferencePtr getArchetypeReference(Gaff::Hash64 archetype);
	ArchetypeReferencePtr getArchetypeReference(EntityID id);

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
		ArchetypeReference* arch_ref = nullptr;

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
	Vector<ECSQuery> _queries;
	Vector<Entity> _entities;

	Vector<EntityID> _free_ids;
	EntityID _next_id = 0;

	//bool loadFile(const char* file_name, IFile* file);
	void destroyEntityInternal(EntityID id, bool change_ref_count);
	void migrate(EntityID id, Gaff::Hash64 new_archetype);
	int32_t allocateIndex(EntityData& data, EntityID id);

	ArchetypeReference* modifyInternal(EntityID& id, ArchetypeModifier modifier);
	ArchetypeReference* addArchetypeInternal(ECSArchetype&& archetype);

	template <class... Components>
	ArchetypeReference* removeSharedComponentsInternal(Gaff::Hash64 archetype_hash);

	template <class... Components>
	ArchetypeReference* removeComponentsInternal(Gaff::Hash64 archetype_hash);

	template <class... Components>
	ArchetypeReference* addSharedComponentsInternal(Gaff::Hash64 archetype_hash);

	template <class... Components>
	ArchetypeReference* addComponentsInternal(Gaff::Hash64 archetype_hash);

	template <class... Components>
	ArchetypeReference* removeSharedComponentsInternal(EntityID id);

	template <class... Components>
	ArchetypeReference* removeComponentsInternal(EntityID id);

	template <class... Components>
	ArchetypeReference* addSharedComponentsInternal(EntityID id);

	template <class... Components>
	ArchetypeReference* addComponentsInternal(EntityID id);

	SHIB_REFLECTION_CLASS_DECLARE(ECSManager);
};

#include "Shibboleth_ECSManager.inl"

NS_END

SHIB_REFLECTION_DECLARE(ECSManager)
