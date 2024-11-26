/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#include "Shibboleth_ReflectionDefines.h"
#include <Gaff_Hash.h>

#define GET_CLASS_ATTRS(T, Allocator) getClassAttrs<T>(Gaff::FNV1aHash64Const(#T))
#define GET_CLASS_ATTR(T) getClassAttr<T>(Gaff::FNV1aHash64Const(#T))
#define GET_VAR_ATTR(T, var_name) getVarAttr<T>(var_name, Gaff::FNV1aHash64Const(#T))
//#define GET_FUNC_ATTR(T, func_name) getFuncAttr<T>(func_name, Gaff::FNV1aHash64Const(#T))
//#define GET_STATIC_FUNC_ATTR(T, static_func_name) getStaticFuncAttr<T>(static_func_name, Gaff::FNV1aHash64Const(#T))
#define GET_ENUM_ATTR(T) getEnumAttr<T>(Gaff::FNV1aHash64Const(#T))
#define GET_ENUM_VALUE_ATTR(T, value_name) getEnumValueAttr<T>(value_name, Gaff::FNV1aHash64Const(#T))

#define REFLECTION_CAST(interface_type, object) Refl::ReflectionCast<interface_type>(object, Gaff::FNV1aHash64Const(#interface_type))


NS_REFLECTION

template <class Derived, class Base>
Derived* ReflectionCast(Base* object)
{
	if constexpr (std::is_same_v<Base, Derived>) {
		return object;
	} else {
		return (object) ? object->getReflectionDefinition().template getInterface<Derived>(object->getBasePointer()) : nullptr;
	}
}

template <class Derived, class Base>
const Derived* ReflectionCast(const Base* object)
{
	return ReflectionCast<Derived, Base>(const_cast<Base*>(object));
}

template <class Derived, class Base>
Derived& ReflectionCast(Base& object)
{
	Derived* const derived = ReflectionCast<Derived, Base>(&object);
	GAFF_ASSERT(derived);

	return *derived;
}

template <class Derived, class Base>
const Derived& ReflectionCast(const Base& object)
{
	return ReflectionCast<Derived, Base>(const_cast<Base&>(object));
}


// Manual interface name hash passed in.
template <class Derived, class Base>
Derived* ReflectionCast(Base* object, Gaff::Hash64 interface_name)
{
	if constexpr (std::is_same_v<Base, Derived>) {
		return object;
	} else {
		return (object) ? reinterpret_cast<Derived*>(object->getReflectionDefinition().getInterface(interface_name, object->getBasePointer())) : nullptr;
	}
}

template <class Derived, class Base>
const Derived* ReflectionCast(const Base* object, Gaff::Hash64 interface_name)
{
	return ReflectionCast<Derived, Base>(const_cast<Base*>(object), interface_name);
}

template <class Derived, class Base>
Derived& ReflectionCast(Base& object, Gaff::Hash64 interface_name)
{
	Derived* const derived = ReflectionCast<Derived, Base>(&object, interface_name);
	GAFF_ASSERT(derived);

	return *derived;
}

template <class Derived, class Base>
const Derived& ReflectionCast(const Base& object, Gaff::Hash64 interface_name)
{
	return ReflectionCast<Derived, Base>(const_cast<Base&>(object), interface_name);
}

NS_END
