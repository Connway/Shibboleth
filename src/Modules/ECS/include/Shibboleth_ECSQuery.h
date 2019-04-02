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
};

class ECSQuery final
{
public:
	using SharedPushToListFunc = eastl::function<void (const void*)>;

	template <class T>
	using TypedFilterFunc = eastl::function<bool (const T&)>;

	using FilterFunc = eastl::function<bool (const void*)>;

	template <class T>
	using SharedOutput = Vector<const typename T*>;

	using Output = Vector<ECSQueryResult>;

	template <class T, class Arg>
	void addShared(SharedOutput<T>& output, TypedFilterFunc<typename T>&& filter)
	{
		auto push_func = Gaff::Func<void (const void*)>(
			[&output](const void* data) -> void { output.emplace_back(reinterpret_cast<const typename T*>(data)); }
		);

		auto filter_func = Gaff::Func<bool (const void*)>(
			[filter](const void* data) -> bool { return filter(*reinterpret_cast<const typename T*>(data)); }
		);

		addShared(Reflection<T>::GetReflectionDefinition(), std::move(push_func), std::move(filter_func));
	}

	template <class T>
	void addShared(SharedOutput<T>& output, FilterFunc&& filter)
	{
		auto push_func = Gaff::Func<void (const void*)>(
			[&output](const void* data) -> void { output.emplace_back(reinterpret_cast<const typename T*>(data)); }
		);

		addShared(Reflection<T>::GetReflectionDefinition(), std::move(push_func), std::move(filter));
	}

	template <class T>
	void addShared(SharedOutput<T>& output)
	{
		auto push_func = Gaff::Func<void (const void*)>(
			[&output](const void* data) -> void { output.emplace_back(reinterpret_cast<const typename T*>(data)); }
		);

		addShared(Reflection<T>::GetReflectionDefinition(), std::move(push_func));
	}

	template <class T>
	void add(Output& output)
	{
		add(Reflection<T>::GetReflectionDefinition(), output);
	}

	template <class T>
	void add(void)
	{
		add(Reflection<T>::GetReflectionDefinition());
	}

	void addShared(const Gaff::IReflectionDefinition& ref_def, SharedPushToListFunc&& push_func, FilterFunc&& filter_func);
	void addShared(const Gaff::IReflectionDefinition& ref_def, SharedPushToListFunc&& push_func);
	void addShared(const Gaff::IReflectionDefinition& ref_def);

	void add(const Gaff::IReflectionDefinition& ref_def, Output& output);
	void add(const Gaff::IReflectionDefinition& ref_def);

	bool filter(const ECSArchetype& archetype, void* entity_data);

private:
	struct QueryDataShared final
	{
		const Gaff::IReflectionDefinition* ref_def;
		SharedPushToListFunc push_func;
		FilterFunc filter_func;
	};

	struct QueryData final
	{
		const Gaff::IReflectionDefinition* ref_def;
		Output* output;
	};

	Vector<QueryDataShared> _shared_components;
	Vector<QueryData> _components;

	friend class ECSManager;
};

NS_END
