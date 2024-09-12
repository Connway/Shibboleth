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

#include "Shibboleth_ReflectionVar.h"
NS_REFLECTION

template <class GetterFunc, class SetterFunc>
using GetterSetterFuncs = eastl::pair<GetterFunc, SetterFunc>;

template <class GetSetFuncs>
struct VarFuncTypeHelper;

template <class T, class VarType>
class VarFunction;

template <class T, class FunctionPair, bool returns_reference>
struct VarFunctionData;


template <class T, class GetType, class SetType>
struct VarFuncTypeHelper< GetterSetterFuncs<GetType (T::*)(void) const, void (T::*)(SetType)> > final
{
	using GetVariableType = GetType;
	using SetVariableType = SetType;
	static constexpr bool k_getter_is_const = true;
};

template <class T, class GetType, class SetType>
struct VarFuncTypeHelper< GetterSetterFuncs<GetType (T::*)(void), void (T::*)(SetType)> > final
{
	using GetVariableType = GetType;
	using SetVariableType = SetType;
	static constexpr bool k_getter_is_const = false;
};

template <class T, class GetterFunc, class SetterFunc>
struct VarTypeHelper< T, GetterSetterFuncs<GetterFunc, SetterFunc> > final
{
	using GetVariableType = VarFuncTypeHelper< GetterSetterFuncs<GetterFunc, SetterFunc> >::GetVariableType;
	using SetVariableType = VarFuncTypeHelper< GetterSetterFuncs<GetterFunc, SetterFunc> >::SetVariableType;

	using ReflectionType = VarTypeHelper< T, std::decay_t<GetVariableType> >::ReflectionType;
	using VariableType = VarTypeHelper< T, std::decay_t<GetVariableType> >::VariableType;

	using Type = VarFunction< T, GetterSetterFuncs<GetterFunc, SetterFunc> >;

	static constexpr bool k_can_copy = VarTypeHelper< T, std::decay_t<GetVariableType> >::k_can_copy;
	static constexpr bool k_can_move = VarTypeHelper< T, std::decay_t<GetVariableType> >::k_can_move;
};



template <class T, class FunctionPair>
struct VarFunctionData<T, FunctionPair, true> final
{
	VarFunctionData<T, FunctionPair, true>(const FunctionPair& funcs): get_set_funcs(funcs) {}

	FunctionPair get_set_funcs{ nullptr, nullptr };
};

template <class T, class FunctionPair>
struct VarFunctionData<T, FunctionPair, false> final
{
	VarFunctionData<T, FunctionPair, false>(const FunctionPair& funcs): get_set_funcs(funcs) {}

	FunctionPair get_set_funcs{ nullptr, nullptr };
	VarTypeHelper<T, FunctionPair>::VariableType cached_value;
};



// VarFunction has a limitation in that it doesn't support containers. Might be worth supporting,
// but at this time, I don't see a great use-case for it.
template <class T, class FunctionPair>
class VarFunction : public IVar<T>
{
public:
	using GetVarType = VarTypeHelper<T, FunctionPair>::GetVariableType;
	using SetVarType = VarTypeHelper<T, FunctionPair>::SetVariableType;

	using ReflectionType = VarTypeHelper<T, FunctionPair>::ReflectionType;
	using VarType = VarTypeHelper<T, FunctionPair>::VariableType;

	static_assert(std::is_same_v< std::decay_t<GetVarType>, std::decay_t<SetVarType> >, "VarFunction getter and setter base type is not the same.");
	static_assert(!std::is_pointer_v<GetVarType>, "VarFunction does not support getters that take pointers.");
	static_assert(!std::is_pointer_v<SetVarType>, "VarFunction does not support setters that take pointers.");
	static_assert(Reflection<ReflectionType>::HasReflection);

	explicit VarFunction(const FunctionPair& get_set_funcs);
	VarFunction(void) = default;

	static const Reflection<ReflectionType>& GetReflection(void);
	const IReflection& getReflection(void) const override;

	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool load(const Shibboleth::ISerializeReader& reader, void* object) override;
	void save(Shibboleth::ISerializeWriter& writer, const void* object) override;

	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	const void* getGetterFunctionPointer(void) const override;
	size_t getGetterFunctionPointerSize(void) const override;
	FunctionSignature getGetterSignature(void) const override;

	const void* getSetterFunctionPointer(void) const override;
	size_t getSetterFunctionPointerSize(void) const override;
	FunctionSignature getSetterSignature(void) const override;

private:
	using FuncData = VarFunctionData< T, FunctionPair, std::is_reference_v<GetVarType> >;
	FuncData _data;
};

NS_END
