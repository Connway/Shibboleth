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
ReflectionBuilder<T, BaseType>::ReflectionBuilder(ReflectionData<T>& data):
	_data(data)
{
}

template <class T, class BaseType>
template <class Base>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::base(const char8_t* name)
{
	static_assert(std::is_base_of_v<Base, T>, "Class is not a base class of T.");

	Shibboleth::HashString64<> name_hash_str{ name, REFLECTION_ALLOCATOR };
	GAFF_ASSERT(_data.base_classes.find(pair.first) == _data.base_classes.end());

	auto& class_data = _data.base_classes[std::move(name_hash_str)]
	class_data.offset = Gaff::OffsetOfClass<Base, T>();
	class_data.reflection = nullptr;

	return *this;
}

template <class T, class BaseType>
template <class Base>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::base(void)
{
	static_assert(Reflection<Base>::HasReflection || Hash::ClassHashable<Base>::k_is_hashable, "Base class has no reflection and is not hashable.");
	static_assert(std::is_base_of_v<Base, T>, "Class is not a base class of T.");

	if constexpr (Reflection<Base>::HasReflection) {
		Shibboleth::HashString64<> name_hash_str{ name, REFLECTION_ALLOCATOR };
		GAFF_ASSERT(_data.base_classes.find(pair.first) == _data.base_classes.end());

		auto& class_data = _data.base_classes[std::move(name_hash_str)]
		class_data.reflection = &Reflection<Base>::GetInstance();
		class_data.offset = Gaff::OffsetOfClass<Base, T>();

		ReflectionBuilder<T, Base> builder{ _data };
		Reflection<Base>::BuildReflection<ReflectionBuilder<T, Base>, T>(builder);

	} else {
		return base<Base>(Hash::ClassHashable<Base>::GetName().data.data());
	}

	return *this;
}

template <class T, class BaseType>
template <class... Args>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::ctor(Gaff::Hash64 factory_hash)
{
	if constexpr (k_is_initial_type) {
		GAFF_ASSERT(_data.factories.find(factory_hash) == _data.factories.end());

		IReflectionDefinition::ConstructFuncT<T, Args...> construct_func = ConstructFuncImpl<T, Args...>;
		IReflectionDefinition::FactoryFuncT<T, Args...> factory_func = FactoryFuncImpl<T, Args...>;

		using ConstructorFunction = ReflectionStaticFunction<void, T*, Args&&...>;

		_data.ctors[factory_hash].reset(SHIB_ALLOCT(ConstructorFunction, REFLECTION_ALLOCATOR, construct_func));
		_data.factories.emplace(factory_hash, reinterpret_cast<VoidFunc>(factory_func));

	} else {
		GAFF_REF(factory_hash);
	}

	return *this;
}

template <class T, class BaseType>
template <class... Args>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::ctor(void)
{
	constexpr Gaff::Hash64 hash = Gaff::CalcTemplateHash<Args...>(Gaff::k_init_hash64);
	return ctor<Args...>(hash);
}

template <class T, class BaseType>
template <class Var, size_t name_size, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::var(const char8_t (&name)[name_size], Var T::* ptr, const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	using RefVarType = VarTypeHelper<T, Var>::Type;

	Shibboleth::HashString32<> name_hash_str{ name, name_size - 1, REFLECTION_ALLOCATOR };
	GAFF_ASSERT(_data.vars.find(name_hash_str) == _vars.end());

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
template <class Var, size_t name_size, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::var(const char (&name)[name_size], Var T::* ptr, const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, ptr, attributes...);
}

template <class T, class BaseType>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::var(const char8_t (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	using GetFunc = decltype(getter);
	using SetFunc = decltype(setter);
	using FuncStorage = GetterSetterFuncs<GetFunc, SetFunc>;
	using RefVarType = VarTypeHelper<T, FuncStorage>::Type;

	Shibboleth::HashString32<> name_hash_str{ name, name_size - 1, REFLECTION_ALLOCATOR };
	GAFF_ASSERT(_data.vars.find(name_hash_str) == _vars.end());

	auto& var_data = _data.vars[std::move(name_hash_str)];
	var_data.var.reset(SHIB_ALLOCT(RefVarType, REFLECTION_ALLOCATOR, FuncStorage(getter, setter)));
	var_data.var->setName(Gaff::FNV1aHash32Const(name));

	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(*var_data.var, ptr, var_data.attrs, attributes...);
	}

	var_data.var->setSubVarBaseName(name);
	return *this;
}

template <class T, class BaseType>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::var(const char (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, getter, setter, attributes...);
}

template <class T, class BaseType>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char8_t (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);

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
			GAFF_SINGLE_ARG(ReflectionFunction<true, Ret, Args...>),
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
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T, class BaseType>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char8_t (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);

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
			GAFF_SINGLE_ARG(ReflectionFunction<false, Ret, Args...>),
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
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T, class BaseType>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char8_t (&name)[name_size], Ret (*ptr)(const T&, Args...), const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);

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
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char (&name)[name_size], Ret (*ptr)(const T&, Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T, class BaseType>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char8_t (&name)[name_size], Ret (*ptr)(T&, Args...), const Attrs&... attributes)
{
	static_assert(name_size > 0, "Name cannot be an empty string.");

	constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);

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
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::func(const char (&name)[name_size], Ret (*ptr)(T&, Args...), const Attrs&... attributes)
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

	auto it = _data.staticFuncs.find(Gaff::FNV1aHash32Const(name));

	if (it == _data.staticFuncs.end()) {
		Shibboleth::HashString32<> name_hash_str{ name, name_size - 1, REFLECTION_ALLOCATOR };
		it = _data.staticFuncs.insert(std::move(name_hash_str)).first;
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
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs + rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T + Other`.");

	const auto ptr = static_cast<T (T::*)(const Other&) const>(&T::operator+);

	return func(OP_ADD_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opSub(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs - rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T - Other`.");

	const auto ptr = static_cast<T (T::*)(const Other&) const>(&T::operator-);

	return func(OP_SUB_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opMul(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs * rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T * Other`.");

	const auto ptr = static_cast<T (T::*)(const Other&) const>(&T::operator*);

	return func(OP_MUL_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opDiv(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs / rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T / Other`.");

	const auto ptr = static_cast<T (T::*)(const Other&) const>(&T::operator/);

	return func(OP_DIV_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opMod(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs % rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T % Other`.");

	const auto ptr = static_cast<T (T::*)(const Other&) const>(&T::operator%);

	return func(OP_MOD_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitAnd(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs & rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T & Other`.");

	const auto ptr = static_cast<T (T::*)(const Other&) const>(&T::operator&);

	return func(OP_BIT_AND_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitOr(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs | rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T | Other`.");

	const auto ptr = static_cast<T (T::*)(const Other&) const>(&T::operator|);

	return func(OP_BIT_OR_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitXor(const Attrs&... attributes);
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs ^ rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T ^ Other`.");

	const auto ptr = static_cast<T (T::*)(const Other&) const>(&T::operator^);

	return func(OP_BIT_XOR_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitShiftLeft(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs << rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T << Other`.");

	const auto ptr = static_cast<T (T::*)(const Other&) const>(&T::operator<<);

	return func(OP_BIT_SHIFT_LEFT_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitShiftRight(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs >> rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T >> Other`.");

	const auto ptr = static_cast<T (T::*)(const Other&) const>(&T::operator>>);

	return func(OP_BIT_SHIFT_RIGHT_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opAnd(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs && rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T && Other`.");

	const auto ptr = static_cast<bool (T::*)(const Other&) const>(&T::operator&&);

	return func(OP_AND_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opOr(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs || rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T || Other`.");

	const auto ptr = static_cast<bool (T::*)(const Other&) const>(&T::operator||);

	return func(OP_OR_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opEqual(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs == rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T == Other`.");

	const auto ptr = static_cast<bool (T::*)(const Other&) const>(&T::operator==);

	return func(OP_EQUAL_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opLessThan(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs < rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T < Other`.");

	const auto ptr = static_cast<bool (T::*)(const Other&) const>(&T::operator<);

	return func(OP_LESS_THAN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opGreaterThan(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs > rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T > Other`.");

	const auto ptr = static_cast<bool (T::*)(const Other&) const>(&T::operator>);

	return func(OP_GREATER_THAN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opLessThanOrEqual(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs <= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T <= Other`.");

	const auto ptr = static_cast<bool (T::*)(const Other&) const>(&T::operator<=);

	return func(OP_LESS_THAN_OR_EQUAL_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opGreaterThanOrEqual(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs >= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T >= Other`.");

	const auto ptr = static_cast<bool (T::*)(const Other&) const>(&T::operator>=);

	return func(OP_GREATER_THAN_OR_EQUAL_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <bool is_const, class Ret, class... Args, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opCall(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, Args&&... args) { lhs(std::forward<Args>(args)...); };
	static_assert(k_can_perform_op, "Cannot perform `T(Args...)`.");

	if constexpr (is_const) {
		const auto ptr = static_cast<Ret (T::*)(Args...) const>(&T::operator());
		return func(OP_CALL_NAME, ptr, attributes...);
	} else {
		const auto ptr = static_cast<Ret (T::*)(Args...)>(&T::operator());
		return func(OP_CALL_NAME, ptr, attributes...);
	}
}

template <class T, class BaseType>
template <class Ret, class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opIndex(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs[rhs]; };
	static_assert(k_can_perform_op, "Cannot perform `T[Other]`.");

	using NoRef = std::remove_reference_t<Ret>;
	using NoPtr = std::remove_pointer_t<Ret>;

	if constexpr (std::is_const_v<NoPtr>) {
		const auto ptr = static_cast<Ret (T::*)(const Other&) const>(&T::operator[]);
		return func(OP_INDEX_NAME, ptr, attributes...);
	} else {
		const auto ptr = static_cast<Ret (T::*)(const Other&)>(&T::operator[]);
		return func(OP_INDEX_NAME, ptr, attributes...);
	}
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitNot(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs) { ~lhs; };
	static_assert(k_can_perform_op, "Cannot perform `~T`.");

	const auto ptr = static_cast<T (T::*)(void) const>(&T::operator~);

	return func(OP_BIT_NOT_NAME, ptr, attributes...);
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
	static constexpr bool k_can_perform_op = requires(T lhs) { -lhs; };
	static_assert(k_can_perform_op, "Cannot perform `-T`.");

	const auto ptr = static_cast<T (T::*)(void) const>(&T::operator-);

	return func(OP_MINUS_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opPlus(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs) { +lhs; };
	static_assert(k_can_perform_op, "Cannot perform `+T`.");

	const auto ptr = static_cast<T (T::*)(void) const>(&T::operator+);

	return func(OP_PLUS_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <int32_t (*to_string_func)(const T&, char8_t*, int32_t), class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opToString(const Attrs&... attributes)
{
	return staticFunc(OP_TO_STRING_NAME, to_string_func);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opComparison(const Attrs&... attributes)
{
	return staticFunc(OP_COMP_NAME, Gaff::Comparison<T, Other>);
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opPreIncrement(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs) { ++lhs; };
	static_assert(k_can_perform_op, "Cannot perform `++T`.");

	const auto ptr = static_cast<T& (T::*)(void)>(&T::operator++);

	return func(OP_PRE_INC_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opPostIncrement(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs) { lhs++; };
	static_assert(k_can_perform_op, "Cannot perform `T++`.");

	const auto ptr = static_cast<T (T::*)(int)>(&T::operator++);

	return func(OP_POST_INC_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opPreDecrement(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs) { --lhs; };
	static_assert(k_can_perform_op, "Cannot perform `--T`.");

	const auto ptr = static_cast<T& (T::*)(void)>(&T::operator--);

	return func(OP_PRE_DEC_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opPostDecrement(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs) { lhs--; };
	static_assert(k_can_perform_op, "Cannot perform `T--`.");

	const auto ptr = static_cast<T (T::*)(int)>(&T::operator--);

	return func(OP_POST_DEC_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs = rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T = Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator=);

	return func(OP_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opAddAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs += rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T += Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator+=);

	return func(OP_ADD_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opSubAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs -= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T -= Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator-=);

	return func(OP_SUB_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opMulAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs *= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T *= Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator*=);

	return func(OP_MUL_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opDivAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs /= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T /= Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator/=);

	return func(OP_DIV_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opModAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs %= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T %= Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator%=);

	return func(OP_MOD_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitAndAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs &= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T &= Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator&=);

	return func(OP_BIT_AND_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitOrAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs |= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T |= Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator|=);

	return func(OP_BIT_OR_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitXorAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs ^= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T ^= Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator^=);

	return func(OP_BIT_XOR_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitShiftLeftAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs <<= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T <<= Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator<<=);

	return func(OP_BIT_SHIFT_LEFT_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class Other, class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::opBitShiftRightAssignment(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs >>= rhs; };
	static_assert(k_can_perform_op, "Cannot perform `T >>= Other`.");

	const auto ptr = static_cast<T& (T::*)(const Other&)>(&T::operator>>=);

	return func(OP_BIT_SHIFT_RIGHT_ASSIGN_NAME, ptr, attributes...);
}

template <class T, class BaseType>
template <class... Attrs>
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::classAttrs(const Attrs&... attributes)
{
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
ReflectionBuilder<T, BaseType>& ReflectionBuilder<T, BaseType>::serialize(LoadFunc serialize_load, SaveFunc serialize_save)
{
	if constexpr (k_is_initial_type) {
		_data.serialize_load = serialize_load;
		_data.serialize_save = serialize_save;
	} else {
		GAFF_REF(serialize_load, serialize_save);
	}

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
				return;
			}
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
				return;
			}
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
				addAttributes(ref_func, func, attrs, rest...);
				return;
			}
		}
	}

	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_func, func);

	if constexpr (sizeof...(Rest) > 0) {
		addAttributes(ref_func, func, attrs, rest...);
	}
}

template <class T, class BaseType>
template <class Ret, class... Args, class First, class... Rest>
void ReflectionBuilder<T, BaseType>::addAttributes(IReflectionFunction<Ret, Args...>& ref_func, Ret (T::*ptr)(Args...), Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
		if constexpr (!k_is_initial_type) {
		if (!first.canInherit()) {
			if constexpr (sizeof...(Rest) > 0) {
				addAttributes(ref_func, func, attrs, rest...);
				return;
			}
		}
	}

	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_func, func);

	if constexpr (sizeof...(Rest) > 0) {
		addAttributes(ref_func, func, attrs, rest...);
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
				addAttributes(ref_func, func, attrs, rest...);
				return;
			}
		}
	}

	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(ref_func, func);

	if constexpr (sizeof...(Rest) > 0) {
		addAttributes(ref_func, func, attrs, rest...);
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
				return;
			}
		}
	}

	First* const clone = reinterpret_cast<First*>(first.clone());
	attrs.emplace_back(IAttributePtr(clone));

	clone->apply(*this);

	if constexpr (sizeof...(Rest) > 0) {
		addAttributes(attrs, rest...);
	}
}

NS_END
