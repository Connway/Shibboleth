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
			BuildReflection(g_version); \
			GAFF_REF_DEF_INIT_INTERNAL; \
			BuildReflection(*g_ref_def); \
			g_defined = true; \
			for (auto& func : g_on_defined_callbacks) { \
				func(); \
			} \
			g_on_defined_callbacks.clear(); \
		}

	#define GAFF_TEMPLATE_REF_DEF_DECLARE(type, allocator, ...)
	#define GAFF_TEMPLATE_REF_DEF_DEFINE(type, allocator, ...) \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> > GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::g_instance; \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::Init(void) \
		{ \
			BuildReflection(g_version); \
			GAFF_REF_DEF_INIT_INTERNAL; \
			BuildReflection(*g_ref_def); \
			g_defined = true; \
			for (auto& func : g_on_defined_callbacks) { \
				func(); \
			} \
			g_on_defined_callbacks.clear(); \
		}

	#ifndef GAFF_REF_DEF_INIT
		#error "Reflection Definition is pointer, but GAFF_REF_DEF_INIT was not defined."
	#endif

	#define GAFF_REF_DEF_INIT_INTERNAL GAFF_REF_DEF_INIT
#else
	#ifndef GAFF_REF_DEF_INIT
		#define GAFF_REF_DEF_INIT
	#endif

	#define GAFF_REF_DEF_DECLARE(type, allocator) static typename Gaff::RefDefType<type, allocator> g_reflection_definition;
	#define GAFF_REF_DEF_DEFINE(type, allocator) \
		typename Gaff::RefDefType<type, allocator> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_reflection_definition; \
		GAFF_REFLECTION_NAMESPACE::Reflection<type> GAFF_REFLECTION_NAMESPACE::Reflection<type>::g_instance; \
		void GAFF_REFLECTION_NAMESPACE::Reflection<type>::Init(void) \
		{ \
			BuildReflection(g_version); \
			GAFF_REF_DEF_INIT_INTERNAL; \
			BuildReflection(*g_ref_def); \
			g_defined = true; \
			for (auto& func : g_on_defined_callbacks) { \
				func(); \
			} \
			g_on_defined_callbacks.clear(); \
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
			BuildReflection(g_version); \
			GAFF_REF_DEF_INIT_INTERNAL; \
			BuildReflection(*g_ref_def); \
			g_defined = true; \
			for (auto& func : g_on_defined_callbacks) { \
				func(); \
			} \
			g_on_defined_callbacks.clear(); \
		}

	#define GAFF_REF_DEF_INIT_INTERNAL \
		g_ref_def = &g_reflection_definition; \
		GAFF_REF_DEF_INIT
#endif

#define GAFF_REFLECTION_DECLARE_NEW(type, allocator) \
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
				GAFF_ASSERT(g_ref_def); \
				return *g_ref_def; \
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

#define GAFF_REFLECTION_CLASS_DECLARE_NEW(type) \
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

#define GAFF_REFLECTION_CLASS_DEFINE_NEW(type) \
	const Gaff::IReflectionDefinition& type::getReflectionDefinition(void) const \
	{ \
		return GAFF_REFLECTION_NAMESPACE::Reflection<type>::GetReflectionDefinition(); \
	}

#define GAFF_REFLECTION_DEFINE_BEGIN_NEW(type, allocator) \
	NS_REFLECTION \
		GAFF_REF_DEF_DEFINE(type, allocator) \
		template <class ReflectionBuilder> \
		void GAFF_REFLECTION_NAMESPACE::Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
		{ \
			builder

#define GAFF_REFLECTION_DEFINE_END_NEW(type) \
			; \
			builder.finish(); \
		} \
	NS_END



#define GAFF_TEMPLATE_REFLECTION_DECLARE_NEW(type, allocator, ...) \
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
				GAFF_ASSERT(g_ref_def); \
				return *g_ref_def; \
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

#define GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_NEW(type, ...) \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	const Gaff::IReflectionDefinition& type<__VA_ARGS__>::getReflectionDefinition(void) const \
	{ \
		return GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::GetReflectionDefinition(); \
	}

#define GAFF_TEMPLATE_REFLECTION_DEFINE_BEGIN_NEW(type, allocator, ...) \
	NS_REFLECTION \
		GAFF_TEMPLATE_REF_DEF_DEFINE(type, allocator, __VA_ARGS__) \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		template <class ReflectionBuilder> \
		void GAFF_REFLECTION_NAMESPACE::Reflection< type<__VA_ARGS__> >::BuildReflection(ReflectionBuilder& builder) \
		{ \
			builder

#define GAFF_TEMPLATE_REFLECTION_CLASS_DECLARE_NEW(type, ...) GAFF_REFLECTION_CLASS_DECLARE_NEW(type<__VA_ARGS__>)
#define GAFF_TEMPLATE_REFLECTION_DEFINE_END_NEW(type, ...) GAFF_REFLECTION_DEFINE_END_NEW(type<__VA_ARGS__>)

#define REFLECTION_CAST_PTR_NAME(T, name, object) \
	reinterpret_cast<T*>( \
		const_cast<Gaff::IReflectionDefinition&>((object)->getReflectionDefinition()).getInterface( \
			Gaff::FNV1aHash64Const(name), (object)->getBasePointer() \
		) \
	)

#define REFLECTION_CAST_NAME(T, name, object) *REFLECTION_CAST_PTR_NAME(T, name, &object)
#define REFLECTION_CAST_PTR(T, object) REFLECTION_CAST_PTR_NAME(T, #T, object)
#define REFLECTION_CAST(T, object) *REFLECTION_CAST_PTR(T, &object)
