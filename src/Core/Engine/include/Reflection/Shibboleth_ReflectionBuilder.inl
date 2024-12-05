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



template <class T, class BaseType>
ReflectionBuilder<T, BaseType>::ReflectionBuilder(ReflectionData<T>& data, IReflectionDefinition& ref_def):
	_ref_def(ref_def),
	_data(data)
{
}

template <class T, class BaseType>
template <class Base>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::base(const char8_t* name)
{
	static_assert(std::is_base_of_v<Base, T>, "Class is not a base class of T.");

	if (!_can_apply) {
		return *this;
	}

	Shibboleth::HashString64<> name_hash_str{ name, REFLECTION_ALLOCATOR };
	GAFF_ASSERT(Gaff::Find(_data.base_classes, name_hash_str) == _data.base_classes.end());

	auto& class_data = _data.base_classes[name_hash_str.getHash()];
	class_data.offset = Gaff::OffsetOfClass<Base, T>();
	class_data.reflection = nullptr;

	return *this;
}

template <class T, class BaseType>
template <class Base>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::base(void)
{
	static_assert(Reflection<Base>::k_has_reflection || Hash::ClassHashable<Base>::k_is_hashable, "Base class has no reflection and is not hashable.");
	static_assert(std::is_base_of_v<Base, T>, "Class is not a base class of T.");

	if (!_can_apply) {
		return *this;
	}

	if constexpr (Reflection<Base>::k_has_reflection) {
		Shibboleth::HashString64<> name_hash_str{ Reflection<Base>::GetName(), REFLECTION_ALLOCATOR };
		GAFF_ASSERT(Gaff::Find(_data.base_classes, name_hash_str) == _data.base_classes.end());

		auto& class_data = _data.base_classes[name_hash_str.getHash()];
		class_data.reflection = &Reflection<Base>::GetInstance();
		class_data.offset = Gaff::OffsetOfClass<Base, T>();

		ReflectionBuilder<T, Base> builder{ _data, _ref_def };
		Reflection<Base>::template BuildReflection<T>(builder);

		return *this;

	} else {
		return base<Base>(Hash::ClassHashable<Base>::GetName().data.data());
	}
}

template <class T, class BaseType>
template <class... Args>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::ctor(Gaff::Hash64 factory_hash)
{
	if (!_can_apply) {
		return *this;
	}

	if constexpr (k_is_initial_type) {
		GAFF_ASSERT(_data.factories.find(factory_hash) == _data.factories.end());

		ConstructFuncT<T, Args...> construct_func = ConstructFuncImpl<T, Args...>;
		FactoryFuncT<T, Args...> factory_func = FactoryFuncImpl<T, Args...>;

		using ConstructorFunction = ReflectionStaticFunction<void, T*, Args&&...>;

		auto& factories = _data.factories[factory_hash];
		factories.ctor.reset(SHIB_ALLOCT(ConstructorFunction, REFLECTION_ALLOCATOR, construct_func));
		factories.factory = reinterpret_cast<VoidFunc>(factory_func);

	} else {
		GAFF_REF(factory_hash);
	}

	return *this;
}

template <class T, class BaseType>
template <class... Args>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::ctor(void)
{
	if (!_can_apply) {
		return *this;
	}

	constexpr Gaff::Hash64 hash = Gaff::CalcTemplateHash<Args...>(Gaff::k_init_hash64);
	return ctor<Args...>(hash);
}

template <class T, class BaseType>
template <class Type, class Var, size_t name_size, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::var(const char8_t (&name)[name_size], Var Type::* ptr, const Attrs&... attributes)
{
	static_assert(std::is_same_v<Type, T> || std::is_same_v<Type, BaseType>, "Type is not of type T or BaseType.");
	static_assert(name_size > 0, "Name cannot be an empty string.");

	using RefVarType = VarTypeHelper<T, Var>::Type;

	if (!_can_apply) {
		return *this;
	}

	Shibboleth::HashString32<> name_hash_str{ name, name_size - 1, REFLECTION_ALLOCATOR };
	GAFF_ASSERT(_data.vars.find(name_hash_str) == _data.vars.end());

	auto& var_data = _data.vars[std::move(name_hash_str)];
	var_data.var.reset(SHIB_ALLOCT(RefVarType, REFLECTION_ALLOCATOR, ptr));
	var_data.var->setName(Gaff::FNV1aHash32Const(name));

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*var_data.var, ptr, var_data.attrs, attributes...);
	}

	var_data.var->setSubVarBaseName(name);
	return *this;
}

template <class T, class BaseType>
template <class Type, class Var, size_t name_size, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::var(const char (&name)[name_size], Var Type::* ptr, const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, ptr, attributes...);
}

template <class T, class BaseType>
template <class Type, class Ret, class Var, size_t name_size, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::varFunc(const char8_t (&name)[name_size], Ret (Type::*getter)(void) const, void (Type::*setter)(Var), const Attrs&... attributes)
{
	static_assert(std::is_same_v<Type, T> || std::is_same_v<Type, BaseType>, "Type is not of type T or BaseType.");
	static_assert(name_size > 0, "Name cannot be an empty string.");

	using GetFunc = decltype(getter);
	using SetFunc = decltype(setter);
	using FuncStorage = GetterSetterFuncs<GetFunc, SetFunc>;
	using RefVarType = VarTypeHelper<T, FuncStorage>::Type;

	if (!_can_apply) {
		return *this;
	}

	Shibboleth::HashString32<> name_hash_str{ name, name_size - 1, REFLECTION_ALLOCATOR };
	GAFF_ASSERT(_data.vars.find(name_hash_str) == _data.vars.end());

	auto& var_data = _data.vars[std::move(name_hash_str)];
	var_data.var.reset(SHIB_ALLOCT(RefVarType, REFLECTION_ALLOCATOR, FuncStorage(getter, setter)));
	var_data.var->setName(Gaff::FNV1aHash32Const(name));

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*var_data.var, getter, setter, var_data.attrs, attributes...);
	}

	var_data.var->setSubVarBaseName(name);
	return *this;
}

template <class T, class BaseType>
template <class Type, class Ret, class Var, size_t name_size, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::varFunc(const char (&name)[name_size], Ret (Type::*getter)(void) const, void (Type::*setter)(Var), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return varFunc(temp_name, getter, setter, attributes...);
}

template <class T, class BaseType>
template <class Type, size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char8_t (&name)[name_size], Ret (Type::*ptr)(Args...) const, const Attrs&... attributes)
{
	static_assert(std::is_same_v<Type, T> || std::is_same_v<Type, BaseType>, "Type is not of type T or BaseType.");
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);

	if (!_can_apply) {
		return *this;
	}

	auto it = _data.funcs.find(Gaff::FNV1aHash32Const(name));

	if (it == _data.funcs.end()) {
		Shibboleth::HashString32<> name_hash_str{ name, name_size - 1, REFLECTION_ALLOCATOR };
		it = _data.funcs.insert(std::move(name_hash_str)).first;
	}

	auto& func_data = it->second;

	GAFF_ASSERT(func_data.overrides.find(arg_hash) == func_data.overrides.end());

	auto& override_data = func_data.overrides[arg_hash];
	auto* const ref_func_ptr = SHIB_ALLOCT(
		GAFF_SINGLE_ARG(ReflectionFunction<true, Ret, Args...>),
		REFLECTION_ALLOCATOR,
		ptr
	);

	override_data.func.reset(ref_func_ptr);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*ref_func_ptr, ptr, override_data.attrs, attributes...);
	}

	return *this;
}

template <class T, class BaseType>
template <class Type, size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char (&name)[name_size], Ret (Type::*ptr)(Args...) const, const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T, class BaseType>
template <class Type, size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char8_t (&name)[name_size], Ret (Type::*ptr)(Args...), const Attrs&... attributes)
{
	static_assert(std::is_same_v<Type, T> || std::is_same_v<Type, BaseType>, "Type is not of type T or BaseType.");
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);

	if (!_can_apply) {
		return *this;
	}

	auto it = _data.funcs.find(Gaff::FNV1aHash32Const(name));

	if (it == _data.funcs.end()) {
		Shibboleth::HashString32<> name_hash_str{ name, name_size - 1, REFLECTION_ALLOCATOR };
		it = _data.funcs.insert(std::move(name_hash_str)).first;
	}

	auto& func_data = it->second;

	GAFF_ASSERT(func_data.overrides.find(arg_hash) == func_data.overrides.end());

	auto& override_data = func_data.overrides[arg_hash];
	auto* const ref_func_ptr = SHIB_ALLOCT(
		GAFF_SINGLE_ARG(ReflectionFunction<false, Ret, Args...>),
		REFLECTION_ALLOCATOR,
		ptr
	);

	override_data.func.reset(ref_func_ptr);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*ref_func_ptr, ptr, override_data.attrs, attributes...);
	}

	return *this;
}

template <class T, class BaseType>
template <class Type, size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char (&name)[name_size], Ret (Type::*ptr)(Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T, class BaseType>
template <class Type, size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char8_t (&name)[name_size], Ret (*ptr)(const Type&, Args...), const Attrs&... attributes)
{
	static_assert(std::is_same_v<Type, T> || std::is_same_v<Type, BaseType>, "Type is not of type T or BaseType.");
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);

	if (!_can_apply) {
		return *this;
	}

	auto it = _data.funcs.find(Gaff::FNV1aHash32Const(name));

	if (it == _data.funcs.end()) {
		Shibboleth::HashString32<> name_hash_str{ name, name_size - 1, REFLECTION_ALLOCATOR };
		it = _data.funcs.insert(std::move(name_hash_str)).first;
	}

	auto& func_data = it->second;

	GAFF_ASSERT(func_data.overrides.find(arg_hash) == func_data.overrides.end());

	auto& override_data = func_data.overrides[arg_hash];
	override_data.func.reset(
		SHIB_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionExtensionFunction<true, Ret, Args...>),
			REFLECTION_ALLOCATOR,
			ptr
		)
	);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*override_data.func, ptr, override_data.attrs, attributes...);
	}

	return *this;
}

template <class T, class BaseType>
template <class Type, size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char (&name)[name_size], Ret (*ptr)(const Type&, Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T, class BaseType>
template <class Type, size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char8_t (&name)[name_size], Ret (*ptr)(Type&, Args...), const Attrs&... attributes)
{
	static_assert(std::is_same_v<Type, T> || std::is_same_v<Type, BaseType>, "Type is not of type T or BaseType.");
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);

	if (!_can_apply) {
		return *this;
	}

	auto it = _data.funcs.find(Gaff::FNV1aHash32Const(name));

	if (it == _data.funcs.end()) {
		Shibboleth::HashString32<> name_hash_str{ name, name_size - 1, REFLECTION_ALLOCATOR };
		it = _data.funcs.insert(std::move(name_hash_str)).first;
	}

	auto& func_data = it->second;

	GAFF_ASSERT(func_data.overrides.find(arg_hash) == func_data.overrides.end());

	auto& override_data = func_data.overrides[arg_hash];
	override_data.func.reset(
		SHIB_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionExtensionFunction<false, Ret, Args...>),
			REFLECTION_ALLOCATOR,
			ptr
		)
	);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*override_data.func, ptr, override_data.attrs, attributes...);
	}

	return *this;
}

template <class T, class BaseType>
template <class Type, size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char (&name)[name_size], Ret (*ptr)(Type&, Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T, class BaseType>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::staticFunc(const char8_t (&name)[name_size], Ret (*ptr)(Args...), const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);

	if (!_can_apply) {
		return *this;
	}

	auto it = _data.static_funcs.find(Gaff::FNV1aHash32Const(name));

	if (it == _data.static_funcs.end()) {
		Shibboleth::HashString32<> name_hash_str{ name, name_size - 1, REFLECTION_ALLOCATOR };
		it = _data.static_funcs.insert(std::move(name_hash_str)).first;
	}

	auto& func_data = it->second;

	GAFF_ASSERT(func_data.overrides.find(arg_hash) == func_data.overrides.end());

	auto& override_data = func_data.overrides[arg_hash];
	override_data.func.reset(
		SHIB_ALLOCT(
			GAFF_SINGLE_ARG(ReflectionStaticFunction<Ret, Args...>),
			REFLECTION_ALLOCATOR,
			ptr
		)
	);

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*override_data.func, ptr, override_data.attrs, attributes...);
	}

	return *this;
}

template <class T, class BaseType>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::staticFunc(const char (&name)[name_size], Ret (*ptr)(Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return staticFunc(temp_name, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opAdd(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(+, OP_ADD_NAME, operator+, Gaff::Add, T)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opSub(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(-, OP_SUB_NAME, operator-, Gaff::Sub, T)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opMul(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(*, OP_MUL_NAME, operator*, Gaff::Mul, T)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opDiv(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(/, OP_DIV_NAME, operator/, Gaff::Div, T)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opMod(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(%, OP_SUB_NAME, operator%, Gaff::Mod, T)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitAnd(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(&, OP_BIT_AND_NAME, operator&, Gaff::BitAnd, T)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitOr(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(|, OP_BIT_OR_NAME, operator|, Gaff::BitOr, T)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitXor(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(^, OP_BIT_XOR_NAME, operator^, Gaff::BitXor, T)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitShiftLeft(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(<<, OP_BIT_SHIFT_LEFT_NAME, operator<<, Gaff::BitShiftLeft, T)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitShiftRight(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(>>, OP_BIT_SHIFT_RIGHT_NAME, operator>>, Gaff::BitShiftRight, T)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opAnd(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(&&, OP_LOGIC_AND_NAME, operator&&, Gaff::LogicAnd, bool)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opOr(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(||, OP_LOGIC_OR_NAME, operator||, Gaff::LogicOr, bool)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opEqual(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(==, OP_EQUAL_NAME, operator==, Gaff::Equal, bool)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opNotEqual(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(!=, OP_NOT_EQUAL_NAME, operator!=, Gaff::NotEqual, bool)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opLessThan(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(<, OP_LESS_THAN_NAME, operator<, Gaff::LessThan, bool)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opGreaterThan(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(>, OP_GREATER_THAN_NAME, operator>, Gaff::GreaterThan, bool)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opLessThanOrEqual(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(<=, OP_LESS_THAN_OR_EQUAL_NAME, operator<=, Gaff::LessThanOrEqual, bool)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opGreaterThanOrEqual(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL_CONST(>=, OP_GREATER_THAN_OR_EQUAL_NAME, operator>=, Gaff::GreaterThanOrEqual, bool)
}

template <class T, class BaseType>
template <bool is_const, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opCall(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, Args&&... args) { lhs(std::forward<Args>(args)...); };
	static_assert(k_can_perform_op, "Cannot perform `T(Args...)`.");

	if (!_can_apply) {
		return *this;
	}

	if constexpr (is_const) {
		static constexpr bool k_is_method = requires(void) { static_cast<Ret (T::*)(Args...) const>(&T::operator()); };

		if constexpr (k_is_method) {
			const auto ptr = static_cast<Ret (T::*)(Args...) const>(&T::operator());
			return func(OP_CALL_NAME, ptr, attributes...);
		} else {
			return staticFunc(OP_CALL_NAME, Gaff::Call<T, Args...>, attributes...);
		}

	} else {
		static constexpr bool k_is_method = requires(void) { static_cast<Ret (T::*)(Args...)>(&T::operator()); };

		if constexpr (k_is_method) {
			const auto ptr = static_cast<Ret (T::*)(Args...)>(&T::operator());
			return func(OP_CALL_NAME, ptr, attributes...);
		} else {
			return staticFunc(OP_CALL_NAME, Gaff::Call<T, Args...>, attributes...);
		}
	}
}

template <class T, class BaseType>
template <class Ret, class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opIndex(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs[rhs]; };
	static_assert(k_can_perform_op, "Cannot perform `T[Other]`.");

	using NoRef = std::remove_reference_t<Ret>;
	using NoPtr = std::remove_pointer_t<NoRef>;

	if (!_can_apply) {
		return *this;
	}

	if constexpr (std::is_const_v<NoPtr>) {
		static constexpr bool k_is_method = requires(void) { static_cast<Ret (T::*)(Other) const>(&T::operator[]); };

		if constexpr (k_is_method) {
			const auto ptr = static_cast<Ret (T::*)(Other) const>(&T::operator[]);
			return func(OP_INDEX_NAME, ptr, attributes...);

		} else {
			return staticFunc(OP_INDEX_NAME, Gaff::Index<T, Other>, attributes...);
		}

	} else {
		static constexpr bool k_is_method = requires(void) { static_cast<Ret (T::*)(Other)>(&T::operator[]); };

		if constexpr (k_is_method) {
			const auto ptr = static_cast<Ret (T::*)(Other)>(&T::operator[]);
			return func(OP_INDEX_NAME, ptr, attributes...);

		} else {
			return staticFunc(OP_INDEX_NAME, Gaff::Index<T, Other>, attributes...);
		}

	}
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitNot(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_UNARY_OP_IMPL_CONST(~, ~value, OP_BIT_NOT_NAME, operator~, Gaff::BitNot, T)
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opNegate(const Attrs&... attributes)
{
	return opMinus(attributes...);
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opMinus(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_UNARY_OP_IMPL_CONST(-, -value, OP_MINUS_NAME, operator-, Gaff::Minus, T)
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opPlus(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_UNARY_OP_IMPL_CONST(+, +value, OP_PLUS_NAME, operator+, Gaff::Plus, T)
}

template <class T, class BaseType>
template <int32_t (*to_string_func)(const T&, char8_t*, int32_t), class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opToString(const Attrs&... attributes)
{
	return staticFunc(OP_TO_STRING_NAME, to_string_func, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opComparison(const Attrs&... attributes)
{
	return staticFunc(OP_COMP_NAME, Gaff::Comparison<T, Other>, attributes...);
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opPreIncrement(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_UNARY_OP_IMPL(++, ++value, OP_PRE_INC_NAME, operator++, Gaff::PreIncrement, T&, void)
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opPostIncrement(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_UNARY_OP_IMPL(++, value++, OP_POST_INC_NAME, operator++, Gaff::PostIncrement, T, int)
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opPreDecrement(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_UNARY_OP_IMPL(--, --value, OP_PRE_DEC_NAME, operator--, Gaff::PreDecrement, T&, void)
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opPostDecrement(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_UNARY_OP_IMPL(--, value--, OP_POST_DEC_NAME, operator--, Gaff::PostDecrement, T, int)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(=, OP_ASSIGN_NAME, operator=, Gaff::Assignment, T&)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opAddAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(+=, OP_ADD_ASSIGN_NAME, operator+=, Gaff::AddAssignment, T&)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opSubAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(-=, OP_SUB_ASSIGN_NAME, operator-=, Gaff::SubAssignment, T&)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opMulAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(*=, OP_MUL_ASSIGN_NAME, operator*=, Gaff::MulAssignment, T&)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opDivAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(/=, OP_DIV_ASSIGN_NAME, operator/=, Gaff::DivAssignment, T&)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opModAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(%=, OP_MOD_ASSIGN_NAME, operator%=, Gaff::ModAssignment, T&)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitAndAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(&=, OP_BIT_AND_ASSIGN_NAME, operator&=, Gaff::BitAndAssignment, T&)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitOrAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(|=, OP_BIT_OR_ASSIGN_NAME, operator|=, Gaff::BitOrAssignment, T&)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitXorAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(^=, OP_BIT_XOR_ASSIGN_NAME, operator^=, Gaff::BitXorAssignment, T&)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitShiftLeftAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(<<=, OP_BIT_SHIFT_LEFT_ASSIGN_NAME, operator<<=, Gaff::BitShiftLeftAssignment, T&)
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitShiftRightAssignment(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	SHIB_REFL_BINARY_OP_IMPL(>>=, OP_BIT_SHIFT_RIGHT_ASSIGN_NAME, operator>>=, Gaff::BitShiftRightAssignment, T&)
}


template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::classAttrs(const Attrs&... attributes)
{
	if (!_can_apply) {
		return *this;
	}

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(_data.class_attrs, attributes...);
	}

	return *this;
}

template <class T, class BaseType>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::version(uint32_t /*version*/)
{
	return *this;
}

template <class T, class BaseType>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::serialize(typename ReflectionData<T>::LoadFunc serialize_load, typename ReflectionData<T>::SaveFunc serialize_save)
{
	if constexpr (k_is_initial_type) {
		_data.serialize_load = serialize_load;
		_data.serialize_save = serialize_save;
	} else {
		GAFF_REF(serialize_load, serialize_save);
	}

	return *this;
}

template <class T, class BaseType>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::setInstanceHash(typename ReflectionData<T>::InstanceHashFunc hash_func)
{
	if constexpr (k_is_initial_type) {
		_data.instance_hash = hash_func;
	} else {
		GAFF_REF(hash_func);
	}

	return *this;
}

template <class T, class BaseType>
void ReflectionBuilder<T, BaseType>::finish(void)
{
	for (auto& attr : _data.class_attrs) {
		attr->finish(_ref_def);
	}

	for (auto& var_entry : _data.vars) {
		for (auto& attr : var_entry.second.attrs) {
			attr->finish(_ref_def);
		}
	}

	for (auto& func_entry : _data.funcs) {
		for (auto& override_entry : func_entry.second.overrides) {
			for (auto& attr : override_entry.second.attrs) {
				attr->finish(_ref_def);
			}
		}
	}

	for (auto& static_func_entry : _data.funcs) {
		for (auto& override_entry : static_func_entry.second.overrides) {
			for (auto& attr : override_entry.second.attrs) {
				attr->finish(_ref_def);
			}
		}
	}
}

template <class T, class BaseType>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::baseOnlyStart(void)
{
	_can_apply = k_is_initial_type;
	return *this;
}

template <class T, class BaseType>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::baseOnlyEnd(void)
{
	_can_apply = true;
	return *this;
}

// Variables
template <class T, class BaseType>
template <class Var, class First, class... Rest>
void ReflectionBuilder<T, BaseType>::addAttributes(IReflectionVar& ref_var, Var T::*var, Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	if constexpr (!k_is_initial_type) {
		if (!first.canInherit()) {
			if constexpr (sizeof...(Rest) > 0) {
				addAttributes(ref_var, var, attrs, rest...);
			}

			return;
		}
	}

	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_var, var);

	if constexpr (sizeof...(Rest) > 0) {
		addAttributes(ref_var, var, attrs, rest...);
	}
}

template <class T, class BaseType>
template <class Var, class Ret, class First, class... Rest>
void ReflectionBuilder<T, BaseType>::addAttributes(IReflectionVar& ref_var, Ret (T::*getter)(void) const, void (T::*setter)(Var), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	if constexpr (!k_is_initial_type) {
		if (!first.canInherit()) {
			if constexpr (sizeof...(Rest) > 0) {
				addAttributes(ref_var, getter, setter, attrs, rest...);
			}

			return;
		}
	}

	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_var, getter, setter);

	if constexpr (sizeof...(Rest) > 0) {
		addAttributes(ref_var, getter, setter, attrs, rest...);
	}
}

// Functions
template <class T, class BaseType>
template <class Ret, class... Args, class First, class... Rest>
void ReflectionBuilder<T, BaseType>::addAttributes(IReflectionFunction<Ret, Args...>& ref_func, Ret (T::*ptr)(Args...) const, Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	if constexpr (!k_is_initial_type) {
		if (!first.canInherit()) {
			if constexpr (sizeof...(Rest) > 0) {
				addAttributes(ref_func, ptr, attrs, rest...);
			}

			return;
		}
	}

	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_func, ptr);

	if constexpr (sizeof...(Rest) > 0) {
		addAttributes(ref_func, ptr, attrs, rest...);
	}
}

template <class T, class BaseType>
template <class Ret, class... Args, class First, class... Rest>
void ReflectionBuilder<T, BaseType>::addAttributes(IReflectionFunction<Ret, Args...>& ref_func, Ret (T::*ptr)(Args...), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
		if constexpr (!k_is_initial_type) {
		if (!first.canInherit()) {
			if constexpr (sizeof...(Rest) > 0) {
				addAttributes(ref_func, ptr, attrs, rest...);
			}

			return;
		}
	}

	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_func, ptr);

	if constexpr (sizeof...(Rest) > 0) {
		addAttributes(ref_func, ptr, attrs, rest...);
	}
}

// Static Functions
template <class T, class BaseType>
template <class Ret, class... Args, class First, class... Rest>
void ReflectionBuilder<T, BaseType>::addAttributes(IReflectionStaticFunction<Ret, Args...>& ref_func, Ret (*ptr)(Args...), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	if constexpr (!k_is_initial_type) {
		if (!first.canInherit()) {
			if constexpr (sizeof...(Rest) > 0) {
				addAttributes(ref_func, ptr, attrs, rest...);
			}

			return;
		}
	}

	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_func, ptr);

	if constexpr (sizeof...(Rest) > 0) {
		addAttributes(ref_func, ptr, attrs, rest...);
	}
}

// Class
template <class T, class BaseType>
template <class First, class... Rest>
void ReflectionBuilder<T, BaseType>::addAttributes(Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	if constexpr (!k_is_initial_type) {
		if (!first.canInherit()) {
			if constexpr (sizeof...(Rest) > 0) {
				addAttributes(attrs, rest...);
			}

			return;
		}
	}

	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(_ref_def);

	if constexpr (sizeof...(Rest) > 0) {
		addAttributes(attrs, rest...);
	}
}

NS_END
