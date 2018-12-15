/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_ProxyAllocator.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_IApp.h"
#include <Gaff_EnumReflection.h>

#define SHIB_ENUM_REFLECTION_DECLARE_BASE(type) \
		void load(Gaff::ISerializeReader& reader, void* object) const override \
		{ \
			GAFF_ASSERT(g_enum_reflection_definition); \
			GAFF_ASSERT(object); \
			g_enum_reflection_definition->load(reader, object); \
		} \
		void save(Gaff::ISerializeWriter& writer, const void* object) const override \
		{ \
			GAFF_ASSERT(g_enum_reflection_definition); \
			GAFF_ASSERT(object); \
			g_enum_reflection_definition->save(writer, object); \
		} \
		static void Load(Gaff::ISerializeReader& reader, type value) \
		{ \
			GAFF_ASSERT(g_enum_reflection_definition); \
			g_enum_reflection_definition->load(reader, &value); \
		} \
		static void Save(Gaff::ISerializeWriter& writer, type value) \
		{ \
			GAFF_ASSERT(g_enum_reflection_definition); \
			g_enum_reflection_definition->save(writer, &value); \
		} \
		static Gaff::Hash64 GetVersion(void) \
		{ \
			return g_enum_reflection_definition->getVersion(); \
		} \
		static const Gaff::EnumReflectionDefinition<type, ProxyAllocator> & GetReflectionDefinition() \
		{ \
			GAFF_ASSERT(g_enum_reflection_definition); \
			return *g_enum_reflection_definition; \
		} \
		static void Init(void); \
	private: \
		static Gaff::EnumReflectionDefinition<type, ProxyAllocator>* g_enum_reflection_definition; \
	}; \
}

#define SHIB_ENUM_REFLECTION_DECLARE(type) \
	GAFF_ENUM_REFLECTION_DECLARE_COMMON(type, ProxyAllocator) \
	SHIB_ENUM_REFLECTION_DECLARE_BASE(type)

#define SHIB_ENUM_REFLECTION_DEFINE_BEGIN_CUSTOM_INIT(type, allocator) \
	Gaff::EnumReflectionDefinition<type, allocator>* GAFF_REFLECTION_NAMESPACE::EnumReflection<type>::g_enum_reflection_definition = nullptr; \
	GAFF_ENUM_REFLECTION_DEFINE_BASE(type, allocator); \
	void GAFF_REFLECTION_NAMESPACE::EnumReflection<type>::Init() \
	{ \
		if (g_enum_reflection_definition) { \
			return; \
		} \
		g_enum_reflection_definition = reinterpret_cast< Gaff::EnumReflectionDefinition<type, ProxyAllocator>* >( \
			const_cast< Gaff::IEnumReflectionDefinition* >( \
				GetApp().getReflectionManager().getEnumReflection(Gaff::FNV1aHash64Const(#type)) \
			) \
		); \
		if (g_enum_reflection_definition) { \
			Gaff::EnumReflectionDefinition<type, ProxyAllocator> temp; \
			BuildReflection(temp); \
			GAFF_ASSERT_MSG( \
				temp.getVersion() == g_enum_reflection_definition->getVersion(), \
				"Version hash for " #type " does not match!" \
			); \
		} else { \
			g_enum_reflection_definition = reinterpret_cast< Gaff::EnumReflectionDefinition<type, ProxyAllocator>* >( \
				ShibbolethAllocate( \
					sizeof(Gaff::EnumReflectionDefinition<type, ProxyAllocator>), \
					GetPoolIndex("Reflection") \
				) \
			); \
			Gaff::Construct(g_enum_reflection_definition); \
			g_enum_reflection_definition->setAllocator(ProxyAllocator("Reflection")); \
			BuildReflection(*g_enum_reflection_definition);

#define SHIB_ENUM_REFLECTION_DEFINE_END_CUSTOM_INIT(type) \
			GetApp().getReflectionManager().registerEnumReflection(g_enum_reflection_definition); \
		} \
	}

#define SHIB_ENUM_REFLECTION_DEFINE_BEGIN(type) \
	SHIB_ENUM_REFLECTION_DEFINE_BEGIN_CUSTOM_INIT(type, Shibboleth::ProxyAllocator) \
	SHIB_ENUM_REFLECTION_DEFINE_END_CUSTOM_INIT(type) \
	SHIB_ENUM_REFLECTION_BUILDER_BEGIN(type)

#define SHIB_ENUM_REFLECTION_DEFINE_END GAFF_ENUM_REFLECTION_DEFINE_END

#define SHIB_ENUM_REFLECTION_BUILDER_BEGIN GAFF_ENUM_REFLECTION_BUILDER_BEGIN
#define SHIB_ENUM_REFLECTION_BUILDER_END GAFF_ENUM_REFLECTION_BUILDER_END
