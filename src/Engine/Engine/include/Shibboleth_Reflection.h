/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#ifndef GAFF_REFLECTION_NAMESPACE
	#define GAFF_REFLECTION_NAMESPACE Shibboleth
#endif

#include "Shibboleth_ReflectionManager.h"
#include "Shibboleth_ProxyAllocator.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_Hashable.h"
#include "Shibboleth_IApp.h"
#include <Gaff_Reflection.h>

#define SHIB_REFLECTION_DECLARE_BASE(type) \
		void load(const Gaff::ISerializeReader& reader, void* object) const override \
		{ \
			GAFF_ASSERT(g_reflection_definition); \
			GAFF_ASSERT(object); \
			g_reflection_definition->load(reader, object); \
		} \
		void save(Gaff::ISerializeWriter& writer, const void* object) const override \
		{ \
			GAFF_ASSERT(g_reflection_definition); \
			GAFF_ASSERT(object); \
			g_reflection_definition->save(writer, object); \
		} \
		static void Load(const Gaff::ISerializeReader& reader, type& object) \
		{ \
			GAFF_ASSERT(g_reflection_definition); \
			g_reflection_definition->load(reader, object); \
		} \
		static void Save(Gaff::ISerializeWriter& writer, const type& object) \
		{ \
			GAFF_ASSERT(g_reflection_definition); \
			g_reflection_definition->save(writer, object); \
		} \
		static const typename Gaff::RefDefType<type, ProxyAllocator>& GetReflectionDefinition(void) \
		{ \
			GAFF_ASSERT(g_reflection_definition); \
			return *g_reflection_definition; \
		} \
		static void SetAllocator(const ProxyAllocator& a) \
		{ \
			if (g_reflection_definition) { \
				g_reflection_definition->setAllocator(a); \
			} \
			g_on_defined_callbacks.set_allocator(a); \
		} \
		static void Init(void); \
	private: \
		static typename Gaff::RefDefType<type, ProxyAllocator>* g_reflection_definition; \
	};

#define SHIB_REFLECTION_DECLARE(type) \
	NS_HASHABLE \
		GAFF_CLASS_HASHABLE(type) \
	NS_END \
	NS_SHIBBOLETH \
		template <> \
		GAFF_REFLECTION_DECLARE_COMMON(type, ProxyAllocator) \
		SHIB_REFLECTION_DECLARE_BASE(type) \
	NS_END

#define SHIB_ENUM_REFLECTION_DECLARE SHIB_REFLECTION_DECLARE
#define SHIB_ENUM_REFLECTION_DEFINE SHIB_REFLECTION_EXTERNAL_DEFINE
#define SHIB_ENUM_REFLECTION_BEGIN SHIB_REFLECTION_BUILDER_BEGIN
#define SHIB_ENUM_REFLECTION_END SHIB_REFLECTION_BUILDER_END

#define SHIB_REFLECTION_CLASS_DECLARE(type) GAFF_REFLECTION_CLASS_DECLARE(type, Shibboleth::ProxyAllocator)

#define SHIB_REFLECTION_DEFINE(type) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
	SHIB_REFLECTION_DEFINE_END(type)

#define SHIB_REFLECTION_EXTERNAL_DEFINE(type) \
	SHIB_REFLECTION_EXTERNAL_DEFINE_BEGIN(type) \
	SHIB_REFLECTION_EXTERNAL_DEFINE_END(type)

#define SHIB_REFLECTION_EXTERNAL_DEFINE_BEGIN(type) \
NS_SHIBBOLETH \
	typename Gaff::RefDefType<type, ProxyAllocator>* Reflection<type>::g_reflection_definition = nullptr; \
	GAFF_REFLECTION_DEFINE_BASE(type, ProxyAllocator); \
	void Reflection<type>::Init() \
	{ \
		if (g_reflection_definition) { \
			return; \
		} \
		const typename Gaff::RefDefInterface<type, ProxyAllocator>* ref_def_interface = nullptr; \
		if constexpr (std::is_enum<type>::value) { \
			ref_def_interface = reinterpret_cast< const typename Gaff::RefDefInterface<type, ProxyAllocator>* >(GetApp().getReflectionManager().getEnumReflection(GetHash())); /* To calm the compiler, even though this should be compiled out ... */ \
		} else { \
			ref_def_interface = reinterpret_cast< const typename Gaff::RefDefInterface<type, ProxyAllocator>* >(GetApp().getReflectionManager().getReflection(GetHash())); /* To calm the compiler, even though this should be compiled out ... */ \
		} \
		g_reflection_definition = static_cast< typename Gaff::RefDefType<type, ProxyAllocator>* >( \
			const_cast< typename Gaff::RefDefInterface<type, ProxyAllocator>* >(ref_def_interface) \
		); \
		if (g_reflection_definition) { \
			BuildReflection(g_version); \
			GAFF_ASSERT_MSG( \
				g_version.getHash() == g_reflection_definition->getReflectionInstance().getVersion(), \
				"Version hash for " #type " does not match!" \
			); \
			g_defined = true; \
		} else { \
			ProxyAllocator allocator("Reflection"); \
			g_reflection_definition = SHIB_ALLOCT(GAFF_SINGLE_ARG(typename Gaff::RefDefType<type, ProxyAllocator>), allocator); \
			g_reflection_definition->setAllocator(allocator); \
			BuildReflection(*g_reflection_definition); \
			BuildReflection(g_version);

#define SHIB_REFLECTION_EXTERNAL_DEFINE_END SHIB_REFLECTION_DEFINE_END

#define SHIB_REFLECTION_DEFINE_BEGIN(type) \
NS_SHIBBOLETH \
	template <class ReflectionBuilder> \
	void Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
	{ \
		type::BuildReflection(builder); \
	} \
NS_END \
	SHIB_REFLECTION_EXTERNAL_DEFINE_BEGIN(type)

#define SHIB_REFLECTION_DEFINE_END(type, ...) \
			GetApp().getReflectionManager().registerReflection(g_reflection_definition); \
		} \
	} \
NS_END

#define SHIB_REFLECTION_BUILDER_BEGIN GAFF_REFLECTION_BUILDER_BEGIN
#define SHIB_REFLECTION_BUILDER_END GAFF_REFLECTION_BUILDER_END

#define SHIB_REFLECTION_CLASS_DEFINE(type) \
	SHIB_REFLECTION_CLASS_DEFINE_BEGIN_NO_BUILD(type) \
	SHIB_REFLECTION_CLASS_DEFINE_END(type)

#define SHIB_REFLECTION_CLASS_DEFINE_BEGIN_NO_BUILD(type) GAFF_REFLECTION_CLASS_DEFINE_BEGIN_NO_BUILD(type, Shibboleth::ProxyAllocator)
#define SHIB_REFLECTION_CLASS_DEFINE_BEGIN(type) GAFF_REFLECTION_CLASS_DEFINE_BEGIN(type, Shibboleth::ProxyAllocator)
#define SHIB_REFLECTION_CLASS_DEFINE_END GAFF_REFLECTION_CLASS_DEFINE_END

#define SHIB_REFLECTION_CLASS_DEFINE_WITH_BASE_NO_INHERITANCE(type, base_class) \
	SHIB_REFLECTION_CLASS_DEFINE_BEGIN(type) \
		.BASE(base_class) \
	SHIB_REFLECTION_CLASS_DEFINE_END(type)

#define SHIB_REFLECTION_CLASS_DEFINE_WITH_BASE(type, base_class) \
	SHIB_REFLECTION_CLASS_DEFINE_BEGIN(type) \
		.base<base_class>() \
	SHIB_REFLECTION_CLASS_DEFINE_END(type)


// Temlate Reflection
#define SHIB_TEMPLATE_REFLECTION_DECLARE(type, ...) \
	NS_HASHABLE \
		GAFF_TEMPLATE_CLASS_HASHABLE(type, __VA_ARGS__) \
	NS_END \
	NS_SHIBBOLETH \
		template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
		GAFF_REFLECTION_DECLARE_COMMON(GAFF_SINGLE_ARG(type<__VA_ARGS__>), ProxyAllocator) \
		SHIB_REFLECTION_DECLARE_BASE(GAFF_SINGLE_ARG(type<__VA_ARGS__>)) \
	NS_END

#define SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(type, ...) GAFF_TEMPLATE_REFLECTION_CLASS_DECLARE(type, Shibboleth::ProxyAllocator, __VA_ARGS__)

#define SHIB_TEMPLATE_REFLECTION_DEFINE(type, ...) \
	SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(type, __VA_ARGS__) \
	SHIB_TEMPLATE_REFLECTION_DEFINE_END(type, __VA_ARGS__)

#define SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE(type, ...) \
	SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, __VA_ARGS__) \
	SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE_END(type, __VA_ARGS__)

#define SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, ...) \
NS_SHIBBOLETH \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	Gaff::ReflectionDefinition< type<__VA_ARGS__>, Shibboleth::ProxyAllocator>* Shibboleth::Reflection< type<__VA_ARGS__> >::g_reflection_definition; \
	GAFF_TEMPLATE_REFLECTION_DEFINE_BASE(type, Shibboleth::ProxyAllocator, __VA_ARGS__); \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	void Shibboleth::Reflection< type<__VA_ARGS__> >::Init() \
	{ \
		if (g_reflection_definition) { \
			return; \
		} \
		g_reflection_definition = static_cast< Gaff::ReflectionDefinition<type<__VA_ARGS__>, ProxyAllocator>* >( \
			const_cast< Gaff::IReflectionDefinition* >( \
				GetApp().getReflectionManager().getReflection(GetHash()) \
			) \
		); \
		if (g_reflection_definition) { \
			BuildReflection(g_version); \
			GAFF_ASSERT_MSG( \
				g_version.getHash() == g_reflection_definition->getReflectionInstance().getVersion(), \
				"Version hash for %s does not match!", \
				GetName() \
			); \
		} else { \
			ProxyAllocator allocator("Reflection"); \
			g_reflection_definition = SHIB_ALLOCT(GAFF_SINGLE_ARG(Gaff::ReflectionDefinition<type<__VA_ARGS__>, ProxyAllocator>), allocator); \
			g_reflection_definition->setAllocator(allocator); \
			BuildReflection(*g_reflection_definition); \
			BuildReflection(g_version);

#define SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE_END SHIB_REFLECTION_DEFINE_END

#define SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(type, ...) \
NS_SHIBBOLETH \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	template <class ReflectionBuilder> \
	void Shibboleth::Reflection< type<__VA_ARGS__> >::BuildReflection(ReflectionBuilder& builder) \
	{ \
		type<__VA_ARGS__>::BuildReflection(builder); \
	} \
NS_END \
	SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE_BEGIN(type, __VA_ARGS__)

#define SHIB_TEMPLATE_REFLECTION_DEFINE_END SHIB_REFLECTION_DEFINE_END

#define SHIB_TEMPLATE_REFLECTION_BUILDER_BEGIN GAFF_TEMPLATE_REFLECTION_BUILDER_BEGIN
#define SHIB_TEMPLATE_REFLECTION_BUILDER_END GAFF_TEMPLATE_REFLECTION_BUILDER_END

#define SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE(type, ...) \
	SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN_NO_BUILD(type, __VA_ARGS__) \
	SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_END(type, __VA_ARGS__)

#define SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(type, ...) GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(type, Shibboleth::ProxyAllocator, __VA_ARGS__)
#define SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN_NO_BUILD(type, ...) GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN_NO_BUILD(type, Shibboleth::ProxyAllocator, __VA_ARGS__)
#define SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_END GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_END

SHIB_REFLECTION_DECLARE(int8_t)
SHIB_REFLECTION_DECLARE(int16_t)
SHIB_REFLECTION_DECLARE(int32_t)
SHIB_REFLECTION_DECLARE(int64_t)
SHIB_REFLECTION_DECLARE(uint8_t)
SHIB_REFLECTION_DECLARE(uint16_t)
SHIB_REFLECTION_DECLARE(uint32_t)
SHIB_REFLECTION_DECLARE(uint64_t)
SHIB_REFLECTION_DECLARE(float)
SHIB_REFLECTION_DECLARE(double)
SHIB_REFLECTION_DECLARE(bool)
SHIB_REFLECTION_DECLARE(U8String)
