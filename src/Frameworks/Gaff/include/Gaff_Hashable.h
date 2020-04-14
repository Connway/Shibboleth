/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Gaff_ArrayString.h"
#include "Gaff_Hash.h"

#ifndef GAFF_HASHABLE_NAMESPACE
	#define GAFF_HASHABLE_NAMESPACE Gaff
#endif

#ifndef NS_HASHABLE
	#define NS_HASHABLE namespace GAFF_HASHABLE_NAMESPACE {
#endif

#define GAFF_TEMPLATE_GET_NAME(x) GAFF_HASHABLE_NAMESPACE::GetName<x>()

#define GAFF_CLASS_HASHABLE(type) \
	template <> \
	constexpr auto GetName<type>(void) \
	{ \
		const char* const name = #type; \
		Gaff::ArrayString<char, eastl::CharStrlen(name) + 1> name_arr; \
		for (int32_t i = 0; i < eastl::CharStrlen(name); ++i) { \
			name_arr.data.mValue[i] = name[i]; \
		} \
		return name_arr; \
	} \
	template <> \
	constexpr Gaff::Hash64 GetHash<type>(void) \
	{ \
		return Gaff::FNV1aHash64Const(#type); \
	}

#define GAFF_TEMPLATE_CLASS_HASHABLE(type, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	struct TemplateClassHashableHelper< GAFF_SINGLE_ARG(type<__VA_ARGS__>) > final \
	{ \
		static constexpr auto GetName(void) \
		{ \
			const auto rest_names = GetNameHelper<__VA_ARGS__>(); \
			const auto name = Gaff::MakeArrayString(#type); \
			const auto lb = Gaff::MakeArrayString("<"); \
			const auto rb = Gaff::MakeArrayString("<"); \
			const auto final_str = name + lb + rest_names + rb; \
			return final_str; \
		} \
		static constexpr Gaff::Hash64 GetHash(void) \
		{ \
			return Gaff::CalcTemplateHash<__VA_ARGS__>(Gaff::FNV1aHash64Const(#type)); \
		} \
	};


NS_HASHABLE

template <class First, class... Rest>
constexpr auto GetNameHelper(void)
{
	if constexpr (sizeof...(Rest) > 0) {
		const auto name = GetNameHelper<First>();
		const auto rest_name = GetNameHelper<Rest...>();
		const auto comma = Gaff::MakeArrayString(", ");

		const auto final_str = name + comma + rest_name;
		return final_str;

	} else {
		return GetName<First>();
	}
}

template <class... T>
struct TemplateClassHashableHelper final
{
	static constexpr auto GetName(void)
	{
		static_assert(false, "Did not overload GetName() for type.");
		return Gaff::MakeArrayString("ERROR: No Class Name");
	}

	static constexpr Gaff::Hash64 GetHash(void)
	{
		static_assert(false, "Did not overload GetHash() for type.");
		return Gaff::INIT_HASH64;
	}
};

template <class T>
constexpr auto GetName(void)
{
	return TemplateClassHashableHelper<T>::GetName();
}

template <class T>
constexpr Gaff::Hash64 GetHash(void)
{
	return TemplateClassHashableHelper<T>::GetHash();
}

GAFF_CLASS_HASHABLE(void)
GAFF_CLASS_HASHABLE(char)

NS_END


NS_GAFF

template <class... T>
constexpr int32_t GetNumArgs(void)
{
	return sizeof...(T);
}

constexpr const char* GetTypeNameBegin(const char* type_name)
{
	const char* const const_string = "const";
	const char* const_begin = const_string;
	const char* type_begin = type_name;

	while (*type_begin && *const_begin) {
		// Type isn't const.
		if (*type_begin != *const_begin) {
			return type_name;
		}

		++const_begin;
		++type_begin;
	}

	// len(type_name) < len(const_string)
	if (!(*type_begin)) {
		return type_name;
	}

	// We reached the end of const_string, so the type name begins with "const".
	// Move pointer over one more to move over the space.
	return type_begin + 1;
}

constexpr const char* GetTypeNameEnd(const char* type_name)
{
	// Find end of string.
	while (*type_name) {
		++type_name;
	}

	--type_name;

	// Walk back until we hit the actual class name.
	while (*type_name == '*' || *type_name == '&' || *type_name == ' ') {
		--type_name;
	}

	// Move forward one so that length calculation is correct.
	return type_name + 1;
}

template <class T>
constexpr Hash64 CalcTypeHash(Hash64 init, const char* type_string, size_t size)
{
	using NoPtr = typename std::remove_pointer<T>::type;
	using NoRef = typename std::remove_reference<NoPtr>::type;
	//using V = typename std::remove_const<NoRef>::type;

	if constexpr (std::is_const<NoRef>::value) {
		init = FNV1aHash64Const("const ", init);
	}

	init = FNV1aHash64Const(type_string, size, init);

	if constexpr (std::is_reference<NoPtr>::value) {
		init = FNV1aHash64Const("&", init);
	}

	if constexpr (std::is_pointer<T>::value) {
		init = FNV1aHash64Const("*", init);
	}

	return init;
}

template <class T>
constexpr Hash64 CalcTypeHash(Hash64 init, const char* type_string)
{
	using NoPtr = typename std::remove_pointer<T>::type;
	using NoRef = typename std::remove_reference<NoPtr>::type;
	//using V = typename std::remove_const<NoRef>::type;

	if constexpr (std::is_const<NoRef>::value) {
		init = FNV1aHash64Const("const ", init);
	}

	init = FNV1aHash64StringConst(type_string, init);

	if constexpr (std::is_reference<NoPtr>::value) {
		init = FNV1aHash64Const("&", init);
	}

	if constexpr (std::is_pointer<T>::value) {
		init = FNV1aHash64Const("*", init);
	}

	return init;
}

template <class First, class... Rest>
constexpr Hash64 CalcTemplateHashHelper(Hash64 init, const char** type_names, int32_t index)
{
	const char* const begin = GetTypeNameBegin(*(type_names + index));
	const char* const end = GetTypeNameEnd(*(type_names + index));

	if constexpr (sizeof...(Rest) == 0) {
		return CalcTypeHash<First>(init, begin, static_cast<size_t>(end - begin));
	} else {
		return CalcTemplateHashHelper<Rest...>(CalcTypeHash<First>(init, begin, static_cast<size_t>(end - begin)), type_names, index + 1);
	}
}

template <class First, class... Rest>
constexpr Hash64 CalcTemplateHashHelper(Hash64 init)
{
	using NoPtr = typename std::remove_pointer<First>::type;
	using NoRef = typename std::remove_reference<NoPtr>::type;
	using V = typename std::remove_const<NoRef>::type;

	const auto name = GAFF_HASHABLE_NAMESPACE::GetName<V>();
	const Gaff::Hash64 hash = CalcTypeHash<First>(init, name.data.data());

	if constexpr (sizeof...(Rest) == 0) {
		return hash;
	} else {
		return CalcTemplateHashHelper<Rest...>(hash);
	}
}

template <class... T>
constexpr Hash64 CalcTemplateHash(Hash64 init, eastl::array<const char*, GetNumArgs<T...>()> type_names)
{
	static_assert(sizeof...(T) > 0, "Initializer list version of CalcTemplateHash must be non-void.");
	return CalcTemplateHashHelper<T...>(init, type_names.data(), 0);
}

template <class... T>
constexpr Hash64 CalcTemplateHash(Hash64 init)
{
	if constexpr (sizeof...(T) == 0) {
		return FNV1aHash64Const("void", init);
	} else {
		return CalcTemplateHashHelper<T...>(init);
	}
}

NS_END
