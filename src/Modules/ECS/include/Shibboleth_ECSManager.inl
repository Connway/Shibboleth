/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

template <size_t index, size_t i, class First, class... Rest>
static constexpr bool IsPtrHelper(void)
{
	if constexpr (index == i) {
		return std::is_pointer<First>::value;
	} else {
		return IsPtrHelper<index, i + 1, Rest...>();
	}
}

template <class Functor>
struct IsPointerHelper;

template <class T, class Ret, class... Args>
struct IsPointerHelper<Ret (T::*)(Args...) const> final
{
	template <size_t index>
	static constexpr bool IsPtr(void)
	{
		static_assert(index < sizeof...(Args), "Index is larger than number of arguments.");
		return IsPtrHelper<index, 0, Args...>();
	}
};

template <class T, class Ret, class... Args>
struct IsPointerHelper<Ret (T::*)(Args...)> final
{
	template <size_t index>
	static constexpr bool IsPtr(void)
	{
		static_assert(index < sizeof...(Args), "Index is larger than number of arguments.");
		return IsPtrHelper<index, 0, Args...>();
	}
};

template <class Ret, class... Args>
struct IsPointerHelper<Ret (*)(Args...)> final
{
	template <size_t index>
	static constexpr bool IsPtr(void)
	{
		static_assert(index < sizeof...(Args), "Index is larger than number of arguments.");
		return IsPtrHelper<index, 0, Args...>();
	}
};

template <class Ret, class... Args>
struct IsPointerHelper<Ret (Args...)> final
{
	template <size_t index>
	static constexpr bool IsPtr(void)
	{
		static_assert(index < sizeof...(Args), "Index is larger than number of arguments.");
		return IsPtrHelper<index, 0, Args...>();
	}
};

template <class Callback, size_t index>
static constexpr bool IsPointer(void)
{
	if constexpr (std::is_class<Callback>::value) {
		static_assert(&Callback::operator(), "Does not have operator()");
		return IsPointerHelper<decltype(&Callback::operator())>::IsPtr<index>();
	} else if constexpr (std::is_member_function_pointer<Callback>::value || std::is_function<Callback>::value) {
		return IsPointerHelper<Callback>::IsPtr<index>();
	} else if constexpr (std::is_pointer<Callback>::value && std::is_function<std::remove_pointer<Callback>::type>::value) {
		return IsPointerHelper<Callback>::IsPtr<index>();
	} else {
		return false;
	}
}

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
	const ECSArchetype& base_archetype = getArchetype(id);
	ECSArchetype archetype;

	archetype.copy(base_archetype, true);
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

template <class Callback, size_t index, class ComponentFirst, class... ComponentsRest, size_t array_size, class... ComponentsPrev>
void ECSManager::iterateInternalHelper(
	Callback&& callback,
	EntityID entity_id,
	int32_t entity_index,
	const ECSQueryResult* (&query_results)[array_size],
	ComponentsPrev&&... prev_comps)
{
	using GetType = typename std::remove_reference<ComponentFirst::GetType>::type;
	constexpr bool is_pointer = IsPointer<Callback, index>();

	const ECSQueryResult& qr = *query_results[index];

	if constexpr (is_pointer) {
		GAFF_ASSERT_MSG(qr.optional, "Function passed to ECSManager::iterate() takes a pointer to a non-optional component.");

		typename GetType* const component_ptr = nullptr;

		if (qr.component_offset >= 0) {
			typename ComponentFirst::GetType component = ComponentFirst::Get(*this, qr, entity_index);
			component_ptr = &component;

			// Final argumant, call the callback.
			if constexpr ((index + 1) == array_size) {
				callback(
					entity_id,
					std::forward<ComponentsPrev>(prev_comps)...,
					component_ptr
				);

			} else {
				iterateInternalHelper<Callback, index + 1, ComponentsRest...>(
					std::forward<Callback>(callback),
					entity_id,
					entity_index,
					query_results,
					std::forward<ComponentsPrev>(prev_comps)...,
					component_ptr
				);
			}

		} else {
			// Final argumant, call the callback.
			if constexpr ((index + 1) == array_size) {
				callback(
					entity_id,
					std::forward<ComponentsPrev>(prev_comps)...,
					component_ptr
				);

			} else {
				iterateInternalHelper<Callback, index + 1, ComponentsRest...>(
					std::forward<Callback>(callback),
					entity_id,
					entity_index,
					query_results,
					std::forward<ComponentsPrev>(prev_comps)...,
					component_ptr
				);
			}
		}

	} else {
		GAFF_ASSERT_MSG(!qr.optional, "Function passed to ECSManager::iterate() takes a reference or copy to an optional component.");
		GAFF_ASSERT_MSG(qr.component_offset >= 0, "Somehow a non-optional component was not found in an entity.");

		typename ComponentFirst::GetType component = ComponentFirst::Get(*this, qr, entity_index);

		// Final argumant, call the callback.
		if constexpr ((index + 1) == array_size) {
			callback(
				entity_id,
				std::forward<ComponentsPrev>(prev_comps)...,
				std::forward<typename ComponentFirst::GetType>(component)
			);

		} else {
			iterateInternalHelper<Callback, index + 1, ComponentsRest...>(
				std::forward<Callback>(callback),
				entity_id,
				entity_index,
				query_results,
				std::forward<ComponentsPrev>(prev_comps)...,
				std::forward<typename ComponentFirst::GetType>(component)
			);
		}
	}
}

template <class Callback, class... Components, size_t array_size>
void ECSManager::iterateInternal(Callback&& callback, const ECSQueryResult* (&query_results)[array_size])
{
	static_assert(sizeof...(Components) == array_size);

	// Assumes all query results are from the same query, and should be pointing at the same entity data.
	const ECSQueryResult* const query_result = query_results[0];
	EntityData* const data = reinterpret_cast<EntityData*>(query_result->entity_data);
	int32_t count = 0;

	for (int32_t i = 0; count < data->num_entities && i < static_cast<int32_t>(data->entity_ids.size()); ++i) {
		if (data->entity_ids[i] == -1) {
			continue;
		}

		if constexpr (sizeof...(Components) == 0) {
			callback(data->entity_ids[i]);
		} else {
			iterateInternalHelper<Callback, 0, Components...>(std::forward<Callback>(callback), data->entity_ids[i], i, query_results);
		}

		++count;
	}
}
