/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4307)
#endif

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
void* FactoryFunc(IAllocator& allocator, Args... args)
{
	return GAFF_ALLOCT(T, allocator, std::forward<Args>(args)...);
}

// IVar
template <class T, class Allocator>
template <class DataType>
const DataType& ReflectionDefinition<T, Allocator>::IVar::getDataT(const T& object) const
{
	GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
	return *reinterpret_cast<const DataType*>(getData(&object));
}

template <class T, class Allocator>
template <class DataType>
void ReflectionDefinition<T, Allocator>::IVar::setDataT(T& object, const DataType& data)
{
	GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
	setData(&object, &data);
}

template <class T, class Allocator>
template <class DataType>
void ReflectionDefinition<T, Allocator>::IVar::setDataMoveT(T& object, DataType&& data)
{
	GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
	setDataMove(&object, &data);
}

template <class T, class Allocator>
template <class DataType>
const DataType& ReflectionDefinition<T, Allocator>::IVar::getElementT(const T& object, int32_t index) const
{
	GAFF_ASSERT((isFixedArray() || isVector()) && size(&object) > index);
	GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
	return *reinterpret_cast<const DataType*>(getElement(&object, index));
}

template <class T, class Allocator>
template <class DataType>
void ReflectionDefinition<T, Allocator>::IVar::setElementT(T& object, int32_t index, const DataType& data)
{
	GAFF_ASSERT((isFixedArray() || isVector()) && size(&object) > index);
	GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
	setElement(&object, index, &data);
}

template <class T, class Allocator>
template <class DataType>
void ReflectionDefinition<T, Allocator>::IVar::setElementMoveT(T& object, int32_t index, DataType&& data)
{
	GAFF_ASSERT((isFixedArray() || isVector()) && size(&object) > index);
	GAFF_ASSERT(getType() == GetRVT< std::remove_reference<DataType>::type >());
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
ReflectionValueType ReflectionDefinition<T, Allocator>::VarPtr<Var>::getType(void) const
{
	return GetRVT<Var>();
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
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::load(const ISerializeReader& reader, T& object)
{
	Var* const var = &(object.*_ptr);
	GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Load(reader, *var);
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
ReflectionValueType ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::getType(void) const
{
	using Type = typename std::remove_const<typename std::remove_reference<Ret>::type>::type;
	return GetRVT<Type>();
}

template <class T, class Allocator>
template <class Ret, class Var>
const void* ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::getData(const void* object) const
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_getter);

	const T* obj = reinterpret_cast<const T*>(object);

	if (std::is_reference<Ret>::value) {
		const Ret& val = (obj->*_getter)();
		_copy_ptr = &val;
		return _copy_ptr;
	} else {
		_copy = (obj->*_getter)();
		return &_copy;
	}
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	using VarNoRef = typename std::remove_reference<Var>::type;
	using VarNoPointer = typename std::remove_pointer<VarNoRef>::type;
	using VarNoConst = typename std::remove_const<VarNoPointer>::type;
	using VarFinal = typename ValueHelper<std::is_pointer<VarNoRef>::value>::template type<VarNoConst>;

	T* obj = reinterpret_cast<T*>(object);
	(obj->*_setter)(*reinterpret_cast<const VarFinal*>(data));
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	using VarNoRef = typename std::remove_reference<Var>::type;
	using VarNoPointer = typename std::remove_pointer<VarNoRef>::type;
	using VarNoConst = typename std::remove_const<VarNoPointer>::type;
	using VarFinal = typename ValueHelper<std::is_pointer<VarNoRef>::value>::template type<VarNoConst>;

	T* obj = reinterpret_cast<T*>(object);
	(obj->*_setter)(*reinterpret_cast<VarFinal*>(data));
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::load(const ISerializeReader& reader, T& object)
{
	GAFF_ASSERT(_getter);
	GAFF_ASSERT(_setter);

	using Type = typename std::remove_const<typename std::remove_reference<Ret>::type>::type;

	if (std::is_reference<Ret>::value) {
		const Type& val = (object.*_getter)();
		Type& ref = const_cast<Type&>(val);
		GAFF_REFLECTION_NAMESPACE::Reflection<Type>::Load(reader, ref);

	} else {
		Type val = Type();
		GAFF_REFLECTION_NAMESPACE::Reflection<Type>::Load(reader, val);
		(object.*_setter)(val);
	}
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::save(ISerializeWriter& writer, const T& object)
{
	GAFF_ASSERT(_getter);

	using Type = typename std::remove_const<typename std::remove_reference<Ret>::type>::type;
	const Type& val = (object.*_getter)();
	GAFF_REFLECTION_NAMESPACE::Reflection<Type>::Save(writer, val);
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
ReflectionValueType ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::getType(void) const
{
	return _base_var->getType();
}

template <class T, class Allocator>
template <class Base>
const void* ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::getData(const void* object) const
{
	GAFF_ASSERT(object);
	const Base* obj = reinterpret_cast<const T*>(object);
	return _base_var->getData(obj);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);
	Base* obj = reinterpret_cast<T*>(object);
	_base_var->setData(obj, data);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);
	Base* obj = reinterpret_cast<T*>(object);
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
	const Base* obj = reinterpret_cast<const T*>(object);
	return _base_var->size(obj);
}

template <class T, class Allocator>
template <class Base>
const void* ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	const Base* obj = reinterpret_cast<const T*>(object);
	return _base_var->getElement(obj, index);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);
	Base* obj = reinterpret_cast<T*>(object);
	_base_var->setElement(obj, index, data);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);
	Base* obj = reinterpret_cast<T*>(object);
	_base_var->setElementMove(obj, index, data);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a < size(object));
	GAFF_ASSERT(index_b < size(object));
	GAFF_ASSERT(object);
	Base* obj = reinterpret_cast<T*>(object);
	_base_var->swap(obj, index_a, index_b);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::resize(void* object, size_t new_size)
{
	GAFF_ASSERT(object);
	Base* obj = reinterpret_cast<T*>(object);
	_base_var->resize(obj, new_size);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::load(const ISerializeReader& reader, T& object)
{
	_base_var->load(reader, object);
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
ReflectionValueType ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::getType(void) const
{
	return GetRVT<Var>();
}

template <class T, class Allocator>
template <class Var, size_t array_size>
const void* ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::getData(const void* object) const
{
	GAFF_ASSERT(object);

	const T* obj = reinterpret_cast<const T*>(object);
	return (obj->*_ptr);
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	const Var* vars = reinterpret_cast<const Var*>(data);
	T* obj = reinterpret_cast<T*>(object);

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

	const Var* vars = reinterpret_cast<Var*>(data);
	T* obj = reinterpret_cast<T*>(object);

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
	const T* obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT(index < array_size);
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);
	T* obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = *reinterpret_cast<const Var*>(data);
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index < array_size);
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);
	T* obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = std::move(*reinterpret_cast<Var*>(data));
}

template <class T, class Allocator>
template <class Var, size_t array_size>
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a < array_size);
	GAFF_ASSERT(index_b < array_size);
	GAFF_ASSERT(object);
	T* obj = reinterpret_cast<T*>(object);
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
void ReflectionDefinition<T, Allocator>::ArrayPtr<Var, array_size>::load(const ISerializeReader& reader, T& object)
{
	constexpr int32_t size = static_cast<int32_t>(array_size);
	GAFF_ASSERT(reader.size() == size);

	for (int32_t i = 0; i < size; ++i) {
		ScopeGuard scope = reader.enterElementGuard(i);
		GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Load(reader, (object.*_ptr)[i]);
	}
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
ReflectionValueType ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::getType(void) const
{
	return GetRVT<Var>();
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
const void* ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::getData(const void* object) const
{
	GAFF_ASSERT(object);

	const T* obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	const Var* vars = reinterpret_cast<const Var*>(data);
	T* obj = reinterpret_cast<T*>(object);
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

	const Var* vars = reinterpret_cast<Var*>(data);
	T* obj = reinterpret_cast<T*>(object);
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
	const T* obj = reinterpret_cast<const T*>(object);
	return static_cast<int32_t>((obj->*_ptr).size());
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
const void* ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	const T* obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);
	T* obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = *reinterpret_cast<const Var*>(data);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);
	T* obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = std::move(*reinterpret_cast<Var*>(data));
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a < size(object));
	GAFF_ASSERT(index_b < size(object));
	GAFF_ASSERT(object);
	T* obj = reinterpret_cast<T*>(object);
	eastl::swap((obj->*_ptr)[index_a], (obj->*_ptr)[index_b]);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::resize(void* object, size_t new_size)
{
	GAFF_ASSERT(object);
	T* obj = reinterpret_cast<T*>(object);
	(obj->*_ptr).resize(new_size);
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator>
void ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::load(const ISerializeReader& reader, T& object)
{
	const int32_t size = reader.size();
	(object.*_ptr).resize(static_cast<size_t>(size));

	for (int32_t i = 0; i < size; ++i) {
		ScopeGuard scope = reader.enterElementGuard(i);
		GAFF_REFLECTION_NAMESPACE::Reflection<Var>::Load(reader, (object.*_ptr)[i]);
	}
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



// ReflectionDefinition
template <class T, class Allocator>
template <class... Args>
T* ReflectionDefinition<T, Allocator>::create(Args&&... args) const
{
	return createT<T>(_allocator, std::forward<Args>(args)...);
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::load(const ISerializeReader& reader, void* object) const
{
	load(reader, *reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::save(ISerializeWriter& writer, const void* object) const
{
	save(writer, *reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::load(const ISerializeReader& reader, T& object) const
{
	if (_serialize_load) {
		_serialize_load(reader, object);

	} else {
		for (auto& entry : _vars) {
			if (!entry.second->isReadOnly()) {
				ScopeGuard scope = reader.enterElementGuard(entry.first.getBuffer());
				entry.second->load(reader, object);
			}
		}
	}
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::save(ISerializeWriter& writer, const T& object) const
{
	if (_serialize_save) {
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

	auto it = Find(_base_class_offsets, class_hash);

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

	auto it = Find(_base_class_offsets, class_hash);
	return it != _base_class_offsets.end();
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_base_class_offsets.set_allocator(allocator);
	_ctors.set_allocator(allocator);
	_vars.set_allocator(allocator);
	_funcs.set_allocator(allocator);

	_var_attrs.set_allocator(allocator);
	_func_attrs.set_allocator(allocator);
	_class_attrs.set_allocator(allocator);

	_allocator = allocator;
}

template <class T, class Allocator>
const IReflection& ReflectionDefinition<T, Allocator>::getReflectionInstance(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetInstance();
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumVars(void) const
{
	return static_cast<int32_t>(_vars.size());
}

template <class T, class Allocator>
Hash32 ReflectionDefinition<T, Allocator>::getVarHash(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	return getVarName(index).getHash();
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
	GAFF_ASSERT(Find(_vars, name) != _vars.end());
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

	if (it == _static_func_attrs.end()) {
		for (auto it_base = _base_classes.begin(); it_base != _base_classes.end(); ++it_base) {
			const int32_t num = it_base->second->getNumStaticFuncAttrs(name);

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
const IAttribute* ReflectionDefinition<T, Allocator>::getStaticFuncAttr(Hash32 name, int32_t index) const
{
	const auto it = _static_func_attrs.find(name);

	if (it == _static_func_attrs.end()) {
		for (auto it_base = _base_classes.begin(); it_base != _base_classes.end(); ++it_base) {
			const int32_t num = it_base->second->getNumStaticFuncAttrs(name);

			if (num > 0) {
				GAFF_ASSERT(index < num);
				return it_base->second->getStaticFuncAttr(name, index);
			}
		}

	} else {
		GAFF_ASSERT(index < static_cast<int32_t>(it->second.size()));
		return it->second[index].get();
	}

	return nullptr;
}

template <class T, class Allocator>
IReflectionDefinition::VoidFunc ReflectionDefinition<T, Allocator>::getFactory(Hash64 ctor_hash) const
{
	const auto it = _ctors.find(ctor_hash);
	return it == _ctors.end() ? nullptr : it->second;
}

template <class T, class Allocator>
IReflectionDefinition::VoidFunc ReflectionDefinition<T, Allocator>::getStaticFunc(Hash32 name, Hash64 args) const
{
	const auto it = Find(_static_funcs, name);

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
	const auto it = Find(_funcs, name);

	if (it != _funcs.end()) {
		for (int32_t i = 0; i < FuncData::NUM_OVERLOADS; ++i) {
			if (it->second.hash[i] == args) {
				return reinterpret_cast<char*>(it->second.func[i].get()) - it->second.offset[i];
			}
		}
	}

	return nullptr;
}

template <class T, class Allocator>
const HashString32<Allocator>& ReflectionDefinition<T, Allocator>::getVarName(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	return (_vars.begin() + index)->first;
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
	const auto it = Find(_vars, name);
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
	// Add IVarPtr's and funcs from base class.
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
			eastl::pair<HashString32<Allocator>, IVarPtr> pair(
				it.first,
				IVarPtr(GAFF_ALLOCT(BaseVarPtr<Base>, _allocator, it.second.get()))
			);

			GAFF_ASSERT(_vars.find(pair.first) == _vars.end());
			_vars.insert(std::move(pair));

			// Base class var attrs
			const auto attr_it = base_ref_def._var_attrs.find(pair.first.getHash());

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
			GAFF_REF(it);
		}

		// Base class static funcs
		for (auto& it : base_ref_def._static_funcs) {
			_static_funcs.emplace(it.first, it.second.template toDerived<T, Allocator>());
		}

		// Base class class attrs
		for (const IAttributePtr& attr : base_ref_def._class_attrs) {
			_class_attrs.emplace_back(attr->clone());
		}

	// Register for callback if base class hasn't been defined yet.
	} else {
		++_base_classes_remaining;

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

	FactoryFunc<Args...> factory_func = Gaff::FactoryFunc<T, Args...>;
	_ctors.emplace(factory_hash, reinterpret_cast<VoidFunc>(factory_func));

	return *this;
}

template <class T, class Allocator>
template <class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::ctor(void)
{
	constexpr Hash64 hash = CalcTemplateHash<Args...>(INIT_HASH64);
	GAFF_ASSERT(!getFactory(hash));

	FactoryFunc<Args...> factory_func = Gaff::FactoryFunc<T, Args...>;
	_ctors.emplace(hash, reinterpret_cast<VoidFunc>(factory_func));

	return *this;
}

template <class T, class Allocator>
template <class Var, size_t size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[size], Var T::*ptr, const Attrs&... attributes)
{
	static_assert(GAFF_REFLECTION_NAMESPACE::Reflection<Var>::HasReflection, "Type is not reflected!");

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, size - 1, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(VarPtr<Var>, _allocator, ptr))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	if constexpr (sizeof...(Attrs) > 0) {
		auto& attrs = _var_attrs[FNV1aHash32Const(name)];
		attrs.set_allocator(_allocator);
		addAttributes(pair.second.get(), ptr, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class Ret, class Var, size_t size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes)
{
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
		HashString32<Allocator>(name, size - 1, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, getter, setter))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	if constexpr (sizeof...(Attrs) > 0) {
		auto& attrs = _var_attrs[FNV1aHash32Const(name)];
		attrs.set_allocator(_allocator);
		addAttributes(pair.second.get(), getter, setter, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator, size_t size, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[size], Vector<Var, Vec_Allocator> T::*vec, const Attrs&... attributes)
{
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");
	using PtrType = VectorPtr<Var, Vec_Allocator>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, size - 1, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, vec))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	if constexpr (sizeof...(Attrs) > 0) {
		auto& attrs = _var_attrs[FNV1aHash32Const(name)];
		attrs.set_allocator(_allocator);
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

	if constexpr (sizeof...(Attrs) > 0) {
		auto& attrs = _var_attrs[FNV1aHash32Const(name)];
		attrs.set_allocator(_allocator);
		addAttributes(pair.second.get(), arr, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::func(const char (&name)[size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes)
{
	auto it = Find(_funcs, FNV1aHash32Const(name));

	const ptrdiff_t offset_interface = OffsetOfClass< ReflectionFunction<Ret, Args...>, IReflectionFunction<Ret, Args...> >();
	const ptrdiff_t offset_ptr = OffsetOfClass<ReflectionFunction<Ret, Args...>, VirtualDestructor>();
	constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(INIT_HASH64);

	if (it == _funcs.end()) {
		ReflectionFunction<Ret, Args...>* const ref_func = SHIB_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionFunction<Ret, Args...>),
			_allocator,
			ptr,
			true
		);

		eastl::pair<HashString32<Allocator>, FuncData> pair(
			HashString32<Allocator>(name, size - 1, nullptr, _allocator),
			FuncData()
		);

		it = _funcs.insert(std::move(pair)).first;
		it->second.func[0].reset(ref_func);
		it->second.hash[0] = arg_hash;
		it->second.offset[0] = static_cast<int32_t>(offset_ptr - offset_interface);
	}
	else {
		FuncData& func_data = it->second;
		bool found = false;

		for (int32_t i = 0; i < FuncData::NUM_OVERLOADS; ++i) {
			GAFF_ASSERT(!func_data.func[i] || func_data.hash[i] != arg_hash);

			if (!func_data.func[i]) {
				ReflectionFunction<Ret, Args...>* const ref_func = SHIB_ALLOCT(
					GAFF_SINGLE_ARG(ReflectionFunction<Ret, Args...>),
					_allocator,
					ptr, true
				);

				func_data.func[i].reset(ref_func);
				func_data.hash[i] = arg_hash;
				func_data.offset[i] = static_cast<int32_t>(offset_ptr - offset_interface);
				found = true;
				break;
			}
		}

		GAFF_ASSERT_MSG(found, "Function overloading only supports 8 overloads per function name!");
	}

	if constexpr (sizeof...(Attrs) > 0) {
		auto& attrs = _func_attrs[FNV1aHash32Const(name)];
		attrs.set_allocator(_allocator);
		addAttributes(ptr, attrs, attributes...);
	}

	return *this;
}

template <class T, class Allocator>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::func(const char (&name)[size], Ret (T::*ptr)(Args...), const Attrs&... attributes)
{
	auto it = Find(_funcs, FNV1aHash32Const(name));

	const ptrdiff_t offset_interface = OffsetOfClass< ReflectionFunction<Ret, Args...>, IReflectionFunction<Ret, Args...> >();
	const ptrdiff_t offset_ptr = OffsetOfClass<ReflectionFunction<Ret, Args...>, VirtualDestructor>();
	constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(INIT_HASH64);

	if (it == _funcs.end()) {
		ReflectionFunction<Ret, Args...>* const ref_func = SHIB_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionFunction<Ret, Args...>),
			_allocator,
			ptr,
			false
		);

		it = _funcs.emplace(
			HashString32<Allocator>(name, size - 1, nullptr, _allocator),
			FuncData()
		).first;
		
		it->second.func[0].reset(ref_func);
		it->second.hash[0] = arg_hash;
		it->second.offset[0] = static_cast<int32_t>(offset_ptr - offset_interface);
	}
	else {
		FuncData& func_data = it->second;
		bool found = false;

		for (int32_t i = 0; i < FuncData::NUM_OVERLOADS; ++i) {
			GAFF_ASSERT(!func_data.func[i] || func_data.hash[i] != arg_hash);

			if (!func_data.func[i]) {
				ReflectionFunction<Ret, Args...>* const ref_func = SHIB_ALLOCT(
					GAFF_SINGLE_ARG(ReflectionFunction<Ret, Args...>),
					_allocator,
					ptr, false
				);

				func_data.func[i].reset(ref_func);
				func_data.hash[i] = arg_hash;
				func_data.offset[i] = static_cast<int32_t>(offset_ptr - offset_interface);
				found = true;
				break;
			}
		}

		GAFF_ASSERT_MSG(found, "Function overloading only supports 8 overloads per function name!");
	}

	if constexpr (sizeof...(Attrs) > 0) {
		auto& attrs = _func_attrs[FNV1aHash32Const(name)];
		attrs.set_allocator(_allocator);
		addAttributes(ptr, attrs, attributes...);
	}

	return *this;
}

template <class T, class Allocator>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::staticFunc(const char (&name)[size], Ret (*func)(Args...), const Attrs&... attributes)
{
	constexpr Hash64 arg_hash = CalcTemplateHash<Ret, Args...>(INIT_HASH64);
	auto it = Find(_static_funcs, FNV1aHash32Const(name));

	if (it == _static_funcs.end()) {
		it = _static_funcs.emplace(
			HashString32<Allocator>(name, size - 1, nullptr, _allocator),
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

	if constexpr (sizeof...(Attrs) > 0) {
		auto& attrs = _static_func_attrs[FNV1aHash32Const(name)];
		attrs.set_allocator(_allocator);
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
void ReflectionDefinition<T, Allocator>::finish(void)
{
	if (!_base_classes_remaining) {
		GAFF_REFLECTION_NAMESPACE::Reflection<T>::g_defined = true;

		// Call finish() on attributes first.
		for (IAttributePtr& attr : _class_attrs) {
			attr->finish(this);
		}

		for (auto& it : _var_attrs) {
			for (IAttributePtr& attr : it.second) {
				attr->finish(this);
			}
		}

		for (auto& it : _func_attrs) {
			for (IAttributePtr& attr : it.second) {
				attr->finish(this);
			}
		}

		for (auto& it : _static_func_attrs) {
			for (IAttributePtr& attr : it.second) {
				attr->finish(this);
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

	--ref_def._base_classes_remaining;
	GAFF_ASSERT(ref_def._base_classes_remaining >= 0);

	ref_def.base<Base>();
	ref_def.finish();
}


// Variables
template <class T, class Allocator>
template <class Var, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Gaff::IReflectionVar* ref_var, Var T::*var, Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_var, var);
	
	return addAttributes(ref_var, var, attrs, rest...);
}

template <class T, class Allocator>
template <class Var, class Ret, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Gaff::IReflectionVar* ref_var, Ret (T::*getter)(void) const, void (T::*setter)(Var), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_var, getter, setter);

	return addAttributes(ref_var, getter, setter, attrs, rest...);
}

template <class T, class Allocator>
template <class Var>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Gaff::IReflectionVar*, Var T::*, Vector<IAttributePtr, Allocator>&)
{
	return *this;
}

template <class T, class Allocator>
template <class Var, class Ret>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Gaff::IReflectionVar*, Ret (T::*)(void) const, void (T::*)(Var), Vector<IAttributePtr, Allocator>&)
{
	return *this;
}

// Functions
template <class T, class Allocator>
template <class Ret, class... Args, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Ret (T::*func)(Args...) const, Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(func);
	
	return addAttributes(func, attrs, rest...);
}

template <class T, class Allocator>
template <class Ret, class... Args, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Ret (T::*func)(Args...), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(func);

	return addAttributes(func, attrs, rest...);
}

template <class T, class Allocator>
template <class Ret, class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Ret (T::*)(Args...) const, Vector<IAttributePtr, Allocator>&)
{
	return *this;
}

template <class T, class Allocator>
template <class Ret, class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Ret (T::*)(Args...), Vector<IAttributePtr, Allocator>&)
{
	return *this;
}

// Static Functions
template <class T, class Allocator>
template <class Ret, class... Args, class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Ret (*func)(Args...), Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(func);

	return addAttributes(func, attrs, rest...);
}

template <class T, class Allocator>
template <class Ret, class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Ret (*)(Args...), Vector<IAttributePtr, Allocator>&)
{
	return *this;
}

// Non-apply() call version.
template <class T, class Allocator>
template <class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	attrs.emplace_back(IAttributePtr(first.clone()));
	return addAttributes(attrs, rest...);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addAttributes(Vector<IAttributePtr, Allocator>&)
{
	return *this;
}

template <class T, class Allocator>
ptrdiff_t ReflectionDefinition<T, Allocator>::getBasePointerOffset(Hash64 interface_name) const
{
	const auto it = Find(_base_class_offsets, interface_name);
	return (it != _base_class_offsets.end()) ? it->second : -1;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::instantiated(void* object) const
{
	for (const IAttributePtr& attr : _class_attrs) {
		const_cast<IAttributePtr&>(attr)->instantiated(this, object);
	}

	for (auto& it : _var_attrs) {
		for (const IAttributePtr& attr : it.second) {
			const_cast<IAttributePtr&>(attr)->instantiated(this, object);
		}
	}

	for (auto& it : _func_attrs) {
		for (const IAttributePtr& attr : it.second) {
			const_cast<IAttributePtr&>(attr)->instantiated(this, object);
		}
	}

	for (auto& it : _static_func_attrs) {
		for (const IAttributePtr& attr : it.second) {
			const_cast<IAttributePtr&>(attr)->instantiated(this, object);
		}
	}
}

NS_END

#ifdef _MSC_VER
	#pragma warning(pop)
#endif
