/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

enum class VersionValues
{
	HasStackCtor,
	HasSerializeFuncs,
	HasInstanceFunc
};

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::friendlyName(const char* name)
{
	_hash = Gaff::FNV1aHash64String(name, _hash);
	return *this;
}

template <class T>
template <class Base>
ReflectionVersion<T>& ReflectionVersion<T>::base(const char8_t* name)
{
	const ptrdiff_t offset = Gaff::OffsetOfClass<T, Base>();
	_hash = Gaff::FNV1aHash64String(name, _hash);
	_hash = Gaff::FNV1aHash64T(offset, _hash);
	return *this;
}

template <class T>
template <class Base>
ReflectionVersion<T>& ReflectionVersion<T>::base(void)
{
	const Gaff::Hash64 version = Reflection<Base>::GetInstance().getVersion();
	const ptrdiff_t offset = Gaff::OffsetOfClass<T, Base>();

	_hash = Gaff::FNV1aHash64T(Reflection<Base>::GetHash(), _hash);
	_hash = Gaff::FNV1aHash64T(offset, _hash);
	_hash = Gaff::FNV1aHash64T(version, _hash);

	return *this;
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::stackCtor(StackCtorFunc /*func*/)
{
	_hash = Gaff::FNV1aHash64T(VersionValues::HasStackCtor, _hash);
	return *this;
}

template <class T>
template <class... Args>
ReflectionVersion<T>& ReflectionVersion<T>::ctor(Gaff::Hash64 factory_hash)
{
	_hash = Gaff::FNV1aHash64T(factory_hash, _hash);
	return *this;
}

template <class T>
template <class... Args>
ReflectionVersion<T>& ReflectionVersion<T>::ctor(void)
{
	_hash = Gaff::CalcTemplateHash<Args...>(_hash);
	return *this;
}

template <class T>
template <class Var, size_t size, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::var(const char8_t (&name)[size], Var T::*ptr, const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), size - 1, _hash);
	_hash = Gaff::FNV1aHash64T(ptr, _hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <class Var, size_t size, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::var(const char (&name)[size], Var T::*ptr, const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, ptr, attributes...);
}

template <class T>
template <class Ret, class Var, size_t size, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::var(const char8_t (&name)[size], Ret (T::*/*getter*/)(void) const, void (T::*/*setter*/)(Var), const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Var>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <class Ret, class Var, size_t size, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, getter, setter, attributes...);
}

template <class T>
template <class Ret, class Var, size_t size, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::var(const char8_t (&name)[size], Ret (*/*getter*/)(const T&), void (*/*setter*/)(T&, Var), const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Var>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <class Ret, class Var, size_t size, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::var(const char (&name)[size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return var(temp_name, getter, setter, attributes...);
}

template <class T>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::func(const char8_t (&name)[size], Ret (T::*/*ptr*/)(Args...) const, const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::func(const char (&name)[size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::func(const char8_t (&name)[size], Ret (T::*/*ptr*/)(Args...), const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::func(const char (&name)[size], Ret (T::*ptr)(Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return func(temp_name, ptr, attributes...);
}

template <class T>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::staticFunc(const char8_t (&name)[size], Ret (*/*func*/)(Args...), const Attrs&... attributes)
{
	const char8_t* const temp_name = name;
	_hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(temp_name), size - 1, _hash);
	_hash = Gaff::CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::staticFunc(const char (&name)[size], Ret (*func)(Args...), const Attrs&... attributes)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return staticFunc(temp_name, func, attributes...);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opAdd(void)
{
	staticFunc(OP_ADD_NAME, Gaff::Add<T, Other>);
	return staticFunc(OP_ADD_NAME, Gaff::Add<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opSub(void)
{
	staticFunc(OP_SUB_NAME, Gaff::Sub<T, Other>);
	return staticFunc(OP_SUB_NAME, Gaff::Sub<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opMul(void)
{
	staticFunc(OP_MUL_NAME, Gaff::Mul<T, Other>);
	return staticFunc(OP_MUL_NAME, Gaff::Mul<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opDiv(void)
{
	staticFunc(OP_DIV_NAME, Gaff::Div<T, Other>);
	return staticFunc(OP_DIV_NAME, Gaff::Div<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opMod(void)
{
	staticFunc(OP_MOD_NAME, Gaff::Mod<T, Other>);
	return staticFunc(OP_MOD_NAME, Gaff::Mod<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opBitAnd(void)
{
	staticFunc(OP_BIT_AND_NAME, Gaff::BitAnd<T, Other>);
	return staticFunc(OP_BIT_AND_NAME, Gaff::BitAnd<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opBitOr(void)
{
	staticFunc(OP_BIT_OR_NAME, Gaff::BitOr<T, Other>);
	return staticFunc(OP_BIT_OR_NAME, Gaff::BitOr<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opBitXor(void)
{
	staticFunc(OP_BIT_XOR_NAME, Gaff::BitXor<T, Other>);
	return staticFunc(OP_BIT_XOR_NAME, Gaff::BitXor<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opBitShiftLeft(void)
{
	staticFunc(OP_BIT_SHIFT_LEFT_NAME, Gaff::BitShiftLeft<T, Other>);
	return staticFunc(OP_BIT_SHIFT_LEFT_NAME, Gaff::BitShiftLeft<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opBitShiftRight(void)
{
	staticFunc(OP_BIT_SHIFT_RIGHT_NAME, Gaff::BitShiftRight<T, Other>);
	return staticFunc(OP_BIT_SHIFT_RIGHT_NAME, Gaff::BitShiftRight<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opAnd(void)
{
	staticFunc(OP_LOGIC_AND_NAME, Gaff::LogicAnd<T, Other>);
	return staticFunc(OP_LOGIC_AND_NAME, Gaff::LogicAnd<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opOr(void)
{
	staticFunc(OP_LOGIC_OR_NAME, Gaff::LogicOr<T, Other>);
	return staticFunc(OP_LOGIC_OR_NAME, Gaff::LogicOr<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opEqual(void)
{
	staticFunc(OP_EQUAL_NAME, Gaff::Equal<T, Other>);
	return staticFunc(OP_EQUAL_NAME, Gaff::Equal<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opLessThan(void)
{
	staticFunc(OP_LESS_THAN_NAME, Gaff::LessThan<T, Other>);
	return staticFunc(OP_LESS_THAN_NAME, Gaff::LessThan<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opGreaterThan(void)
{
	staticFunc(OP_GREATER_THAN_NAME, Gaff::GreaterThan<T, Other>);
	return staticFunc(OP_GREATER_THAN_NAME, Gaff::GreaterThan<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opLessThanOrEqual(void)
{
	staticFunc(OP_LESS_THAN_OR_EQUAL_NAME, Gaff::LessThanOrEqual<T, Other>);
	return staticFunc(OP_LESS_THAN_OR_EQUAL_NAME, Gaff::LessThanOrEqual<Other, T>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opGreaterThanOrEqual(void)
{
	staticFunc(OP_GREATER_THAN_OR_EQUAL_NAME, Gaff::GreaterThanOrEqual<T, Other>);
	return staticFunc(OP_GREATER_THAN_OR_EQUAL_NAME, Gaff::GreaterThanOrEqual<Other, T>);
}

template <class T>
template <class... Args>
ReflectionVersion<T>& ReflectionVersion<T>::opCall(void)
{
	return staticFunc(OP_CALL_NAME, Gaff::Call<T, Args...>);
}

template <class T>
template <class Other>
ReflectionVersion<T>& ReflectionVersion<T>::opIndex(void)
{
	return staticFunc(OP_INDEX_NAME, Gaff::Index<T, Other>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opAdd(void)
{
	return staticFunc(OP_ADD_NAME, Gaff::Add<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opSub(void)
{
	return staticFunc(OP_SUB_NAME, Gaff::Sub<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opMul(void)
{
	return staticFunc(OP_MUL_NAME, Gaff::Mul<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opDiv(void)
{
	return staticFunc(OP_DIV_NAME, Gaff::Div<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opMod(void)
{
	return staticFunc(OP_MOD_NAME, Gaff::Mod<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opBitAnd(void)
{
	return staticFunc(OP_BIT_AND_NAME, Gaff::BitAnd<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opBitOr(void)
{
	return staticFunc(OP_BIT_OR_NAME, Gaff::BitOr<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opBitXor(void)
{
	return staticFunc(OP_BIT_XOR_NAME, Gaff::BitXor<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opBitNot(void)
{
	return staticFunc(OP_BIT_NOT_NAME, Gaff::BitNot<T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opBitShiftLeft(void)
{
	return staticFunc(OP_BIT_SHIFT_LEFT_NAME, Gaff::BitShiftLeft<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opBitShiftRight(void)
{
	return staticFunc(OP_BIT_SHIFT_RIGHT_NAME, Gaff::BitShiftRight<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opAnd(void)
{
	return staticFunc(OP_LOGIC_AND_NAME, Gaff::LogicAnd<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opOr(void)
{
	return staticFunc(OP_LOGIC_OR_NAME, Gaff::LogicOr<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opEqual(void)
{
	return staticFunc(OP_EQUAL_NAME, Gaff::Equal<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opLessThan(void)
{
	return staticFunc(OP_LESS_THAN_NAME, Gaff::LessThan<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opGreaterThan(void)
{
	return staticFunc(OP_GREATER_THAN_NAME, Gaff::GreaterThan<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opLessThanOrEqual(void)
{
	return staticFunc(OP_LESS_THAN_OR_EQUAL_NAME, Gaff::LessThanOrEqual<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opGreaterThanOrEqual(void)
{
	return staticFunc(OP_GREATER_THAN_OR_EQUAL_NAME, Gaff::GreaterThanOrEqual<T, T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opMinus(void)
{
	return staticFunc(OP_MINUS_NAME, Gaff::Minus<T>);
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::opPlus(void)
{
	return staticFunc(OP_PLUS_NAME, Gaff::Plus<T>);
}

template <class T>
template <int32_t (*to_string_func)(const T&, char8_t*, int32_t)>
ReflectionVersion<T>& ReflectionVersion<T>::opToString(void)
{
	staticFunc(OP_TO_STRING_NAME, Gaff::ToStringHelper<T, to_string_func>);
	return staticFunc(OP_TO_STRING_NAME, to_string_func);
}

template <class T>
template <class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::classAttrs(const Attrs&... attributes)
{
	static_assert(sizeof...(Attrs) > 0, "classAttrs() called with no arguments.");
	_hash = Gaff::FNV1aHash64String("class", _hash);
	_hash = Gaff::CalcTemplateHash<Attrs...>(_hash);
	_hash = getAttributeHashes(_hash, attributes...);
	return *this;
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::version(uint32_t version)
{
	GAFF_ASSERT(_hash == Gaff::k_init_hash64);
	_hash = Gaff::FNV1aHash64T(version, _hash);
	return *this;
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::serialize(LoadFunc /*serialize_load*/, SaveFunc /*serialize_save*/)
{
	_hash = Gaff::FNV1aHash64T(VersionValues::HasSerializeFuncs, _hash);
	return *this;
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::setInstanceHash(InstanceHashFunc /*hash_func*/)
{
	_hash = Gaff::FNV1aHash64T(VersionValues::HasInstanceFunc, _hash);
	return *this;
}

template <class T>
template <size_t size, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::entry(const char(&name)[size], T value)
{
	_hash = Gaff::FNV1aHash64(name, size - 1, _hash);
	_hash = Gaff::FNV1aHash64T(value, _hash);

	return *this;
}

template <class T>
Gaff::Hash64 ReflectionVersion<T>::getHash(void) const
{
	return _hash;
}

template <class T>
void ReflectionVersion<T>::finish(void)
{
}

template <class T>
template <class First, class... Rest>
Gaff::Hash64 ReflectionVersion<T>::getAttributeHashes(Gaff::Hash64 hash, const First& first, const Rest&... rest) const
{
	hash = first.applyVersioning(hash);

	if constexpr (sizeof...(Rest) > 0) {
		return getAttributeHashes(hash, rest...);
	} else {
		return hash;
	}
}

NS_END
