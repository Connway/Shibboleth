/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

NS_REFLECTION

template <class T, class ContainerType>
const Reflection<typename MapVar<T, ContainerType>::KeyReflectionType>& MapVar<T, ContainerType>::GetReflectionKey(void)
{
	return Reflection<KeyReflectionType>::GetReflection();
}

template <class T, class ContainerType>
const Reflection<typename MapVar<T, ContainerType>::ValueReflectionType>& MapVar<T, ContainerType>::GetReflectionValue(void)
{
	return Reflection<ValueReflectionType>::GetReflection();
}

template <class T, class ContainerType>
const IReflection& MapVar<T, ContainerType>::getReflectionKey(void) const
{
	return GetReflectionKey();
}

template <class T, class ContainerType>
const IReflection& MapVar<T, ContainerType>::getReflection(void) const
{
	return GetReflectionValue();
}

template <class T, class ContainerType>
int32_t MapVar<T, ContainerType>::getMapEntryIndex(const void* object, const void* key)
{
	const ContainerType& map = *IVar<T>::template get<ContainerType>(object);
	const auto it = map.find(*reinterpret_cast<const KeyVarType*>(key));

	return static_cast<int32_t>(eastl::distance(map.begin(), it));
}

template <class T, class ContainerType>
void MapVar<T, ContainerType>::addMapEntry(void* object, const void* key)
{
	if constexpr (VarTypeHelper<T, ContainerType>::k_key_can_copy) {
		if (IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		(*IVar<T>::template get<ContainerType>(object))[*reinterpret_cast<const KeyVarType*>(key)];

	} else {
		GAFF_REF(object, key);

		GAFF_ASSERT_MSG(
			false,
			"MapVar<T, ContainerType>::addMapEntry() was called with [Key/Value]ReflectionType of '%s'. Element name: '%s'.",
			reinterpret_cast<const char*>(Reflection<KeyReflectionType>::GetName()),
			reinterpret_cast<const char*>(Reflection<ValueReflectionType>::GetName()),
			reinterpret_cast<const char*>(_base_name.data())
		);
	}
}

template <class T, class ContainerType>
void MapVar<T, ContainerType>::addMapEntryMove(void* object, void* key)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	(*IVar<T>::template get<ContainerType>(object))[std::move(*reinterpret_cast<const KeyVarType*>(key))];
}

template <class T, class ContainerType>
const void* MapVar<T, ContainerType>::getData(const void* object) const
{
	return const_cast<MapVar<T, ContainerType>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class ContainerType>
void* MapVar<T, ContainerType>::getData(void* object)
{
	return IVar<T>::template get<ContainerType>(object);
}

template <class T, class ContainerType>
void MapVar<T, ContainerType>::setData(void* object, const void* data)
{
	if constexpr (VarTypeHelper<T, ContainerType>::k_key_can_copy && VarTypeHelper<T, ContainerType>::k_value_can_copy) {
		if (IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		*IVar<T>::template get<ContainerType>(object) = *reinterpret_cast<const ContainerType*>(data);

	} else {
		GAFF_REF(object, data);

		GAFF_ASSERT_MSG(
			false,
			"MapVar<T, ContainerType>::setData() was called with [Key/Value]ReflectionType of '%s'. Element name: '%s'.",
			reinterpret_cast<const char*>(Reflection<KeyReflectionType>::GetName()),
			reinterpret_cast<const char*>(Reflection<ValueReflectionType>::GetName()),
			reinterpret_cast<const char*>(_base_name.data())
		);
	}
}

template <class T, class ContainerType>
void MapVar<T, ContainerType>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	*IVar<T>::template get<ContainerType>(object) = std::move(*reinterpret_cast<ContainerType*>(data));
}

template <class T, class ContainerType>
bool MapVar<T, ContainerType>::isMap(void) const
{
	return true;
}

template <class T, class ContainerType>
int32_t MapVar<T, ContainerType>::size(const void* object) const
{
	return IVar<T>::template get<ContainerType>(object)->size();
}

template <class T, class ContainerType>
const void* MapVar<T, ContainerType>::getElement(const void* object, int32_t index) const
{
	return const_cast<MapVar<T, ContainerType>*>(this)->getElement(const_cast<void*>(object), index);
}

template <class T, class ContainerType>
void* MapVar<T, ContainerType>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(Gaff::ValidIndex(index, size(object)));
	return &IVar<T>::template get<ContainerType>(object)->at(index);
}

template <class T, class ContainerType>
void MapVar<T, ContainerType>::setElement(void* object, int32_t index, const void* data)
{
	if constexpr (VarTypeHelper<T, ContainerType>::k_value_can_copy) {
		GAFF_ASSERT(Gaff::ValidIndex(index, size(object)));

		if (IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		IVar<T>::template get<ContainerType>(object)->at(index).second = *reinterpret_cast<const ValueVarType*>(data);

	} else {
		GAFF_REF(object, index, data);

		GAFF_ASSERT_MSG(
			false,
			"MapVar<T, ContainerType>::setElement() was called with [Key/Value]ReflectionType of '%s'. Element name: '%s'.",
			reinterpret_cast<const char*>(Reflection<KeyReflectionType>::GetName()),
			reinterpret_cast<const char*>(Reflection<ValueReflectionType>::GetName()),
			reinterpret_cast<const char*>(_base_name.data())
		);
	}
}

template <class T, class ContainerType>
void MapVar<T, ContainerType>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(Gaff::ValidIndex(index, size(object)));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	IVar<T>::template get<ContainerType>(object)->at(index).second = std::move(*reinterpret_cast<const ValueVarType*>(data));
}

template <class T, class ContainerType>
void MapVar<T, ContainerType>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(Gaff::ValidIndex(index_a, size(object)));
	GAFF_ASSERT(Gaff::ValidIndex(index_b, size(object)));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	ContainerType& map = *IVar<T>::template get<ContainerType>(object);
	eastl::swap(map.at(index_a).second, map.at(index_b).second);
	//eastl::swap(_elements[index_a], _elements[index_b]);
	//_cached_element_vars[index_a].second = &_elements[index_a];
	//_cached_element_vars[index_b].second = &_elements[index_b];
}

template <class T, class ContainerType>
bool MapVar<T, ContainerType>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	const int32_t size = reader.size();
	ContainerType& map = *IVar<T>::template get<ContainerType>(&object);

	map.reserve(static_cast<size_t>(size));

	bool success = true;

	for (int32_t i = 0; i < size; ++i) {
		Shibboleth::ScopeGuard scope = reader.enterElementGuard(i);
		bool key_loaded = true;
		KeyVarType key;

		{
			Shibboleth::ScopeGuard guard_key = reader.enterElementGuard(u8"key");
			key_loaded = Reflection<KeyReflectionType>::GetInstance().load(reader, key);
			success = success && key_loaded;
		}

		if (key_loaded)
		{
			Shibboleth::ScopeGuard guard_value = reader.enterElementGuard(u8"value");
			success = Reflection<ValueReflectionType>::GetInstance().load(reader, map[key]) && success;

		} else {
			// $TODO: log error.
		}
	}

	//_cached_element_vars.resize(static_cast<size_t>(size));
	//_elements.resize(static_cast<size_t>(size));
	//regenerateSubVars(0, size);

	return success;
}

template <class T, class ContainerType>
void MapVar<T, ContainerType>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const ContainerType& vec = *IVar<T>::template get<ContainerType>(&object);
	const int32_t size = static_cast<int32_t>(vec.size());
	writer.startArray(static_cast<uint32_t>(size));

	for (int32_t i = 0; i < size; ++i) {
		const auto& entry = vec.at(i);

		writer.startObject(2);

		writer.writeKey(u8"key");
		Reflection<KeyReflectionType>::GetInstance().save(writer, entry.first);

		writer.writeKey(u8"value");
		Reflection<ValueReflectionType>::GetInstance().save(writer, entry.second);

		writer.endObject();
	}

	writer.endArray();
}

//template <class T, class ContainerType>
//const Shibboleth::Vector<IReflectionVar::SubVarData>& MapVar<T, ContainerType>::getSubVars(void)
//{
//	return _cached_elements_vars;
//}

template <class T, class ContainerType>
void MapVar<T, ContainerType>::setSubVarBaseName(eastl::u8string_view base_name)
{
	_base_name = base_name;
	//regenerateSubVars(0, static_cast<int32_t>(_elements.size()));
}

//template <class T, class ContainerType>
//void MapVar<T, ContainerType>::regenerateSubVars(int32_t range_begin, int32_t range_end)
//{
//	for (int32_t i = range_begin; i < range_end; ++i) {
//		Shibboleth::U8String element_path(Shibboleth::ProxyAllocator("Reflection"));
//		element_path.append_sprintf(u8"%s[%i]", _base_name.data(), i);

//		_elements[i].setOffset(static_cast<ptrdiff_t>(i * sizeof(VarType)));
//		_elements[i].setParent(this);
//		_elements[i].setNoSerialize(true);
//		_cached_element_vars[i].first = Shibboleth::HashString32<>(element_path);
//		_cached_element_vars[i].second = &_elements[i];
//	}
//}

NS_END