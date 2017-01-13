/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
	const T* obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class Allocator>
template <class Var>
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::setData(void* object, const void* data)
{
	T* obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = *reinterpret_cast<const Var*>(data);
}

template <class T, class Allocator>
template <class Var>
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::setDataMove(void* object, void* data)
{
	T* obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = std::move(*reinterpret_cast<Var*>(data));
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
	T* obj = reinterpret_cast<T*>(object);
	(obj->*_setter)(*reinterpret_cast<const Var*>(data));
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);
	T* obj = reinterpret_cast<T*>(object);
	(obj->*_setter)(*reinterpret_cast<Var*>(data));
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



// VectorPtr
template <class T, class Allocator>
template <class Var, class Vec_Allocator>
ReflectionDefinition<T, Allocator>::VectorPtr<Var, Vec_Allocator>::VectorPtr(Vector<Var, Vec_Allocator> (T::*ptr)) :
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



// ReflectionDefinition
template <class T, class Allocator>
template <class... Args>
T* ReflectionDefinition<T, Allocator>::create(Args&&... args) const
{
	return createAllocT<T>(_allocator, std::forward<Args>(args)...);
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::load(ISerializeReader& reader, void* object) const
{
	load(reader, *reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::save(ISerializeWriter& writer, const void* object) const
{
	save(writer, *reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::load(ISerializeReader& /*reader*/, T& /*object*/) const
{
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::save(ISerializeWriter& /*writer*/, const T& /*object*/) const
{
}

template <class T, class Allocator>
const void* ReflectionDefinition<T, Allocator>::getInterface(Hash64 class_hash, const void* object) const
{
	if (class_hash == GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetHash()) {
		return object;
	}

	auto it = Gaff::Find(_base_class_offsets, class_hash);

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

	auto it = Gaff::Find(_base_class_offsets, class_hash);
	return it != _base_class_offsets.end();
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_base_class_offsets.set_allocator(allocator);
	_ctors.set_allocator(allocator);
	_vars.set_allocator(allocator);

	_base_class_attrs.set_allocator(allocator);
	_var_attrs.set_allocator(allocator);
	_class_attrs.set_allocator(allocator);

	_allocator = allocator;
}

template <class T, class Allocator>
const IReflection& ReflectionDefinition<T, Allocator>::getReflectionInstance(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetInstance();
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumVariables(void) const
{
	return static_cast<int32_t>(_vars.size());
}

template <class T, class Allocator>
Hash32 ReflectionDefinition<T, Allocator>::getVariableHash(int32_t index) const
{
	GAFF_ASSERT(index < _vars.size());
	return getVariableName(index).getHash();
}

template <class T, class Allocator>
IReflectionVar* ReflectionDefinition<T, Allocator>::getVariable(int32_t index) const
{
	GAFF_ASSERT(index < _vars.size());
	return getVar(index);
}

template <class T, class Allocator>
IReflectionVar* ReflectionDefinition<T, Allocator>::getVariable(Hash32 name) const
{
	GAFF_ASSERT(Find(_vars, name) != _vars.end());
	return getVar(name);
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumClassAttributes(void) const
{
	return static_cast<int32_t>(_class_attrs.size());
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getClassAttribute(int32_t index) const
{
	GAFF_ASSERT(index < getNumClassAttributes());
	return _class_attrs[index].get();
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumVarAttributes(Hash32 name) const
{
	auto it = _var_attrs.find(name);
	GAFF_ASSERT(it != _var_attrs.end());
	return static_cast<int32_t>(it->second.size());
}

template <class T, class Allocator>
const IAttribute* ReflectionDefinition<T, Allocator>::getVarAttribute(Hash32 name, int32_t index) const
{
	auto it = _var_attrs.find(name);
	GAFF_ASSERT(it != _var_attrs.end());
	GAFF_ASSERT(index < getNumVarAttributes(name));
	return it->second[index].get();
}

template <class T, class Allocator>
IReflectionDefinition::VoidFunc ReflectionDefinition<T, Allocator>::getFactory(Hash64 ctor_hash) const
{
	auto it = _ctors.find(ctor_hash);
	return it == _ctors.end() ? 0 : it->second;
}

template <class T, class Allocator>
const HashString32<Allocator>& ReflectionDefinition<T, Allocator>::getVariableName(int32_t index) const
{
	GAFF_ASSERT(index < _vars.size());
	return (_vars.begin() + index)->first;
}

template <class T, class Allocator>
typename ReflectionDefinition<T, Allocator>::IVar* ReflectionDefinition<T, Allocator>::getVar(int32_t index) const
{
	GAFF_ASSERT(index < _vars.size());
	return (_vars.begin() + index)->second.get();
}

template <class T, class Allocator>
typename ReflectionDefinition<T, Allocator>::IVar* ReflectionDefinition<T, Allocator>::getVar(Hash32 name) const
{
	auto it = Find(_vars, name);
	GAFF_ASSERT(it != _vars.end());
	return it->second.get();
}

template <class T, class Allocator>
template <class Base>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::base(const char* name)
{
	const ptrdiff_t offset = Gaff::OffsetOfClass<T, Base>();
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

		for (int32_t i = 0; i < base_ref_def.getNumVariables(); ++i) {
			eastl::pair<HashString32<Allocator>, IVarPtr> pair(
				base_ref_def.getVariableName(i),
				IVarPtr(GAFF_ALLOCT(BaseVarPtr<Base>, _allocator, base_ref_def.getVar(i)))
			);

			GAFF_ASSERT(_vars.find(pair.first) == _vars.end());
			_vars.insert(std::move(pair));
		}

		// add base class var attrs

		// add base class funcs

		// add base class func attrs

	// Register for callback if base class hasn't been defined yet.
	} else {
		++_base_classes_remaining;
		GAFF_REFLECTION_NAMESPACE::Reflection<Base>::g_on_defined_callbacks.emplace_back(&RegisterBaseVariables<Base>);
	}

	return *this;
}

template <class T, class Allocator>
template <class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::ctor(void)
{
	Hash64 hash = CalcTemplateHash<Args...>(INIT_HASH64);
	GAFF_ASSERT(!getFactory(hash));

	void* (*factory_func)(IAllocator&, Args&&...) = Gaff::FactoryFunc<T, Args...>;
	_ctors.emplace(hash, reinterpret_cast<VoidFunc>(factory_func));

	return *this;
}

template <class T, class Allocator>
template <class Var, size_t size>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[size], Var T::*ptr)
{
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, size - 1, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(VarPtr<Var>, _allocator, ptr))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class Ret, class Var, size_t size>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var))
{
	static_assert(!std::is_pointer<Ret>::value, "Cannot reflect pointers.");
	using PtrType = VarFuncPtr<Ret, Var>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, size - 1, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, getter, setter))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator, size_t size>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[size], Vector<Var, Vec_Allocator> T::*vec)
{
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");
	using PtrType = VectorPtr<Var, Vec_Allocator>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, size - 1, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, vec))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class Var, size_t array_size, size_t name_size>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[name_size], Var (T::*arr)[array_size])
{
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");
	using PtrType = ArrayPtr<Var, array_size>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, name_size - 1, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, arr))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	_vars.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <size_t size, class Ret, class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::func(const char (&name)[size], Ret (T::*ptr)(Args...) const)
{
	GAFF_REF(name); GAFF_REF(ptr);
	//using PtrType = FuncPtr<Ret, Args...>;

	//eastl::pair<HashString32<Allocator>, IVarPtr> pair(
	//	HashString32<Allocator>(name, size - 1, nullptr, _allocator),
	//	IVarPtr(GAFF_ALLOCT(PtrType, _allocator, arr))
	//);

	//GAFF_ASSERT(_funcs.find(pair.first) == _funcs.end());

	//_funcs.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <size_t size, class Ret, class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::func(const char (&name)[size], Ret (T::*ptr)(Args...))
{
	GAFF_REF(name); GAFF_REF(ptr);
	//using PtrType = FuncPtr<Ret, Args...>;

	//eastl::pair<HashString32<Allocator>, IVarPtr> pair(
	//	HashString32<Allocator>(name, size - 1, nullptr, _allocator),
	//	IVarPtr(GAFF_ALLOCT(PtrType, _allocator, arr))
	//);

	//GAFF_ASSERT(_funcs.find(pair.first) == _funcs.end());

	//_funcs.insert(std::move(pair));
	return *this;
}

template <class T, class Allocator>
template <class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::classAttrs(const Args&... args)
{
	return addClassAttributes(args...);
}

template <class T, class Allocator>
template <size_t size, class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::varAttrs(const char (&name)[size], const Args&... args)
{
	auto& attrs = _var_attrs[FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);
	return addVarAttributes(attrs, args...);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::attrFile(const char* file)
{
	_attr_file = file;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::finish(void)
{
	if (!_base_classes_remaining) {
		GAFF_REFLECTION_NAMESPACE::Reflection<T>::g_defined = true;

		for (auto func : GAFF_REFLECTION_NAMESPACE::Reflection<T>::g_on_defined_callbacks) {
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

template <class T, class Allocator>
template <class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addVarAttributes(Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	attrs.emplace_back(IAttributePtr(GAFF_ALLOCT(First, _allocator, first)));
	return addVarAttributes(attrs, rest...);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addVarAttributes(Vector<IAttributePtr, Allocator>&)
{
	return *this;
}

template <class T, class Allocator>
template <class First, class... Rest>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addClassAttributes(const First& first, const Rest&... rest)
{
	_class_attrs.emplace_back(IAttributePtr(GAFF_ALLOCT(First, _allocator, first)));
	return addClassAttributes(rest...);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::addClassAttributes(void)
{
	return *this;
}

NS_END
