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

#pragma once

#include "Shibboleth_ECSEntity.h"
#include <Shibboleth_Vector.h>
#include <Gaff_Function.h>

NS_GAFF
	class IReflectionDefinition;
NS_END

NS_SHIBBOLETH

class ECSArchetype;

struct ECSQueryResult final
{
	int32_t component_offset;
	void* entity_data;
	bool optional;
};

class ECSQuery final
{
public:
	using SharedPushToListFunc = eastl::function<void (const void*)>;
	using SharedEraseFromListFunc = eastl::function<void (int32_t)>;

	template <class T>
	using TypedFilterFunc = eastl::function<bool (const T&)>;

	using FilterFunc = eastl::function<bool (const void*)>;

	template <class T>
	using SharedOutput = Vector<const typename T*>;

	using Output = Vector<ECSQueryResult>;

	template <class T>
	void addShared(SharedOutput<T>& output, TypedFilterFunc<typename T>&& filter, bool optional = false)
	{
		auto push_func = Gaff::Func<void (const void*)>(
			[&output](const void* data) -> void { output.emplace_back(reinterpret_cast<const typename T*>(data)); }
		);

		auto erase_func = Gaff::Func<void (int32_t)>(
			[&output](int32_t index) -> void { output.erase(output.begin() + index); }
		);

		auto filter_func = Gaff::Func<bool (const void*)>(
			[filter](const void* data) -> bool { return filter(*reinterpret_cast<const typename T*>(data)); }
		);

		addShared(Reflection<T>::GetReflectionDefinition(), std::move(push_func), std::move(erase_func), std::move(filter_func), optional);
	}

	template <class T>
	void addShared(SharedOutput<T>& output, FilterFunc&& filter, bool optional = false)
	{
		auto push_func = Gaff::Func<void (const void*)>(
			[&output](const void* data) -> void { output.emplace_back(reinterpret_cast<const typename T*>(data)); }
		);

		auto erase_func = Gaff::Func<void(int32_t)>(
			[&output](int32_t index) -> void { output.erase(output.begin() + index); }
		);

		addShared(Reflection<T>::GetReflectionDefinition(), std::move(push_func), std::move(erase_func), std::move(filter), optional);
	}

	template <class T>
	void addShared(SharedOutput<T>& output, bool optional = false)
	{
		auto push_func = Gaff::Func<void (const void*)>(
			[&output](const void* data) -> void { output.emplace_back(reinterpret_cast<const typename T*>(data)); }
		);

		auto erase_func = Gaff::Func<void(int32_t)>(
			[&output](int32_t index) -> void { output.erase(output.begin() + index); }
		);

		addShared(Reflection<T>::GetReflectionDefinition(), std::move(push_func), std::move(erase_func), optional);
	}

	template <class T>
	void add(Output& output, bool optional = false)
	{
		add(Reflection<T>::GetReflectionDefinition(), output, optional);
	}

	template <class T>
	void add(void)
	{
		add(Reflection<T>::GetReflectionDefinition());
	}

	ECSQuery(const ProxyAllocator& allocator = ProxyAllocator::GetGlobal());

	void addShared(const Gaff::IReflectionDefinition& ref_def, SharedPushToListFunc&& push_func, SharedEraseFromListFunc&& erase_func, FilterFunc&& filter_func, bool optional = false);
	void addShared(const Gaff::IReflectionDefinition& ref_def, SharedPushToListFunc&& push_func, SharedEraseFromListFunc&& erase_func, bool optional = false);
	void addShared(const Gaff::IReflectionDefinition& ref_def, bool optional = false);

	void add(const Gaff::IReflectionDefinition& ref_def, Output& output, bool optional = false);
	void add(const Gaff::IReflectionDefinition& ref_def);

	// Used when only querying for shared components and still want to iterate over entities.
	void addEntities(Output& output);

	void addArchetypeCallbacks(eastl::function<void (const ECSArchetype&)>&& added_callback, eastl::function<void (int32_t)>&& removed_callback);
	void removeArchetype(const void* entity_data);

	bool filter(const ECSArchetype& archetype, void* entity_data);

private:
	struct QueryDataShared final
	{
		const Gaff::IReflectionDefinition* ref_def;
		SharedPushToListFunc push_func;
		SharedEraseFromListFunc erase_func;
		FilterFunc filter_func;
		bool optional;
	};

	struct QueryData final
	{
		const Gaff::IReflectionDefinition* ref_def;
		Output* output;
		bool optional;
	};

	struct Callbacks final
	{
		eastl::function<void (const ECSArchetype&)> add;
		eastl::function<void (int32_t)> remove;
	};

	Vector<QueryDataShared> _shared_components;
	Vector<QueryData> _components;
	Vector<Output*> _entities;
	Vector<Callbacks> _callbacks;
	Vector<void*> _entity_data;

	friend class ECSManager;
};

NS_END
