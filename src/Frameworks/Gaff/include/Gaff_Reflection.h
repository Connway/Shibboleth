/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Gaff_ReflectionBase.h"

#define GAFF_TEMPLATE_REFLECTION_CLASS(x) class x

#ifndef GAFF_REFLECTION_NAMESPACE
	#define GAFF_REFLECTION_NAMESPACE Gaff
#endif

#ifndef NS_REFLECTION
	#define NS_REFLECTION namespace GAFF_REFLECTION_NAMESPACE {
#endif

#ifdef GAFF_REF_DEF_IS_PTR
	#define GAFF_REF_DEF_DECLARE(type, allocator)
	#define GAFF_REF_DEF_DEFINE(type, allocator) \
		GAFF_REFLECTION_NAMESPACE::Reflection<type> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_instance; \
		void GAFF_REFLECTION_NAMESPACE::Reflection<type>::Init(void) \
		{ \
			if (g_instance._defined) { \
				return; \
			} \
			BuildReflection(g_instance._version); \
			GAFF_REF_DEF_INIT_INTERNAL; \
			if (!g_instance._defined) { \
				BuildReflection(*g_instance._ref_def); \
				g_instance._defined = true; \
			} \
			for (auto& func : g_instance._on_defined_callbacks) { \
				func(); \
			} \
			g_instance._on_defined_callbacks.clear(); \
		}

	#define GAFF_TEMPLATE_REF_DEF_DECLARE(type, allocator, ...)
	#define GAFF_TEMPLATE_REF_DEF_DEFINE(type, allocator, ...) \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> > GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_instance; \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::Init(void) \
		{ \
			if (g_instance._defined) { \
				return; \
			} \
			BuildReflection(g_instance._version); \
			GAFF_REF_DEF_INIT_INTERNAL; \
			if (!g_instance._defined) { \
				BuildReflection(*g_instance._ref_def); \
				g_instance._defined = true; \
			} \
			for (auto& func : g_instance._on_defined_callbacks) { \
				func(); \
			} \
			g_instance._on_defined_callbacks.clear(); \
		}

	#ifndef GAFF_REF_DEF_INIT
		#error "Reflection Definition is pointer, but GAFF_REF_DEF_INIT was not defined."
	#endif

	#define GAFF_REF_DEF_INIT_INTERNAL GAFF_REF_DEF_INIT
#else
	#ifndef GAFF_REF_DEF_INIT
		#define GAFF_REF_DEF_INIT
	#endif

	#define GAFF_REF_DEF_DECLARE(type, allocator) typename Gaff::RefDefType<type, allocator> g_reflection_definition;
	#define GAFF_REF_DEF_DEFINE(type, allocator) \
		typename Gaff::RefDefType<type, allocator> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_reflection_definition; \
		GAFF_REFLECTION_NAMESPACE::Reflection<type> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_instance; \
		void GAFF_REFLECTION_NAMESPACE::Reflection<type>::Init(void) \
		{ \
			if (g_instance._defined) { \
				return; \
			} \
			BuildReflection(g_instance._version); \
			GAFF_REF_DEF_INIT_INTERNAL; \
			if (!g_instance._defined) { \
				BuildReflection(*g_instance._ref_def); \
				g_instance._defined = true; \
			} \
			for (auto& func : g_instance._on_defined_callbacks) { \
				func(); \
			} \
			g_instance._on_defined_callbacks.clear(); \
		}

	#define GAFF_TEMPLATE_REF_DEF_DECLARE(type, allocator, ...) \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		static typename Gaff::RefDefType<type<__VA_ARGS__>, allocator> g_reflection_definition;

	#define GAFF_TEMPLATE_REF_DEF_DEFINE(type, allocator, ...) \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		typename Gaff::RefDefType<type<__VA_ARGS__>, allocator> GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_reflection_definition; \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> > GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_instance; \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::Init(void) \
		{ \
			if (g_instance._instance._defined) { \
				return; \
			} \
			BuildReflection(g_instance._version); \
			GAFF_REF_DEF_INIT_INTERNAL; \
			if (!g_instance._defined) { \
				BuildReflection(*g_instance._ref_def); \
				g_instance._defined = true; \
			} \
			for (auto& func : g_instance._on_defined_callbacks) { \
				func(); \
			} \
			g_instance._on_defined_callbacks.clear(); \
		}

	#define GAFF_REF_DEF_INIT_INTERNAL \
		g_instance._ref_def = &g_reflection_definition; \
		GAFF_REF_DEF_INIT
#endif

#define GAFF_REFLECTION_DECLARE(type, allocator) \
	NS_HASHABLE \
		GAFF_CLASS_HASHABLE(type) \
	NS_END \
	NS_REFLECTION \
		template <> \
		class Reflection<type> final : public Gaff::ReflectionBase<type, allocator> \
		{ \
		private: \
			using ThisType = type; \
		public: \
			constexpr static bool HasReflection = true; \
			template <class ReflectionBuilder> \
			static void BuildReflection(ReflectionBuilder& builder); \
			static void Init(void); \
			static Reflection<type>& GetInstance(void) \
			{ \
				return g_instance; \
			} \
			static const typename Gaff::RefDefType<type, allocator>& GetReflectionDefinition(void) \
			{ \
				GAFF_ASSERT(g_instance._ref_def); \
				return *g_instance._ref_def; \
			} \
			static void RegisterOnDefinedCallback(const eastl::function<void (void)>& callback) \
			{ \
				g_instance._on_defined_callbacks.emplace_back(callback); \
			} \
			static void RegisterOnDefinedCallback(eastl::function<void(void)>&& callback) \
			{ \
				g_instance._on_defined_callbacks.emplace_back(std::move(callback)); \
			} \
			static bool Load(const Gaff::ISerializeReader& reader, type& object, bool refl_load = false) \
			{ \
				return g_instance._ref_def->load(reader, object, refl_load); \
			} \
			static void Save(Gaff::ISerializeWriter& writer, const type& object, bool refl_save = false) \
			{ \
				g_instance._ref_def->save(writer, object, refl_save); \
			} \
			static bool IsDefined(void) \
			{ \
				return g_instance._defined; \
			} \
			void init(void) override \
			{ \
				Init(); \
			} \
		private: \
			static Reflection<type> g_instance; \
			GAFF_REF_DEF_DECLARE(type, allocator) \
		}; \
	NS_END

#define GAFF_REFLECTION_CLASS_DECLARE(type) \
	public: \
		const Gaff::IReflectionDefinition& getReflectionDefinition(void) const override; \
		const void* getBasePointer(void) const override \
		{ \
			return this; \
		} \
		void* getBasePointer(void) override \
		{ \
			return this; \
		} \
	private: \
		friend class GAFF_REFLECTION_NAMESPACE::Reflection<type>

#define GAFF_REFLECTION_CLASS_DEFINE(type) \
	const Gaff::IReflectionDefinition& type::getReflectionDefinition(void) const \
	{ \
		return GAFF_REFLECTION_NAMESPACE::Reflection<type>::GetReflectionDefinition(); \
	}

#define GAFF_REFLECTION_DEFINE_BEGIN(type, allocator) \
	NS_REFLECTION \
		GAFF_REF_DEF_DEFINE(type, allocator) \
		template <class ReflectionBuilder> \
		void GAFF_REFLECTION_NAMESPACE::Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
		{ \
			builder

#define GAFF_REFLECTION_DEFINE_END(type) \
			; \
			builder.finish(); \
		} \
	NS_END



#define GAFF_TEMPLATE_REFLECTION_DECLARE(type, allocator, ...) \
	NS_HASHABLE \
		GAFF_TEMPLATE_CLASS_HASHABLE(type, __VA_ARGS__) \
	NS_END \
	NS_REFLECTION \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		class Reflection< type<__VA_ARGS__> > final : public Gaff::ReflectionBase<type<__VA_ARGS__>, allocator> \
		{ \
		private: \
			using ThisType = type<__VA_ARGS__>; \
		public: \
			constexpr static bool HasReflection = true; \
			template <class ReflectionBuilder> \
			static void BuildReflection(ReflectionBuilder& builder); \
			static void Init(void); \
			static Reflection<ThisType>& GetInstance(void) \
			{ \
				return g_instance; \
			} \
			static const typename Gaff::RefDefType<ThisType, allocator>& GetReflectionDefinition(void) \
			{ \
				GAFF_ASSERT(g_instance._ref_def); \
				return *g_instance._ref_def; \
			} \
			static void RegisterOnDefinedCallback(const eastl::function<void (void)>& callback) \
			{ \
				g_instance._on_defined_callbacks.emplace_back(callback); \
			} \
			static void RegisterOnDefinedCallback(eastl::function<void(void)>&& callback) \
			{ \
				g_instance._on_defined_callbacks.emplace_back(std::move(callback)); \
			} \
			static bool Load(const Gaff::ISerializeReader& reader, ThisType& object, bool refl_load = false) \
			{ \
				return g_instance._ref_def->load(reader, object, refl_load); \
			} \
			static void Save(Gaff::ISerializeWriter& writer, const ThisType& object, bool refl_save = false) \
			{ \
				g_instance._ref_def->save(writer, object, refl_save); \
			} \
			static bool IsDefined(void) \
			{ \
				return g_instance._defined; \
			} \
			void init(void) override \
			{ \
				Init(); \
			} \
		private: \
			static Reflection<ThisType> g_instance; \
			GAFF_REF_DEF_DECLARE(ThisType, allocator) \
		}; \
	NS_END

#define GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE(type, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	const Gaff::IReflectionDefinition& type<__VA_ARGS__>::getReflectionDefinition(void) const \
	{ \
		return GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::GetReflectionDefinition(); \
	}

#define GAFF_TEMPLATE_REFLECTION_DEFINE_BEGIN(type, allocator, ...) \
	NS_REFLECTION \
		GAFF_TEMPLATE_REF_DEF_DEFINE(type, allocator, __VA_ARGS__) \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		template <class ReflectionBuilder> \
		void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::BuildReflection(ReflectionBuilder& builder) \
		{ \
			builder

#define GAFF_TEMPLATE_REFLECTION_CLASS_DECLARE(type, ...) GAFF_REFLECTION_CLASS_DECLARE(type<__VA_ARGS__>)
#define GAFF_TEMPLATE_REFLECTION_DEFINE_END(type, ...) GAFF_REFLECTION_DEFINE_END(type<__VA_ARGS__>)

#define REFLECTION_CAST_PTR_NAME(T, name, object) \
	reinterpret_cast<T*>( \
		const_cast<Gaff::IReflectionDefinition&>((object)->getReflectionDefinition()).getInterface( \
			Gaff::FNV1aHash64Const(name), (object)->getBasePointer() \
		) \
	)

#define REFLECTION_CAST_NAME(T, name, object) *REFLECTION_CAST_PTR_NAME(T, name, &object)
#define REFLECTION_CAST_PTR(T, object) REFLECTION_CAST_PTR_NAME(T, #T, object)
#define REFLECTION_CAST(T, object) *REFLECTION_CAST_PTR(T, &object)



NS_GAFF

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static Gaff::Hash64 HashStringInstanceHash(const Gaff::HashString<T, HashType, HashingFunc, Allocator, true> & hash_string, Gaff::Hash64 init)
{
	return Gaff::FNV1aHash64String(hash_string.getBuffer(), init);
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static Gaff::Hash64 HashStringInstanceHash(const Gaff::HashString<T, HashType, HashingFunc, Allocator, false> & hash_string, Gaff::Hash64 init)
{
	return Gaff::FNV1aHash64T(hash_string.getHash(), init);
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc>
static Gaff::Hash64 HashStringViewInstanceHash(const Gaff::HashStringView<T, HashType, HashingFunc>& hash_string, Gaff::Hash64 init)
{
	if (hash_string.getBuffer()) {
		return Gaff::FNV1aHash64String(hash_string.getBuffer(), init);
	} else {
		return Gaff::FNV1aHash64T(hash_string.getHash(), init);
	}
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static bool LoadHashString(const Gaff::ISerializeReader& reader, Gaff::HashString<T, HashType, HashingFunc, Allocator, true>& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char* const str = reader.readString();
	out = str;
	reader.freeString(str);

	return true;
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static bool LoadHashString(const Gaff::ISerializeReader& reader, Gaff::HashString<T, HashType, HashingFunc, Allocator, false>& out)
{
	if (!reader.isString() && !reader.isUInt32() && !reader.isUInt64()) {
		return false;
	}

	if (reader.isString()) {
		const char* const str = reader.readString();
		out = str;
		reader.freeString(str);

	} else {
		if constexpr (std::is_same<HashType, Gaff::Hash32>::value) {
			out = Gaff::HashString<T, HashType, HashingFunc, Allocator, false>(HashType(static_cast<T>(reader.readUInt32())));
		} else if constexpr (std::is_same<HashType, Gaff::Hash64>::value) {
			out = Gaff::HashString<T, HashType, HashingFunc, Allocator, false>(HashType(static_cast<T>(reader.readUInt64())));
		} else {
			static_assert(false, "Unknown hash type in LoadHashString.");
		}
	}

	return true;
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc>
static bool LoadHashStringView(const Gaff::ISerializeReader& reader, Gaff::HashStringView<T, HashType, HashingFunc>& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char* const str = reader.readString();
	out = Gaff::HashStringView<T, HashType, HashingFunc>(str);
	reader.freeString(str);

	return true;
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static void SaveHashString(Gaff::ISerializeWriter& writer, const Gaff::HashString<T, HashType, HashingFunc, Allocator, true>& value)
{
	writer.writeString(value.getBuffer());
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static void SaveHashString(Gaff::ISerializeWriter& writer, const Gaff::HashString<T, HashType, HashingFunc, Allocator, false>& value)
{
	writer.writeUInt64(value.getHash().getHash());
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc>
static void SaveHashStringView(Gaff::ISerializeWriter& writer, const Gaff::HashStringView<T, HashType, HashingFunc>& value)
{
	if (value.getBuffer()) {
		writer.writeString(value.getBuffer());
	} else {
		writer.writeUInt64(value.getHash().getHash());
	}
}

template <class Allocator>
static Gaff::Hash64 HashStringInstance(const Gaff::U8String<Allocator>& string, Gaff::Hash64 init)
{
	return Gaff::FNV1aHash64String(string.data(), init);
}

template <class Allocator>
static bool LoadString(const Gaff::ISerializeReader& reader, Gaff::U8String<Allocator>& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char* const str = reader.readString();
	out = str;
	reader.freeString(str);

	return true;
}

template <class Allocator>
static void SaveString(Gaff::ISerializeWriter& writer, const Gaff::U8String<Allocator>& value)
{
	writer.writeString(value.data());
}

NS_END

NS_HASHABLE
	GAFF_CLASS_HASHABLE(Gaff::IReflectionDefinition)
NS_END
