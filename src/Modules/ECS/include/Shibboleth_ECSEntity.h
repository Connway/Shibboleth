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

#include <Shibboleth_Vector.h>
#include <Gaff_Function.h>

NS_GAFF
	class IReflectionDefinition;
NS_END

NS_SHIBBOLETH

using EntityID = int32_t;

class ECSQuery final
{
public:
	template <class T>
	using TypedFilterFunc = eastl::function<bool (T&)>;

	template <class T>
	using TypedPushToListFunc = eastl::function<void ()>;

	using PushToListFunc = eastl::function<void ()>;
	using FilterFunc = eastl::function<bool (void*)>;

	template <class T, class Arg>
	void addShared(PushToListFunc&& push_to_list, TypedFilterFunc<Arg>&& filter)
	{
		auto func = Gaff::Func([filter](void* data) -> bool { return filter(*reinterpret_cast<Arg*>(data)); });
		addShared(Reflection<T>::GetReflectionDefinition(), std::move(push_to_list), std::move(func));
	}

	template <class T>
	void addShared(PushToListFunc&& push_to_list, FilterFunc&& filter)
	{
		addShared(Reflection<T>::GetReflectionDefinition(), std::move(push_to_list), std::move(filter));
	}

	template <class T>
	void addShared(PushToListFunc&& push_to_list)
	{
		addShared(Reflection<T>::GetReflectionDefinition(), std::move(push_to_list));
	}

	template <class T>
	void add(PushToListFunc&& push_to_list)
	{
		add(Reflection<T>::GetReflectionDefinition(), std::move(push_to_list));
	}

	void addShared(const Gaff::IReflectionDefinition* ref_def, PushToListFunc&& push_to_list, FilterFunc&& filter);
	void addShared(const Gaff::IReflectionDefinition* ref_def, PushToListFunc&& push_to_list);
	void add(const Gaff::IReflectionDefinition* ref_def, PushToListFunc&& push_to_list);

	//bool filter() const;

private:
	struct QueryDataShared final
	{
		const Gaff::IReflectionDefinition* ref_def;
		PushToListFunc push_to_list;
		FilterFunc filter;
	};

	struct QueryData final
	{
		const Gaff::IReflectionDefinition* ref_def;
		PushToListFunc push_to_list;
	};

	Vector<QueryDataShared> _shared_components;
	Vector<QueryData> _components;
};

NS_END
