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
const void* ReflectionDefinition<T, Allocator>::getInterface(ReflectionHash class_hash, const void* object) const
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
void* ReflectionDefinition<T, Allocator>::getInterface(ReflectionHash class_hash, void* object) const
{
	return const_cast<void*>(getInterface(class_hash, const_cast<const void*>(object)));
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::hasInterface(ReflectionHash class_hash) const
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
	_allocator = allocator;
}

template <class T, class Allocator>
const ISerializeInfo& ReflectionDefinition<T, Allocator>::getReflectionInstance(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<T>::GetInstance();
}

template <class T, class Allocator>
Hash64 ReflectionDefinition<T, Allocator>::getVersionHash(void) const
{
	return _version.getHash();
}

template <class T, class Allocator>
int32_t ReflectionDefinition<T, Allocator>::getNumVariables(void) const
{
	return static_cast<int32_t>(_vars.size());
}

template <class T, class Allocator>
ReflectionHash ReflectionDefinition<T, Allocator>::getVariableHash(int32_t index) const
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
IReflectionVar* ReflectionDefinition<T, Allocator>::getVariable(ReflectionHash name) const
{
	GAFF_ASSERT(Find(_vars, name) != _vars.end());
	return getVar(name);
}

template <class T, class Allocator>
const ReflectionHashString<Allocator>& ReflectionDefinition<T, Allocator>::getVariableName(int32_t index) const
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
typename ReflectionDefinition<T, Allocator>::IVar* ReflectionDefinition<T, Allocator>::getVar(ReflectionHash name) const
{
	auto it = Find(_vars, name);
	GAFF_ASSERT(it != _vars.end());
	return it->second.get();
}

template <class T, class Allocator>
template <class Base>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::base(const char* name, ReflectionHash hash)
{
	const ptrdiff_t offset = Gaff::OffsetOfClass<T, Base>();
	auto pair = std::move(
		eastl::make_pair(
			ReflectionHashString<Allocator>(name, hash, nullptr, _allocator),
			offset
		)
	);

	GAFF_ASSERT(_base_class_offsets.find(pair.first) == _base_class_offsets.end());
	_base_class_offsets.insert(std::move(pair));

	_version.base<Base>(name, hash);
	return *this;
}

template <class T, class Allocator>
template <class Base>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::base(void)
{
	base<Base>(Base::GetReflectionName(), Base::GetReflectionHash());

	// Add IVarPtr's from base class.
	if (GAFF_REFLECTION_NAMESPACE::Reflection<Base>::g_defined) {
		ReflectionDefinition<Base, Allocator>& base_ref_def = const_cast<ReflectionDefinition<Base, Allocator>&>(
			GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetReflectionDefinition()
		);

		for (int32_t i = 0; i < base_ref_def.getNumVariables(); ++i) {
			eastl::pair<ReflectionHashString<Allocator>, IVarPtr> pair(
				base_ref_def.getVariableName(i),
				IVarPtr(GAFF_ALLOCT(BaseVarPtr<Base>, _allocator, base_ref_def.getVar(i)))
			);

			GAFF_ASSERT(_vars.find(pair.first) == _vars.end());
			_vars.insert(std::move(pair));
		}

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
	_version.ctor<T(Args...)>();
	return *this;
}

template <class T, class Allocator>
template <class Var, size_t size>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[size], Var T::*ptr)
{
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");

	eastl::pair<ReflectionHashString<Allocator>, IVarPtr> pair(
		ReflectionHashString<Allocator>(name, size, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(VarPtr<Var>, _allocator, ptr))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	_vars.insert(std::move(pair));
	_version.var(name, ptr);
	return *this;
}

template <class T, class Allocator>
template <class Ret, class Var, size_t size>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var))
{
	static_assert(!std::is_pointer<Ret>::value, "Cannot reflect pointers.");
	using PtrType = VarFuncPtr<Ret, Var>;

	eastl::pair<ReflectionHashString<Allocator>, IVarPtr> pair(
		ReflectionHashString<Allocator>(name, size, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, getter, setter))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	_vars.insert(std::move(pair));
	_version.var(name, getter, setter);
	return *this;
}

template <class T, class Allocator>
template <class Var, class Vec_Allocator, size_t size>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[size], Vector<Var, Vec_Allocator> T::*vec)
{
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");
	using PtrType = VectorPtr<Var, Vec_Allocator>;

	eastl::pair<ReflectionHashString<Allocator>, IVarPtr> pair(
		ReflectionHashString<Allocator>(name, size, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, vec))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	_vars.insert(std::move(pair));
	_version.var(name, vec);
	return *this;
}

template <class T, class Allocator>
template <class Var, size_t array_size, size_t name_size>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char (&name)[name_size], Var (T::*arr)[array_size])
{
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");
	using PtrType = ArrayPtr<Var, array_size>;

	eastl::pair<ReflectionHashString<Allocator>, IVarPtr> pair(
		ReflectionHashString<Allocator>(name, name_size, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, arr))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	_vars.insert(std::move(pair));
	_version.var(name, arr);
	return *this;
}

template <class T, class Allocator>
template <size_t size, class Ret, class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::func(const char (&name)[size], Ret (T::*ptr)(Args...) const)
{
	//using PtrType = FuncPtr<Ret, Args...>;

	//eastl::pair<ReflectionHashString<Allocator>, IVarPtr> pair(
	//	ReflectionHashString<Allocator>(name, name_size, nullptr, _allocator),
	//	IVarPtr(GAFF_ALLOCT(PtrType, _allocator, arr))
	//);

	//GAFF_ASSERT(_funcs.find(pair.first) == _funcs.end());

	//_funcs.insert(std::move(pair));
	_version.func(name, ptr);
	return *this;
}

template <class T, class Allocator>
template <size_t size, class Ret, class... Args>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::func(const char (&name)[size], Ret (T::*ptr)(Args...))
{
	//using PtrType = FuncPtr<Ret, Args...>;

	//eastl::pair<ReflectionHashString<Allocator>, IVarPtr> pair(
	//	ReflectionHashString<Allocator>(name, name_size, nullptr, _allocator),
	//	IVarPtr(GAFF_ALLOCT(PtrType, _allocator, arr))
	//);

	//GAFF_ASSERT(_funcs.find(pair.first) == _funcs.end());

	//_funcs.insert(std::move(pair));
	_version.func(name, ptr);
	return *this;
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

	ReflectionDefinition<Base, Allocator>& base_ref_def = const_cast<ReflectionDefinition<Base, Allocator>&>(
		GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetReflectionDefinition()
	);

	GAFF_ASSERT(GAFF_REFLECTION_NAMESPACE::Reflection<Base>::g_defined);

	for (int32_t i = 0; i < base_ref_def.getNumVariables(); ++i) {
		eastl::pair<ReflectionHashString<Allocator>, IVarPtr> pair(
			base_ref_def.getVariableName(i),
			IVarPtr(GAFF_ALLOCT(BaseVarPtr<Base>, ref_def._allocator, base_ref_def.getVar(i)))
		);

		GAFF_ASSERT(ref_def._vars.find(pair.first) == ref_def._vars.end());
		ref_def._vars.insert(std::move(pair));
	}

	ref_def.finish();
}

NS_END