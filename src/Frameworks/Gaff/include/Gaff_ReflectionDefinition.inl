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

NS_GAFF

template <bool is_pointer>
struct ValueHelper;

template <>
struct ValueHelper<true>
{
	template <class T>
	using type = typename std::add_pointer<T>::type;
};

template <>
struct ValueHelper<false>
{
	template <class T>
	using type = T;
};

template <class T, class... Args>
void ConstructFunc(void* obj, Args&&... args)
{
	Gaff::Construct(reinterpret_cast<T*>(obj), std::forward<Args>(args)...);
}

template <class T, class... Args>
void* FactoryFunc(IAllocator& allocator, Args&&... args)
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
const Gaff::IReflection& ReflectionDefinition<T, Allocator>::VarPtr<Var>::getReflection(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<Var>::GetInstance();
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
	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = *reinterpret_cast<const Var*>(data);
}

template <class T, class Allocator>
template <class Var>
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::setDataMove(void* object, void* data)
{
	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = std::move(*reinterpret_cast<Var*>(data));
}

template <class T, class Allocator>
template <class Var>
bool ReflectionDefinition<T, Allocator>::VarPtr<Var>::load(const ISerializeReader& reader, T& object)
{
	Var* const var = &(object.*_ptr);
	return GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Load(reader, *var);
}

template <class T, class Allocator>
template <class Var>
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::save(ISerializeWriter& writer, const T& object)
{
	const Var* const var = &(object.*_ptr);
	GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Save(writer, *var);
}



// VarFuncPtr
template <class T, class Allocator>
template <class Ret, class Var>
ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::VarFuncPtr(Getter getter, Setter setter):
	_getter(getter), _setter(setter)
{
	GAFF_ASSERT(getter);
}

template <class T, class Allocator>
template <class Ret, class Var>
const Gaff::IReflection& ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::getReflection(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::GetInstance();
}

template <class T, class Allocator>
template <class Ret, class Var>
const void* ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::getData(const void* object) const
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_getter);

	const T* const obj = reinterpret_cast<const T*>(object);

	if constexpr (std::is_reference<Ret>::value) {
		const Ret& val = (obj->*_getter)();
		return &val;

	} else if (std::is_pointer<Ret>::value) {
		return (obj->*_getter)();
	}
}

template <class T, class Allocator>
template <class Ret, class Var>
void* ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::getData(void* object)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_getter);

	T* const obj = reinterpret_cast<T*>(object);

	if constexpr (std::is_reference<Ret>::value) {
		const Ret& val = (obj->*_getter)();
		RetType* const ptr = const_cast<RetType*>(&val);
		return ptr;

	} else if (std::is_pointer<Ret>::value) {
		return const_cast<RetType*>((obj->*_getter)());
	}
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_setter)(*reinterpret_cast<const RetType*>(data));
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_setter)(*reinterpret_cast<RetType*>(data));
}

template <class T, class Allocator>
template <class Ret, class Var>
bool ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::load(const ISerializeReader& reader, T& object)
{
	GAFF_ASSERT(_getter);
	GAFF_ASSERT(_setter);

	if constexpr (std::is_reference<Ret>::value) {
		RetType& val = const_cast<RetType&>((object.*_getter)());
		return GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::Load(reader, val);

	} else {
		RetType* const val = const_cast<RetType*>((object.*_getter)());
		return GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::Load(reader, *val);
	}
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::save(ISerializeWriter& writer, const T& object)
{
	GAFF_ASSERT(_getter);

	if constexpr (std::is_reference<Ret>::value) {
		const RetType& val = (object.*_getter)();
		GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::Save(writer, val);

	} else {
		const RetType* const val = (object.*_getter)();
		GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::Save(writer, *val);
	}
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
const Gaff::IReflection& ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::getReflection(void) const
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
	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->setData(obj, data);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);
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
	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->swap(obj, index_a, index_b);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::resize(void* object, size_t new_size)
{
	GAFF_ASSERT(object);
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
const Gaff::IReflection& ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::getReflection(void) const
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
const Gaff::IReflection& ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::getReflection(void) const
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
	T* const obj = reinterpret_cast<T*>(object);
	eastl::swap((obj->*_ptr)[index_a], (obj->*_ptr)[index_b]);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::resize(void* object, size_t new_size)
{
	GAFF_ASSERT(object);
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
const Gaff::IReflection& ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::getReflection(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<Value>::GetInstance();
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
const Gaff::IReflection& ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::getReflectionKey(void) const
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
	T* const obj = reinterpret_cast<T*>(object);
	eastl::swap((obj->*_ptr).at(index_a).second, (obj->*_ptr).at(index_b).second);
}

template <class T, class Allocator>
template <class Key, class Value, class VecMap_Allocator>
void ReflectionDefinition<T, Allocator>::VectorMapPtr<Key, Value, VecMap_Allocator>::resize(void* object, size_t new_size)
{
	GAFF_ASSERT(object);
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
			if (!entry.second->isReadOnly()) {
				const char* const name = entry.first.getBuffer();

				if (!reader.exists(name)) {
					// I don't like this method of determining something as optional.
					const auto* const attr = getVarAttr<IAttribute>(Gaff::FNV1aHash32String(name), Gaff::FNV1aHash64Const("OptionalAttribute"));

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
			if (!entry.second->isReadOnly()) {
				++writable_vars;
			}
		}

		// Write out the object.
		writer.startObject(writable_vars);

		writer.writeUInt64("version", getReflectionInstance().getVersion());

		for (auto& entry : _vars) {
			if (!entry.second->isReadOnly()) {
				writer.writeKey(entry.first.getBuffer());
				entry.second->save(writer, object);
			}
		}

		writer.endObject();
	}
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

	_allocator = allocator;
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
bool ReflectionDefinition<T, Allocator>::isStandardLayout(void) const
{
	return std::is_standard_layout<T>::value;
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::isTrivial(void) const
{
	return std::is_trivial<T>::value;
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumVars(void) const
{
	return static_cast<int32_t>(_vars.size());
}

template <class T, class Allocator>
const char* ReflectionDefinition<T, Allocator>::getVarName(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	return (_vars.begin() + index)->first.getBuffer();
}

template <class T, class Allocator>
Hash32 ReflectionDefinition<T, Allocator>::getVarHash(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	return (_vars.begin() + index)->first.getHash();
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
	GAFF_ASSERT(_vars.find(name) != _vars.end());
	return getVarT(name);
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumFuncs(void) const
{
	return static_cast<int32_t>(_funcs.size());
}

template <class T, class Allocator>
Hash32 ReflectionDefinition<T, Allocator>::getFuncHash(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_funcs.size()));
	return getFuncName(index).getHash();
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumStaticFuncs(void) const
{
	return static_cast<int32_t>(_static_funcs.size());
}

template <class T, class Allocator>
Hash32 ReflectionDefinition<T, Allocator>::getStaticFuncHash(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_static_funcs.size()));
	return getStaticFuncName(index).getHash();
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
int32_t ReflectionDefinition<T, Allocator>::getNumFuncAttrs(Hash32 name) const
{
	const auto it = _func_attrs.find(name);

	if (it == _func_attrs.end()) {
		for (auto it_base = _base_classes.begin(); it_base != _base_classes.end(); ++it_base) {
			const int32_t num = it_base->second->getNumFuncAttrs(name);

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
const IAttribute* ReflectionDefinition<T, Allocator>::getFuncAttr(Hash32 name, Hash64 attr_name) const
{
	const auto it = _func_attrs.find(name);
	GAFF_ASSERT(it != _func_attrs.end());

	return getAttribute(it->second, attr_name);
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getFuncAttr(Hash32 name, int32_t index) const
{
	const auto it = _func_attrs.find(name);

	if (it == _func_attrs.end()) {
		for (auto it_base = _base_classes.begin(); it_base != _base_classes.end(); ++it_base) {
			const int32_t num = it_base->second->getNumFuncAttrs(name);

			if (num > 0) {
				GAFF_ASSERT(index < num);
				return it_base->second->getFuncAttr(name, index);
			}
		}

	} else {
		GAFF_ASSERT(index < static_cast<int32_t>(it->second.size()));
		return it->second[index].get();
	}

	return nullptr;
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumStaticFuncAttrs(Hash32 name) const
{
	const auto it = _static_func_attrs.find(name);
	return (it != _static_func_attrs.end()) ? static_cast<int32_t>(it->second.size()) : 0;
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getStaticFuncAttr(Hash32 name, Hash64 attr_name) const
{
	const auto it = _static_func_attrs.find(name);
	GAFF_ASSERT(it != _static_func_attrs.end());

	return getAttribute(it->second, attr_name);
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getStaticFuncAttr(Hash32 name, int32_t index) const
{
	const auto it = _static_func_attrs.find(name);
	GAFF_ASSERT(it != _static_func_attrs.end());
	GAFF_ASSERT(index < static_cast<int32_t>(it->second.size()));
	return it->second[index].get();
}

template <class T, class Allocator>
IReflectionDefinition::VoidFunc ReflectionDefinition<T, Allocator>::getConstructor(Hash64 ctor_hash) const
{
	const auto it = _ctors.find(ctor_hash);
	return it == _ctors.end() ? nullptr : it->second;
}

template <class T, class Allocator>
IReflectionDefinition::VoidFunc ReflectionDefinition<T, Allocator>::getFactory(Hash64 ctor_hash) const
{
	const auto it = _factories.find(ctor_hash);
	return it == _factories.end() ? nullptr : it->second;
}

template <class T, class Allocator>
IReflectionDefinition::VoidFunc ReflectionDefinition<T, Allocator>::getStaticFunc(Hash32 name, Hash64 args) const
{
	const auto it = _static_funcs.find(name);

	if (it != _static_funcs.end()) {
		for (int32_t i = 0; i < StaticFuncData::NUM_OVERLOADS; ++i) {
			if (it->second.hash[i] == args) {
				return it->second.func[i];
			}
		}
	}

	return nullptr;
}

template <class T, class Allocator>
void* ReflectionDefinition<T, Allocator>::getFunc(Hash32 name, Hash64 args) const
{
	const auto it = _funcs.find(name);

	if (it != _funcs.end()) {
		for (int32_t i = 0; i < FuncData::NUM_OVERLOADS; ++i) {
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
	Gaff::Deconstruct(instance);
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
	GAFF_ASSERT(it != _vars.end());
	return it->second.get();
}

template <class T, class Allocator>
const HashString32<Allocator>& ReflectionDefinition<T, Allocator>::getFuncName(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_funcs.size()));
	return (_funcs.begin() + index)->first;
}

template <class T, class Allocator>
const HashString32<Allocator>& ReflectionDefinition<T, Allocator>::getStaticFuncName(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_static_funcs.size()));
	return (_static_funcs.begin() + index)->first;
}

template <class T, class Allocator>
template <class Base>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::base(const char* name)
{
	const ptrdiff_t offset = OffsetOfClass<T, Base>();
	auto pair = std::move(
		eastl::make_pair(
			HashString64<Allocator>(name, nullptr, _allocator),
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
	// Add vars, funcs, and static funcs and attrs from base class.
	if (GAFF_REFLECTION_NAMESPACE::Reflection<Base>::g_defined) {
		const ReflectionDefinition<Base, Allocator>& base_ref_def = GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetReflectionDefinition();
		base<Base>(GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetName());

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

			_vars.insert(std::move(pair));

			// Base class var attrs
			const auto attr_it = base_ref_def._var_attrs.find(pair.first.getHash());

			// Copy attributes
			if (attr_it != base_ref_def._var_attrs.end()) {
				auto& attrs = _var_attrs[pair.first.getHash()];
				attrs.set_allocator(_allocator);

				for (const IAttributePtr& attr : attr_it->second) {
					attrs.emplace_back(attr->clone());
				}
			}
		}

		// Base class funcs
		for (auto& it : base_ref_def._funcs) {
			FuncData& func_data = _funcs[it.first.getHash()];
			bool found = false;

			for (int32_t i = 0; i < FuncData::NUM_OVERLOADS; ++i) {
				GAFF_ASSERT(!func_data.func[i] || func_data.hash[i] != it.first);

				if (!func_data.func[i]) {
					ReflectionBaseFunction* const ref_func = SHIB_ALLOCT(
						ReflectionBaseFunction,
						_allocator,
						it.second.func[i]->getBaseRefDef(),
						it.second.func[i].get()
					);

					func_data.hash[i] = it.second.hash[i];
					func_data.func[i].reset(ref_func);
					found = true;

					// Copy attributes
					const Hash64 attr_hash = FNV1aHash64T(func_data.hash[i], FNV1aHash64T(FNV1aHash32T(it.first.getHash())));
					const auto attr_it = base_ref_def._func_attrs.find(attr_hash);

					if (attr_it != base_ref_def._func_attrs.end()) {
						auto& attrs = _func_attrs[attr_hash];
						attrs.set_allocator(_allocator);

						for (const IAttributePtr& attr : attr_it->second) {
							attrs.emplace_back(attr->clone());
						}
					}

					break;
				}
			}

			GAFF_ASSERT_MSG(found, "Function overloading only supports 8 overloads per function name!");
		}

		// Base class static funcs
		for (auto& it : base_ref_def._static_funcs) {
			GAFF_ASSERT(_static_funcs.find(it.first) == _static_funcs.end());

			_static_funcs.emplace(it.first, it.second.template toDerived<T, Allocator>());

			// Copy attributes
			StaticFuncData& static_func_data = _static_funcs[it.first.getHash()];

			for (int32_t i = 0; i < StaticFuncData::NUM_OVERLOADS; ++i) {
				if (!static_func_data.func[i]) {
					break;
				}

				const Hash64 attr_hash = FNV1aHash64T(static_func_data.hash[i], FNV1aHash64T(FNV1aHash32T(it.first.getHash())));
				const auto attr_it = base_ref_def._static_func_attrs.find(attr_hash);

				if (attr_it != base_ref_def._static_func_attrs.end()) {
					auto& attrs = _static_func_attrs[attr_hash];
					attrs.set_allocator(_allocator);

					for (const IAttributePtr& attr : attr_it->second) {
						attrs.emplace_back(attr->clone());
					}
				}
			}
		}

		// Base class class attrs
		for (const IAttributePtr& attr : base_ref_def._class_attrs) {
			_class_attrs.emplace_back(attr->clone());
		}

	// Register for callback if base class hasn't been defined yet.
	} else {
		++_dependents_remaining;

		eastl::function<void (void)> cb(&RegisterBaseVariables<Base>);
		GAFF_REFLECTION_NAMESPACE::Reflection<Base>::g_on_defined_callbacks.emplace_back(std::move(cb));
	}

	return *this;
}

template <class T, class Allocator>
template <class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::ctor(Hash64 factory_hash)
{
	GAFF_ASSERT(!getFactory(factory_hash));

	ConstructFunc<Args...> construct_func = Gaff::ConstructFunc<T, Args...>;
	FactoryFunc<Args...> factory_func = Gaff::FactoryFunc<T, Args...>;

	_factories.emplace(factory_hash, reinterpret_cast<VoidFunc>(factory_func));
	_ctors.emplace(factory_hash, reinterpret_cast<VoidFunc>(construct_func));	

	return *this;
}

template <class T, class Allocator>
template <class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::ctor(void)
{
	constexpr Hash64 hash = CalcTemplateHash<Args...>(INIT_HASH64);
	GAFF_ASSERT(!getFactory(hash));

	ConstructFunc<Args...> construct_func = Gaff::ConstructFunc<T, Args...>;
	FactoryFunc<Args...> factory_func = Gaff::FactoryFunc<T, Args...>;

	_factories.emplace(hash, reinterpret_cast<VoidFunc>(factory_func));
	_ctors.emplace(hash, reinterpret_cast<VoidFunc>(construct_func));

	return *this;
}

template <class T, class Allocator>
template <class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[name_size], Var T::*ptr, const Attrs&... attributes)
{
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<Var>::HasReflection, "Type is not reflected!");

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, nullptr, _allocator),
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
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes)
{
	static_assert(std::is_reference<Ret>::value || std::is_pointer<Ret>::value, "Function version of var() only supports reference and pointer return types!");

	using RetNoRef = typename std::remove_reference<Ret>::type;
	using RetNoPointer = typename std::remove_pointer<RetNoRef>::type;
	using RetNoConst = typename std::remove_const<RetNoPointer>::type;
	using RetFinal = typename ValueHelper<std::is_pointer<RetNoRef>::value>::template type<RetNoConst>;

	using VarNoRef = typename std::remove_reference<Var>::type;
	using VarNoPointer = typename std::remove_pointer<VarNoRef>::type;
	using VarNoConst = typename std::remove_const<VarNoPointer>::type;
	using VarFinal = typename ValueHelper<std::is_pointer<VarNoRef>::value>::template type<VarNoConst>;

	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<RetFinal>::HasReflection, "Getter return type is not reflected!");
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<VarFinal>::HasReflection, "Setter arg type is not reflected!");
	using PtrType = VarFuncPtr<Ret, Var>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, getter, setter))
	);

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
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");
	using PtrType = VectorPtr<Var, Vec_Allocator>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, nullptr, _allocator),
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
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");
	using PtrType = ArrayPtr<Var, array_size>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, nullptr, _allocator),
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
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<Key>::HasReflection, "Key type is not reflected!");
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<Value>::HasReflection, "Value type is not reflected!");

	using PtrType = VectorMapPtr<Key, Value, VecMap_Allocator>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, nullptr, _allocator),
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
	constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(INIT_HASH64);
	auto it = _funcs.find(FNV1aHash32Const(name));

	if (it == _funcs.end()) {
		ReflectionFunction<Ret, Args...>* const ref_func = SHIB_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionFunction<Ret, Args...>),
			_allocator,
			ptr,
			true
		);

		eastl::pair<HashString32<Allocator>, FuncData> pair(
			HashString32<Allocator>(name, name_size - 1, nullptr, _allocator),
			FuncData()
		);

		it = _funcs.insert(std::move(pair)).first;
		it->second.func[0].reset(ref_func);
		it->second.hash[0] = arg_hash;

	} else {
		FuncData& func_data = it->second;
		bool found = false;

		for (int32_t i = 0; i < FuncData::NUM_OVERLOADS; ++i) {
			GAFF_ASSERT(!func_data.func[i] || func_data.hash[i] != arg_hash);

			if (!func_data.func[i] || func_data.func[i]->isBase()) {
				ReflectionFunction<Ret, Args...>* const ref_func = SHIB_ALLOCT(
					GAFF_SINGLE_ARG(ReflectionFunction<Ret, Args...>),
					_allocator,
					ptr, true
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
	constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(INIT_HASH64);
	auto it = _funcs.find(FNV1aHash32Const(name));

	if (it == _funcs.end()) {
		ReflectionFunction<Ret, Args...>* const ref_func = SHIB_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionFunction<Ret, Args...>),
			_allocator,
			ptr,
			false
		);

		it = _funcs.emplace(
			HashString32<Allocator>(name, name_size - 1, nullptr, _allocator),
			FuncData()
		).first;
		
		it->second.func[0].reset(ref_func);
		it->second.hash[0] = arg_hash;

	} else {
		FuncData& func_data = it->second;
		bool found = false;

		for (int32_t i = 0; i < FuncData::NUM_OVERLOADS; ++i) {
			GAFF_ASSERT(!func_data.func[i] || func_data.hash[i] != arg_hash);

			if (!func_data.func[i] || func_data.func[i]->isBase()) {
				ReflectionFunction<Ret, Args...>* const ref_func = SHIB_ALLOCT(
					GAFF_SINGLE_ARG(ReflectionFunction<Ret, Args...>),
					_allocator,
					ptr, false
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
	constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(INIT_HASH64);
	auto it = _static_funcs.find(FNV1aHash32Const(name));

	if (it == _static_funcs.end()) {
		it = _static_funcs.emplace(
			HashString32<Allocator>(name, name_size - 1, nullptr, _allocator),
			StaticFuncData()
		).first;

		it->second.func[0] = reinterpret_cast<VoidFunc>(func);
		it->second.hash[0] = arg_hash;

	} else {
		StaticFuncData& func_data = it->second;
		bool found = false;

		for (int32_t i = 0; i < FuncData::NUM_OVERLOADS; ++i) {
			if (func_data.func[i]) {
				continue;
			}

			func_data.func[i] = reinterpret_cast<VoidFunc>(func);
			func_data.hash[i] = arg_hash;
			found = true;
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
template <class T2>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::dependsOn(void)
{
	if (!GAFF_REFLECTION_NAMESPACE::Reflection<T2>::g_defined) {
		eastl::function<void(void)> cb(&FinishAfterDependent);
		GAFF_REFLECTION_NAMESPACE::Reflection<T2>::g_on_defined_callbacks.emplace_back(std::move(cb));
	}

	return *this;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::finish(void)
{
	if (!_dependents_remaining) {
		GAFF_REFLECTION_NAMESPACE::Reflection<T>::g_defined = true;

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

		// Call all callbacks waiting for us to finish being defined.
		for (eastl::function<void(void)>& func : GAFF_REFLECTION_NAMESPACE::Reflection<T>::g_on_defined_callbacks) {
			func();
		}

		GAFF_REFLECTION_NAMESPACE::Reflection<T>::g_on_defined_callbacks.clear();
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
	ref_def.finish();
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::FinishAfterDependent(void)
{
	ReflectionDefinition<T, Allocator>& ref_def = const_cast<ReflectionDefinition<T, Allocator>&>(
		GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetReflectionDefinition()
	);

	--ref_def._dependents_remaining;
	GAFF_ASSERT(ref_def._dependents_remaining >= 0);

	ref_def.finish();
}


// Variables
template <class T, class Allocator>
template <class Var, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Gaff::IReflectionVar* ref_var, Var T::*var, Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
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
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Gaff::IReflectionVar* ref_var, Ret (T::*getter)(void) const, void (T::*setter)(Var), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
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
		const_cast<IAttributePtr&>(attr)->instantiated(*this, object);
	}

	for (auto& it : _var_attrs) {
		for (const IAttributePtr& attr : it.second) {
			const_cast<IAttributePtr&>(attr)->instantiated(*this, object);
		}
	}

	for (auto& it : _func_attrs) {
		for (const IAttributePtr& attr : it.second) {
			const_cast<IAttributePtr&>(attr)->instantiated(*this, object);
		}
	}

	for (auto& it : _static_func_attrs) {
		for (const IAttributePtr& attr : it.second) {
			const_cast<IAttributePtr&>(attr)->instantiated(*this, object);
		}
	}
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getAttribute(const AttributeList& attributes, Gaff::Hash64 attr_name) const
{
	for (const auto& attr : attributes) {
		if (attr->getReflectionDefinition().hasInterface(attr_name)) {
			return attr.get();
		}
	}

	return nullptr;
}

NS_END
