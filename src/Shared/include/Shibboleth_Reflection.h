/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
#include <Gaff_IReflectionObject.h>

NS_SHIBBOLETH

GAFF_REFLECTION_DECLARE_DEFAULT_AND_POD();

#define SHIB_REFLECTION_CLASS_DECLARE(type) \
	GAFF_REFLECTION_CLASS_DECLARE_BASE(type, ProxyAllocator); \
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
	private: \
		static Gaff::ReflectionDefinition<type, ProxyAllocator>* g_reflection_definition; \

#define SHIB_REFLECTION_CLASS_DEFINE_BEGIN(type) \
	GAFF_REFLECTION_CLASS_DEFINE_BEGIN_BASE_BEGIN(type, ProxyAllocator); \
	Gaff::ReflectionDefinition<type, ProxyAllocator>* Reflection<type>::g_reflection_definition = nullptr; \
	void Reflection<type>::Init(void) \
	{ \
		g_reflection_definition = reinterpret_cast< Gaff::ReflectionDefinition<type, ProxyAllocator>* >( \
			const_cast< Gaff::IReflectionDefinition* >( \
				Shibboleth::GetApp().getReflection(REFL_HASH_CONST(#type)) \
			) \
		); \
		if (g_reflection_definition) { \
			Gaff::ReflectionVersion<type> version; \
			type::ReflectionInit(version); \
			GAFF_ASSERT_MSG( \
				version.getHash() == g_reflection_definition->getVersionHash(), \
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
			Shibboleth::GetApp().registerReflection(REFL_HASH_CONST(#type),  g_reflection_definition); \
			g_reflection_definition->setAllocator(ProxyAllocator("Reflection")); \
			g_reflection_definition->setReflectionInstance(&g_instance); \
			type::ReflectionInit(*g_reflection_definition); \
		} \
	} \
	GAFF_REFLECTION_CLASS_DEFINE_BEGIN_BASE_END(type)

#define SHIB_REFLECTION_CLASS_DEFINE_END GAFF_REFLECTION_CLASS_DEFINE_END

NS_END
