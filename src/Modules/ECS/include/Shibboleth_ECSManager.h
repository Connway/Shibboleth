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

#pragma once

#include "Shibboleth_ECSSceneResource.h"
#include "Shibboleth_ECSArchetype.h"
#include "Shibboleth_ECSEntity.h"
#include "Shibboleth_ECSQuery.h"
#include <Shibboleth_IManager.h>
#include <eathread/eathread_mutex.h>

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
	static constexpr const char8_t* const k_empty_archetype_res_name = u8"EmptyECSArchetype";

	struct ArchetypeModifier final
	{
		void (*removeShared)(ECSArchetype&);
		void (*addShared)(ECSArchetype&);
		void (*remove)(ECSArchetype&);
		void (*add)(ECSArchetype&);
	};

	template <class T>
	const T* getComponentShared(Gaff::Hash64 archetype) const
	{
		return reinterpret_cast<T*>(getComponentShared(archetype, Refl::Reflection<T>::GetHash()));
	}

	template <class T>
	const T* getComponentShared(ECSEntityID id) const
	{
		return reinterpret_cast<T*>(getComponentShared(id, Refl::Reflection<T>::GetHash()));
	}

	template <class T>
	T* getComponentShared(Gaff::Hash64 archetype)
	{
		return reinterpret_cast<T*>(getComponentShared(archetype, Refl::Reflection<T>::GetHash()));
	}

	template <class T>
	T* getComponentShared(ECSEntityID id)
	{
		return reinterpret_cast<T*>(getComponentShared(id, Refl::Reflection<T>::GetHash()));
	}

	template <class T>
	const void* getComponent(ECSEntityID id) const
	{
		return getComponent(id, Refl::Reflection<T>::GetHash());
	}

	template <class T>
	void* getComponent(ECSEntityID id)
	{
		return getComponent(id, Refl::Reflection<T>::GetHash());
	}

	template <class T>
	bool hasComponent(Gaff::Hash64 archetype) const
	{
		return hasComponent(archetype, Refl::Reflection<T>::GetHash());
	}

	template <class T>
	bool hasComponent(ECSEntityID id) const
	{
		return hasComponent(id, Refl::Reflection<T>::GetHash());
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
	void removeSharedComponents(ECSEntityID id, ArchetypeReferencePtr& out_ref)
	{
		out_ref = removeSharedComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void removeSharedComponents(ECSEntityID id)
	{
		removeSharedComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void removeComponents(ECSEntityID id, ArchetypeReferencePtr& out_ref)
	{
		out_ref = removeComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void removeComponents(ECSEntityID id)
	{
		removeComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void addSharedComponents(ECSEntityID id, ArchetypeReferencePtr& out_ref)
	{
		out_ref = addSharedComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void addSharedComponents(ECSEntityID id)
	{
		addSharedComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void addComponents(ECSEntityID id, ArchetypeReferencePtr& out_ref)
	{
		out_ref = addComponentsInternal<Components...>(id);
	}

	template <class... Components>
	void addComponents(ECSEntityID id)
	{
		addComponentsInternal<Components...>(id);
	}

	void modify(ECSEntityID& id, ArchetypeModifier modifier, ArchetypeReferencePtr& out_ref)
	{
		out_ref = modifyInternal(id, modifier);
	}

	void modify(ECSEntityID& id, ArchetypeModifier modifier)
	{
		modifyInternal(id, modifier);
	}

	template <class T1, class T2, class T3, class T4, class T5, class Callback>
	void iterate(
		const ECSQueryResult& query_result1,
		const ECSQueryResult& query_result2,
		const ECSQueryResult& query_result3,
		const ECSQueryResult& query_result4,
		const ECSQueryResult& query_result5,
		Callback&& callback)
	{
		const ECSQueryResult* query_results[] = { &query_result1, &query_result2, &query_result3, &query_result4, &query_result5 };
		iterateInternal<Callback, T1, T2, T3, T4, T5>(std::forward<Callback>(callback), query_results);
	}

	template <class T1, class T2, class T3, class T4, class Callback>
	void iterate(
		const ECSQueryResult& query_result1,
		const ECSQueryResult& query_result2,
		const ECSQueryResult& query_result3,
		const ECSQueryResult& query_result4,
		Callback&& callback)
	{
		const ECSQueryResult* query_results[] = { &query_result1, &query_result2, &query_result3, &query_result4 };
		iterateInternal<Callback, T1, T2, T3, T4>(std::forward<Callback>(callback), query_results);
	}

	template <class T1, class T2, class T3, class Callback>
	void iterate(
		const ECSQueryResult& query_result1,
		const ECSQueryResult& query_result2,
		const ECSQueryResult& query_result3,
		Callback&& callback)
	{
		const ECSQueryResult* query_results[] = { &query_result1, &query_result2, &query_result3 };
		iterateInternal<Callback, T1, T2, T3>(std::forward<Callback>(callback), query_results);
	}

	template <class T1, class T2, class Callback>
	void iterate(
		const ECSQueryResult& query_result1,
		const ECSQueryResult& query_result2,
		Callback&& callback)
	{
		const ECSQueryResult* query_results[] = { &query_result1, &query_result2 };
		iterateInternal<Callback, T1, T2>(std::forward<Callback>(callback), query_results);
	}

	template <class T, class Callback>
	void iterate(const ECSQueryResult& query_result, Callback&& callback)
	{
		const ECSQueryResult* query_results[] = { &query_result };
		iterateInternal<Callback, T>(std::forward<Callback>(callback), query_results);
	}

	~ECSManager(void);

	bool initAllModulesLoaded(void) override;

	void addArchetype(ECSArchetype&& archetype, ArchetypeReferencePtr& out_ref);
	ArchetypeReference* addArchetype(ECSArchetype&& archetype);

	// THIS FUNCTION IS EXTREMELY DANGEROUS!
	// If calling outside of ArchetypeReference, be extremely careful!
	// Can be bad if there are dangling references to the ArchetypeReference instance!
	void removeArchetype(Gaff::Hash64 archetype);

	const ECSArchetype& getArchetype(Gaff::Hash64 archetype) const;
	const ECSArchetype& getArchetype(ECSEntityID id) const;

	ArchetypeReferencePtr getArchetypeReference(Gaff::Hash64 archetype);
	ArchetypeReferencePtr getArchetypeReference(ECSEntityID id);

	ECSEntityID createEntity(const ECSArchetype& archetype);
	ECSEntityID createEntity(Gaff::Hash64 archetype);

	ECSEntityID loadEntity(const ECSArchetype& archetype, const ISerializeReader& reader);
	ECSEntityID loadEntity(Gaff::Hash64 archetype, const ISerializeReader& reader);

	void destroyEntity(ECSEntityID id);

	const void* getComponentShared(Gaff::Hash64 archetype, const Refl::IReflectionDefinition& component) const;
	const void* getComponentShared(Gaff::Hash64 archetype, Gaff::Hash64 component) const;
	const void* getComponentShared(ECSEntityID id, const Refl::IReflectionDefinition& component) const;
	const void* getComponentShared(ECSEntityID id, Gaff::Hash64 component) const;

	void* getComponentShared(Gaff::Hash64 archetype, const Refl::IReflectionDefinition& component);
	void* getComponentShared(Gaff::Hash64 archetype, Gaff::Hash64 component);
	void* getComponentShared(ECSEntityID id, const Refl::IReflectionDefinition& component);
	void* getComponentShared(ECSEntityID id, Gaff::Hash64 component);

	const void* getComponent(ECSEntityID id, const Refl::IReflectionDefinition& component) const;
	const void* getComponent(ECSEntityID id, Gaff::Hash64 component) const;
	void* getComponent(ECSEntityID id, const Refl::IReflectionDefinition& component);
	void* getComponent(ECSEntityID id, Gaff::Hash64 component);

	bool hasComponent(Gaff::Hash64 archetype, const Refl::IReflectionDefinition& component) const;
	bool hasComponent(Gaff::Hash64 archetype, Gaff::Hash64 component) const;
	bool hasComponent(ECSEntityID id, const Refl::IReflectionDefinition& component) const;
	bool hasComponent(ECSEntityID id, Gaff::Hash64 component) const;

	int32_t getComponentIndex(const ECSQueryResult& query_result, int32_t entity_index) const;
	int32_t getComponentIndex(ECSEntityID id) const;

	int32_t getPageIndex(const ECSQueryResult& query_result, int32_t entity_index) const;
	int32_t getPageIndex(ECSEntityID id) const;

	const void* getComponent(const ECSQueryResult& query_result, int32_t entity_index) const;
	void* getComponent(const ECSQueryResult& query_result, int32_t entity_index);
	int32_t getNumEntities(const ECSQueryResult& query_result) const;

	void registerQuery(ECSQuery&& query);

	const ECSSceneResourcePtr& getCurrentScene(void) const;
	void switchScene(const ECSSceneResourcePtr scene);

	bool isValid(ECSEntityID id) const;

	const ECSArchetype& getEmptyArchetype(void) const;

private:
	struct EntityData;

	struct alignas(16) EntityPage final
	{
		int32_t num_entities;
		int32_t next_index;

		uint8_t padding[8]; // To suppress errors.

		// Data after this.
	};

	struct EntityData final
	{
		ECSArchetype archetype;
		ArchetypeReference* arch_ref = nullptr;

		int32_t num_entities_per_page = 0;
		int32_t num_entities = 0;
		int32_t page_size = static_cast<int32_t>(EA_KIBIBYTE(64));

		Vector< UniquePtr<EntityPage> > pages{ ProxyAllocator("ECS") };
		Vector<ECSEntityID> entity_ids{ ProxyAllocator("ECS") };
		Vector<int32_t> free_indices{ ProxyAllocator("ECS") };
		Vector<int32_t> queries{ ProxyAllocator("ECS") };
	};

	struct Entity final
	{
		EntityData* data = nullptr;
		EntityPage* page = nullptr;
		int32_t index = -1;
	};

	mutable EA::Thread::Mutex _entity_page_lock;

	VectorMap< Gaff::Hash64, UniquePtr<EntityData> > _entity_pages{ ProxyAllocator("ECS") };
	Vector<ECSQuery> _queries{ ProxyAllocator("ECS") };
	Vector<Entity> _entities{ ProxyAllocator("ECS") };

	Vector<ECSEntityID> _free_ids{ ProxyAllocator("ECS") };
	ECSEntityID _next_id = 0;

	ECSArchetypeResourcePtr _empty_arch_res;
	ECSSceneResourcePtr _curr_scene;

	//bool loadFile(const char* file_name, IFile* file);
	void destroyEntityInternal(ECSEntityID id, bool change_ref_count);
	void migrate(ECSEntityID id, Gaff::Hash64 new_archetype);
	int32_t allocateIndex(EntityData& data, ECSEntityID id);

	ArchetypeReference* modifyInternal(ECSEntityID& id, ArchetypeModifier modifier);
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
	ArchetypeReference* removeSharedComponentsInternal(ECSEntityID id);

	template <class... Components>
	ArchetypeReference* removeComponentsInternal(ECSEntityID id);

	template <class... Components>
	ArchetypeReference* addSharedComponentsInternal(ECSEntityID id);

	template <class... Components>
	ArchetypeReference* addComponentsInternal(ECSEntityID id);

	template <class Callback, size_t index, class ComponentFirst, class... ComponentsRest, size_t array_size, class... ComponentsPrev>
	void iterateInternalHelper(
		Callback&& callback,
		ECSEntityID entity_id,
		int32_t entity_index,
		const ECSQueryResult* (&query_results)[array_size],
		ComponentsPrev&&... prev_comps
	);

	template <class Callback, class... Components, size_t array_size>
	void iterateInternal(Callback&& callback, const ECSQueryResult* (&query_results)[array_size]);

	SHIB_REFLECTION_CLASS_DECLARE(ECSManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ECSManager)

#include "Shibboleth_ECSManager.inl"
