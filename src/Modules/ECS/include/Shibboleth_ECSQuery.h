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

template <class T>
class ECSQueryResult final
{
public:

private:
	void* _archetype_data;
	int32_t _component_offset;

	friend class ECSManager;
};

class ECSQuery final
{
public:
	template <class T>
	using SharedOutput = Vector<typename T::SharedData*>;

	template <class T>
	using TypedFilterFunc = eastl::function<bool (const T&)>;

	template <class T>
	using TypedPushToListFunc = eastl::function<void ()>;

	using PushToListFunc = eastl::function<void ()>;
	using FilterFunc = eastl::function<bool (const void*)>;

	template <class T, class Arg>
	void addShared(TypedFilterFunc<Arg>&& filter)
	{
		//auto func = Gaff::Func([filter](const void* data) -> bool { return filter(*reinterpret_cast<const Arg*>(data)); });
		//addShared(Reflection<T>::GetReflectionDefinition(), std::move(func));
	}

	template <class T>
	void addShared(SharedOutput<T>& output, FilterFunc&& filter)
	{
		//addShared(Reflection<T>::GetReflectionDefinition(), std::move(filter));
	}

	template <class T>
	void addShared(SharedOutput<T>& output)
	{
		//addShared(Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	void add(void)
	{
		//add(Reflection<T>::GetReflectionDefinition());
	}

	void addShared(const Gaff::IReflectionDefinition* ref_def, FilterFunc&& filter);
	void addShared(const Gaff::IReflectionDefinition* ref_def);
	void add(const Gaff::IReflectionDefinition* ref_def);

	bool filter(const ECSArchetype& archetype) const;

private:
	struct QueryDataShared final
	{
		const Gaff::IReflectionDefinition* ref_def;
		FilterFunc filter;
	};

	Vector<QueryDataShared> _shared_components;
	Vector<const Gaff::IReflectionDefinition*> _components;
};

NS_END
