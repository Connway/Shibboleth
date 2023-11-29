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

template <class T, class... Args>
void ConstructFuncImpl(T* obj, Args&&... args)
{
	Gaff::Construct(obj, std::forward<Args>(args)...);
}

template <class T, class... Args>
T* FactoryFuncImpl(Gaff::IAllocator& allocator, Args&&... args)
{
	return SHIB_ALLOCT(T, allocator, std::forward<Args>(args)...);
}



// VarFuncPtrWithCache
template <class T>
template <class Ret, class Var>
ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::VarFuncPtrWithCache(GetterMemberFunc getter, SetterMemberFunc setter):
	_getter_member(getter), _setter_member(setter), _member_func(true)
{
	GAFF_ASSERT(getter);
}

template <class T>
template <class Ret, class Var>
ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::VarFuncPtrWithCache(GetterFunc getter, SetterFunc setter):
	_getter(getter), _setter(setter), _member_func(false)
{
	GAFF_ASSERT(getter);
}

template <class T>
template <class Ret, class Var>
const IReflection& ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::getReflection(void) const
{
	return Reflection<RetType>::GetInstance();
}

template <class T>
template <class Ret, class Var>
const void* ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::getData(const void* object) const
{
	return const_cast<ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>*>(this)->getData(const_cast<void*>(object));
}

template <class T>
template <class Ret, class Var>
void* ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::getData(void* object)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_getter);

	const T* const obj = reinterpret_cast<const T*>(object);
	_cache = callGetter(*obj);

	return &_cache;
}

template <class T>
template <class Ret, class Var>
void ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	callSetter(*obj, *reinterpret_cast<const VarType*>(data));
}

template <class T>
template <class Ret, class Var>
void ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	callSetter(*obj, std::move(*reinterpret_cast<VarType*>(data)));
}

template <class T>
template <class Ret, class Var>
bool ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	GAFF_ASSERT(_getter);
	GAFF_ASSERT(_setter);

	VarType var;

	if (!Reflection<RetType>::GetInstance().load(reader, var)) {
		return false;
	}

	callSetter(object, var);
	return true;
}

template <class T>
template <class Ret, class Var>
void ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	GAFF_ASSERT(_getter);
	Reflection<RetType>::GetInstance().save(writer, callGetter(object));
}

template <class T>
template <class Ret, class Var>
void ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::callSetter(T& object, Var value) const
{
	GAFF_ASSERT(_setter);
	return (_member_func) ? (object.*_setter_member)(value) : _setter(object, value);
}

template <class T>
template <class Ret, class Var>
Ret ReflectionDefinition<T>::VarFuncPtrWithCache<Ret, Var>::callGetter(const T& object) const
{
	GAFF_ASSERT(_getter);
	return (_member_func) ? (object.*_getter_member)() : _getter(object);
}



// VarFuncPtr
template <class T>
template <class Ret, class Var>
ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::VarFuncPtr(GetterMemberFunc getter, SetterMemberFunc setter):
	_getter_member(getter),
	_setter_member(setter),
	_member_func(true)
{
	GAFF_ASSERT(getter);
}

template <class T>
template <class Ret, class Var>
ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::VarFuncPtr(GetterFunc getter, SetterFunc setter) :
	_getter(getter),
	_setter(setter),
	_member_func(false)
{
	GAFF_ASSERT(getter);
}

template <class T>
template <class Ret, class Var>
const IReflection& ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::getReflection(void) const
{
	return Reflection<RetType>::GetInstance();
}

template <class T>
template <class Ret, class Var>
const void* ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::getData(const void* object) const
{
	return const_cast<ReflectionDefinition<T>::VarFuncPtr<Ret, Var>*>(this)->getData(const_cast<void*>(object));
}

template <class T>
template <class Ret, class Var>
void* ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::getData(void* object)
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

template <class T>
template <class Ret, class Var>
void ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	callSetter(*obj, *reinterpret_cast<const RetType*>(data));
}

template <class T>
template <class Ret, class Var>
void ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(_setter);
	GAFF_ASSERT(data);

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	callSetter(*obj, std::move(*reinterpret_cast<RetType*>(data)));
}

template <class T>
template <class Ret, class Var>
bool ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	GAFF_ASSERT(_getter);

	if constexpr (std::is_reference<Ret>::value) {
		RetType& val = const_cast<RetType&>(callGetter(object));
		return Reflection<RetType>::GetInstance().load(reader, val);

	} else {
		RetType* const val = const_cast<RetType*>(callGetter(object));
		return Reflection<RetType>::GetInstance().load(reader, *val);
	}
}

template <class T>
template <class Ret, class Var>
void ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	GAFF_ASSERT(_getter);

	if constexpr (std::is_reference<Ret>::value) {
		const RetType& val = callGetter(object);
		Reflection<RetType>::GetInstance().save(writer, val);

	} else {
		const RetType* const val = callGetter(object);
		Reflection<RetType>::GetInstance().save(writer, *val);
	}
}

template <class T>
template <class Ret, class Var>
void ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::callSetter(T& object, Var value) const
{
	GAFF_ASSERT(_setter);
	return (_member_func) ? (object.*_setter_member)(value) : _setter(object, value);
}

template <class T>
template <class Ret, class Var>
Ret ReflectionDefinition<T>::VarFuncPtr<Ret, Var>::callGetter(const T& object) const
{
	GAFF_ASSERT(_getter);
	return (_member_func) ? (object.*_getter_member)() : _getter(object);
}



// BaseVarPtr
template <class T>
template <class Base>
ReflectionDefinition<T>::BaseVarPtr<Base>::BaseVarPtr(IVar<Base>* base_var):
	_base_var(base_var)
{
}

template <class T>
template <class Base>
const IReflection& ReflectionDefinition<T>::BaseVarPtr<Base>::getReflectionKey(void) const
{
	return _base_var->getReflectionKey();
}

template <class T>
template <class Base>
const IReflection& ReflectionDefinition<T>::BaseVarPtr<Base>::getReflection(void) const
{
	return _base_var->getReflection();
}

template <class T>
template <class Base>
const void* ReflectionDefinition<T>::BaseVarPtr<Base>::getData(const void* object) const
{
	GAFF_ASSERT(object);
	const Base* const obj = reinterpret_cast<const T*>(object);
	return _base_var->getData(obj);
}

template <class T>
template <class Base>
void* ReflectionDefinition<T>::BaseVarPtr<Base>::getData(void* object)
{
	GAFF_ASSERT(object);
	Base* const obj = reinterpret_cast<T*>(object);
	return _base_var->getData(obj);
}

template <class T>
template <class Base>
void ReflectionDefinition<T>::BaseVarPtr<Base>::setData(void* object, const void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (IReflectionVar::isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->setData(obj, data);
}

template <class T>
template <class Base>
void ReflectionDefinition<T>::BaseVarPtr<Base>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (IReflectionVar::isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->setData(obj, data);
}

template <class T>
template <class Base>
bool ReflectionDefinition<T>::BaseVarPtr<Base>::isFixedArray(void) const
{
	return _base_var->isFixedArray();
}

template <class T>
template <class Base>
bool ReflectionDefinition<T>::BaseVarPtr<Base>::isVector(void) const
{
	return _base_var->isVector();
}

template <class T>
template <class Base>
bool ReflectionDefinition<T>::BaseVarPtr<Base>::isFlags(void) const
{
	return _base_var->isFlags();
}

template <class T>
template <class Base>
int32_t ReflectionDefinition<T>::BaseVarPtr<Base>::size(const void* object) const
{
	GAFF_ASSERT(object);
	const Base* const obj = reinterpret_cast<const T*>(object);
	return _base_var->size(obj);
}

template <class T>
template <class Base>
const void* ReflectionDefinition<T>::BaseVarPtr<Base>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	const Base* const obj = reinterpret_cast<const T*>(object);
	return _base_var->getElement(obj, index);
}

template <class T>
template <class Base>
void* ReflectionDefinition<T>::BaseVarPtr<Base>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	Base* const obj = reinterpret_cast<T*>(object);
	return _base_var->getElement(obj, index);
}

template <class T>
template <class Base>
void ReflectionDefinition<T>::BaseVarPtr<Base>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (IReflectionVar::isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->setElement(obj, index, data);
}

template <class T>
template <class Base>
void ReflectionDefinition<T>::BaseVarPtr<Base>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index < size(object));
	GAFF_ASSERT(object);
	GAFF_ASSERT(data);

	if (IReflectionVar::isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->setElementMove(obj, index, data);
}

template <class T>
template <class Base>
void ReflectionDefinition<T>::BaseVarPtr<Base>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a < size(object));
	GAFF_ASSERT(index_b < size(object));
	GAFF_ASSERT(object);

	if (IReflectionVar::isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->swap(obj, index_a, index_b);
}

template <class T>
template <class Base>
void ReflectionDefinition<T>::BaseVarPtr<Base>::resize(void* object, size_t new_size)
{
	GAFF_ASSERT(object);

	if (IReflectionVar::isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->resize(obj, new_size);
}

template <class T>
template <class Base>
void ReflectionDefinition<T>::BaseVarPtr<Base>::remove(void* object, int32_t index)
{
	GAFF_ASSERT(object);

	if (IReflectionVar::isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->remove(obj, index);
}

template <class T>
template <class Base>
void ReflectionDefinition<T>::BaseVarPtr<Base>::setFlagValue(void* object, int32_t flag_index, bool value)
{
	GAFF_ASSERT(object);

	if (IReflectionVar::isReadOnly() || _base_var->isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Base* const obj = reinterpret_cast<T*>(object);
	_base_var->setFlagValue(obj, flag_index, value);
}

template <class T>
template <class Base>
bool ReflectionDefinition<T>::BaseVarPtr<Base>::getFlagValue(const void* object, int32_t flag_index) const
{
	GAFF_ASSERT(object);

	const Base* const obj = reinterpret_cast<const T*>(object);
	return _base_var->getFlagValue(obj, flag_index);
}

template <class T>
template <class Base>
bool ReflectionDefinition<T>::BaseVarPtr<Base>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	return _base_var->load(reader, object);
}

template <class T>
template <class Base>
void ReflectionDefinition<T>::BaseVarPtr<Base>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	_base_var->save(writer, object);
}



// ReflectionDefinition
template <class T>
const char8_t* ReflectionDefinition<T>::getFriendlyName(void) const
{
	return _friendly_name.data();
}

template <class T>
bool ReflectionDefinition<T>::load(const Shibboleth::ISerializeReader& reader, void* object, bool refl_load) const
{
	return load(reader, *reinterpret_cast<T*>(object), refl_load);
}

template <class T>
void ReflectionDefinition<T>::save(Shibboleth::ISerializeWriter& writer, const void* object, bool refl_save) const
{
	save(writer, *reinterpret_cast<const T*>(object), refl_save);
}

template <class T>
bool ReflectionDefinition<T>::load(const Shibboleth::ISerializeReader& reader, T& object, bool refl_load) const
{
	if (_serialize_load && !refl_load) {
		return _serialize_load(reader, object);

	} else {
		for (auto& entry : _vars) {
			if (entry.second->canSerialize()) {
				const char8_t* const name = entry.first.getBuffer();

				if (!reader.exists(name)) {
					// Skip over optional variables.
					if (entry.second->isOptional()) {
						continue;

					} else {
						// $TODO: Log error.
						return false;
					}
				}

				Shibboleth::ScopeGuard scope = reader.enterElementGuard(name);
				entry.second->load(reader, object);
			}
		}
	}

	return true;
}

template <class T>
void ReflectionDefinition<T>::save(Shibboleth::ISerializeWriter& writer, const T& object, bool refl_save) const
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

		writer.writeUInt64(u8"version", getReflectionInstance().getVersion().getHash());

		for (auto& entry : _vars) {
			if (entry.second->canSerialize()) {
				writer.writeKey(entry.first.getBuffer());
				entry.second->save(writer, object);
			}
		}

		writer.endObject();
	}
}

template <class T>
Gaff::Hash64 ReflectionDefinition<T>::getInstanceHash(const void* object, Gaff::Hash64 init) const
{
	return getInstanceHash(*reinterpret_cast<const T*>(object), init);
}

template <class T>
Gaff::Hash64 ReflectionDefinition<T>::getInstanceHash(const T& object, Gaff::Hash64 init) const
{
	return (_instance_hash) ? _instance_hash(object, init) : Gaff::FNV1aHash64T(object, init);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::setInstanceHash(InstanceHashFunc hash_func)
{
	_instance_hash = hash_func;
	return *this;
}

template <class T>
const void* ReflectionDefinition<T>::getInterface(Gaff::Hash64 class_hash, const void* object) const
{
	if (class_hash == Reflection<T>::GetNameHash()) {
		return object;
	}

	auto it = _base_class_offsets.find(class_hash);

	if (it == _base_class_offsets.end()) {
		return nullptr;
	}

	return reinterpret_cast<const int8_t*>(object) + it->second;
}

template <class T>
void* ReflectionDefinition<T>::getInterface(Gaff::Hash64 class_hash, void* object) const
{
	return const_cast<void*>(getInterface(class_hash, const_cast<const void*>(object)));
}

template <class T>
bool ReflectionDefinition<T>::hasInterface(Gaff::Hash64 class_hash) const
{
	if (class_hash == Reflection<T>::GetNameHash()) {
		return true;
	}

	auto it = _base_class_offsets.find(class_hash);
	return it != _base_class_offsets.end();
}

template <class T>
void ReflectionDefinition<T>::setAllocator(const Shibboleth::ProxyAllocator& allocator)
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

template <class T>
Shibboleth::ProxyAllocator& ReflectionDefinition<T>::getAllocator(void)
{
	return _allocator;
}

template <class T>
const IReflection& ReflectionDefinition<T>::getReflectionInstance(void) const
{
	return Reflection<T>::GetInstance();
}

template <class T>
int32_t ReflectionDefinition<T>::size(void) const
{
	return sizeof(T);
}

template <class T>
bool ReflectionDefinition<T>::isPolymorphic(void) const
{
	return std::is_polymorphic<T>::value;
}

template <class T>
bool ReflectionDefinition<T>::isBuiltIn(void) const
{
	return false;
}

template <class T>
int32_t ReflectionDefinition<T>::getNumVars(void) const
{
	return _num_vars;
}

template <class T>
Shibboleth::HashStringView32<> ReflectionDefinition<T>::getVarName(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < _num_vars);

	int32_t index_begin = 0;

	for (const auto& entry : _vars) {
		const auto& sub_vars = entry.second->getSubVars();
		const int32_t index_end = index_begin + 1 + static_cast<int32_t>(sub_vars.size());

		if (Gaff::Between(index, index_begin, index_end - 1)) {
			if (index == index_begin) {
				return Shibboleth::HashStringView32<>(entry.first);
			} else {
				return sub_vars[index - index_begin + 1].first;
			}
		}
	}

	GAFF_ASSERT(false);
	return Shibboleth::HashStringView32<>();
}

template <class T>
IReflectionVar* ReflectionDefinition<T>::getVar(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	return getVarT(index);
}

template <class T>
IReflectionVar* ReflectionDefinition<T>::getVar(Gaff::Hash32 name) const
{
	return getVarT(name);
}

template <class T>
int32_t ReflectionDefinition<T>::getNumFuncs(void) const
{
	return static_cast<int32_t>(_funcs.size());
}

template <class T>
int32_t ReflectionDefinition<T>::getNumFuncOverrides(int32_t index) const
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

template <class T>
Shibboleth::HashStringView32<> ReflectionDefinition<T>::getFuncName(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_funcs.size()));
	return Shibboleth::HashStringView32<>((_funcs.begin() + index)->first);
}

template <class T>
int32_t ReflectionDefinition<T>::getFuncIndex(Gaff::Hash32 name) const
{
	const auto it = _funcs.find(name);
	return (it == _funcs.end()) ? -1 : static_cast<int32_t>(eastl::distance(_funcs.begin(), it));
}

template <class T>
int32_t ReflectionDefinition<T>::getNumStaticFuncs(void) const
{
	return static_cast<int32_t>(_static_funcs.size());
}

template <class T>
int32_t ReflectionDefinition<T>::getNumStaticFuncOverrides(int32_t index) const
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

template <class T>
Shibboleth::HashStringView32<> ReflectionDefinition<T>::getStaticFuncName(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_static_funcs.size()));
	return Shibboleth::HashStringView32<>((_static_funcs.begin() + index)->first);
}

template <class T>
int32_t ReflectionDefinition<T>::getStaticFuncIndex(Gaff::Hash32 name) const
{
	const auto it = _static_funcs.find(name);
	return (it == _static_funcs.end()) ? -1 : static_cast<int32_t>(eastl::distance(_static_funcs.begin(), it));
}

template <class T>
int32_t ReflectionDefinition<T>::getNumClassAttrs(void) const
{
	return static_cast<int32_t>(_class_attrs.size());
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getClassAttr(Gaff::Hash64 attr_name) const
{
	return getAttribute(_class_attrs, attr_name);
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getClassAttr(int32_t index) const
{
	GAFF_ASSERT(index < getNumClassAttrs());
	return _class_attrs[index].get();
}

template <class T>
bool ReflectionDefinition<T>::hasClassAttr(Gaff::Hash64 attr_name) const
{
	return getClassAttr(attr_name) != nullptr;
}

template <class T>
void ReflectionDefinition<T>::addClassAttr(IAttribute& attribute)
{
	_class_attrs.emplace_back(IAttributePtr(attribute.clone()));
}

template <class T>
int32_t ReflectionDefinition<T>::getNumVarAttrs(Gaff::Hash32 name) const
{
	const auto it = _var_attrs.find(name);
	return (it != _var_attrs.end()) ? static_cast<int32_t>(it->second.size()) : 0;
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getVarAttr(Gaff::Hash32 name, Gaff::Hash64 attr_name) const
{
	const auto it = _var_attrs.find(name);
	GAFF_ASSERT(it != _var_attrs.end());

	return getAttribute(it->second, attr_name);
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getVarAttr(Gaff::Hash32 name, int32_t index) const
{
	const auto it = _var_attrs.find(name);
	GAFF_ASSERT(it != _var_attrs.end());
	GAFF_ASSERT(index < static_cast<int32_t>(it->second.size()));
	return it->second[index].get();
}

template <class T>
bool ReflectionDefinition<T>::hasVarAttr(Gaff::Hash64 attr_name) const
{
	for (const auto& attrs : _var_attrs) {
		if (getAttribute(attrs.second, attr_name) != nullptr) {
			return true;
		}
	}

	return false;
}

template <class T>
int32_t ReflectionDefinition<T>::getNumFuncAttrs(Gaff::Hash64 name_arg_hash) const
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

template <class T>
const IAttribute* ReflectionDefinition<T>::getFuncAttr(Gaff::Hash64 name_arg_hash, Gaff::Hash64 attr_name) const
{
	const auto it = _func_attrs.find(name_arg_hash);
	GAFF_ASSERT(it != _func_attrs.end());

	return getAttribute(it->second, attr_name);
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getFuncAttr(Gaff::Hash64 name_arg_hash, int32_t index) const
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

template <class T>
bool ReflectionDefinition<T>::hasFuncAttr(Gaff::Hash64 attr_name) const
{
	for (const auto& attrs : _func_attrs) {
		if (getFuncAttr(attrs.first, attr_name) != nullptr) {
			return true;
		}
	}

	return false;
}

template <class T>
int32_t ReflectionDefinition<T>::getNumStaticFuncAttrs(Gaff::Hash64 name_arg_hash) const
{
	const auto it = _static_func_attrs.find(name_arg_hash);
	return (it != _static_func_attrs.end()) ? static_cast<int32_t>(it->second.size()) : 0;
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getStaticFuncAttr(Gaff::Hash64 name_arg_hash, Gaff::Hash64 attr_name) const
{
	const auto it = _static_func_attrs.find(name_arg_hash);
	GAFF_ASSERT(it != _static_func_attrs.end());

	return getAttribute(it->second, attr_name);
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getStaticFuncAttr(Gaff::Hash64 name_arg_hash, int32_t index) const
{
	const auto it = _static_func_attrs.find(name_arg_hash);
	GAFF_ASSERT(it != _static_func_attrs.end());
	GAFF_ASSERT(index < static_cast<int32_t>(it->second.size()));
	return it->second[index].get();
}

template <class T>
bool ReflectionDefinition<T>::hasStaticFuncAttr(Gaff::Hash64 attr_name) const
{
	for (const auto& attrs : _static_func_attrs) {
		if (getStaticFuncAttr(attrs.first, attr_name) != nullptr) {
			return true;
		}
	}

	return false;
}

template <class T>
int32_t ReflectionDefinition<T>::getNumConstructors(void) const
{
	return static_cast<int32_t>(_ctors.size());
}

template <class T>
IReflectionStaticFunctionBase* ReflectionDefinition<T>::getConstructor(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_ctors.size()));
	return ((_ctors.begin()) + index)->second.get();
}

template <class T>
IReflectionDefinition::VoidFunc ReflectionDefinition<T>::getConstructor(Gaff::Hash64 ctor_hash) const
{
	const auto it = _ctors.find(ctor_hash);
	return it == _ctors.end() ? nullptr : it->second->getFunc();
}

template <class T>
IReflectionDefinition::VoidFunc ReflectionDefinition<T>::getFactory(Gaff::Hash64 ctor_hash) const
{
	const auto it = _factories.find(ctor_hash);
	return it == _factories.end() ? nullptr : it->second;
}

template <class T>
IReflectionStaticFunctionBase* ReflectionDefinition<T>::getStaticFunc(int32_t name_index, int32_t override_index) const
{
	GAFF_ASSERT(name_index < static_cast<int32_t>(_static_funcs.size()));
	GAFF_ASSERT(override_index < StaticFuncData::k_num_overloads);

	return (_static_funcs.begin() + name_index)->second.func[override_index].get();
}

template <class T>
IReflectionStaticFunctionBase* ReflectionDefinition<T>::getStaticFunc(Gaff::Hash32 name, Gaff::Hash64 args) const
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

template <class T>
IReflectionFunctionBase* ReflectionDefinition<T>::getFunc(int32_t name_index, int32_t override_index) const
{
	GAFF_ASSERT(name_index < static_cast<int32_t>(_funcs.size()));
	GAFF_ASSERT(override_index < FuncData::k_num_overloads);

	return (_funcs.begin() + name_index)->second.func[override_index].get();
}

template <class T>
IReflectionFunctionBase* ReflectionDefinition<T>::getFunc(Gaff::Hash32 name, Gaff::Hash64 args) const
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

template <class T>
void* ReflectionDefinition<T>::duplicate(const void* object, Gaff::IAllocator& allocator) const
{
	Gaff::Hash64 copy_ctor_hash = Gaff::FNV1aHash64Const(u8"const ");
	copy_ctor_hash = Gaff::FNV1aHash64String(getReflectionInstance().getName(), copy_ctor_hash);
	copy_ctor_hash = Gaff::FNV1aHash64Const(u8"&", copy_ctor_hash);

	void* copy = create(
		copy_ctor_hash,
		allocator,
		object
	);

	if (!copy) {
		copy = create(allocator);

		const int32_t num_vars = getNumVars();

		for (int32_t i = 0; i < num_vars; ++i) {
			Refl::IReflectionVar* const var = getVar(i);

			if (var->isNoCopy()) {
				continue;
			}

			const void* const orig_data = var->getData(object);
			var->setData(copy, orig_data);
		}
	}

	return copy;
}

template <class T>
void ReflectionDefinition<T>::destroyInstance(void* data) const
{
	T* const instance = reinterpret_cast<T*>(data);
	Gaff::Deconstruct(instance);
}

template <class T>
IVar<T>* ReflectionDefinition<T>::getVarT(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_vars.size()));
	return (_vars.begin() + index)->second.get();
}

template <class T>
IVar<T>* ReflectionDefinition<T>::getVarT(Gaff::Hash32 name) const
{
	const auto it = _vars.find(name);
	return (it == _vars.end()) ? nullptr : it->second.get();
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::friendlyName(const char8_t* name)
{
	_friendly_name = name;
	return *this;
}

template <class T>
template <class Base>
ReflectionDefinition<T>& ReflectionDefinition<T>::base(const char8_t* name)
{
	static_assert(std::is_base_of<Base, T>::value, "Class is not a base class of T.");

	const ptrdiff_t offset = Gaff::OffsetOfClass<T, Base>();
	auto pair = eastl::make_pair(
		Shibboleth::HashString64<>(name, _allocator),
		offset
	);

	GAFF_ASSERT(_base_class_offsets.find(pair.first) == _base_class_offsets.end());
	_base_class_offsets.insert(std::move(pair));

	return *this;
}

template <class T>
template <class Base>
ReflectionDefinition<T>& ReflectionDefinition<T>::base(void)
{
	static_assert(std::is_base_of<Base, T>::value, "Class is not a base class of T.");

	// So that hasInterface() calls will properly report inheritance if the base class hasn't been defined yet.
	if (_base_class_offsets.find(Reflection<Base>::GetNameHash()) == _base_class_offsets.end()) {
		base<Base>(Reflection<Base>::GetName());
	}

	// Add vars, funcs, and static funcs and attrs from base class.
	if (Reflection<Base>::GetInstance().isDefined()) {
		const ReflectionDefinition<Base>& base_ref_def = Reflection<Base>::GetReflectionDefinition();

		// For calling base class functions.
		_base_classes.emplace(
			Reflection<Base>::GetNameHash(),
			&base_ref_def
		);

		// Base class vars
		for (auto& it : base_ref_def._vars) {
			GAFF_ASSERT(_vars.find(it.first) == _vars.end());

			eastl::pair<Shibboleth::HashString32<>, IVarPtr> pair(
				it.first,
				IVarPtr(SHIB_ALLOCT(BaseVarPtr<Base>, _allocator, it.second.get()))
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

				ReflectionBaseFunction* const ref_func = SHIB_ALLOCT(
					ReflectionBaseFunction,
					_allocator,
					it.second.func[i]->getBaseRefDef(),
					it.second.func[i].get()
				);

				func_data.hash[index] = it.second.hash[i];
				func_data.func[index].reset(ref_func);

				// Copy attributes.
				const Gaff::Hash64 attr_hash = Gaff::FNV1aHash64T(func_data.hash[i], Gaff::FNV1aHash64T(Gaff::FNV1aHash32T(it.first.getHash())));
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
				const Gaff::Hash64 attr_hash = Gaff::FNV1aHash64T(static_func_data.hash[i], Gaff::FNV1aHash64T(Gaff::FNV1aHash32T(it.first.getHash())));
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
		Reflection<Base>::GetInstance().registerOnDefinedCallback(std::move(cb));
	}

	return *this;
}

template <class T>
template <class... Args>
ReflectionDefinition<T>& ReflectionDefinition<T>::ctor(Gaff::Hash64 factory_hash)
{
	GAFF_ASSERT(!getFactory(factory_hash));

	ConstructFuncT<T, Args...> construct_func = ConstructFuncImpl<T, Args...>;
	FactoryFuncT<T, Args...> factory_func = FactoryFuncImpl<T, Args...>;

	using ConstructorFunction = ReflectionStaticFunction<void, T*, Args&&...>;

	_ctors[factory_hash].reset(SHIB_ALLOCT(ConstructorFunction, _allocator, construct_func));
	_factories.emplace(factory_hash, reinterpret_cast<VoidFunc>(factory_func));

	return *this;
}

template <class T>
template <class... Args>
ReflectionDefinition<T>& ReflectionDefinition<T>::ctor(void)
{
	constexpr Gaff::Hash64 hash = Gaff::CalcTemplateHash<Args...>(Gaff::k_init_hash64);
	return ctor<Args...>(hash);
}

template <class T>
template <class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::var(const char8_t (&name)[name_size], Var T::*ptr, const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	using RefVarType = VarTypeHelper<T, Var>::Type;

	eastl::pair<Shibboleth::HashString32<>, IVarPtr> pair(
		Shibboleth::HashString32<>(name, name_size - 1, _allocator),
		IVarPtr(SHIB_ALLOCT(RefVarType, _allocator, ptr))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	auto& attrs = _var_attrs[Gaff::FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*pair.second, ptr, attrs, attributes...);
	}

	_num_vars += 1 + static_cast<int32_t>(pair.second->getSubVars().size());
	pair.second->setSubVarBaseName(name);

	_vars.insert(std::move(pair));

	return *this;
}

template <class T>
template <class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::var(const char (&name)[name_size], Var T::*ptr, const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, ptr, attributes...);
}

template <class T>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::var(const char8_t (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes)
{
	//static_assert(std::is_reference<Ret>::value || std::is_pointer<Ret>::value, "Function version of var() only supports reference and pointer return types!");

	// $TODO: Use std::decay instead.
	using RetNoRef = typename std::remove_reference<Ret>::type;
	using RetNoPointer = typename std::remove_pointer<RetNoRef>::type;
	using RetNoConst = typename std::remove_const<RetNoPointer>::type;

	using VarNoRef = typename std::remove_reference<Var>::type;
	using VarNoPointer = typename std::remove_pointer<VarNoRef>::type;
	using VarNoConst = typename std::remove_const<VarNoPointer>::type;

	static_assert(Reflection<RetNoConst>::HasReflection, "Getter return type is not reflected!");
	static_assert(Reflection<VarNoConst>::HasReflection, "Setter arg type is not reflected!");
	static_assert(name_size > 0, "Name cannot be an empty string.");

	eastl::pair<Shibboleth::HashString32<>, IVarPtr> pair;

	if constexpr (std::is_reference<Ret>::value || std::is_pointer<Ret>::value) {
		using PtrType = VarFuncPtr<Ret, Var>;

		pair = eastl::pair<Shibboleth::HashString32<>, IVarPtr>(
			Shibboleth::HashString32<>(name, name_size - 1, _allocator),
			IVarPtr(SHIB_ALLOCT(PtrType, _allocator, getter, setter))
		);
	} else {
		using PtrType = VarFuncPtrWithCache<Ret, Var>;

		pair = eastl::pair<Shibboleth::HashString32<>, IVarPtr>(
			Shibboleth::HashString32<>(name, name_size - 1, _allocator),
			IVarPtr(SHIB_ALLOCT(PtrType, _allocator, getter, setter))
		);
	}

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	auto& attrs = _var_attrs[Gaff::FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*pair.second, getter, setter, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::var(const char (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, getter, setter, attributes...);
}

template <class T>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::var(const char8_t (&name)[name_size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes)
{
	//static_assert(std::is_reference<Ret>::value || std::is_pointer<Ret>::value, "Function version of var() only supports reference and pointer return types!");

	using RetNoRef = typename std::remove_reference<Ret>::type;
	using RetNoPointer = typename std::remove_pointer<RetNoRef>::type;
	using RetNoConst = typename std::remove_const<RetNoPointer>::type;

	using VarNoRef = typename std::remove_reference<Var>::type;
	using VarNoPointer = typename std::remove_pointer<VarNoRef>::type;
	using VarNoConst = typename std::remove_const<VarNoPointer>::type;

	static_assert(Reflection<RetNoConst>::HasReflection, "Getter return type is not reflected!");
	static_assert(Reflection<VarNoConst>::HasReflection, "Setter arg type is not reflected!");
	static_assert(name_size > 0, "Name cannot be an empty string.");

	eastl::pair<Shibboleth::HashString32<>, IVarPtr> pair;

	if constexpr (std::is_reference<Ret>::value || std::is_pointer<Ret>::value) {
		using PtrType = VarFuncPtr<Ret, Var>;

		pair = eastl::pair<Shibboleth::HashString32<>, IVarPtr>(
			Shibboleth::HashString32<>(name, name_size - 1, _allocator),
			IVarPtr(SHIB_ALLOCT(PtrType, _allocator, getter, setter))
		);
	} else {
		using PtrType = VarFuncPtrWithCache<Ret, Var>;

		pair = eastl::pair<Shibboleth::HashString32<>, IVarPtr>(
			Shibboleth::HashString32<>(name, name_size - 1, _allocator),
			IVarPtr(SHIB_ALLOCT(PtrType, _allocator, getter, setter))
		);
	}

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	auto& attrs = _var_attrs[Gaff::FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*pair.second, getter, setter, attrs, attributes...);
	}

	_vars.insert(std::move(pair));
	return *this;
}

template <class T>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::var(const char (&name)[name_size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, getter, setter, attributes...);
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::func(const char8_t (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
	auto it = _funcs.find(Gaff::FNV1aHash32Const(name));

	ReflectionFunction<true, Ret, Args...>* ref_func = nullptr;

	if (it == _funcs.end()) {
		ref_func = SHIB_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionFunction<true, Ret, Args...>),
			_allocator,
			ptr
		);

		eastl::pair<Shibboleth::HashString32<>, FuncData> pair(
			Shibboleth::HashString32<>(name, name_size - 1, _allocator),
			FuncData()
		);

		it = _funcs.insert(std::move(pair)).first;
		it->second.func[0].reset(ref_func);
		it->second.hash[0] = arg_hash;

	} else {
		FuncData& func_data = it->second;

		for (int32_t i = 0; i < FuncData::k_num_overloads; ++i) {
			GAFF_ASSERT(!func_data.func[i] || func_data.hash[i] != arg_hash);

			if (!func_data.func[i] || func_data.func[i]->isBase()) {
				ref_func = SHIB_ALLOCT(
					GAFF_SINGLE_ARG(ReflectionFunction<true, Ret, Args...>),
					_allocator,
					ptr
				);

				func_data.func[i].reset(ref_func);
				func_data.hash[i] = arg_hash;
				break;
			}
		}

		GAFF_ASSERT_MSG(ref_func, "Function overloading only supports 8 overloads per function name!");
	}

	const Gaff::Hash32 name_hash = Gaff::FNV1aHash32Const(name);
	const Gaff::Hash64 attr_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name_hash));

	auto& attrs = _func_attrs[attr_hash];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*ref_func, ptr, attrs, attributes...);
	}

	return *this;
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::func(const char (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::func(const char8_t (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
	auto it = _funcs.find(Gaff::FNV1aHash32Const(name));

	ReflectionFunction<false, Ret, Args...>* ref_func = nullptr;

	if (it == _funcs.end()) {
		ref_func = SHIB_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionFunction<false, Ret, Args...>),
			_allocator,
			ptr
		);

		it = _funcs.emplace(
			Shibboleth::HashString32<>(name, name_size - 1, _allocator),
			FuncData()
		).first;

		it->second.func[0].reset(ref_func);
		it->second.hash[0] = arg_hash;

	} else {
		FuncData& func_data = it->second;

		for (int32_t i = 0; i < FuncData::k_num_overloads; ++i) {
			GAFF_ASSERT(!func_data.func[i] || func_data.hash[i] != arg_hash);

			if (!func_data.func[i] || func_data.func[i]->isBase()) {
				ref_func = SHIB_ALLOCT(
					GAFF_SINGLE_ARG(ReflectionFunction<false, Ret, Args...>),
					_allocator,
					ptr
				);

				func_data.func[i].reset(ref_func);
				func_data.hash[i] = arg_hash;
				break;
			}
		}

		GAFF_ASSERT_MSG(ref_func, "Function overloading only supports 8 overloads per function name!");
	}

	const Gaff::Hash32 name_hash = Gaff::FNV1aHash32Const(name);
	const Gaff::Hash64 attr_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name_hash));

	auto& attrs = _func_attrs[attr_hash];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*ref_func, ptr, attrs, attributes...);
	}

	return *this;
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::func(const char (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::staticFunc(const char8_t (&name)[name_size], Ret (*func)(Args...), const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
	auto it = _static_funcs.find(Gaff::FNV1aHash32Const(name));

	using StaticFuncType = ReflectionStaticFunction<Ret, Args...>;
	StaticFuncType* ref_func = nullptr;

	if (it == _static_funcs.end()) {
		it = _static_funcs.emplace(
			Shibboleth::HashString32<>(name, name_size - 1, _allocator),
			StaticFuncData(_allocator)
		).first;

		ref_func = SHIB_ALLOCT(StaticFuncType, _allocator, func);
		it->second.func[0].reset(ref_func);
		it->second.hash[0] = arg_hash;

	} else {
		StaticFuncData& func_data = it->second;

		for (int32_t i = 0; i < FuncData::k_num_overloads; ++i) {
			// Replace an open slot or replace an already existing overload.
			if (func_data.func[i] && func_data.hash[i] != arg_hash) {
				continue;
			}

			ref_func = SHIB_ALLOCT(StaticFuncType, _allocator, func);
			func_data.func[i].reset(ref_func);
			func_data.hash[i] = arg_hash;
			break;
		}

		GAFF_ASSERT_MSG(ref_func, "Function overloading only supports 8 overloads per function name!");
	}

	const Gaff::Hash32 name_hash = Gaff::FNV1aHash32Const(name);
	const Gaff::Hash64 attr_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name_hash));

	auto& attrs = _static_func_attrs[attr_hash];
	attrs.set_allocator(_allocator);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*ref_func, func, attrs, attributes...);
	}

	return *this;
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::staticFunc(const char (&name)[name_size], Ret (*func)(Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return staticFunc(temp_name, func, attributes...);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opAdd(void)
{
	staticFunc(OP_ADD_NAME, Gaff::Add<T, Other>);
	return staticFunc(OP_ADD_NAME, Gaff::Add<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opSub(void)
{
	staticFunc(OP_SUB_NAME, Gaff::Sub<T, Other>);
	return staticFunc(OP_SUB_NAME, Gaff::Sub<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opMul(void)
{
	staticFunc(OP_MUL_NAME, Gaff::Mul<T, Other>);
	return staticFunc(OP_MUL_NAME, Gaff::Mul<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opDiv(void)
{
	staticFunc(OP_DIV_NAME, Gaff::Div<T, Other>);
	return staticFunc(OP_DIV_NAME, Gaff::Div<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opMod(void)
{
	staticFunc(OP_MOD_NAME, Gaff::Mod<T, Other>);
	return staticFunc(OP_MOD_NAME, Gaff::Mod<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitAnd(void)
{
	staticFunc(OP_BIT_AND_NAME, Gaff::BitAnd<T, Other>);
	return staticFunc(OP_BIT_AND_NAME, Gaff::BitAnd<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitOr(void)
{
	staticFunc(OP_BIT_OR_NAME, Gaff::BitOr<T, Other>);
	return staticFunc(OP_BIT_OR_NAME, Gaff::BitOr<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitXor(void)
{
	staticFunc(OP_BIT_XOR_NAME, Gaff::BitXor<T, Other>);
	return staticFunc(OP_BIT_XOR_NAME, Gaff::BitXor<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitShiftLeft(void)
{
	staticFunc(OP_BIT_SHIFT_LEFT_NAME, Gaff::BitShiftLeft<T, Other>);
	return staticFunc(OP_BIT_SHIFT_LEFT_NAME, Gaff::BitShiftLeft<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitShiftRight(void)
{
	staticFunc(OP_BIT_SHIFT_RIGHT_NAME, Gaff::BitShiftRight<T, Other>);
	return staticFunc(OP_BIT_SHIFT_RIGHT_NAME, Gaff::BitShiftRight<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opAnd(void)
{
	staticFunc(OP_LOGIC_AND_NAME, Gaff::LogicAnd<T, Other>);
	return staticFunc(OP_LOGIC_AND_NAME, Gaff::LogicAnd<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opOr(void)
{
	staticFunc(OP_LOGIC_OR_NAME, Gaff::LogicOr<T, Other>);
	return staticFunc(OP_LOGIC_OR_NAME, Gaff::LogicOr<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opEqual(void)
{
	staticFunc(OP_EQUAL_NAME, Gaff::Equal<T, Other>);
	return staticFunc(OP_EQUAL_NAME, Gaff::Equal<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opLessThan(void)
{
	staticFunc(OP_LESS_THAN_NAME, Gaff::LessThan<T, Other>);
	return staticFunc(OP_LESS_THAN_NAME, Gaff::LessThan<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opGreaterThan(void)
{
	staticFunc(OP_GREATER_THAN_NAME, Gaff::GreaterThan<T, Other>);
	return staticFunc(OP_GREATER_THAN_NAME, Gaff::GreaterThan<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opLessThanOrEqual(void)
{
	staticFunc(OP_LESS_THAN_OR_EQUAL_NAME, Gaff::LessThanOrEqual<T, Other>);
	return staticFunc(OP_LESS_THAN_OR_EQUAL_NAME, Gaff::LessThanOrEqual<Other, T>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opGreaterThanOrEqual(void)
{
	staticFunc(OP_GREATER_THAN_OR_EQUAL_NAME, Gaff::GreaterThanOrEqual<T, Other>);
	return staticFunc(OP_GREATER_THAN_OR_EQUAL_NAME, Gaff::GreaterThanOrEqual<Other, T>);
}

template <class T>
template <class... Args>
ReflectionDefinition<T>& ReflectionDefinition<T>::opCall(void)
{
	return staticFunc(OP_CALL_NAME, Gaff::Call<T, Args...>);
}

template <class T>
template <class Other>
ReflectionDefinition<T>& ReflectionDefinition<T>::opIndex(void)
{
	return staticFunc(OP_INDEX_NAME, Gaff::Index<T, Other>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opAdd(void)
{
	return staticFunc(OP_ADD_NAME, Gaff::Add<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opSub(void)
{
	return staticFunc(OP_SUB_NAME, Gaff::Sub<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opMul(void)
{
	return staticFunc(OP_MUL_NAME, Gaff::Mul<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opDiv(void)
{
	return staticFunc(OP_DIV_NAME, Gaff::Div<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opMod(void)
{
	return staticFunc(OP_MOD_NAME, Gaff::Mod<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitAnd(void)
{
	return staticFunc(OP_BIT_AND_NAME, Gaff::BitAnd<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitOr(void)
{
	return staticFunc(OP_BIT_OR_NAME, Gaff::BitOr<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitXor(void)
{
	return staticFunc(OP_BIT_XOR_NAME, Gaff::BitXor<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitNot(void)
{
	return staticFunc(OP_BIT_NOT_NAME, Gaff::BitNot<T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitShiftLeft(void)
{
	return staticFunc(OP_BIT_SHIFT_LEFT_NAME, Gaff::BitShiftLeft<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opBitShiftRight(void)
{
	return staticFunc(OP_BIT_SHIFT_RIGHT_NAME, Gaff::BitShiftRight<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opAnd(void)
{
	return staticFunc(OP_LOGIC_AND_NAME, Gaff::LogicAnd<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opOr(void)
{
	return staticFunc(OP_LOGIC_OR_NAME, Gaff::LogicOr<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opEqual(void)
{
	return staticFunc(OP_EQUAL_NAME, Gaff::Equal<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opLessThan(void)
{
	return staticFunc(OP_LESS_THAN_NAME, Gaff::LessThan<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opGreaterThan(void)
{
	return staticFunc(OP_GREATER_THAN_NAME, Gaff::GreaterThan<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opLessThanOrEqual(void)
{
	return staticFunc(OP_LESS_THAN_OR_EQUAL_NAME, Gaff::LessThanOrEqual<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opGreaterThanOrEqual(void)
{
	return staticFunc(OP_GREATER_THAN_OR_EQUAL_NAME, Gaff::GreaterThanOrEqual<T, T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opMinus(void)
{
	return staticFunc(OP_MINUS_NAME, Gaff::Minus<T>);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::opPlus(void)
{
	return staticFunc(OP_PLUS_NAME, Gaff::Plus<T>);
}

template <class T>
template <int32_t (*to_string_func)(const T&, char8_t*, int32_t)>
ReflectionDefinition<T>& ReflectionDefinition<T>::opToString()
{
	staticFunc(OP_TO_STRING_NAME, Gaff::ToStringHelper<T, to_string_func>);
	return staticFunc(OP_TO_STRING_NAME, to_string_func);
}

template <class T>
template <class... Attrs>
ReflectionDefinition<T>& ReflectionDefinition<T>::classAttrs(const Attrs&... attributes)
{
	return addAttributes(_class_attrs, attributes...);
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::version(uint32_t /*version*/)
{
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::serialize(LoadFunc serialize_load, SaveFunc serialize_save)
{
	_serialize_load = serialize_load;
	_serialize_save = serialize_save;
	return *this;
}

template <class T>
void ReflectionDefinition<T>::finish(void)
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

template <class T>
template <class Base>
void ReflectionDefinition<T>::RegisterBaseVariables(void)
{
	ReflectionDefinition<T>& ref_def = const_cast<ReflectionDefinition<T>&>(
		Reflection<T>::GetReflectionDefinition()
	);

	--ref_def._dependents_remaining;
	GAFF_ASSERT(ref_def._dependents_remaining >= 0);

	ref_def.base<Base>();

	if (ref_def._dependents_remaining == 0) {
		ref_def.finish();
	}
}



// Variables
template <class T>
template <class Var, class First, class... Rest>
ReflectionDefinition<T>& ReflectionDefinition<T>::addAttributes(IReflectionVar& ref_var, Var T::*var, Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_var, var);

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(ref_var, var, attrs, rest...);
	} else {
		return *this;
	}
}

template <class T>
template <class Var, class Ret, class First, class... Rest>
ReflectionDefinition<T>& ReflectionDefinition<T>::addAttributes(IReflectionVar& ref_var, Ret (T::*getter)(void) const, void (T::*setter)(Var), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_var, getter, setter);

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(ref_var, getter, setter, attrs, rest...);
	} else {
		return *this;
	}
}

// Functions
template <class T>
template <class Ret, class... Args, class First, class... Rest>
ReflectionDefinition<T>& ReflectionDefinition<T>::addAttributes(IReflectionFunction<Ret, Args...>& ref_func, Ret (T::*func)(Args...) const, Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_func, func);

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(ref_func, func, attrs, rest...);
	} else {
		return *this;
	}
}

template <class T>
template <class Ret, class... Args, class First, class... Rest>
ReflectionDefinition<T>& ReflectionDefinition<T>::addAttributes(IReflectionFunction<Ret, Args...>& ref_func, Ret (T::*func)(Args...), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_func, func);

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(ref_func, func, attrs, rest...);
	} else {
		return *this;
	}
}

// Static Functions
template <class T>
template <class Ret, class... Args, class First, class... Rest>
ReflectionDefinition<T>& ReflectionDefinition<T>::addAttributes(IReflectionStaticFunction<Ret, Args...>& ref_func, Ret (*func)(Args...), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_func, func);

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(ref_func, func, attrs, rest...);
	} else {
		return *this;
	}
}

// Non-apply() call version.
template <class T>
template <class First, class... Rest>
ReflectionDefinition<T>& ReflectionDefinition<T>::addAttributes(Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	attrs.emplace_back(IAttributePtr(first.clone()));

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(attrs, rest...);
	} else {
		return *this;
	}
}

template <class T>
ptrdiff_t ReflectionDefinition<T>::getBasePointerOffset(Gaff::Hash64 interface_name) const
{
	const auto it = _base_class_offsets.find(interface_name);
	return (it != _base_class_offsets.end()) ? it->second : -1;
}

template <class T>
void ReflectionDefinition<T>::instantiated(void* object) const
{
	for (const IAttributePtr& attr : _class_attrs) {
		attr->instantiated(object, *this);
	}

	for (auto& it : _var_attrs) {
		for (const IAttributePtr& attr : it.second) {
			attr->instantiated(object, *this);
		}
	}

	for (auto& it : _func_attrs) {
		for (const IAttributePtr& attr : it.second) {
			attr->instantiated(object, *this);
		}
	}

	for (auto& it : _static_func_attrs) {
		for (const IAttributePtr& attr : it.second) {
			attr->instantiated(object, *this);
		}
	}
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getAttribute(const AttributeList& attributes, Gaff::Hash64 attr_name) const
{
	for (const auto& attr : attributes) {
		if (attr->getReflectionDefinition().hasInterface(attr_name)) {
			return attr.get();
		}
	}

	return nullptr;
}

REF_DEF_BUILTIN(int8_t, Int8);
REF_DEF_BUILTIN(int16_t, Int16);
REF_DEF_BUILTIN(int32_t, Int32);
REF_DEF_BUILTIN(int64_t, Int64);
REF_DEF_BUILTIN(uint8_t, UInt8);
REF_DEF_BUILTIN(uint16_t, UInt16);
REF_DEF_BUILTIN(uint32_t, UInt32);
REF_DEF_BUILTIN(uint64_t, UInt64);
REF_DEF_BUILTIN(float, Float);
REF_DEF_BUILTIN(double, Double);
REF_DEF_BUILTIN(bool, Bool);

NS_END
