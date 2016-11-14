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
}

template <class T, class Allocator>
template <class Var>
ReflectionValueType ReflectionDefinition<T, Allocator>::VarPtr<Var>::getType(void) const
{
	return GetRVT<Var>();
}

template <class T, class Allocator>
template <class Var>
const void* ReflectionDefinition<T, Allocator>::VarPtr<Var>::getData(const T& object) const
{
	return &(object.*_ptr);
}

template <class T, class Allocator>
template <class Var>
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::setData(T& object, const void* data)
{
	(object.*_ptr) = *reinterpret_cast<const Var*>(data);
}

template <class T, class Allocator>
template <class Var>
void ReflectionDefinition<T, Allocator>::VarPtr<Var>::setDataMove(T& object, void* data)
{
	(object.*_ptr) = std::move(*reinterpret_cast<Var*>(data));
}



// VarFuncPtr
template <class T, class Allocator>
template <class Ret, class Var>
ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::VarFuncPtr(Getter getter, Setter setter):
	_getter(getter), _setter(setter)
{
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
const void* ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::getData(const T& object) const
{
	GAFF_ASSERT(_getter);

	if (std::is_reference<Ret>::value) {
		const Ret& val = (object.*_getter)();
		_copy_ptr = &val;
		return _copy_ptr;
	} else {
		_copy = (object.*_getter)();
		return &_copy;
	}
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::setData(T& object, const void* data)
{
	GAFF_ASSERT(_setter);
	(object.*_setter)(*reinterpret_cast<const Var*>(data));
}

template <class T, class Allocator>
template <class Ret, class Var>
void ReflectionDefinition<T, Allocator>::VarFuncPtr<Ret, Var>::setDataMove(T& object, void* data)
{
	GAFF_ASSERT(_setter);
	(object.*_setter)(*reinterpret_cast<Var*>(data));
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
const void* ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::getData(const T& object) const
{
	return _base_var->getData(object);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setData(T& object, const void* data)
{
	_base_var->setData(object, data);
}

template <class T, class Allocator>
template <class Base>
void ReflectionDefinition<T, Allocator>::BaseVarPtr<Base>::setDataMove(T& object, void* data)
{
	_base_var->setData(object, data);
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
	if (class_hash == T::GetReflectionHash()) {
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
void ReflectionDefinition<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_base_class_offsets.set_allocator(allocator);
	_allocator = allocator;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::setReflectionInstance(const ISerializeInfo& reflection_instance)
{
	_reflection_instance = &reflection_instance;
}

template <class T, class Allocator>
const ISerializeInfo& ReflectionDefinition<T, Allocator>::getReflectionInstance(void) const
{
	GAFF_ASSERT(_reflection_instance);
	return *_reflection_instance;
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
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::baseClass(const char* name, ReflectionHash hash, ptrdiff_t offset)
{
	auto pair = std::move(eastl::make_pair(HashString32<Allocator>(name, hash, nullptr, _allocator), offset));

	GAFF_ASSERT(_base_class_offsets.find(pair.first) == _base_class_offsets.end());
	_base_class_offsets.insert(std::move(pair));

	_version.baseClass(name, hash, offset);
	return *this;
}

template <class T, class Allocator>
template <class Base>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::baseClass(void)
{
	baseClass(Base::GetReflectionName(), Base::GetReflectionHash(), Gaff::OffsetOfClass<T, Base>());

	// Add IVarPtr's from base class.
	if (GAFF_REFLECTION_NAMESPACE::Reflection<Base>::g_defined) {
		ReflectionDefinition<Base, Allocator>& base_ref_def = const_cast<ReflectionDefinition<Base, Allocator>&>(
			GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetReflectionDefinition()
			);

		for (int32_t i = 0; i < base_ref_def.getNumVariables(); ++i) {
			eastl::pair<HashString32<Allocator>, IVarPtr> pair(
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
template <class Var, size_t size>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char(&name)[size], Var T::*ptr)
{
	static_assert(!std::is_pointer<Var>::value, "Cannot reflect pointers.");

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, size, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(VarPtr<Var>, _allocator, ptr))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	_vars.insert(std::move(pair));
	_version.var(name, ptr);
	return *this;
}

template <class T, class Allocator>
template <class Ret, class Var, size_t size>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::var(const char(&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var))
{
	static_assert(!std::is_pointer<Ret>::value, "Cannot reflect pointers.");
	using PtrType = VarFuncPtr<Ret, Var>;

	eastl::pair<HashString32<Allocator>, IVarPtr> pair(
		HashString32<Allocator>(name, size, nullptr, _allocator),
		IVarPtr(GAFF_ALLOCT(PtrType, _allocator, getter, setter))
	);

	GAFF_ASSERT(_vars.find(pair.first) == _vars.end());

	_vars.insert(std::move(pair));
	_version.var(name, getter, setter);
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
		eastl::pair<HashString32<Allocator>, IVarPtr> pair(
			base_ref_def.getVariableName(i),
			IVarPtr(GAFF_ALLOCT(BaseVarPtr<Base>, ref_def._allocator, base_ref_def.getVar(i)))
		);

		GAFF_ASSERT(ref_def._vars.find(pair.first) == ref_def._vars.end());
		ref_def._vars.insert(std::move(pair));
	}

	ref_def.finish();
}

NS_END
