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

#ifndef GAFF_REFLECTION_NAMESPACE
	#define GAFF_REFLECTION_NAMESPACE Shibboleth
#endif

#ifndef GAFF_REF_DEF_IS_PTR
	#define GAFF_REF_DEF_IS_PTR
#endif

#ifndef GAFF_REF_DEF_INIT
	#define GAFF_REF_DEF_INIT \
		GAFF_ASSERT(!g_ref_def); \
		const typename Gaff::RefDefInterface<ThisType, ProxyAllocator>* ref_def_interface = nullptr; \
		if constexpr (std::is_enum<ThisType>::value) { \
			ref_def_interface = reinterpret_cast< const typename Gaff::RefDefInterface<ThisType, ProxyAllocator>* >(GetApp().getReflectionManager().getEnumReflection(GetHash())); /* To calm the compiler, even though this should be compiled out ... */ \
		} else { \
			ref_def_interface = reinterpret_cast< const typename Gaff::RefDefInterface<ThisType, ProxyAllocator>* >(GetApp().getReflectionManager().getReflection(GetHash())); /* To calm the compiler, even though this should be compiled out ... */ \
		} \
		g_ref_def = static_cast< typename Gaff::RefDefType<ThisType, ProxyAllocator>* >( \
			const_cast< typename Gaff::RefDefInterface<ThisType, ProxyAllocator>* >(ref_def_interface) \
		); \
		if (g_ref_def) { \
			GAFF_ASSERT_MSG( \
				g_version.getHash() == g_ref_def->getReflectionInstance().getVersion(), \
				"Version hash for %s does not match!", \
				GetName() \
			); \
		} else { \
			ProxyAllocator allocator("Reflection"); \
			g_ref_def = SHIB_ALLOCT(GAFF_SINGLE_ARG(typename Gaff::RefDefType<ThisType, ProxyAllocator>), allocator); \
			g_ref_def->setAllocator(allocator); \
			BuildReflection(*g_ref_def); \
		}
#endif

#include "Shibboleth_ReflectionManager.h"
#include "Shibboleth_ProxyAllocator.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_Hashable.h"
#include "Shibboleth_IApp.h"
#include <Gaff_Reflection.h>

#define SHIB_REFLECTION_CLASS_DECLARE_NEW GAFF_REFLECTION_CLASS_DECLARE_NEW
#define SHIB_REFLECTION_CLASS_DEFINE_NEW GAFF_REFLECTION_CLASS_DEFINE_NEW
#define SHIB_REFLECTION_DECLARE_NEW(type) GAFF_REFLECTION_DECLARE_NEW(type, ProxyAllocator)
#define SHIB_REFLECTION_DEFINE_BEGIN_NEW(type) GAFF_REFLECTION_DEFINE_BEGIN_NEW(type, ProxyAllocator)
#define SHIB_REFLECTION_DEFINE_END_NEW GAFF_REFLECTION_DEFINE_END_NEW
#define SHIB_REFLECTION_DEFINE_NEW(type) \
	SHIB_REFLECTION_DEFINE_BEGIN_NEW(type) \
	SHIB_REFLECTION_DEFINE_END_NEW(type)

#define SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE_NEW(type, base) \
	SHIB_REFLECTION_DEFINE_BEGIN_NEW(type) \
		.BASE(base) \
	SHIB_REFLECTION_DEFINE_END_NEW(type)


#define SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE_NEW GAFF_TEMPLATE_REFLECTION_CLASS_DECLARE_NEW
#define SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_NEW GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE_NEW
#define SHIB_TEMPLATE_REFLECTION_DECLARE_NEW(type, ...) GAFF_TEMPLATE_REFLECTION_DECLARE_NEW(type, ProxyAllocator, __VA_ARGS__)
#define SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN_NEW(type, ...) GAFF_TEMPLATE_REFLECTION_DEFINE_BEGIN_NEW(type, ProxyAllocator, __VA_ARGS__)
#define SHIB_TEMPLATE_REFLECTION_DEFINE_END_NEW GAFF_TEMPLATE_REFLECTION_DEFINE_END_NEW


SHIB_REFLECTION_DECLARE_NEW(int8_t)
SHIB_REFLECTION_DECLARE_NEW(int16_t)
SHIB_REFLECTION_DECLARE_NEW(int32_t)
SHIB_REFLECTION_DECLARE_NEW(int64_t)
SHIB_REFLECTION_DECLARE_NEW(uint8_t)
SHIB_REFLECTION_DECLARE_NEW(uint16_t)
SHIB_REFLECTION_DECLARE_NEW(uint32_t)
SHIB_REFLECTION_DECLARE_NEW(uint64_t)
SHIB_REFLECTION_DECLARE_NEW(float)
SHIB_REFLECTION_DECLARE_NEW(double)
SHIB_REFLECTION_DECLARE_NEW(bool)
SHIB_REFLECTION_DECLARE_NEW(U8String)
