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

// IVar
template <class T>
template <class VarType>
IVar<T>::IVar(VarType T::*ptr):
	IVar(Gaff::OffsetOfMember(ptr))
{
	GAFF_ASSERT(ptr);
}

template <class T>
IVar<T>::IVar(ptrdiff_t offset):
	_offset(offset)
{
	GAFF_ASSERT(offset >= 0);
}

template <class T>
template <class VarType>
const VarType& IVar<T>::IVar::getDataT(const T& object) const
{
	using Type = typename std::remove_reference<VarType>::type;

	const auto& other_refl = Reflection<Type>::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<Type>::value) {
		GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
	} else {
		GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
	}

	return *reinterpret_cast<const VarType*>(getData(&object));
}

template <class T>
template <class VarType>
void IVar<T>::IVar::setDataT(T& object, const VarType& data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	using Type = typename std::remove_reference<VarType>::type;

	const auto& other_refl = Reflection<Type>::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<Type>::value) {
		GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
	} else {
		GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
	}

	setData(&object, &data);
}

template <class T>
template <class VarType>
void IVar<T>::IVar::setDataMoveT(T& object, VarType&& data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	using Type = typename std::remove_reference<VarType>::type;

	const auto& other_refl = Reflection<Type>::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<Type>::value) {
		GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
	} else {
		GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
	}

	setDataMove(&object, &data);
}

template <class T>
template <class VarType>
const VarType& IVar<T>::IVar::getElementT(const T& object, int32_t index) const
{
	using Type = typename std::remove_reference<VarType>::type;

	const auto& other_refl = Reflection<Type>::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<Type>::value) {
		GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
	} else {
		GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
	}

	GAFF_ASSERT((isFixedArray() || isVector()) && size(&object) > index);
	return *reinterpret_cast<const VarType*>(getElement(&object, index));
}

template <class T>
template <class VarType>
void IVar<T>::IVar::setElementT(T& object, int32_t index, const VarType& data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	using Type = typename std::remove_reference<VarType>::type;

	const auto& other_refl = Reflection<Type>::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<Type>::value) {
		GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
	} else {
		GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
	}

	GAFF_ASSERT((isFixedArray() || isVector()) && size(&object) > index);
	setElement(&object, index, &data);
}

template <class T>
template <class VarType>
void IVar<T>::IVar::setElementMoveT(T& object, int32_t index, VarType&& data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	using Type = typename std::remove_reference<VarType>::type;

	const auto& other_refl = Reflection<Type>::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<Type>::value) {
		GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
	} else {
		GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
	}

	GAFF_ASSERT((isFixedArray() || isVector()) && size(&object) > index);
	setElementMove(&object, index, &data);
}

template <class T>
void IVar<T>::setOffset(ptrdiff_t offset)
{
	_offset = offset;
}

template <class T>
void IVar<T>::setParent(IVar<T>* parent)
{
	_parent = parent;
}

template <class T>
const void* IVar<T>::adjust(const void* object) const
{
	return const_cast<IVar<T>*>(this)->adjust(const_cast<void*>(object));
}

template <class T>
void* IVar<T>::adjust(void* object)
{
	if (_parent) {
		object = _parent->adjust(object);

		if (_parent->isVector() || _parent->isFixedArray()) {
			object = _parent->getElement(object, 0);
		}
	}

	return reinterpret_cast<int8_t*>(object) + _offset;
}

template <class T>
template <class VarType>
const VarType* IVar<T>::get(const void* object) const
{
	return const_cast<IVar<T>*>(this)->template get<VarType>(const_cast<void*>(object));
}

template <class T>
template <class VarType>
VarType* IVar<T>::get(void* object)
{
	return reinterpret_cast<VarType*>(adjust(object));
}



// Var
template <class T, class VarType>
Var<T, VarType>::Var(VarType T::*ptr):
	IVar<T>(ptr)
{
}

template <class T, class VarType>
const Reflection<typename Var<T, VarType>::ReflectionType>& Var<T, VarType>::GetReflection(void)
{
	return Reflection<ReflectionType>::GetInstance();
}

template <class T, class VarType>
const IReflection& Var<T, VarType>::getReflection(void) const
{
	return GetReflection();
}

template <class T, class VarType>
const void* Var<T, VarType>::getData(const void* object) const
{
	return const_cast<Var<T, VarType>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class VarType>
void* Var<T, VarType>::getData(void* object)
{
	return IVar<T>::template get<VarType>(object);
}

template <class T, class VarType>
void Var<T, VarType>::setData(void* object, const void* data)
{
	if constexpr (VarTypeHelper<T, VarType>::k_can_copy) {
		if (IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		VarType* const var = IVar<T>::template get<VarType>(&object);
		*var = *reinterpret_cast<const VarType*>(data);

	} else {
		GAFF_REF(object, data);

		GAFF_ASSERT_MSG(
			false,
			"Var<T, VarType>::setData() was called with ReflectionType of '%s'.",
			reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName())
		);
	}
}

template <class T, class VarType>
void Var<T, VarType>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	VarType* const var = IVar<T>::template get<VarType>(&object);
	*var = std::move(*reinterpret_cast<VarType*>(data));
}

template <class T, class VarType>
bool Var<T, VarType>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	VarType* const var = IVar<T>::template get<VarType>(&object);
	return Reflection<ReflectionType>::GetInstance().load(reader, *var);
}

template <class T, class VarType>
void Var<T, VarType>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const VarType* const var = IVar<T>::template get<VarType>(&object);
	Reflection<ReflectionType>::GetInstance().save(writer, *var);
}



// VectorVar
template <class T, class ContainerType>
VectorVar<T, ContainerType>::VectorVar(ContainerType T::*ptr):
	IVar<T>(ptr)
{
	GAFF_ASSERT(ptr);

	if constexpr (!std::is_enum_v<ReflectionType> && !ReflectionDefinition<ReflectionType>::IsBuiltIn()) {
		static_assert(std::is_base_of_v<ReflectionType, VarType>);
	}
}

template <class T, class ContainerType>
VectorVar<T, ContainerType>::VectorVar(void)
{
	if constexpr (!std::is_enum_v<ReflectionType> && !ReflectionDefinition<ReflectionType>::IsBuiltIn()) {
		static_assert(std::is_base_of_v<ReflectionType, VarType>);
	}
}

template <class T, class ContainerType>
const Reflection<typename VectorVar<T, ContainerType>::ReflectionType>& VectorVar<T, ContainerType>::GetReflection(void)
{
	return Reflection<ReflectionType>::GetInstance();
}

template <class T, class ContainerType>
const IReflection& VectorVar<T, ContainerType>::getReflection(void) const
{
	return GetReflection();
}

template <class T, class ContainerType>
const void* VectorVar<T, ContainerType>::getData(const void* object) const
{
	return const_cast<VectorVar<T, ContainerType>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class ContainerType>
void* VectorVar<T, ContainerType>::getData(void* object)
{
	return IVar<T>::template get<ContainerType>(object);
}

template <class T, class ContainerType>
void VectorVar<T, ContainerType>::setData(void* object, const void* data)
{
	if constexpr (VarTypeHelper<T, ContainerType>::k_can_copy) {
		if (IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		*IVar<T>::template get<ContainerType>(object) = *reinterpret_cast<const ContainerType*>(data);

	} else {
		GAFF_REF(object, data);

		GAFF_ASSERT_MSG(
			false,
			"VectorVar<T, ContainerType>::setData() was called with ReflectionType of '%s'. Element name: '%s'.",
			reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName()),
			reinterpret_cast<const char*>(_base_name.data())
		);
	}
}

template <class T, class ContainerType>
void VectorVar<T, ContainerType>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	*IVar<T>::template get<ContainerType>(object) = std::move(*reinterpret_cast<ContainerType*>(data));
}

template <class T, class ContainerType>
bool VectorVar<T, ContainerType>::isVector(void) const
{
	return !VarTypeHelper<T, ContainerType>::k_is_fixed_array;
}

template <class T, class ContainerType>
bool VectorVar<T, ContainerType>::isFixedArray(void) const
{
	return VarTypeHelper<T, ContainerType>::k_is_fixed_array;
}

template <class T, class ContainerType>
int32_t VectorVar<T, ContainerType>::size(const void* object) const
{
	return IVar<T>::template get<ContainerType>(object)->size();
}

template <class T, class ContainerType>
const void* VectorVar<T, ContainerType>::getElement(const void* object, int32_t index) const
{
	return const_cast<VectorVar<T, ContainerType>*>(this)->getElement(const_cast<void*>(object), index);
}

template <class T, class ContainerType>
void* VectorVar<T, ContainerType>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(index >= 0 && index < size(object));
	return &(IVar<T>::template get<ContainerType>(object))[index];
}

template <class T, class ContainerType>
void VectorVar<T, ContainerType>::setElement(void* object, int32_t index, const void* data)
{
	if constexpr (VarTypeHelper<T, ContainerType>::k_can_copy) {
		GAFF_ASSERT(index >= 0 && index < size(object));

		if (IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		(*IVar<T>::template get<ContainerType>(object))[index] = *reinterpret_cast<const VarType*>(data);

	} else {
		GAFF_REF(object, index, data);

		GAFF_ASSERT_MSG(
			false,
			"VectorVar<T, ContainerType>::setElement() was called with ReflectionType of '%s'. Element name: '%s'.",
			reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName()),
			reinterpret_cast<const char*>(_base_name.data())
		);
	}
}

template <class T, class ContainerType>
void VectorVar<T, ContainerType>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	(*IVar<T>::template get<ContainerType>(object))[index] = std::move(*reinterpret_cast<VarType*>(data));
}

template <class T, class ContainerType>
void VectorVar<T, ContainerType>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a >= 0 && index_a < size(object));
	GAFF_ASSERT(index_b >= 0 && index_b < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	ContainerType& vec = *IVar<T>::template get<ContainerType>(object);
	eastl::swap(vec[index_a], vec[index_b]);
	eastl::swap(_elements[index_a], _elements[index_b]);
	_cached_element_vars[index_a].second = &_elements[index_a];
	_cached_element_vars[index_b].second = &_elements[index_b];
}

template <class T, class ContainerType>
void VectorVar<T, ContainerType>::resize(void* object, size_t new_size)
{
	if constexpr (VarTypeHelper<T, ContainerType>::k_is_fixed_array) {
		GAFF_REF(object, new_size);

		GAFF_ASSERT_MSG(
			false,
			"VectorVar<T, ContainerType>::resize() was called with ReflectionType of '%s' on a fixed size array. Element name: '%s'.",
			reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName()),
			reinterpret_cast<const char*>(_base_name.data())
		);

	} else {
		if (IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		IVar<T>::template get<ContainerType>(object)->resize(new_size);

		const size_t old_size = _elements.size();

		_cached_element_vars.resize(new_size);
		_elements.resize(new_size);

		if (new_size > old_size) {
			regenerateSubVars(old_size + 1, new_size);
		}
	}
}

template <class T, class ContainerType>
void VectorVar<T, ContainerType>::remove(void* object, int32_t index)
{
	if constexpr (VarTypeHelper<T, ContainerType>::k_is_fixed_array) {
		GAFF_REF(object, index);

		GAFF_ASSERT_MSG(
			false,
			"VectorVar<T, ContainerType>::remove() was called with ReflectionType of '%s' on a fixed size array. Element name: '%s'.",
			reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName()),
			reinterpret_cast<const char*>(_base_name.data())
		);

	} else {
		if (IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		ContainerType& vec = *IVar<T>::template get<ContainerType>(object);
		vec.erase(vec.begin() + index);

		_cached_element_vars.erase(_cached_element_vars.begin() + index);
		_elements.erase(_elements.begin() + index);

		regenerateSubVars(index, static_cast<int32_t>(_elements.size()));
	}
}

template <class T, class ContainerType>
bool VectorVar<T, ContainerType>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	const int32_t size = reader.size();
	ContainerType& vec = *IVar<T>::template get<ContainerType>(&object);

	if constexpr (VarTypeHelper<T, ContainerType>::k_is_fixed_array) {
		GAFF_ASSERT(size == static_cast<int32_t>(vec.size()));
	} else {
		vec.resize(static_cast<size_t>(size));
	}

	bool success = true;

	for (int32_t i = 0; i < size; ++i) {
		Shibboleth::ScopeGuard scope = reader.enterElementGuard(i);

		if (!Reflection<ReflectionType>::GetInstance().load(reader, vec[i])) {
			// $TODO: Log error.
			success = false;
		}
	}

	_cached_element_vars.resize(static_cast<size_t>(size));
	_elements.resize(static_cast<size_t>(size));
	regenerateSubVars(0, size);

	return success;
}

template <class T, class ContainerType>
void VectorVar<T, ContainerType>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const ContainerType& vec = *IVar<T>::template get<ContainerType>(&object);
	const int32_t size = static_cast<int32_t>(vec.size());
	writer.startArray(static_cast<uint32_t>(size));

	for (int32_t i = 0; i < size; ++i) {
		Reflection<ReflectionType>::GetInstance().save(writer, vec[i]);
	}

	writer.endArray();
}

template <class T, class ContainerType>
const Shibboleth::Vector<IReflectionVar::SubVarData>& VectorVar<T, ContainerType>::getSubVars(void)
{
	return _cached_element_vars;
}

template <class T, class ContainerType>
void VectorVar<T, ContainerType>::setSubVarBaseName(eastl::u8string_view base_name)
{
	_base_name = base_name;
	regenerateSubVars(0, static_cast<int32_t>(_elements.size()));
}

template <class T, class ContainerType>
void VectorVar<T, ContainerType>::regenerateSubVars(int32_t range_begin, int32_t range_end)
{
	for (int32_t i = range_begin; i < range_end; ++i) {
		Shibboleth::U8String element_path(Shibboleth::ProxyAllocator("Reflection"));
		element_path.append_sprintf(u8"%s[%i]", _base_name.data(), i);

		_elements[i].setOffset(static_cast<ptrdiff_t>(i * sizeof(VarType)));
		_elements[i].setParent(this);
		_elements[i].setNoSerialize(true);
		_cached_element_vars[i].first = Shibboleth::HashString32<>(element_path);
		_cached_element_vars[i].second = &_elements[i];
	}
}

NS_END
