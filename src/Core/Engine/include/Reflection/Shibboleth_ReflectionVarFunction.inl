/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

// VarFunction
template <class T, class FunctionPair>
VarFunction<T, FunctionPair>::VarFunction(const FunctionPair& get_set_funcs):
	_data(get_set_funcs)
{
	GAFF_ASSERT(get_set_funcs.first);

	if (!get_set_funcs.second) {
		IReflectionVar::setNoSerialize(true);
		IReflectionVar::setReadOnly(true);
	}
}

template <class T, class FunctionPair>
const Reflection<typename VarFunction<T, FunctionPair>::ReflectionType>& VarFunction<T, FunctionPair>::GetReflection(void)
{
	return Reflection<ReflectionType>::GetInstance();
}

template <class T, class FunctionPair>
const IReflection& VarFunction<T, FunctionPair>::getReflection(void) const
{
	return GetReflection();
}

template <class T, class FunctionPair>
const void* VarFunction<T, FunctionPair>::getData(const void* object) const
{
	return const_cast<VarFunction<T, FunctionPair>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class FunctionPair>
void* VarFunction<T, FunctionPair>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);

	if constexpr (std::is_reference<GetVarType>::value) {
		const auto& value = (obj->*_data.get_set_funcs.first)();
		return const_cast<VarType*>(&value);

	} else {
		_data.cached_value = (obj->*_data.get_set_funcs.first)();
		return &_data.cached_value;
	}
}

template <class T, class FunctionPair>
void VarFunction<T, FunctionPair>::setData(void* object, const void* data)
{
	GAFF_ASSERT(_data.get_set_funcs.second);

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_data.get_set_funcs.second)(*reinterpret_cast<const VarType*>(data));
}

template <class T, class FunctionPair>
void VarFunction<T, FunctionPair>::setDataMove(void* object, void* data)
{
	GAFF_ASSERT(_data.get_set_funcs.second);

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_data.get_set_funcs.second)(std::move(*reinterpret_cast<const VarType*>(data)));
}

template <class T, class FunctionPair>
bool VarFunction<T, FunctionPair>::load(const Shibboleth::ISerializeReader& reader, void* object)
{
	VarType* const var = reinterpret_cast<VarType*>(object);
	return Reflection<ReflectionType>::GetInstance().load(reader, *var);
}

template <class T, class FunctionPair>
void VarFunction<T, FunctionPair>::save(Shibboleth::ISerializeWriter& writer, const void* object)
{
	const VarType* const var = reinterpret_cast<const VarType*>(object);
	Reflection<ReflectionType>::GetInstance().save(writer, *var);
}

template <class T, class FunctionPair>
bool VarFunction<T, FunctionPair>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	GAFF_ASSERT(_data.get_set_funcs.second);

	VarType value;

	if (!load(reader, &value)) {
		return false;
	}

	setData(&object, &value);
	return true;
}

template <class T, class FunctionPair>
void VarFunction<T, FunctionPair>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const VarType* const value = reinterpret_cast<const VarType*>(getData(&object));
	save(writer, value);
}

template <class T, class FunctionPair>
const void* VarFunction<T, FunctionPair>::getGetterFunctionPointer(void) const
{
	return &_data.get_set_funcs.first;
}

template <class T, class FunctionPair>
size_t VarFunction<T, FunctionPair>::getGetterFunctionPointerSize(void) const
{
	return sizeof(_data.get_set_funcs.first);
}

template <class T, class FunctionPair>
FunctionSignature VarFunction<T, FunctionPair>::getGetterSignature(void) const
{
	FunctionSignature signature;

	signature.return_value = GetFunctionArg<typename VarFuncTypeHelper<FunctionPair>::GetVariableType>();

	if constexpr (VarFuncTypeHelper<FunctionPair>::k_getter_is_const) {
		signature.flags.set(FunctionSignature::Flag::Const);
	}

	return signature;
}

template <class T, class FunctionPair>
const void* VarFunction<T, FunctionPair>::getSetterFunctionPointer(void) const
{
	return &_data.get_set_funcs.second;
}

template <class T, class FunctionPair>
size_t VarFunction<T, FunctionPair>::getSetterFunctionPointerSize(void) const
{
	return sizeof(_data.get_set_funcs.second);
}

template <class T, class FunctionPair>
FunctionSignature VarFunction<T, FunctionPair>::getSetterSignature(void) const
{
	FunctionSignature signature;

	GetFunctionArgs<typename VarFuncTypeHelper<FunctionPair>::SetVariableType>(signature.args);

	return signature;
}

NS_END
