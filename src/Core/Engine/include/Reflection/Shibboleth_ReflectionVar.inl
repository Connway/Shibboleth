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



// Var
template <class T, class VarType>
Var<T, VarType>::Var(VarType T::*ptr):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class VarType>
const IReflection& Var<T, VarType>::getReflection(void) const
{
	if constexpr (Gaff::IsFlags<VarType>()) {
		return Reflection<typename Gaff::GetFlagsEnum<VarType>::Enum>::GetInstance();
	} else {
		return Reflection<VarType>::GetInstance();
	}
}

template <class T, class VarType>
const void* Var<T, VarType>::getData(const void* object) const
{
	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType>
void* Var<T, VarType>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType>
void Var<T, VarType>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = *reinterpret_cast<const VarType*>(data);
}

template <class T, class VarType>
void Var<T, VarType>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = std::move(*reinterpret_cast<VarType*>(data));
}

template <class T, class VarType>
bool Var<T, VarType>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	VarType* const var = &(object.*_ptr);
	return Reflection<VarType>::GetInstance().load(reader, *var);
}

template <class T, class VarType>
void Var<T, VarType>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const VarType* const var = &(object.*_ptr);
	Reflection<VarType>::GetInstance().save(writer, *var);
}



template <class T, class VarType>
IVar<T>* VarPtrTypeHelper<T, VarType>::Create(
	eastl::u8string_view name,
	VarType T::* ptr,
	const Shibboleth::ProxyAllocator& allocator,
	Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >&)
{
	static_assert(!std::is_reference<VarType>::value, "Cannot reflect references.");
	static_assert(!std::is_pointer<VarType>::value, "Cannot reflect pointers.");
	static_assert(!std::is_const<VarType>::value, "Cannot reflect const values.");
	static_assert(Reflection<VarType>::HasReflection, "Var type is not reflected!");

	GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string.");

	IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(Var<T, VarType>), allocator, ptr);

	return var_ptr;
}

NS_END
