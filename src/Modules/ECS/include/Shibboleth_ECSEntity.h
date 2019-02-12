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
	template <class T, class Arg>
	void addShared(const eastl::function<bool (Arg&)>& filter)
	{
		auto func = Gaff::Func([filter](void* data) -> bool { return filter(*reinterpret_cast<Arg*>(data)); });
		addShared(Reflection<T>::GetReflectionDefinition(), std::move(func));
	}

	template <class T, class Arg>
	void addShared(eastl::function<bool (Arg&)>&& filter)
	{
		auto func = Gaff::Func([filter](void* data) -> bool { return filter(*reinterpret_cast<Arg*>(data)); });
		addShared(Reflection<T>::GetReflectionDefinition(), std::move(func));
	}

	template <class T, class Arg>
	void add(const eastl::function<bool (Arg&)>& filter)
	{
		auto func = Gaff::Func([filter](void* data) -> bool { return filter(*reinterpret_cast<Arg*>(data)); });
		add(Reflection<T>::GetReflectionDefinition(), std::move(func));
	}

	template <class T, class Arg>
	void add(eastl::function<bool (Arg&)>&& filter)
	{
		auto func = Gaff::Func([filter](void* data) -> bool { return filter(*reinterpret_cast<Arg*>(data)); });
		add(Reflection<T>::GetReflectionDefinition(), std::move(func));
	}

	template <class T>
	void addShared(const eastl::function<bool (void*)>& filter)
	{
		addShared(Reflection<T>::GetReflectionDefinition(), filter);
	}

	template <class T>
	void addShared(eastl::function<bool (void*)>&& filter)
	{
		addShared(Reflection<T>::GetReflectionDefinition(), std::move(filter));
	}

	template <class T>
	void addShared(void)
	{
		addShared(Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	void add(const eastl::function<bool (void*)>& filter)
	{
		add(Reflection<T>::GetReflectionDefinition(), filter);
	}

	template <class T>
	void add(eastl::function<bool (void*)>&& filter)
	{
		add(Reflection<T>::GetReflectionDefinition(), std::move(filter));
	}

	template <class T>
	void add(void)
	{
		add(Reflection<T>::GetReflectionDefinition());
	}

	void addShared(const Gaff::IReflectionDefinition* ref_def, const eastl::function<bool (void*)>& filter);
	void addShared(const Gaff::IReflectionDefinition* ref_def, eastl::function<bool (void*)>&& filter);
	void addShared(const Gaff::IReflectionDefinition* ref_def);
	void add(const Gaff::IReflectionDefinition* ref_def, const eastl::function<bool (void*)>& filter);
	void add(const Gaff::IReflectionDefinition* ref_def, eastl::function<bool (void*)>&& filter);
	void add(const Gaff::IReflectionDefinition* ref_def);

	//bool filter() const;

private:
	struct QueryData final
	{
		const Gaff::IReflectionDefinition* ref_def;
		eastl::function<bool (void*)> filter;
	};

	Vector<QueryData> _shared_components;
	Vector<QueryData> _components;
};

NS_END
