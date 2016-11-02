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

#include "Shibboleth_ProxyAllocator.h"
#include <Gaff_SerializeInterfaces.h>
#include <Gaff_ReflectionDefinition.h>
#include <Gaff_Assert.h>

NS_SHIBBOLETH

GAFF_REFLECTION_DEFINE_DEFAULT_AND_POD();

#define SHIB_REFLECTION_CLASS_DECLARE(type) \
	GAFF_REFLECTION_CLASS_DECLARE_BASE(type); \
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
		static const Gaff::IReflectionDefinition* GetReflectionDefinition(void) \
		{ \
			return g_reflection_definition; \
		} \
	private: \
		static Gaff::ReflectionDefinition<type, ProxyAllocator>* g_reflection_definition; \
	}

#define SHIB_REFLECTION_CLASS_DEFINE_BEGIN(type) \
	Gaff::ReflectionDefinition<type, ProxyAllocator>* Reflection<type>::g_reflection_definition = nullptr; \
	void Reflection<type>::Init(void) \
	{ \
		g_reflection_definition->setAllocator(ProxyAllocator("Reflection"))

#define SHIB_REFLECTION_CLASS_DEFINE_END GAFF_REFLECTION_CLASS_DEFINE_END

NS_END
