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



// VectorVar
template <class T, class VarType, class Vec_Allocator>
VectorVar<T, VarType, Vec_Allocator>::VectorVar(VectorType T::*ptr):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class VarType, class Vec_Allocator>
const IReflection& VectorVar<T, VarType, Vec_Allocator>::getReflection(void) const
{
	return Reflection<VarType>::GetInstance();
}

template <class T, class VarType, class Vec_Allocator>
const void* VectorVar<T, VarType, Vec_Allocator>::getData(const void* object) const
{
	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType, class Vec_Allocator>
void* VectorVar<T, VarType, Vec_Allocator>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType, class Vec_Allocator>
void VectorVar<T, VarType, Vec_Allocator>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = *reinterpret_cast<const VectorType*>(data);
}

template <class T, class VarType, class Vec_Allocator>
void VectorVar<T, VarType, Vec_Allocator>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = std::move(*reinterpret_cast<VectorType*>(data));
}

template <class T, class VarType, class Vec_Allocator>
int32_t VectorVar<T, VarType, Vec_Allocator>::size(const void* object) const
{
	const T* const obj = reinterpret_cast<const T*>(object);
	return static_cast<int32_t>((obj->*_ptr).size());
}

template <class T, class VarType, class Vec_Allocator>
const void* VectorVar<T, VarType, Vec_Allocator>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class VarType, class Vec_Allocator>
void* VectorVar<T, VarType, Vec_Allocator>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class VarType, class Vec_Allocator>
void VectorVar<T, VarType, Vec_Allocator>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = *reinterpret_cast<const VarType*>(data);
}

template <class T, class VarType, class Vec_Allocator>
void VectorVar<T, VarType, Vec_Allocator>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = std::move(*reinterpret_cast<const VarType*>(data));
}

template <class T, class VarType, class Vec_Allocator>
void VectorVar<T, VarType, Vec_Allocator>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a >= 0 && index_a < size(object));
	GAFF_ASSERT(index_b >= 0 && index_b < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	eastl::swap((obj->*_ptr)[index_a], (obj->*_ptr)[index_b]);
}

template <class T, class VarType, class Vec_Allocator>
void VectorVar<T, VarType, Vec_Allocator>::resize(void* object, size_t new_size)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr).resize(new_size);
}

template <class T, class VarType, class Vec_Allocator>
bool VectorVar<T, VarType, Vec_Allocator>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	const int32_t size = reader.size();
	(object.*_ptr).resize(static_cast<size_t>(size));

	bool success = true;

	for (int32_t i = 0; i < size; ++i) {
		Shibboleth::ScopeGuard scope = reader.enterElementGuard(i);

		if (!Reflection<VarType>::GetInstance().load(reader, (object.*_ptr)[i])) {
			// $TODO: Log error.
			success = false;
		}
	}

	return success;
}

template <class T, class VarType, class Vec_Allocator>
void VectorVar<T, VarType, Vec_Allocator>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const int32_t size = static_cast<int32_t>((object.*_ptr).size());
	writer.startArray(static_cast<uint32_t>(size));

	for (int32_t i = 0; i < size; ++i) {
		Reflection<VarType>::GetInstance().save(writer, (object.*_ptr)[i]);
	}

	writer.endArray();
}



// ArrayVar
template <class T, class VarType, size_t array_size>
ArrayVar<T, VarType, array_size>::ArrayVar(VarType (T::*ptr)[array_size]):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class VarType, size_t array_size>
const IReflection& ArrayVar<T, VarType, array_size>::getReflection(void) const
{
	return Reflection<VarType>::GetInstance();
}

template <class T, class VarType, size_t array_size>
const void* ArrayVar<T, VarType, array_size>::getData(const void* object) const
{
	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType, size_t array_size>
void* ArrayVar<T, VarType, array_size>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType, size_t array_size>
void ArrayVar<T, VarType, array_size>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	const VarType* const vars = reinterpret_cast<const VarType*>(data);
	T* const obj = reinterpret_cast<T*>(object);

	for (int32_t i = 0; i < static_cast<int32_t>(array_size); ++i) {
		(obj->*_ptr)[i] = vars[i];
	}}

template <class T, class VarType, size_t array_size>
void ArrayVar<T, VarType, array_size>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	const VarType* const vars = reinterpret_cast<const VarType*>(data);
	T* const obj = reinterpret_cast<T*>(object);

	for (int32_t i = 0; i < static_cast<int32_t>(array_size); ++i) {
		(obj->*_ptr)[i] = std::move(vars[i]);
	}
}

template <class T, class VarType, size_t array_size>
const void* ArrayVar<T, VarType, array_size>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class VarType, size_t array_size>
void* ArrayVar<T, VarType, array_size>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class VarType, size_t array_size>
void ArrayVar<T, VarType, array_size>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = *reinterpret_cast<const VarType*>(data);
}

template <class T, class VarType, size_t array_size>
void ArrayVar<T, VarType, array_size>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = std::move(*reinterpret_cast<const VarType*>(data));
}

template <class T, class VarType, size_t array_size>
void ArrayVar<T, VarType, array_size>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a >= 0 && index_a < size(object));
	GAFF_ASSERT(index_b >= 0 && index_b < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	eastl::swap((obj->*_ptr)[index_a], (obj->*_ptr)[index_b]);
}

template <class T, class VarType, size_t array_size>
bool ArrayVar<T, VarType, array_size>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	const int32_t size = reader.size();
	(object.*_ptr).resize(static_cast<size_t>(size));

	bool success = true;

	for (int32_t i = 0; i < size; ++i) {
		Shibboleth::ScopeGuard scope = reader.enterElementGuard(i);

		if (!Reflection<VarType>::GetInstance().load(reader, (object.*_ptr)[i])) {
			// $TODO: Log error.
			success = false;
		}
	}

	return success;
}

template <class T, class VarType, size_t array_size>
void ArrayVar<T, VarType, array_size>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const int32_t size = static_cast<int32_t>((object.*_ptr).size());
	writer.startArray(static_cast<uint32_t>(size));

	for (int32_t i = 0; i < size; ++i) {
		Reflection<VarType>::GetInstance().save(writer, (object.*_ptr)[i]);
	}

	writer.endArray();
}



template <class T, class VarType>
IVar<T>* VarPtrTypeHelper<T, VarType>::Create(
	eastl::u8string_view name,
	VarType T::* ptr,
	Shibboleth::ProxyAllocator& allocator,
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

template <class T, class VarType, class Vec_Allocator>
IVar<T>* VarPtrTypeHelper< T, Gaff::Vector<VarType, Vec_Allocator> >::Create(
	eastl::u8string_view name,
	Gaff::Vector<VarType, Vec_Allocator> T::* ptr,
	Shibboleth::ProxyAllocator& allocator,
	Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >&)
{
	static_assert(!std::is_reference<VarType>::value, "Cannot reflect references.");
	static_assert(!std::is_pointer<VarType>::value, "Cannot reflect pointers.");
	static_assert(!std::is_const<VarType>::value, "Cannot reflect const values.");
	static_assert(Reflection<VarType>::HasReflection, "Var type is not reflected!");

	GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string.");

	IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(VectorVar<T, VarType, Vec_Allocator>), allocator, ptr);

	return var_ptr;
}

template <class T, class VarType, size_t array_size>
IVar<T>* VarPtrTypeHelper< T, VarType (T::*)[array_size] >::Create(
	eastl::u8string_view name,
	VarType (T::*arr)[array_size],
	Shibboleth::ProxyAllocator& allocator,
	Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >&)
{
	static_assert(!std::is_reference<VarType>::value, "Cannot reflect references.");
	static_assert(!std::is_pointer<VarType>::value, "Cannot reflect pointers.");
	static_assert(!std::is_const<VarType>::value, "Cannot reflect const values.");
	static_assert(Reflection<VarType>::HasReflection, "Var type is not reflected!");
	static_assert(array_size > 0, "Cannot reflect a zero size array.");

	GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string.");

	IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(ArrayVar<T, VarType, array_size>), allocator, arr);

	return var_ptr;
}

NS_END
