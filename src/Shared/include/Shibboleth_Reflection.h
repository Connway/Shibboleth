/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#define GAFF_REFLECTION_NAMESPACE Shibboleth

#include "Shibboleth_ProxyAllocator.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_IApp.h"
#include <Gaff_SerializeInterfaces.h>
#include <Gaff_ReflectionDefinition.h>
#include <Gaff_ReflectionVersion.h>
#include <Gaff_Reflection.h>

NS_SHIBBOLETH

GAFF_REFLECTION_DECLARE_DEFAULT_AND_POD();

#define SHIB_REFLECTION_DECLARE_BASE(type) \
		void load(Gaff::ISerializeReader& reader, void* object) const override \
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
		void Load(Gaff::ISerializeReader& reader, type& object) \
		{ \
			GAFF_ASSERT(g_reflection_definition); \
			g_reflection_definition->load(reader, object); \
		} \
		void Save(Gaff::ISerializeWriter& writer, const type& object) const \
		{ \
			GAFF_ASSERT(g_reflection_definition); \
			g_reflection_definition->save(writer, object); \
		} \
		static const Gaff::ReflectionDefinition<type, ProxyAllocator>& GetReflectionDefinition(void) \
		{ \
			GAFF_ASSERT(g_reflection_definition); \
			return *g_reflection_definition; \
		} \
		static void SetAllocator(const ProxyAllocator& a) \
		{ \
			if (g_reflection_definition) {\
				g_reflection_definition->setAllocator(a); \
			} \
			g_on_defined_callbacks.set_allocator(a); \
		} \
		static void Init(void); \
	private: \
		static Gaff::ReflectionDefinition<type, ProxyAllocator>* g_reflection_definition; \
	};

#define SHIB_REFLECTION_DECLARE(type) \
NS_SHIBBOLETH \
	template <> \
	GAFF_REFLECTION_DECLARE_COMMON(type, ProxyAllocator) \
	constexpr static Gaff::Hash64 GetHash(void) \
	{ \
		return Gaff::FNV1aHash64Const(#type); \
	} \
	constexpr static const char* GetName(void) \
	{ \
		return #type; \
	} \
	SHIB_REFLECTION_DECLARE_BASE(type) \
NS_END

#define SHIB_REFLECTION_CLASS_DECLARE(type) GAFF_REFLECTION_CLASS_DECLARE(type, Shibboleth::ProxyAllocator)

#define SHIB_REFLECTION_DEFINE(type) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
	SHIB_REFLECTION_DEFINE_END(type)

#define SHIB_REFLECTION_DEFINE_BEGIN_CUSTOM_BUILDER(type) \
NS_SHIBBOLETH \
	Gaff::ReflectionDefinition<type, ProxyAllocator>* Reflection<type>::g_reflection_definition = nullptr; \
	GAFF_REFLECTION_DEFINE_BASE(type, ProxyAllocator); \
	void Reflection<type>::Init() \
	{ \
		if (g_reflection_definition) { \
			return; \
		} \
		g_reflection_definition = reinterpret_cast< Gaff::ReflectionDefinition<type, ProxyAllocator>* >( \
			const_cast< Gaff::IReflectionDefinition* >( \
				GetApp().getReflection(Gaff::FNV1aHash64Const(#type)) \
			) \
		); \
		if (g_reflection_definition) { \
			Gaff::ReflectionVersion<type> version; \
			BuildReflection(version); \
			GAFF_ASSERT_MSG( \
				version.getHash() == g_reflection_definition->getReflectionInstance().getVersion(), \
				"Version hash for " #type " does not match!" \
			); \
		} else { \
			g_reflection_definition = reinterpret_cast< Gaff::ReflectionDefinition<type, ProxyAllocator>* >( \
				ShibbolethAllocate( \
					sizeof(Gaff::ReflectionDefinition<type, ProxyAllocator>), \
					GetPoolIndex("Reflection") \
				) \
			); \
			Gaff::Construct(g_reflection_definition); \
			g_reflection_definition->setAllocator(ProxyAllocator("Reflection")); \
			BuildReflection(*g_reflection_definition);

#define SHIB_REFLECTION_DEFINE_BEGIN(type) \
NS_SHIBBOLETH \
	template <class ReflectionBuilder> \
	void Reflection<type>::BuildReflection(ReflectionBuilder& builder) \
	{ \
		type::BuildReflection(builder); \
	} \
NS_END \
	SHIB_REFLECTION_DEFINE_BEGIN_CUSTOM_BUILDER(type)

#define SHIB_REFLECTION_DEFINE_END(type, ...) \
			GetApp().registerReflection(GetHash(), *g_reflection_definition); \
			g_reflection_definition->finish(); \
		} \
	} \
NS_END

#define SHIB_REFLECTION_BUILDER_BEGIN GAFF_REFLECTION_BUILDER_BEGIN
#define SHIB_REFLECTION_BUILDER_END GAFF_REFLECTION_BUILDER_END

#define SHIB_REFLECTION_CLASS_DEFINE(type) \
	SHIB_REFLECTION_CLASS_DEFINE_BEGIN(type) \
	SHIB_REFLECTION_CLASS_DEFINE_END(type)

#define SHIB_REFLECTION_CLASS_DEFINE_BEGIN(type) GAFF_REFLECTION_CLASS_DEFINE_BEGIN(type, Shibboleth::ProxyAllocator)
#define SHIB_REFLECTION_CLASS_DEFINE_END GAFF_REFLECTION_CLASS_DEFINE_END

#define SHIB_TEMPLATE_REFLECTION_DECLARE(type, ...) \
NS_SHIBBOLETH \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	GAFF_REFLECTION_DECLARE_COMMON(GAFF_SINGLE_ARG(type<__VA_ARGS__>), ProxyAllocator) \
	constexpr static Gaff::Hash64 GetHash(void) \
	{ \
		return Gaff::CalcTemplateHash<__VA_ARGS__>(Gaff::FNV1aHash64Const(#type)); \
	} \
	static const char* GetName(void) \
	{ \
		static char s_name[128] = { 0 }; \
		if (!s_name[0]) { \
			char format[64] = { 0 }; \
			int32_t index = snprintf(format, 32, "%s<", #type); \
			for (int32_t i = 0; i < Gaff::GetNumArgs<__VA_ARGS__>(); ++i) { \
				if (i) { \
					format[index++] = ','; \
					format[index++] = ' '; \
				} \
				format[index++] = '%'; \
				format[index++] = 's'; \
			} \
			format[index++] = '>'; \
			snprintf(s_name, 128, format, GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_GET_NAME, __VA_ARGS__)); \
		} \
		return s_name; \
	} \
	SHIB_REFLECTION_DECLARE_BASE(GAFF_SINGLE_ARG(type<__VA_ARGS__>)) \
NS_END

#define SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(type, ...) GAFF_TEMPLATE_REFLECTION_CLASS_DECLARE(type, Shibboleth::ProxyAllocator, __VA_ARGS__)

#define SHIB_TEMPLATE_REFLECTION_DEFINE(type, ...) \
	SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(type, __VA_ARGS__) \
	SHIB_TEMPLATE_REFLECTION_DEFINE_END(type, __VA_ARGS__)

#define SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN_CUSTOM_BUILDER(type, ...) \
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
		g_reflection_definition = reinterpret_cast< Gaff::ReflectionDefinition<type<__VA_ARGS__>, ProxyAllocator>* >( \
			const_cast< Gaff::IReflectionDefinition* >( \
				Shibboleth::GetApp().getReflection(GetHash()) \
			) \
		); \
		if (g_reflection_definition) { \
			Gaff::ReflectionVersion< type<__VA_ARGS__> > version; \
			BuildReflection(version); \
			GAFF_ASSERT_MSG( \
				version.getHash() == g_reflection_definition->getReflectionInstance().getVersion(), \
				"Version hash for %s does not match!", \
				GetName() \
			); \
		} else { \
			g_reflection_definition = reinterpret_cast< Gaff::ReflectionDefinition<type<__VA_ARGS__>, ProxyAllocator>* >( \
				ShibbolethAllocate( \
					sizeof(Gaff::ReflectionDefinition<type<__VA_ARGS__>, ProxyAllocator>), \
					GetPoolIndex("Reflection") \
				) \
			); \
			Gaff::Construct(g_reflection_definition); \
			g_reflection_definition->setAllocator(ProxyAllocator("Reflection")); \
			BuildReflection(*g_reflection_definition);

#define SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(type, ...) \
NS_SHIBBOLETH \
	template < GAFF_FOR_EACH_COMMA(GAFF_TEMPLATE_REFLECTION_CLASS, __VA_ARGS__) > \
	template <class ReflectionBuilder> \
	void Shibboleth::Reflection< type<__VA_ARGS__> >::BuildReflection(ReflectionBuilder& builder) \
	{ \
		type<__VA_ARGS__>::BuildReflection(builder); \
	} \
NS_END \
	SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN_CUSTOM_BUILDER(type, __VA_ARGS__)

#define SHIB_TEMPLATE_REFLECTION_DEFINE_END SHIB_REFLECTION_DEFINE_END

#define SHIB_TEMPLATE_REFLECTION_BUILDER_BEGIN GAFF_TEMPLATE_REFLECTION_BUILDER_BEGIN
#define SHIB_TEMPLATE_REFLECTION_BUILDER_END GAFF_TEMPLATE_REFLECTION_BUILDER_END

#define SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(type, ...) GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(type, Shibboleth::ProxyAllocator, __VA_ARGS__)
#define SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_END GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_END 

NS_END
