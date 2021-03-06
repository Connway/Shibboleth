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

NS_GAFF

template <class T, class... Args>
void ConstructFunc(T* obj, Args&&... args)
{
	Construct(obj, std::forward<Args>(args)...);
}

template <class T, class... Args>
T* FactoryFunc(IAllocator& allocator, Args&&... args)
{
	return GAFF_ALLOCT(T, allocator, std::forward<Args>(args)...);
}

// IVar
template <class T, class Allocator>
template <class DataType>
const DataType& ReflectionDefinition<T, Allocator>::IVar::getDataT(const T& object) const
{
	using Type = std::remove_reference<DataType>::type;

	const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());
	GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());

	return *reinterpret_cast<const DataType*>(getData(&object));
}

template <class T, class Allocator>
template <class DataType>
void ReflectionDefinition<T, Allocator>::IVar::setDataT(T& object, const DataType& data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	using Type = std::remove_reference<DataType>::type;

	const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());
	GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());

	setData(&object, &data);
}

template <class T, class Allocator>
template <class DataType>
void ReflectionDefinition<T, Allocator>::IVar::setDataMoveT(T& object, DataType&& data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	using Type = std::remove_reference<DataType>::type;

	const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());
	GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());

	setDataMove(&object, &data);
}

template <class T, class Allocator>
template <class DataType>
const DataType& ReflectionDefinition<T, Allocator>::IVar::getElementT(const T& object, int32_t index) const
{
	using Type = std::remove_reference<DataType>::type;

	const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
	const auto& refl = getReflection();

	GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

	if constexpr (std::is_enum<Type>::value) {
		GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
	} else {
		GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
	}

	GAFF_ASSERT((isFixedArray() || isVector()) && size(&object) > index);
	return *reinterpret_cast<const DataType*>(getElement(&object, index));
}

template <class T, class Allocator>
template <class DataType>
void ReflectionDefinition<T, Allocator>::IVar::setElementT(T& object, int32_t index, const DataType& data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	using Type = std::remove_reference<DataType>::type;

	const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
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

template <class T, class Allocator>
template <class DataType>
void ReflectionDefinition<T, Allocator>::IVar::setElementMoveT(T& object, int32_t index, DataType&& data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	using Type = std::remove_reference<DataType>::type;

	const auto& other_refl = GAFF_REFLECTION_NAMESPACE::Reflection<Type>::GetInstance();
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



// VarPtr
template <class T, class Allocator>
template <class Var>
ReflectionDefinition<T, Allocator>::VarPtr<Var>::VarPtr(Var T::*ptr):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class Allocator>
template <class Var>
const IReflection& ReflectionDefinition<T, Allocator>::VarPtr<Var>::getReflection(void) const
{
	if constexpr (IsFlags<Var>()) {
		return GAFF_REFLECTION_NAMESPACE::Reflection<typename GetFlagsEnum<Var>::Enum>::GetInstance();
	} else {
		return GAFF_REFLECTION_NAMESPACE::Reflection<Var>::GetInstance();
	}
}

template <class T, class Allocator>
template <class Var>
const void* ReflectionDefinition<T, Allocator>::VarPtr<Var>::getData(const void* object) const
{
	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class Allocator>
template <class Var>
void* ReflectionDefinition<T, Allocator>::VarPtr<Var>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr);
}

template <class T, class Allocator>
template <class Var>
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::setData(void* object, const void* data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = *reinterpret_cast<const Var*>(data);
}

template <class T, class Allocator>
template <class Var>
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::setDataMove(void* object, void* data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = std::move(*reinterpret_cast<Var*>(data));
}

template <class T, class Allocator>
template <class Var>
bool ReflectionDefinition<T, Allocator>::VarPtr<Var>::load(const ISerializeReader& reader, T& object)
{
	Var* const var = &(object.*_ptr);

	if constexpr (IsFlags<Var>()) {
		using Enum = typename GetFlagsEnum<Var>::Enum;

		// Iterate over all the flags and read values.
		const IEnumReflectionDefinition& ref_def = getReflection().getEnumReflectionDefinition();
		const int32_t num_entries = ref_def.getNumEntries();
		bool success = true;

		for (int32_t i = 0; i < num_entries; ++i) {
			const HashStringView32<> flag_name = ref_def.getEntryNameFromIndex(i);
			const int32_t flag_index = ref_def.getEntryValue(i);

			const auto guard = reader.enterElementGuard(flag_name.getBuffer());

			if (!reader.isBool() && !reader.isNull()) {
				success = false;
				continue;
			}

			const bool value = reader.readBool(false);
			var->set(value, static_cast<Enum>(flag_index));
		}

		return success;

	} else {
		return GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Load(reader, *var);
	}
}

template <class T, class Allocator>
template <class Var>
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::save(ISerializeWriter& writer, const T& object)
{
	const Var* const var = &(object.*_ptr);

	if constexpr (IsFlags<Var>()) {
		using Enum = typename GetFlagsEnum<Var>::Enum;

		// Iterate over all the flags and write values.
		const IEnumReflectionDefinition& ref_def = getReflection().getEnumReflectionDefinition();
		const int32_t num_entries = ref_def.getNumEntries();

		for (int32_t i = 0; i < num_entries; ++i) {
			const HashStringView32<> flag_name = ref_def.getEntryNameFromIndex(i);
			const int32_t flag_index = ref_def.getEntryValue(i);
			const bool value = var->testAll(static_cast<Enum>(flag_index));

			writer.writeBool(flag_name.getBuffer(), value);
		}

	} else {
		GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Save(writer, *var);
	}
}

template <class T, class Allocator>
template <class Var>
bool ReflectionDefinition<T, Allocator>::VarPtr<Var>::isFlags(void) const
{
	return IsFlags<Var>();
}

template <class T, class Allocator>
template <class Var>
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::setFlagValue(void* object, int32_t flag_index, bool value)
{
	if constexpr (IsFlags<Var>()) {
		using Enum = typename GetFlagsEnum<Var>::Enum;
		(reinterpret_cast<T*>(object)->*_ptr).set(value, static_cast<Enum>(flag_index));

	} else {
		GAFF_ASSERT_MSG(false, "Reflection variable is not flags!");
		GAFF_REF(object, flag_index, value);
	}
}

template <class T, class Allocator>
template <class Var>
bool ReflectionDefinition<T, Allocator>::VarPtr<Var>::getFlagValue(void* object, int32_t flag_index) const
{
	if constexpr (IsFlags<Var>()) {
		using Enum = typename GetFlagsEnum<Var>::Enum;
		return (reinterpret_cast<T*>(object)->*_ptr).testAll(static_cast<Enum>(flag_index));

	} else {
		GAFF_ASSERT_MSG(false, "Reflection variable is not flags!");
		GAFF_REF(object, flag_index);
		return false;
	}
}



// VarFlagPtr
template <class T, class Allocator>
template <class Enum>
ReflectionDefinition<T, Allocator>::VarFlagPtr<Enum>::VarFlagPtr(Flags<Enum> T::*ptr, uint8_t flag_index):
	_ptr(ptr), _flag_index(flag_index), _cache(false)
{
	GAFF_ASSERT(ptr);
}

template <class T, class Allocator>
template <class Enum>
const IReflection& ReflectionDefinition<T, Allocator>::VarFlagPtr<Enum>::getReflection(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<bool>::GetInstance();
}

template <class T, class Allocator>
template <class Enum>
const void* ReflectionDefinition<T, Allocator>::VarFlagPtr<Enum>::getData(const void* object) const
{
	return const_cast<VarFlagPtr<Enum>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class Allocator>
template <class Enum>
void* ReflectionDefinition<T, Allocator>::VarFlagPtr<Enum>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);
	_cache = (obj->*_ptr).testAll(static_cast<Enum>(_flag_index));
	return &_cache;
}

template <class T, class Allocator>
template <class Enum>
void ReflectionDefinition<T, Allocator>::VarFlagPtr<Enum>::setData(void* object, const void* data)
{
	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr).set(*reinterpret_cast<const bool*>(data), static_cast<Enum>(_flag_index));
}

template <class T, class Allocator>
template <class Enum>
void ReflectionDefinition<T, Allocator>::VarFlagPtr<Enum>::setDataMove(void* object, void* data)
{
	setData(object, data);
}

template <class T, class Allocator>
template <class Enum>
bool ReflectionDefinition<T, Allocator>::VarFlagPtr<Enum>::load(const ISerializeReader& /*reader*/, T& /*object*/)
{
	GAFF_ASSERT_MSG(false, "VarFlagPtr::load() should never be called.");
	return false;
}

template <class T, class Allocator>
template <class Enum>
void ReflectionDefinition<T, Allocator>::VarFlagPtr<Enum>::save(ISerializeWriter& /*writer*/, const T& /*object*/)
{
	GAFF_ASSERT_MSG(false, "VarFlagPtr::save() should never be called.");
}



// VarFuncPtrWithCache
template <class T, class Allocator>
template <class Ret, class Var>
ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::VarFuncPtrWithCache(GetterMemberFunc getter, SetterMemberFunc setter):
	_getter_member(getter), _setter_member(setter), _member_func(true)
{
	GAFF_ASSERT(getter);
}

template <class T, class Allocator>
template <class Ret, class Var>
ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::VarFuncPtrWithCache(GetterFunc getter, SetterFunc setter):
	_getter(getter), _setter(setter), _member_func(false)
{
	GAFF_ASSERT(getter);
}

template <class T, class Allocator>
template <class Ret, class Var>
const IReflection& ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::getReflection(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::GetInstance();
}

template <class T, class Allocator>
template <class Ret, class Var>
const void* ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::getData(const void* object) const
{
	return const_cast<ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class Allocator>
template <class Ret, class Var>
void* ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::getData(void* object)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_getter);

	const T* const obj = reinterpret_cast<const T*>(object);
	_cache = callGetter(*obj);

	return &_cache;
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	callSetter(*obj, *reinterpret_cast<const VarType*>(data));
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	callSetter(*obj, std::move(*reinterpret_cast<VarType*>(data)));
}

template <class T, class Allocator>
template <class Ret, class Var>
bool ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::load(const ISerializeReader& reader, T& object)
{
	GAFF_ASSERT(_getter);
	GAFF_ASSERT(_setter);

	VarType var;
		
	if (!GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::Load(reader, var)) {
		return false;
	}

	callSetter(object, var);
	return true;
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::save(ISerializeWriter& writer, const T& object)
{
	GAFF_ASSERT(_getter);
	GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::Save(writer, callGetter(object));
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::callSetter(T& object, Var value) const
{
	GAFF_ASSERT(_setter);
	return (_member_func) ? (object.*_setter_member)(value) : _setter(object, value);
}

template <class T, class Allocator>
template <class Ret, class Var>
Ret ReflectionDefinition<T, Allocator>::VarFuncPtrWithCache<Ret, Var>::callGetter(const T& object) const
{
	GAFF_ASSERT(_getter);
	return (_member_func) ? (object.*_getter_member)() : _getter(object);
}



// VarFuncPtr
template <class T, class Allocator>
template <class Ret, class Var>
ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::VarFuncPtr(GetterMemberFunc getter, SetterMemberFunc setter):
	_getter_member(getter),
	_setter_member(setter),
	_member_func(true)
{
	GAFF_ASSERT(getter);
}

template <class T, class Allocator>
template <class Ret, class Var>
ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::VarFuncPtr(GetterFunc getter, SetterFunc setter) :
	_getter(getter),
	_setter(setter),
	_member_func(false)
{
	GAFF_ASSERT(getter);
}

template <class T, class Allocator>
template <class Ret, class Var>
const IReflection& ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::getReflection(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::GetInstance();
}

template <class T, class Allocator>
template <class Ret, class Var>
const void* ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::getData(const void* object) const
{
	return const_cast<ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class Allocator>
template <class Ret, class Var>
void* ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::getData(void* object)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_getter);

	T* const obj = reinterpret_cast<T*>(object);

	if constexpr (std::is_reference<Ret>::value) {
		const Ret& val = callGetter(*obj);
		RetType* const ptr = const_cast<RetType*>(&val);
		return ptr;

	} else if (std::is_pointer<Ret>::value) {
		return const_cast<RetType*>(callGetter(*obj));
	}
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	callSetter(*obj, *reinterpret_cast<const RetType*>(data));
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	callSetter(*obj, std::move(*reinterpret_cast<RetType*>(data)));
}

template <class T, class Allocator>
template <class Ret, class Var>
bool ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::load(const ISerializeReader& reader, T& object)
{
	GAFF_ASSERT(_getter);

	if constexpr (std::is_reference<Ret>::value) {
		RetType& val = const_cast<RetType&>(callGetter(object));
		return GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::Load(reader, val);

	} else {
		RetType* const val = const_cast<RetType*>(callGetter(object));
		return GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::Load(reader, *val);
	}
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::save(ISerializeWriter& writer, const T& object)
{
	GAFF_ASSERT(_getter);

	if constexpr (std::is_reference<Ret>::value) {
		const RetType& val = callGetter(object);
		GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::Save(writer, val);

	} else {
		const RetType* const val = callGetter(object);
		GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::Save(writer, *val);
	}
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::callSetter(T& object, Var value) const
{
	GAFF_ASSERT(_setter);
	return (_member_func) ? (object.*_setter_member)(value) : _setter(object, value);
}

template <class T, class Allocator>
template <class Ret, class Var>
Ret ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::callGetter(const T& object) const
{
	GAFF_ASSERT(_getter);
	return (_member_func) ? (object.*_getter_member)() : _getter(object);
}



// BaseVarPtr
template <class T, class Allocator>
template <class Base>
ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::BaseVarPtr(typename ReflectionDefinition<Base, Allocator>::IVar* base_var):
	_base_var(base_var)
{
}

template <class T, class Allocator>
template <class Base>
const IReflection& ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::getReflection(void) const
{
	return _base_var->getReflection();
}

template <class T, class Allocator>
template <class Base>
const void* ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::getData(const void* object) const
{
	GAFF_ASSERT(object);
	const Base* const obj = reinterpret_cast<const T*>(object);
	return _base_var->getData(obj);
}

template <class T, class Allocator>
template <class Base>
void* ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::getData(void* object)
{
	GAFF_ASSERT(object);
	Base* const obj = reinterpret_cast<T*>(object);
	return _base_var->getData(obj);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->setData(obj, data);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->setData(obj, data);
}

template <class T, class Allocator>
template <class Base>
bool ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::isFixedArray(void) const
{
	return _base_var->isFixedArray();
}

template <class T, class Allocator>
template <class Base>
bool ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::isVector(void) const
{
	return _base_var->isVector();
}

template <class T, class Allocator>
template <class Base>
bool ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::isFlags(void) const
{
	return _base_var->isFlags();
}

template <class T, class Allocator>
template <class Base>
int32_t ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::size(const void* object) const
{
	GAFF_ASSERT(object);
	const Base* const obj = reinterpret_cast<const T*>(object);
	return _base_var->size(obj);
}

template <class T, class Allocator>
template <class Base>
const void* ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	const Base* const obj = reinterpret_cast<const T*>(object);
	return _base_var->getElement(obj, index);
}

template <class T, class Allocator>
template <class Base>
void* ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	Base* const obj = reinterpret_cast<T*>(object);
	return _base_var->getElement(obj, index);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->setElement(obj, index, data);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->setElementMove(obj, index, data);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a < size(object));
	GAFF_ASSERT(index_b < size(object));
	GAFF_ASSERT(object);

	if (isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->swap(obj, index_a, index_b);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::resize(void* object, size_t new_size)
{
	GAFF_ASSERT(object);

	if (isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->resize(obj, new_size);
}

template <class T, class Allocator>
template <class Base>
bool ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::load(const ISerializeReader& reader, T& object)
{
	return _base_var->load(reader, object);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::save(ISerializeWriter& writer, const T& object)
{
	_base_var->save(writer, object);
}



// ArrayPtr
template <class T, class Allocator>
template <class Var, size_t array_size>
ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::ArrayPtr(Var (T::*ptr)[array_size]):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class Allocator>
template <class Var, size_t array_size>
const IReflection& ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::getReflection(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<Var>::GetInstance();
}

template <class T, class Allocator>
template <class Var, size_t array_size>
const void* ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::getData(const void* object) const
{
	GAFF_ASSERT(object);

	const T* const obj = reinterpret_cast<const T*>(object);
	return (obj->*_ptr);
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void* ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::getData(void* object)
{
	GAFF_ASSERT(object);

	T* const obj = reinterpret_cast<T*>(object);
	return (obj->*_ptr);
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	const Var* const vars = reinterpret_cast<const Var*>(data);
	T* const obj = reinterpret_cast<T*>(object);

	for (int32_t i = 0; i < static_cast<int32_t>(array_size); ++i) {
		(obj->*_ptr)[i] = vars[i];
	}
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	const Var* const vars = reinterpret_cast<Var*>(data);
	T* const obj = reinterpret_cast<T*>(object);

	for (int32_t i = 0; i < static_cast<int32_t>(array_size); ++i) {
		(obj->*_ptr)[i] = std::move(vars[i]);
	}
}

template <class T, class Allocator>
template <class Var, size_t array_size>
const void* ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index < array_size);
	GAFF_ASSERT(object);
	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void* ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(index < array_size);
	GAFF_ASSERT(object);
	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT(index < array_size);
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = *reinterpret_cast<const Var*>(data);
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index < array_size);
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = std::move(*reinterpret_cast<Var*>(data));
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a < array_size);
	GAFF_ASSERT(index_b < array_size);
	GAFF_ASSERT(object);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	eastl::swap((obj->*_ptr)[index_a], (obj->*_ptr)[index_b]);
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::resize(void*, size_t)
{
	GAFF_ASSERT_MSG(false, "Reflection variable is a fixed size array!");
}

template <class T, class Allocator>
template <class Var, size_t array_size>
bool ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::load(const ISerializeReader& reader, T& object)
{
	constexpr int32_t size = static_cast<int32_t>(array_size);
	GAFF_ASSERT(reader.size() == size);

	bool success = true;

	for (int32_t i = 0; i < size; ++i) {
		ScopeGuard scope = reader.enterElementGuard(i);
		success = success && GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Load(reader, (object.*_ptr)[i]);
	}

	return success;
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::save(ISerializeWriter& writer, const T& object)
{
	constexpr int32_t size = static_cast<int32_t>(array_size);
	writer.startArray(static_cast<uint32_t>(array_size));

	for (int32_t i = 0; i < size; ++i) {
		GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Save(writer, (object.*_ptr)[i]);
	}

	writer.endArray();
}



// VectorPtr
template <class T, class Allocator>
template <class Var, class Vec_Allocator>
ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::VectorPtr(Vector<Var, Vec_Allocator> (T::*ptr)):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
const IReflection& ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::getReflection(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<Var>::GetInstance();
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
const void* ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::getData(const void* object) const
{
	GAFF_ASSERT(object);

	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void* ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::getData(void* object)
{
	GAFF_ASSERT(object);

	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	const Var* const vars = reinterpret_cast<const Var*>(data);
	T* const obj = reinterpret_cast<T*>(object);
	int32_t arr_size = size(object);

	for (int32_t i = 0; i < arr_size; ++i) {
		(obj->*_ptr)[i] = vars[i];
	}
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	const Var* const vars = reinterpret_cast<Var*>(data);
	T* const obj = reinterpret_cast<T*>(object);
	int32_t arr_size = size(object);

	for (int32_t i = 0; i < arr_size; ++i) {
		(obj->*_ptr)[i] = std::move(vars[i]);
	}
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
int32_t ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::size(const void* object) const
{
	GAFF_ASSERT(object);

	const T* const obj = reinterpret_cast<const T*>(object);
	return static_cast<int32_t>((obj->*_ptr).size());
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
const void* ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);

	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void* ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);

	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = *reinterpret_cast<const Var*>(data);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = std::move(*reinterpret_cast<Var*>(data));
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a < size(object));
	GAFF_ASSERT(index_b < size(object));
	GAFF_ASSERT(object);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	eastl::swap((obj->*_ptr)[index_a], (obj->*_ptr)[index_b]);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::resize(void* object, size_t new_size)
{
	GAFF_ASSERT(object);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr).resize(new_size);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
bool ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::load(const ISerializeReader& reader, T& object)
{
	const int32_t size = reader.size();
	(object.*_ptr).resize(static_cast<size_t>(size));

	bool success = true;

	for (int32_t i = 0; i < size; ++i) {
		ScopeGuard scope = reader.enterElementGuard(i);
		success = success && GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Load(reader, (object.*_ptr)[i]);
	}

	return success;
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::save(ISerializeWriter& writer, const T& object)
{
	const int32_t size = static_cast<int32_t>((object.*_ptr).size());
	writer.startArray(static_cast<uint32_t>(size));

	for (int32_t i = 0; i < size; ++i) {
		GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Save(writer, (object.*_ptr)[i]);
	}

	writer.endArray();
}



// VectorMapPtr
template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::VectorMapPtr(VectorMap<Key, Value, VecMap_Allocator> T::* ptr):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
const IReflection& ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::getReflection(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<Value>::GetInstance();
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
const IReflection& ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::getReflectionKey(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<Key>::GetInstance();
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
const void* ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::getData(const void* object) const
{
	GAFF_ASSERT(object);

	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
void* ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::getData(void* object)
{
	GAFF_ASSERT(object);

	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr);
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
void ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	const eastl::pair<Key, Value>* const vars = reinterpret_cast<const eastl::pair<Key, Value>*>(data);
	T* const obj = reinterpret_cast<T*>(object);
	int32_t arr_size = size(object);

	for (int32_t i = 0; i < arr_size; ++i) {
		(obj->*_ptr)[vars[i].first] = vars[i].second;
	}
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
void ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	const eastl::pair<Key, Value>* const vars = reinterpret_cast<const eastl::pair<Key, Value>*>(data);
	T* const obj = reinterpret_cast<T*>(object);
	int32_t arr_size = size(object);

	for (int32_t i = 0; i < arr_size; ++i) {
		(obj->*_ptr)[std::move(vars[i].first)] = std::move(vars[i].second);
	}
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
int32_t ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::size(const void* object) const
{
	GAFF_ASSERT(object);

	const T* const obj = reinterpret_cast<const T*>(object);
	return static_cast<int32_t>((obj->*_ptr).size());
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
const void* ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);

	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr).at(index);
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
void* ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);

	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr).at(index);
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
void ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr).at(index).second = *reinterpret_cast<const Value*>(data);
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
void ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr).at(index).second = std::move(*reinterpret_cast<const Value*>(data));
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
void ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a < size(object));
	GAFF_ASSERT(index_b < size(object));
	GAFF_ASSERT(object);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	eastl::swap((obj->*_ptr).at(index_a).second, (obj->*_ptr).at(index_b).second);
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
void ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::resize(void* object, size_t new_size)
{
	GAFF_ASSERT(object);

	if (isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	return (obj->*_ptr).resize(new_size);
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
bool ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::load(const ISerializeReader& reader, T& object)
{
	const int32_t size = reader.size();
	(object.*_ptr).reserve(static_cast<size_t>(size));

	bool success = true;

	for (int32_t i = 0; i < size; ++i) {
		ScopeGuard scope = reader.enterElementGuard(i);
		bool key_loaded = true;
		Key key;

		{
			ScopeGuard guard_key = reader.enterElementGuard("key");
			key_loaded = GAFF_REFLECTION_NAMESPACE::Reflection<Key>::Load(reader, key);
			success = success && key_loaded;
		}

		if (key_loaded)
		{
			ScopeGuard guard_value = reader.enterElementGuard("value");
			success = success && GAFF_REFLECTION_NAMESPACE::Reflection<Value>::Load(reader, (object.*_ptr)[key]);
		}
	}

	return success;
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
void ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::save(ISerializeWriter& writer, const T& object)
{
	const int32_t size = static_cast<int32_t>((object.*_ptr).size());
	writer.startArray(static_cast<uint32_t>(size));

	for (int32_t i = 0; i < size; ++i) {
		writer.startObject(2);

		writer.writeKey("key");
		GAFF_REFLECTION_NAMESPACE::Reflection<Key>::Save(writer, (object.*_ptr).at(i).first);

		writer.writeKey("value");
		GAFF_REFLECTION_NAMESPACE::Reflection<Value>::Save(writer, (object.*_ptr).at(i).second);

		writer.endObject();
	}

	writer.endArray();
}



// ReflectionDefinition
template <class T, class Allocator>
template <class... Args>
T* ReflectionDefinition<T, Allocator>::create(Args&&... args) const
{
	return createT<T>(_allocator, std::forward<Args>(args)...);
}

template <class T, class Allocator>
const char* ReflectionDefinition<T, Allocator>::getFriendlyName(void) const
{
	return _friendly_name.data();
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::load(const ISerializeReader& reader, void* object, bool refl_load) const
{
	return load(reader, *reinterpret_cast<T*>(object), refl_load);
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::save(ISerializeWriter& writer, const void* object, bool refl_save) const
{
	save(writer, *reinterpret_cast<const T*>(object), refl_save);
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::load(const ISerializeReader& reader, T& object, bool refl_load) const
{
	if (_serialize_load && !refl_load) {
		return _serialize_load(reader, object);

	} else {
		for (auto& entry : _vars) {
			if (entry.second->canSerialize()) {
				const char* const name = entry.first.getBuffer();

				if (!reader.exists(name)) {
					// I don't like this method of determining something as optional.
					const auto* const attr = getVarAttr<IAttribute>(FNV1aHash32String(name), FNV1aHash64Const("OptionalAttribute"));

					if (!attr) {
						// $TODO: Log error.
						return false;
					}

					continue;
				}

				ScopeGuard scope = reader.enterElementGuard(name);
				entry.second->load(reader, object);
			}
		}
	}

	return true;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::save(ISerializeWriter& writer, const T& object, bool refl_save) const
{
	if (_serialize_save && !refl_save) {
		_serialize_save(writer, object);

	} else {
		uint32_t writable_vars = 0;

		// Count how many vars we're actually writing to the object.
		for (auto& entry : _vars) {
			// If not read-only and does not have the NoSerialize attribute.
			if (entry.second->canSerialize()) {
				++writable_vars;
			}
		}

		// Write out the object.
		writer.startObject(writable_vars + 1);

		writer.writeUInt64("version", getReflectionInstance().getVersion().getHash());

		for (auto& entry : _vars) {
			if (entry.second->canSerialize()) {
				writer.writeKey(entry.first.getBuffer());
				entry.second->save(writer, object);
			}
		}

		writer.endObject();
	}
}

template <class T, class Allocator>
Hash64 ReflectionDefinition<T, Allocator>::getInstanceHash(const void* object, Hash64 init) const
{
	return getInstanceHash(*reinterpret_cast<const T*>(object), init);
}

template <class T, class Allocator>
Hash64 ReflectionDefinition<T, Allocator>::getInstanceHash(const T& object, Hash64 init) const
{
	return (_instance_hash) ? _instance_hash(object, init) : FNV1aHash64T(object, init);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::setInstanceHash(InstanceHashFunc hash_func)
{
	_instance_hash = hash_func;
	return *this;
}

template <class T, class Allocator>
const void* ReflectionDefinition<T, Allocator>::getInterface(Hash64 class_hash, const void* object) const
{
	if (class_hash == GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash()) {
		return object;
	}

	auto it = _base_class_offsets.find(class_hash);

	if (it == _base_class_offsets.end()) {
		return nullptr;
	}

	return reinterpret_cast<const int8_t*>(object) + it->second;
}

template <class T, class Allocator>
void* ReflectionDefinition<T, Allocator>::getInterface(Hash64 class_hash, void* object) const
{
	return const_cast<void*>(getInterface(class_hash, const_cast<const void*>(object)));
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::hasInterface(Hash64 class_hash) const
{
	if (class_hash == GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash()) {
		return true;
	}

	auto it = _base_class_offsets.find(class_hash);
	return it != _base_class_offsets.end();
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_base_class_offsets.set_allocator(allocator);
	_factories.set_allocator(allocator);
	_ctors.set_allocator(allocator);
	_vars.set_allocator(allocator);
	_funcs.set_allocator(allocator);
	_static_funcs.set_allocator(allocator);

	_var_attrs.set_allocator(allocator);
	_func_attrs.set_allocator(allocator);
	_class_attrs.set_allocator(allocator);
	_static_func_attrs.set_allocator(allocator);

	_friendly_name.set_allocator(allocator);

	_allocator = allocator;
}

template <class T, class Allocator>
IAllocator& ReflectionDefinition<T, Allocator>::getAllocator(void)
{
	return _allocator;
}

template <class T, class Allocator>
const IReflection& ReflectionDefinition<T, Allocator>::getReflectionInstance(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetInstance();
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::size(void) const
{
	return sizeof(T);
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::isPolymorphic(void) const
{
	return std::is_polymorphic<T>::value;
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::isBuiltIn(void) const
{
	return false;
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumVars(void) const
{
	return static_cast<int32_t>(_vars.size());
}

template <class T, class Allocator>
HashStringView32<> ReflectionDefinition<T, Allocator>::getVarName(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	return HashStringView32<>((_vars.begin() + index)->first);
}

template <class T, class Allocator>
IReflectionVar* ReflectionDefinition<T, Allocator>::getVar(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	return getVarT(index);
}

template <class T, class Allocator>
IReflectionVar* ReflectionDefinition<T, Allocator>::getVar(Hash32 name) const
{
	return getVarT(name);
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumFuncs(void) const
{
	return static_cast<int32_t>(_funcs.size());
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumFuncOverrides(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_funcs.size()));

	int32_t count = 0;

	for (const IRefFuncPtr& func : (_funcs.begin() + index)->second.func) {
		if (!func) {
			break;
		}

		++count;
	}

	return count;
}

template <class T, class Allocator>
HashStringView32<> ReflectionDefinition<T, Allocator>::getFuncName(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_funcs.size()));
	return HashStringView32<>((_funcs.begin() + index)->first);
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getFuncIndex(Hash32 name) const
{
	const auto it = _funcs.find(name);
	return (it == _funcs.end()) ? -1 : static_cast<int32_t>(eastl::distance(_funcs.begin(), it));
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumStaticFuncs(void) const
{
	return static_cast<int32_t>(_static_funcs.size());
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumStaticFuncOverrides(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_static_funcs.size()));

	int32_t count = 0;

	for (const IRefStaticFuncPtr& func : (_static_funcs.begin() + index)->second.func) {
		if (!func) {
			break;
		}

		++count;
	}

	return count;
}

template <class T, class Allocator>
HashStringView32<> ReflectionDefinition<T, Allocator>::getStaticFuncName(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_static_funcs.size()));
	return HashStringView32<>((_static_funcs.begin() + index)->first);
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getStaticFuncIndex(Hash32 name) const
{
	const auto it = _static_funcs.find(name);
	return (it == _static_funcs.end()) ? -1 : static_cast<int32_t>(eastl::distance(_static_funcs.begin(), it));
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumClassAttrs(void) const
{
	return static_cast<int32_t>(_class_attrs.size());
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getClassAttr(Hash64 attr_name) const
{
	return getAttribute(_class_attrs, attr_name);
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getClassAttr(int32_t index) const
{
	GAFF_ASSERT(index < getNumClassAttrs());
	return _class_attrs[index].get();
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::hasClassAttr(Hash64 attr_name) const
{
	return getClassAttr(attr_name) != nullptr;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::addClassAttr(IAttribute& attribute)
{
	_class_attrs.emplace_back(IAttributePtr(attribute.clone()));
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumVarAttrs(Hash32 name) const
{
	const auto it = _var_attrs.find(name);
	return (it != _var_attrs.end()) ? static_cast<int32_t>(it->second.size()) : 0;
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getVarAttr(Hash32 name, Hash64 attr_name) const
{
	const auto it = _var_attrs.find(name);
	GAFF_ASSERT(it != _var_attrs.end());

	return getAttribute(it->second, attr_name);
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getVarAttr(Hash32 name, int32_t index) const
{
	const auto it = _var_attrs.find(name);
	GAFF_ASSERT(it != _var_attrs.end());
	GAFF_ASSERT(index < static_cast<int32_t>(it->second.size()));
	return it->second[index].get();
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::hasVarAttr(Hash64 attr_name) const
{
	for (const auto& attrs : _var_attrs) {
		if (getAttribute(attrs.second, attr_name) != nullptr) {
			return true;
		}
	}

	return false;
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumFuncAttrs(Hash64 name_arg_hash) const
{
	const auto it = _func_attrs.find(name_arg_hash);

	if (it == _func_attrs.end()) {
		for (auto it_base = _base_classes.begin(); it_base != _base_classes.end(); ++it_base) {
			const int32_t num = it_base->second->getNumFuncAttrs(name_arg_hash);

			if (num > 0) {
				return num;
			}
		}

	} else {
		return static_cast<int32_t>(it->second.size());
	}

	return 0;
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getFuncAttr(Hash64 name_arg_hash, Hash64 attr_name) const
{
	const auto it = _func_attrs.find(name_arg_hash);
	GAFF_ASSERT(it != _func_attrs.end());

	return getAttribute(it->second, attr_name);
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getFuncAttr(Hash64 name_arg_hash, int32_t index) const
{
	const auto it = _func_attrs.find(name_arg_hash);

	if (it == _func_attrs.end()) {
		for (auto it_base = _base_classes.begin(); it_base != _base_classes.end(); ++it_base) {
			const int32_t num = it_base->second->getNumFuncAttrs(name_arg_hash);

			if (num > 0) {
				GAFF_ASSERT(index < num);
				return it_base->second->getFuncAttr(name_arg_hash, index);
			}
		}

	} else {
		GAFF_ASSERT(index < static_cast<int32_t>(it->second.size()));
		return it->second[index].get();
	}

	return nullptr;
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::hasFuncAttr(Hash64 attr_name) const
{
	for (const auto& attrs : _func_attrs) {
		if (getFuncAttr(attrs.first, attr_name) != nullptr) {
			return true;
		}
	}

	return false;
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumStaticFuncAttrs(Hash64 name_arg_hash) const
{
	const auto it = _static_func_attrs.find(name_arg_hash);
	return (it != _static_func_attrs.end()) ? static_cast<int32_t>(it->second.size()) : 0;
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getStaticFuncAttr(Hash64 name_arg_hash, Hash64 attr_name) const
{
	const auto it = _static_func_attrs.find(name_arg_hash);
	GAFF_ASSERT(it != _static_func_attrs.end());

	return getAttribute(it->second, attr_name);
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getStaticFuncAttr(Hash64 name_arg_hash, int32_t index) const
{
	const auto it = _static_func_attrs.find(name_arg_hash);
	GAFF_ASSERT(it != _static_func_attrs.end());
	GAFF_ASSERT(index < static_cast<int32_t>(it->second.size()));
	return it->second[index].get();
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::hasStaticFuncAttr(Hash64 attr_name) const
{
	for (const auto& attrs : _static_func_attrs) {
		if (getStaticFuncAttr(attrs.first, attr_name) != nullptr) {
			return true;
		}
	}

	return false;
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumConstructors(void) const
{
	return static_cast<int32_t>(_ctors.size());
}

template <class T, class Allocator>
IReflectionStaticFunctionBase* ReflectionDefinition<T, Allocator>::getConstructor(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_ctors.size()));
	return ((_ctors.begin()) + index)->second.get();
}

template <class T, class Allocator>
IReflectionDefinition::VoidFunc ReflectionDefinition<T, Allocator>::getConstructor(Hash64 ctor_hash) const
{
	const auto it = _ctors.find(ctor_hash);
	return it == _ctors.end() ? nullptr : it->second->getFunc();
}

template <class T, class Allocator>
IReflectionDefinition::VoidFunc ReflectionDefinition<T, Allocator>::getFactory(Hash64 ctor_hash) const
{
	const auto it = _factories.find(ctor_hash);
	return it == _factories.end() ? nullptr : it->second;
}

template <class T, class Allocator>
IReflectionStaticFunctionBase* ReflectionDefinition<T, Allocator>::getStaticFunc(int32_t name_index, int32_t override_index) const
{
	GAFF_ASSERT(name_index < static_cast<int32_t>(_static_funcs.size()));
	GAFF_ASSERT(override_index < StaticFuncData::k_num_overloads);

	return (_static_funcs.begin() + name_index)->second.func[override_index].get();
}

template <class T, class Allocator>
IReflectionStaticFunctionBase* ReflectionDefinition<T, Allocator>::getStaticFunc(Hash32 name, Hash64 args) const
{
	const auto it = _static_funcs.find(name);

	if (it != _static_funcs.end()) {
		for (int32_t i = 0; i < StaticFuncData::k_num_overloads; ++i) {
			if (it->second.hash[i] == args) {
				return it->second.func[i].get();
			}
		}
	}

	return nullptr;
}

template <class T, class Allocator>
IReflectionFunctionBase* ReflectionDefinition<T, Allocator>::getFunc(int32_t name_index, int32_t override_index) const
{
	GAFF_ASSERT(name_index < static_cast<int32_t>(_funcs.size()));
	GAFF_ASSERT(override_index < FuncData::k_num_overloads);

	return (_funcs.begin() + name_index)->second.func[override_index].get();
}

template <class T, class Allocator>
IReflectionFunctionBase* ReflectionDefinition<T, Allocator>::getFunc(Hash32 name, Hash64 args) const
{
	const auto it = _funcs.find(name);

	if (it != _funcs.end()) {
		for (int32_t i = 0; i < FuncData::k_num_overloads; ++i) {
			if (it->second.hash[i] == args) {
				return it->second.func[i].get();
			}
		}
	}

	return nullptr;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::destroyInstance(void* data) const
{
	T* const instance = reinterpret_cast<T*>(data);
	Deconstruct(instance);
}

template <class T, class Allocator>
typename ReflectionDefinition<T, Allocator>::IVar* ReflectionDefinition<T, Allocator>::getVarT(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	return (_vars.begin() + index)->second.get();
}

template <class T, class Allocator>
typename ReflectionDefinition<T, Allocator>::IVar* ReflectionDefinition<T, Allocator>::getVarT(Hash32 name) const
{
	const auto it = _vars.find(name);
	return (it == _vars.end()) ? nullptr : it->second.get();
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::friendlyName(const char* name)
{
	_friendly_name = name;
	return *this;
}

template <class T, class Allocator>
template <class Base>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::base(const char* name)
{
	static_assert(std::is_base_of<Base, T>::value, "Class is not a base class of T.");

	const ptrdiff_t offset = OffsetOfClass<T, Base>();
	auto pair = std::move(
		eastl::make_pair(
			HashString64<Allocator>(name, _allocator),
			offset
		)
	);

	GAFF_ASSERT(_base_class_offsets.find(pair.first) == _base_class_offsets.end());
	_base_class_offsets.insert(std::move(pair));

	return *this;
}

template <class T, class Allocator>
template <class Base>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::base(void)
{
	static_assert(std::is_base_of<Base, T>::value, "Class is not a base class of T.");

	// So that hasInterface() calls will properly report inheritance if the base class hasn't been defined yet.
	if (_base_class_offsets.find(GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetHash()) == _base_class_offsets.end()) {
		base<Base>(GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetName());
	}

	// Add vars, funcs, and static funcs and attrs from base class.
	if (GAFF_REFLECTION_NAMESPACE::Reflection<Base>::IsDefined()) {
		const ReflectionDefinition<Base, Allocator>& base_ref_def = GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetReflectionDefinition();

		// For calling base class functions.
		_base_classes.emplace(
			GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetHash(),
			&base_ref_def
		);

		// Base class vars
		for (auto& it : base_ref_def._vars) {
			GAFF_ASSERT(_vars.find(it.first) == _vars.end());

			eastl::pair<HashString32<Allocator>, IVarPtr> pair(
				it.first,
				IVarPtr(GAFF_ALLOCT(BaseVarPtr<Base>, _allocator, it.second.get()))
			);

			pair.second->setNoSerialize(!it.second->canSerialize());
			pair.second->setReadOnly(it.second->isReadOnly());

			_vars.insert(std::move(pair));

			// Base class var attrs
			const auto attr_it = base_ref_def._var_attrs.find(pair.first.getHash());

			// Copy attributes
			if (attr_it != base_ref_def._var_attrs.end()) {
				auto& attrs = _var_attrs[pair.first.getHash()];
				attrs.set_allocator(_allocator);

				for (const IAttributePtr& attr : attr_it->second) {
					if (attr->canInherit()) {
						attrs.emplace_back(attr->clone());
					}
				}
			}
		}

		// Base class funcs
		for (auto& it : base_ref_def._funcs) {
			FuncData& func_data = _funcs[it.first];

			for (int32_t i = 0; i < FuncData::k_num_overloads; ++i) {
				if (!it.second.func[i]) {
					break;
				}

				int32_t index = -1;

				for (int32_t j = 0; j < FuncData::k_num_overloads; ++j) {
					if (!func_data.func[j]) {
						index = j;
						break;
					}

					if (it.second.hash[i] == func_data.hash[j]) {
						break;
					}

					--index;
				}

				if (index < 0) {
					GAFF_ASSERT_MSG(index > -(FuncData::k_num_overloads + 1), "Function overloading only supports %i overloads per function name!", FuncData::k_num_overloads);
					continue;
				}

				ReflectionBaseFunction* const ref_func = GAFF_ALLOCT(
					ReflectionBaseFunction,
					_allocator,
					it.second.func[i]->getBaseRefDef(),
					it.second.func[i].get()
				);

				func_data.hash[index] = it.second.hash[i];
				func_data.func[index].reset(ref_func);

				// Copy attributes.
				const Hash64 attr_hash = FNV1aHash64T(func_data.hash[i], FNV1aHash64T(FNV1aHash32T(it.first.getHash())));
				const auto attr_it = base_ref_def._func_attrs.find(attr_hash);

				if (attr_it != base_ref_def._func_attrs.end()) {
					auto& attrs = _func_attrs[attr_hash];
					attrs.set_allocator(_allocator);

					for (const IAttributePtr& attr : attr_it->second) {
						if (attr->canInherit()) {
							attrs.emplace_back(attr->clone());
						}
					}
				}
			}
		}

		// Base class static funcs
		for (auto& it : base_ref_def._static_funcs) {
			StaticFuncData& static_func_data = _static_funcs[it.first];

			for (int32_t i = 0; i < StaticFuncData::k_num_overloads; ++i) {
				if (!it.second.func[i]) {
					break;
				}

				int32_t index = -1;

				for (int32_t j = 0; j < StaticFuncData::k_num_overloads; ++j) {
					if (!static_func_data.func[j]) {
						index = j;
						break;
					}

					if (it.second.hash[i] == static_func_data.hash[j]) {
						break;
					}
				}

				if (index < 0) {
					GAFF_ASSERT_MSG(index > -(StaticFuncData::k_num_overloads + 1), "Function overloading only supports %i overloads per function name!", StaticFuncData::k_num_overloads);
					continue;
				}

				static_func_data.hash[index] = it.second.hash[i];
				static_func_data.func[index].reset(it.second.func[i]->clone(_allocator));

				// Copy attributes.
				const Hash64 attr_hash = FNV1aHash64T(static_func_data.hash[i], FNV1aHash64T(FNV1aHash32T(it.first.getHash())));
				const auto attr_it = base_ref_def._static_func_attrs.find(attr_hash);

				if (attr_it != base_ref_def._static_func_attrs.end()) {
					auto& attrs = _static_func_attrs[attr_hash];
					attrs.set_allocator(_allocator);

					for (const IAttributePtr& attr : attr_it->second) {
						if (attr->canInherit()) {
							attrs.emplace_back(attr->clone());
						}
					}
				}
			}
		}

		// Base class class attrs
		for (const IAttributePtr& attr : base_ref_def._class_attrs) {
			if (attr->canInherit()) {
				_class_attrs.emplace_back(attr->clone());
			}
		}

	// Register for callback if base class hasn't been defined yet.
	} else {
		++_dependents_remaining;

		eastl::function<void (void)> cb(&RegisterBaseVariables<Base>);
		GAFF_REFLECTION_NAMESPACE::Reflection<Base>::RegisterOnDefinedCallback(std::move(cb));
	}

	return *this;
}

template <class T, class Allocator>
template <class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::ctor(Hash64 factory_hash)
{
	GAFF_ASSERT(!getFactory(factory_hash));

	ConstructFuncT<T, Args...> construct_func = Gaff::ConstructFunc<T, Args...>;
	FactoryFuncT<T, Args...> factory_func = Gaff::FactoryFunc<T, Args...>;

	using ConstructorFunction = StaticFunction<void, T*, Args&&...>;

	_ctors[factory_hash].reset(GAFF_ALLOCT(ConstructorFunction, _allocator, construct_func));
	_factories.emplace(factory_hash, reinterpret_cast<VoidFunc>(factory_func));

	return *this;
}

template <class T, class Allocator>
template <class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::ctor(void)
{
	constexpr Hash64 hash = CalcTemplateHash<Args...>(k_init_hash64);
	return ctor<Args...>(hash);
}

template <class T, class Allocator>
template <class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[name_size], Var T::*ptr, const Attrs&... attributes)
{
	static_assert(!std::is_reference<Var>::value, "Cannot reflect references.");
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");
	static_assert(!std::is_const<Var>::value, "Cannot reflect const values.");
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<Var>::HasReflection, "Var type is not reflected!");

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, _allocator),
		IVarPtr(GAFF_ALLOCT(VarPtr<Var>, _allocator, ptr))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	auto& attrs = _var_attrs[FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(pair.second.get(), ptr, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class Enum, size_t name_size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[name_size], Flags<Enum> T::*ptr, const Attrs&... attributes)
{
	static_assert(std::is_enum<Enum>::value, "Flags does not contain an enum.");
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<Enum>::HasReflection, "Enum is not reflected!");

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, _allocator),
		IVarPtr(GAFF_ALLOCT(VarPtr< Flags<Enum> >, _allocator, ptr))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	auto& attrs = _var_attrs[FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(pair.second.get(), ptr, attrs, attributes...);
	}

	_vars.insert(std::move(pair));

	// For each reflected entry in Enum, add a reflection var for that entry.
	const IEnumReflectionDefinition& ref_def = GAFF_REFLECTION_NAMESPACE::Reflection<Enum>::GetReflectionDefinition();
	const int32_t num_entries = ref_def.getNumEntries();

	for (int32_t i = 0; i < num_entries; ++i) {
		const HashStringView32<> flag_name = ref_def.getEntryNameFromIndex(i);
		const int32_t flag_index = ref_def.getEntryValue(i);

		U8String<Allocator> flag_path(_allocator);
		flag_path.append_sprintf("%s/%s", name, flag_name.getBuffer());

		eastl::pair<HashString32<Allocator>, IVarPtr> flag_pair(
			HashString32<Allocator>(flag_path),
			IVarPtr(GAFF_ALLOCT(VarFlagPtr<Enum>, _allocator, ptr, static_cast<uint8_t>(i)))
		);

		flag_pair.second->setNoSerialize(true);
		_vars.insert(std::move(flag_pair));
	}

	return *this;
}

template <class T, class Allocator>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes)
{
	//static_assert(std::is_reference<Ret>::value || std::is_pointer<Ret>::value, "Function version of var() only supports reference and pointer return types!");

	using RetNoRef = typename std::remove_reference<Ret>::type;
	using RetNoPointer = typename std::remove_pointer<RetNoRef>::type;
	using RetNoConst = typename std::remove_const<RetNoPointer>::type;

	using VarNoRef = typename std::remove_reference<Var>::type;
	using VarNoPointer = typename std::remove_pointer<VarNoRef>::type;
	using VarNoConst = typename std::remove_const<VarNoPointer>::type;

	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<RetNoConst>::HasReflection, "Getter return type is not reflected!");
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<VarNoConst>::HasReflection, "Setter arg type is not reflected!");

	eastl::pair<HashString32<Allocator>, IVarPtr> pair;

	if constexpr (std::is_reference<Ret>::value || std::is_pointer<Ret>::value) {
		using PtrType = VarFuncPtr<Ret, Var>;

		pair = eastl::pair<HashString32<Allocator>, IVarPtr>(
			HashString32<Allocator>(name, name_size - 1, _allocator),
			IVarPtr(GAFF_ALLOCT(PtrType, _allocator, getter, setter))
		);
	} else {
		using PtrType = VarFuncPtrWithCache<Ret, Var>;

		pair = eastl::pair<HashString32<Allocator>, IVarPtr>(
			HashString32<Allocator>(name, name_size - 1, _allocator),
			IVarPtr(GAFF_ALLOCT(PtrType, _allocator, getter, setter))
		);
	}

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	auto& attrs = _var_attrs[FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(pair.second.get(), getter, setter, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[name_size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes)
{
	//static_assert(std::is_reference<Ret>::value || std::is_pointer<Ret>::value, "Function version of var() only supports reference and pointer return types!");

	using RetNoRef = typename std::remove_reference<Ret>::type;
	using RetNoPointer = typename std::remove_pointer<RetNoRef>::type;
	using RetNoConst = typename std::remove_const<RetNoPointer>::type;

	using VarNoRef = typename std::remove_reference<Var>::type;
	using VarNoPointer = typename std::remove_pointer<VarNoRef>::type;
	using VarNoConst = typename std::remove_const<VarNoPointer>::type;

	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<RetNoConst>::HasReflection, "Getter return type is not reflected!");
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<VarNoConst>::HasReflection, "Setter arg type is not reflected!");

	eastl::pair<HashString32<Allocator>, IVarPtr> pair;

	if constexpr (std::is_reference<Ret>::value || std::is_pointer<Ret>::value) {
		using PtrType = VarFuncPtr<Ret, Var>;

		pair = eastl::pair<HashString32<Allocator>, IVarPtr>(
			HashString32<Allocator>(name, name_size - 1, _allocator),
			IVarPtr(GAFF_ALLOCT(PtrType, _allocator, getter, setter))
		);
	} else {
		using PtrType = VarFuncPtrWithCache<Ret, Var>;

		pair = eastl::pair<HashString32<Allocator>, IVarPtr>(
			HashString32<Allocator>(name, name_size - 1, _allocator),
			IVarPtr(GAFF_ALLOCT(PtrType, _allocator, getter, setter))
		);
	}

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	auto& attrs = _var_attrs[FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(pair.second.get(), getter, setter, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator, size_t name_size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[name_size], Vector<Var, Vec_Allocator> T::*vec, const Attrs&... attributes)
{
	static_assert(!std::is_reference<Var>::value, "Cannot reflect references.");
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");
	static_assert(!std::is_const<Var>::value, "Cannot reflect const values.");
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<Var>::HasReflection, "Vector data type is not reflected!");

	using PtrType = VectorPtr<Var, Vec_Allocator>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, vec))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	auto& attrs = _var_attrs[FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(pair.second.get(), vec, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class Var, size_t array_size, size_t name_size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[name_size], Var (T::*arr)[array_size], const Attrs&... attributes)
{
	static_assert(!std::is_reference<Var>::value, "Cannot reflect references.");
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");
	static_assert(!std::is_const<Var>::value, "Cannot reflect const values.");
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<Var>::HasReflection, "Array data type is not reflected!");

	using PtrType = ArrayPtr<Var, array_size>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, arr))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	auto& attrs = _var_attrs[FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(pair.second.get(), arr, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator, size_t name_size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char(&name)[name_size], VectorMap<Key, Value, VecMap_Allocator> T::* vec_map, const Attrs&... attributes)
{
	static_assert(!std::is_reference<Key>::value, "Cannot reflect references.");
	static_assert(!std::is_pointer<Key>::value, "Cannot reflect pointers.");
	static_assert(!std::is_const<Key>::value, "Cannot reflect const values.");
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<Key>::HasReflection, "Key data type is not reflected!");

	static_assert(!std::is_reference<Value>::value, "Cannot reflect references.");
	static_assert(!std::is_pointer<Value>::value, "Cannot reflect pointers.");
	static_assert(!std::is_const<Value>::value, "Cannot reflect const values.");
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<Value>::HasReflection, "Value data type is not reflected!");

	using PtrType = VectorMapPtr<Key, Value, VecMap_Allocator>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, vec_map))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	auto& attrs = _var_attrs[FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(pair.second.get(), vec_map, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::func(const char (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes)
{
	constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(k_init_hash64);
	auto it = _funcs.find(FNV1aHash32Const(name));

	if (it == _funcs.end()) {
		ReflectionFunction<true, Ret, Args...>* const ref_func = GAFF_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionFunction<true, Ret, Args...>),
			_allocator,
			ptr
		);

		eastl::pair<HashString32<Allocator>, FuncData> pair(
			HashString32<Allocator>(name, name_size - 1, _allocator),
			FuncData()
		);

		it = _funcs.insert(std::move(pair)).first;
		it->second.func[0].reset(ref_func);
		it->second.hash[0] = arg_hash;

	} else {
		FuncData& func_data = it->second;
		bool found = false;

		for (int32_t i = 0; i < FuncData::k_num_overloads; ++i) {
			GAFF_ASSERT(!func_data.func[i] || func_data.hash[i] != arg_hash);

			if (!func_data.func[i] || func_data.func[i]->isBase()) {
				ReflectionFunction<true, Ret, Args...>* const ref_func = GAFF_ALLOCT(
					GAFF_SINGLE_ARG(ReflectionFunction<true, Ret, Args...>),
					_allocator,
					ptr
				);

				func_data.func[i].reset(ref_func);
				func_data.hash[i] = arg_hash;
				found = true;
				break;
			}
		}

		GAFF_ASSERT_MSG(found, "Function overloading only supports 8 overloads per function name!");
	}

	const Hash32 name_hash = FNV1aHash32Const(name);
	const Hash64 attr_hash = FNV1aHash64T(arg_hash, FNV1aHash64T(name_hash));

	auto& attrs = _func_attrs[attr_hash];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(ptr, attrs, attributes...);
	}

	return *this;
}

template <class T, class Allocator>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::func(const char (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes)
{
	constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(k_init_hash64);
	auto it = _funcs.find(FNV1aHash32Const(name));

	if (it == _funcs.end()) {
		ReflectionFunction<false, Ret, Args...>* const ref_func = GAFF_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionFunction<false, Ret, Args...>),
			_allocator,
			ptr
		);

		it = _funcs.emplace(
			HashString32<Allocator>(name, name_size - 1, _allocator),
			FuncData()
		).first;
		
		it->second.func[0].reset(ref_func);
		it->second.hash[0] = arg_hash;

	} else {
		FuncData& func_data = it->second;
		bool found = false;

		for (int32_t i = 0; i < FuncData::k_num_overloads; ++i) {
			GAFF_ASSERT(!func_data.func[i] || func_data.hash[i] != arg_hash);

			if (!func_data.func[i] || func_data.func[i]->isBase()) {
				ReflectionFunction<false, Ret, Args...>* const ref_func = GAFF_ALLOCT(
					GAFF_SINGLE_ARG(ReflectionFunction<false, Ret, Args...>),
					_allocator,
					ptr
				);

				func_data.func[i].reset(ref_func);
				func_data.hash[i] = arg_hash;
				found = true;
				break;
			}
		}

		GAFF_ASSERT_MSG(found, "Function overloading only supports 8 overloads per function name!");
	}

	const Hash32 name_hash = FNV1aHash32Const(name);
	const Hash64 attr_hash = FNV1aHash64T(arg_hash, FNV1aHash64T(name_hash));

	auto& attrs = _func_attrs[attr_hash];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(ptr, attrs, attributes...);
	}

	return *this;
}

template <class T, class Allocator>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::staticFunc(const char (&name)[name_size], Ret (*func)(Args...), const Attrs&... attributes)
{
	constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(k_init_hash64);
	auto it = _static_funcs.find(FNV1aHash32Const(name));

	using StaticFuncType = StaticFunction<Ret, Args...>;

	if (it == _static_funcs.end()) {
		it = _static_funcs.emplace(
			HashString32<Allocator>(name, name_size - 1, _allocator),
			StaticFuncData(_allocator)
		).first;

		it->second.func[0].reset(GAFF_ALLOCT(StaticFuncType, _allocator, func));
		it->second.hash[0] = arg_hash;

	} else {
		StaticFuncData& func_data = it->second;
		bool found = false;

		for (int32_t i = 0; i < FuncData::k_num_overloads; ++i) {
			// Replace an open slot or replace an already existing overload.
			if (func_data.func[i] && func_data.hash[i] != arg_hash) {
				continue;
			}

			func_data.func[i].reset(GAFF_ALLOCT(StaticFuncType, _allocator, func));
			func_data.hash[i] = arg_hash;
			found = true;
			break;
		}

		GAFF_ASSERT_MSG(found, "Function overloading only supports 8 overloads per function name!");
	}

	const Hash32 name_hash = FNV1aHash32Const(name);
	const Hash64 attr_hash = FNV1aHash64T(arg_hash, FNV1aHash64T(name_hash));

	auto& attrs = _static_func_attrs[attr_hash];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(func, attrs, attributes...);
	}

	return *this;
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opAdd(void)
{
	staticFunc(OP_ADD_NAME, Add<T, Other>);
	return staticFunc(OP_ADD_NAME, Add<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opSub(void)
{
	staticFunc(OP_SUB_NAME, Sub<T, Other>);
	return staticFunc(OP_SUB_NAME, Sub<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opMul(void)
{
	staticFunc(OP_MUL_NAME, Mul<T, Other>);
	return staticFunc(OP_MUL_NAME, Mul<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opDiv(void)
{
	staticFunc(OP_DIV_NAME, Div<T, Other>);
	return staticFunc(OP_DIV_NAME, Div<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opMod(void)
{
	staticFunc(OP_MOD_NAME, Mod<T, Other>);
	return staticFunc(OP_MOD_NAME, Mod<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitAnd(void)
{
	staticFunc(OP_BIT_AND_NAME, BitAnd<T, Other>);
	return staticFunc(OP_BIT_AND_NAME, BitAnd<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitOr(void)
{
	staticFunc(OP_BIT_OR_NAME, BitOr<T, Other>);
	return staticFunc(OP_BIT_OR_NAME, BitOr<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitXor(void)
{
	staticFunc(OP_BIT_XOR_NAME, BitXor<T, Other>);
	return staticFunc(OP_BIT_XOR_NAME, BitXor<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitShiftLeft(void)
{
	staticFunc(OP_BIT_SHIFT_LEFT_NAME, BitShiftLeft<T, Other>);
	return staticFunc(OP_BIT_SHIFT_LEFT_NAME, BitShiftLeft<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitShiftRight(void)
{
	staticFunc(OP_BIT_SHIFT_RIGHT_NAME, BitShiftRight<T, Other>);
	return staticFunc(OP_BIT_SHIFT_RIGHT_NAME, BitShiftRight<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opAnd(void)
{
	staticFunc(OP_LOGIC_AND_NAME, LogicAnd<T, Other>);
	return staticFunc(OP_LOGIC_AND_NAME, LogicAnd<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opOr(void)
{
	staticFunc(OP_LOGIC_OR_NAME, LogicOr<T, Other>);
	return staticFunc(OP_LOGIC_OR_NAME, LogicOr<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opEqual(void)
{
	staticFunc(OP_EQUAL_NAME, Equal<T, Other>);
	return staticFunc(OP_EQUAL_NAME, Equal<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opLessThan(void)
{
	staticFunc(OP_LESS_THAN_NAME, LessThan<T, Other>);
	return staticFunc(OP_LESS_THAN_NAME, LessThan<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opGreaterThan(void)
{
	staticFunc(OP_GREATER_THAN_NAME, GreaterThan<T, Other>);
	return staticFunc(OP_GREATER_THAN_NAME, GreaterThan<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opLessThanOrEqual(void)
{
	staticFunc(OP_LESS_THAN_OR_EQUAL_NAME, LessThanOrEqual<T, Other>);
	return staticFunc(OP_LESS_THAN_OR_EQUAL_NAME, LessThanOrEqual<Other, T>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opGreaterThanOrEqual(void)
{
	staticFunc(OP_GREATER_THAN_OR_EQUAL_NAME, GreaterThanOrEqual<T, Other>);
	return staticFunc(OP_GREATER_THAN_OR_EQUAL_NAME, GreaterThanOrEqual<Other, T>);
}

template <class T, class Allocator>
template <class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opCall(void)
{
	return staticFunc(OP_CALL_NAME, Call<T, Args...>);
}

template <class T, class Allocator>
template <class Other>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opIndex(void)
{
	return staticFunc(OP_INDEX_NAME, Index<T, Other>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opAdd(void)
{
	return staticFunc(OP_ADD_NAME, Add<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opSub(void)
{
	return staticFunc(OP_SUB_NAME, Sub<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opMul(void)
{
	return staticFunc(OP_MUL_NAME, Mul<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opDiv(void)
{
	return staticFunc(OP_DIV_NAME, Div<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opMod(void)
{
	return staticFunc(OP_MOD_NAME, Mod<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitAnd(void)
{
	return staticFunc(OP_BIT_AND_NAME, BitAnd<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitOr(void)
{
	return staticFunc(OP_BIT_OR_NAME, BitOr<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitXor(void)
{
	return staticFunc(OP_BIT_XOR_NAME, BitXor<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitNot(void)
{
	return staticFunc(OP_BIT_NOT_NAME, BitNot<T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitShiftLeft(void)
{
	return staticFunc(OP_BIT_SHIFT_LEFT_NAME, BitShiftLeft<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opBitShiftRight(void)
{
	return staticFunc(OP_BIT_SHIFT_RIGHT_NAME, BitShiftRight<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opAnd(void)
{
	return staticFunc(OP_LOGIC_AND_NAME, LogicAnd<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opOr(void)
{
	return staticFunc(OP_LOGIC_OR_NAME, LogicOr<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opEqual(void)
{
	return staticFunc(OP_EQUAL_NAME, Equal<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opLessThan(void)
{
	return staticFunc(OP_LESS_THAN_NAME, LessThan<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opGreaterThan(void)
{
	return staticFunc(OP_GREATER_THAN_NAME, GreaterThan<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opLessThanOrEqual(void)
{
	return staticFunc(OP_LESS_THAN_OR_EQUAL_NAME, LessThanOrEqual<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opGreaterThanOrEqual(void)
{
	return staticFunc(OP_GREATER_THAN_OR_EQUAL_NAME, GreaterThanOrEqual<T, T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opMinus(void)
{
	return staticFunc(OP_MINUS_NAME, Minus<T>);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opPlus(void)
{
	return staticFunc(OP_PLUS_NAME, Plus<T>);
}

template <class T, class Allocator>
template <int32_t (*to_string_func)(const T&, char*, int32_t)>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::opToString()
{
	staticFunc(OP_TO_STRING_NAME, ToStringHelper<T>::ToString<to_string_func>);
	return staticFunc(OP_TO_STRING_NAME, to_string_func);
}

template <class T, class Allocator>
template <class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::classAttrs(const Attrs&... attributes)
{
	return addAttributes(_class_attrs, attributes...);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::version(uint32_t /*version*/)
{
	return *this;
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::serialize(LoadFunc serialize_load, SaveFunc serialize_save)
{
	_serialize_load = serialize_load;
	_serialize_save = serialize_save;
	return *this;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::finish(void)
{
	if (!_dependents_remaining) {
		// Call finish() on attributes first.
		for (IAttributePtr& attr : _class_attrs) {
			attr->finish(*this);
		}

		for (auto& it : _var_attrs) {
			for (IAttributePtr& attr : it.second) {
				attr->finish(*this);
			}
		}

		for (auto& it : _func_attrs) {
			for (IAttributePtr& attr : it.second) {
				attr->finish(*this);
			}
		}

		for (auto& it : _static_func_attrs) {
			for (IAttributePtr& attr : it.second) {
				attr->finish(*this);
			}
		}

		if (_friendly_name.empty()) {
			_friendly_name = getReflectionInstance().getName();
		}
	}
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::RegisterBaseVariables(void)
{
	ReflectionDefinition<T, Allocator>& ref_def = const_cast<ReflectionDefinition<T, Allocator>&>(
		GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetReflectionDefinition()
	);

	--ref_def._dependents_remaining;
	GAFF_ASSERT(ref_def._dependents_remaining >= 0);

	ref_def.base<Base>();

	if (ref_def._dependents_remaining == 0) {
		ref_def.finish();
	}
}



// Variables
template <class T, class Allocator>
template <class Var, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(IReflectionVar* ref_var, Var T::*var, Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(*ref_var, var);

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(ref_var, var, attrs, rest...);
	} else {
		return *this;
	}
}

template <class T, class Allocator>
template <class Var, class Ret, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(IReflectionVar* ref_var, Ret (T::*getter)(void) const, void (T::*setter)(Var), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(*ref_var, getter, setter);

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(ref_var, getter, setter, attrs, rest...);
	} else {
		return *this;
	}
}

// Functions
template <class T, class Allocator>
template <class Ret, class... Args, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Ret (T::*func)(Args...) const, Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(func);
	
	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(func, attrs, rest...);
	} else {
		return *this;
	}
}

template <class T, class Allocator>
template <class Ret, class... Args, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Ret (T::*func)(Args...), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(func);

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(func, attrs, rest...);
	} else {
		return *this;
	}
}

// Static Functions
template <class T, class Allocator>
template <class Ret, class... Args, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Ret (*func)(Args...), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(func);

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(func, attrs, rest...);
	} else {
		return *this;
	}
}

// Non-apply() call version.
template <class T, class Allocator>
template <class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	attrs.emplace_back(IAttributePtr(first.clone()));

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(attrs, rest...);
	} else {
		return *this;
	}
}

template <class T, class Allocator>
ptrdiff_t ReflectionDefinition<T, Allocator>::getBasePointerOffset(Hash64 interface_name) const
{
	const auto it = _base_class_offsets.find(interface_name);
	return (it != _base_class_offsets.end()) ? it->second : -1;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::instantiated(void* object) const
{
	for (const IAttributePtr& attr : _class_attrs) {
		const_cast<IAttributePtr&>(attr)->instantiated(object, *this);
	}

	for (auto& it : _var_attrs) {
		for (const IAttributePtr& attr : it.second) {
			const_cast<IAttributePtr&>(attr)->instantiated(object, *this);
		}
	}

	for (auto& it : _func_attrs) {
		for (const IAttributePtr& attr : it.second) {
			const_cast<IAttributePtr&>(attr)->instantiated(object, *this);
		}
	}

	for (auto& it : _static_func_attrs) {
		for (const IAttributePtr& attr : it.second) {
			const_cast<IAttributePtr&>(attr)->instantiated(object, *this);
		}
	}
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getAttribute(const AttributeList& attributes, Hash64 attr_name) const
{
	for (const auto& attr : attributes) {
		if (attr->getReflectionDefinition().hasInterface(attr_name)) {
			return attr.get();
		}
	}

	return nullptr;
}

NS_END
