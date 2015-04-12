/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
#include <Gaff_ReflectionDefinitions.h>

NS_SHIBBOLETH

template <class T> using EnumReflectionDefinition = Gaff::EnumReflectionDefinition<T, ProxyAllocator>;
template <class T> using ReflectionDefinition = Gaff::ReflectionDefinition<T, ProxyAllocator>;
using IEnumReflectionDefinition = Gaff::IEnumReflectionDefinition<ProxyAllocator>;

template <class T>
EnumReflectionDefinition<T>& GetEnumRefDef(void)
{
	assert(0 && "GetEnumRefDef() for type not overloaded.");
	static EnumReflectionDefinition<T> never_used;
	return never_used;
}

#define SHIB_REF_DEF(ClassName) REF_DEF(ClassName, ProxyAllocator)
#define SHIB_REF_IMPL(ClassName) REF_IMPL_ASSIGN(ClassName, ProxyAllocator, ProxyAllocator("Reflection"))

#define SHIB_ENUM_REF_DEF_EMBEDDED(EnumName, Type) \
	template<> EnumReflectionDefinition<Type>& GetEnumRefDef<Type>(void); \
	ENUM_REF_DEF_EMBEDDED(EnumName, Type, ProxyAllocator)

#define SHIB_ENUM_REF_IMPL_EMBEDDED(EnumName, Type) \
	template<> EnumReflectionDefinition<Type>& GetEnumRefDef<Type>(void) { return g##EnumName##RefDef; } \
	ENUM_REFL_IMPL_ASSIGN_EMBEDDED(EnumName, Type, ProxyAllocator, ProxyAllocator("Reflection"))


#define SHIB_ENUM_REF_DEF(EnumName) \
	template<> EnumReflectionDefinition<EnumName>& GetEnumRefDef<EnumName>(void); \
	ENUM_REF_DEF(EnumName, ProxyAllocator)

#define SHIB_ENUM_REF_IMPL(EnumName) \
	template<> EnumReflectionDefinition<EnumName>& GetEnumRefDef<EnumName>(void) { return g##EnumName##RefDef; } \
	ENUM_REF_IMPL_ASSIGN(EnumName, ProxyAllocator, ProxyAllocator("Reflection"))

NS_END
