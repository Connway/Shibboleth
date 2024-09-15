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

template <class T>
template <class... Attrs>
ReflectionVersionEnum<T>& ReflectionVersionEnum<T>::enumAttrs(const Attrs&... attributes)
{
	static_assert(sizeof...(Attrs) > 0, "enumAttrs() called with no arguments.");
	_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
	_hash = getAttributeHashes(_hash, attributes...);
	return *this;
}

template <class T>
template <size_t name_size, class... Attrs>
ReflectionVersionEnum<T>& ReflectionVersionEnum<T>::entry(const char8_t (&name)[name_size], T value)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), name_size - 1, _hash);
	_hash = Gaff::FNV1aHash64T(value, _hash);

	return *this;
}

template <class T>
template <size_t name_size, class... Attrs>
ReflectionVersionEnum<T>& ReflectionVersionEnum<T>::entry(const char (&name)[name_size], T value)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return entry(temp_name, value);
}

template <class T>
template <class First, class... Rest>
Gaff::Hash64 ReflectionVersionEnum<T>::getAttributeHashes(Gaff::Hash64 hash, const First& first, const Rest&... rest) const
{
	hash = first.applyVersioning(hash);

	if constexpr (sizeof...(Rest) > 0) {
		return getAttributeHashes(hash, rest...);
	} else {
		return hash;
	}
}

template <class T>
void ReflectionVersionEnum<T>::finish(void)
{
}

template <class T>
Gaff::Hash64 ReflectionVersionEnum<T>::getHash(void) const
{
	return _hash;
}



enum class VersionValues
{
	HasSerializeFuncs,
	HasInstanceFunc
};

template <class T>
template <class Base>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::base(const char8_t* name)
{
	const ptrdiff_t offset = Gaff::OffsetOfClass<Base, T>();
	_hash = Gaff::FNV1aHash64String(name, _hash);
	_hash = Gaff::FNV1aHash64T(offset, _hash);
	return *this;
}

template <class T>
template <class Base>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::base(void)
{
	if constexpr (Reflection<Base>::HasReflection) {
		const Gaff::Hash64 version = Reflection<Base>::GetInstance().getVersion();
		const ptrdiff_t offset = Gaff::OffsetOfClass<Base, T>();

		_hash = Gaff::FNV1aHash64T(Reflection<Base>::GetNameHash(), _hash);
		_hash = Gaff::FNV1aHash64T(offset, _hash);
		_hash = Gaff::FNV1aHash64T(version, _hash);

		return *this;

	} else {
		return base<Base>(Hash::ClassHashable<Base>::GetName().data.data());
	}
}

template <class T>
template <class... Args>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::ctor(Gaff::Hash64 factory_hash)
{
	_hash = Gaff::FNV1aHash64T(factory_hash, _hash);
	return *this;
}

template <class T>
template <class... Args>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::ctor(void)
{
	_hash = Gaff::CalcTemplateHash<Args...>(_hash);
	return *this;
}

template <class T>
template <class Var, size_t name_size, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::var(const char8_t (&name)[name_size], Var T::*ptr, const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), name_size - 1, _hash);
	_hash = Gaff::FNV1aHash64T(ptr, _hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <class Var, size_t name_size, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::var(const char (&name)[name_size], Var T::*ptr, const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, ptr, attributes...);
}

template <class T>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::var(const char8_t (&name)[name_size], Ret (T::*/*getter*/)(void) const, void (T::*/*setter*/)(Var), const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), name_size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Var>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::var(const char (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, getter, setter, attributes...);
}

template <class T>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::var(const char8_t (&name)[name_size], Ret (*/*getter*/)(const T&), void (*/*setter*/)(T&, Var), const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), name_size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Var>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <class Ret, class Var, size_t name_size, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::var(const char (&name)[name_size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, getter, setter, attributes...);
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::func(const char8_t (&name)[name_size], Ret (T::*/*ptr*/)(Args...) const, const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), name_size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::func(const char (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::func(const char8_t (&name)[name_size], Ret (T::*/*ptr*/)(Args...), const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), name_size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::func(const char (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::func(const char8_t (&name)[name_size], Ret (*/*ptr*/)(const T&, Args...), const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), name_size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;

}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::func(const char (&name)[name_size], Ret (*ptr)(const T&, Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::func(const char8_t (&name)[name_size], Ret (*/*ptr*/)(T&, Args...), const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), name_size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;

}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::func(const char (&name)[name_size], Ret (*ptr)(T&, Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::staticFunc(const char8_t (&name)[name_size], Ret (*/*func*/)(Args...), const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), name_size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <size_t name_size, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::staticFunc(const char (&name)[name_size], Ret (*func)(Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return staticFunc(temp_name, func, attributes...);
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opAdd(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(+, OP_ADD_NAME, operator+, Gaff::Add, T)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opSub(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(-, OP_SUB_NAME, operator-, Gaff::Sub, T)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opMul(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(*, OP_MUL_NAME, operator*, Gaff::Mul, T)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opDiv(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(/, OP_DIV_NAME, operator/, Gaff::Div, T)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opMod(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(%, OP_SUB_NAME, operator%, Gaff::Mod, T)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitAnd(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(&, OP_BIT_AND_NAME, operator&, Gaff::BitAnd, T)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitOr(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(|, OP_BIT_OR_NAME, operator|, Gaff::BitOr, T)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitXor(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(^, OP_BIT_XOR_NAME, operator^, Gaff::BitXor, T)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitShiftLeft(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(<<, OP_BIT_SHIFT_LEFT_NAME, operator<<, Gaff::BitShiftLeft, T)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitShiftRight(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(>>, OP_BIT_SHIFT_RIGHT_NAME, operator>>, Gaff::BitShiftRight, T)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opAnd(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(&&, OP_LOGIC_AND_NAME, operator&&, Gaff::LogicAnd, bool)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opOr(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(||, OP_LOGIC_OR_NAME, operator||, Gaff::LogicOr, bool)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opEqual(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(==, OP_EQUAL_NAME, operator==, Gaff::Equal, bool)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opNotEqual(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(!=, OP_NOT_EQUAL_NAME, operator!=, Gaff::NotEqual, bool)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opLessThan(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(<, OP_LESS_THAN_NAME, operator<, Gaff::LessThan, bool)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opGreaterThan(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(>, OP_GREATER_THAN_NAME, operator>, Gaff::GreaterThan, bool)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opLessThanOrEqual(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(<=, OP_LESS_THAN_OR_EQUAL_NAME, operator<=, Gaff::LessThanOrEqual, bool)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opGreaterThanOrEqual(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL_CONST(>=, OP_GREATER_THAN_OR_EQUAL_NAME, operator>=, Gaff::GreaterThanOrEqual, bool)
}

template <class T>
template <bool is_const, class Ret, class... Args, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opCall(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, Args&&... args) { lhs(std::forward<Args>(args)...); };
	static_assert(k_can_perform_op, "Cannot perform `T(Args...)`.");

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

template <class T>
template <class Ret, class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opIndex(const Attrs&... attributes)
{
	static constexpr bool k_can_perform_op = requires(T lhs, const Other& rhs) { lhs[rhs]; };
	static_assert(k_can_perform_op, "Cannot perform `T[Other]`.");

	using NoRef = std::remove_reference_t<Ret>;
	using NoPtr = std::remove_pointer_t<NoRef>;

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

template <class T>
template <class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitNot(const Attrs&... attributes)
{
	SHIB_REFL_UNARY_OP_IMPL_CONST(~, ~value, OP_BIT_NOT_NAME, operator~, Gaff::BitNot, T)
}

template <class T>
template <class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opNegate(const Attrs&... attributes)
{
	return opMinus(attributes...);
}

template <class T>
template <class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opMinus(const Attrs&... attributes)
{
	SHIB_REFL_UNARY_OP_IMPL_CONST(-, -value, OP_MINUS_NAME, operator-, Gaff::Minus, T)
}

template <class T>
template <class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opPlus(const Attrs&... attributes)
{
	SHIB_REFL_UNARY_OP_IMPL_CONST(+, +value, OP_PLUS_NAME, operator+, Gaff::Plus, T)
}

template <class T>
template <int32_t (*to_string_func)(const T&, char8_t*, int32_t), class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opToString(const Attrs&... attributes)
{
	return staticFunc(OP_TO_STRING_NAME, to_string_func, attributes...);
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opComparison(const Attrs&... attributes)
{
	return staticFunc(OP_COMP_NAME, Gaff::Comparison<T, Other>, attributes...);
}

template <class T>
template <class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opPreIncrement(const Attrs&... attributes)
{
	SHIB_REFL_UNARY_OP_IMPL(++, ++value, OP_PRE_INC_NAME, operator++, Gaff::PreIncrement, T&, void)
}

template <class T>
template <class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opPostIncrement(const Attrs&... attributes)
{
	SHIB_REFL_UNARY_OP_IMPL(++, value++, OP_POST_INC_NAME, operator++, Gaff::PostIncrement, T, int)
}

template <class T>
template <class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opPreDecrement(const Attrs&... attributes)
{
	SHIB_REFL_UNARY_OP_IMPL(--, --value, OP_PRE_DEC_NAME, operator--, Gaff::PreDecrement, T&, void)
}

template <class T>
template <class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opPostDecrement(const Attrs&... attributes)
{
	SHIB_REFL_UNARY_OP_IMPL(--, value--, OP_POST_DEC_NAME, operator--, Gaff::PostDecrement, T, int)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(=, OP_ASSIGN_NAME, operator=, Gaff::Assignment, T&)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opAddAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(+=, OP_ADD_ASSIGN_NAME, operator+=, Gaff::AddAssignment, T&)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opSubAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(-=, OP_SUB_ASSIGN_NAME, operator-=, Gaff::SubAssignment, T&)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opMulAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(*=, OP_MUL_ASSIGN_NAME, operator*=, Gaff::MulAssignment, T&)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opDivAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(/=, OP_DIV_ASSIGN_NAME, operator/=, Gaff::DivAssignment, T&)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opModAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(%=, OP_MOD_ASSIGN_NAME, operator%=, Gaff::ModAssignment, T&)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitAndAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(&=, OP_BIT_AND_ASSIGN_NAME, operator&=, Gaff::BitAndAssignment, T&)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitOrAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(|=, OP_BIT_OR_ASSIGN_NAME, operator|=, Gaff::BitOrAssignment, T&)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitXorAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(^=, OP_BIT_XOR_ASSIGN_NAME, operator^=, Gaff::BitXorAssignment, T&)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitShiftLeftAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(<<=, OP_BIT_SHIFT_LEFT_ASSIGN_NAME, operator<<=, Gaff::BitShiftLeftAssignment, T&)
}

template <class T>
template <class Other, class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::opBitShiftRightAssignment(const Attrs&... attributes)
{
	SHIB_REFL_BINARY_OP_IMPL(>>=, OP_BIT_SHIFT_RIGHT_ASSIGN_NAME, operator>>=, Gaff::BitShiftRightAssignment, T&)
}

template <class T>
template <class... Attrs>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::classAttrs(const Attrs&... attributes)
{
	static_assert(sizeof...(Attrs) > 0, "classAttrs() called with no arguments.");
	_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
	_hash = getAttributeHashes(_hash, attributes...);
	return *this;
}

template <class T>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::version(uint32_t version)
{
	GAFF_ASSERT(_hash == Gaff::k_init_hash64);
	_hash = Gaff::FNV1aHash64T(version, _hash);
	return *this;
}

template <class T>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::serialize(LoadFunc /*serialize_load*/, SaveFunc /*serialize_save*/)
{
	_hash = Gaff::FNV1aHash64T(VersionValues::HasSerializeFuncs, _hash);
	return *this;
}

template <class T>
ReflectionVersionClass<T>& ReflectionVersionClass<T>::setInstanceHash(InstanceHashFunc /*hash_func*/)
{
	_hash = Gaff::FNV1aHash64T(VersionValues::HasInstanceFunc, _hash);
	return *this;
}

template <class T>
Gaff::Hash64 ReflectionVersionClass<T>::getHash(void) const
{
	return _hash;
}

template <class T>
void ReflectionVersionClass<T>::finish(void)
{
}

template <class T>
template <class First, class... Rest>
Gaff::Hash64 ReflectionVersionClass<T>::getAttributeHashes(Gaff::Hash64 hash, const First& first, const Rest&... rest) const
{
	hash = first.applyVersioning(hash);

	if constexpr (sizeof...(Rest) > 0) {
		return getAttributeHashes(hash, rest...);
	} else {
		return hash;
	}
}

NS_END
