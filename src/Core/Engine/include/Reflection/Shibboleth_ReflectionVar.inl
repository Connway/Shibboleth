/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
	// Pointers are always read only variables.
	if constexpr (std::is_pointer_v<VarType>) {
		setReadOnly(true);
	}

	GAFF_ASSERT(ptr);
}

template <class T>
IVar<T>::IVar(ptrdiff_t offset):
	_offset(offset)
{
	GAFF_ASSERT(offset >= 0);
}

template <class T>
const IReflection& IVar<T>::getOwnerReflection(void) const
{
	return Reflection<T>::GetInstance();
}

template <class T>
template <class VarType>
const VarType& IVar<T>::IVar::getDataT(const T& object) const
{
	const auto& other_refl = Reflection< std::remove_pointer_t< std::decay_t<VarType> > >::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<VarType>::value) {
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

	const auto& other_refl = Reflection< std::remove_pointer_t< std::decay_t<VarType> > >::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<VarType>::value) {
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

	const auto& other_refl = Reflection< std::remove_pointer_t< std::decay_t<VarType> > >::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<VarType>::value) {
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
	const auto& other_refl = Reflection< std::remove_pointer_t< std::decay_t<VarType> > >::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<VarType>::value) {
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

	const auto& other_refl = Reflection< std::remove_pointer_t< std::decay_t<VarType> > >::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<VarType>::value) {
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

	const auto& other_refl = Reflection< std::remove_pointer_t< std::decay_t<VarType> > >::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<VarType>::value) {
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
const IVar<T>* IVar<T>::getParent(void) const
{
	return _parent;
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
		} else if (_parent->isMap()) {
			return _parent->getElement(object, static_cast<int32_t>(_offset));
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

		VarType* const var = IVar<T>::template get<VarType>(object);
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
	if constexpr (VarTypeHelper<T, VarType>::k_can_move) {
		if (IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		VarType* const var = IVar<T>::template get<VarType>(object);
		*var = std::move(*reinterpret_cast<VarType*>(data));

	} else {
		GAFF_REF(object, data);

		GAFF_ASSERT_MSG(
			false,
			"Var<T, VarType>::setDataMove() was called with ReflectionType of '%s'.",
			reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName())
		);

	}
}

template <class T, class VarType>
bool Var<T, VarType>::load(const Shibboleth::ISerializeReader& reader, void* object)
{
	if constexpr (std::is_pointer_v<VarType>) {
		GAFF_REF(reader, object);
		GAFF_ASSERT_MSG(
			false,
			"Var<T, VarType>::load: Called on a pointer of type '%s'.",
			reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName())
		);

	} else {
		VarType* const var = reinterpret_cast<VarType*>(object);
		return Reflection<ReflectionType>::GetInstance().load(reader, *var);
	}
}

template <class T, class VarType>
void Var<T, VarType>::save(Shibboleth::ISerializeWriter& writer, const void* object)
{
	if constexpr (std::is_pointer_v<VarType>) {
		GAFF_REF(writer, object);
		GAFF_ASSERT_MSG(
			false,
			"Var<T, VarType>::save: Called on a pointer of type '%s'.",
			reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName())
		);

	} else {
		const VarType* const var = reinterpret_cast<const VarType*>(object);
		Reflection<ReflectionType>::GetInstance().save(writer, *var);
	}
}

template <class T, class VarType>
bool Var<T, VarType>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	VarType* const var = IVar<T>::template get<VarType>(&object);
	return load(reader, var);
}

template <class T, class VarType>
void Var<T, VarType>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const VarType* const var = IVar<T>::template get<VarType>(&object);
	save(writer, var);
}

NS_END
