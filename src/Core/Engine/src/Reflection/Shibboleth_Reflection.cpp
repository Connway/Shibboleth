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

#include "Attributes/Shibboleth_EngineAttributesCommon.h"

SHIB_REFLECTION_DEFINE(int8_t)
SHIB_REFLECTION_DEFINE(int16_t)
SHIB_REFLECTION_DEFINE(int32_t)
SHIB_REFLECTION_DEFINE(int64_t)
SHIB_REFLECTION_DEFINE(uint8_t)
SHIB_REFLECTION_DEFINE(uint16_t)
SHIB_REFLECTION_DEFINE(uint32_t)
SHIB_REFLECTION_DEFINE(uint64_t)
SHIB_REFLECTION_DEFINE(float)
SHIB_REFLECTION_DEFINE(double)
SHIB_REFLECTION_DEFINE(bool)

SHIB_REFLECTION_DEFINE_BEGIN(Gaff::Hash32)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute{
			Shibboleth::ScriptFlagsAttribute::Flag::ValueType
		}
	)

	.template ctor<const Gaff::Hash32&>()
	.template ctor<>()

	.template opGreaterThanOrEqual<>()
	.template opGreaterThan<>()

	.template opLessThanOrEqual<>()
	.template opLessThan<>()

	.template opNotEqual<>()
	.template opEqual<>()

	.template opComparison<>()

	.func("getHash", &Gaff::Hash32::getHash)
	.func("setHash", &Gaff::Hash32::setHash)
SHIB_REFLECTION_DEFINE_END(Gaff::Hash32)

SHIB_REFLECTION_DEFINE_BEGIN(Gaff::Hash64)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute{
			Shibboleth::ScriptFlagsAttribute::Flag::ValueType
		}
	)

	.template ctor<const Gaff::Hash64&>()
	.template ctor<>()

	.template opGreaterThanOrEqual<>()
	.template opGreaterThan<>()

	.template opLessThanOrEqual<>()
	.template opLessThan<>()

	.template opNotEqual<>()
	.template opEqual<>()

	.template opComparison<>()

	.func("getHash", &Gaff::Hash64::getHash)
	.func("setHash", &Gaff::Hash64::setHash)
SHIB_REFLECTION_DEFINE_END(Gaff::Hash64)

// $TODO: May want to reflect the Gaff versions of these in the future.
SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::U8String)
	.setInstanceHash(Shibboleth::HashStringInstance<Shibboleth::ProxyAllocator>)
	.serialize(Shibboleth::LoadString<Shibboleth::ProxyAllocator>, Shibboleth::SaveString<Shibboleth::ProxyAllocator>)
SHIB_REFLECTION_DEFINE_END(Shibboleth::U8String)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashString32<>)
	// $TODO: String factory support?
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute{
			Shibboleth::ScriptFlagsAttribute::Flag::ValueType
		}
	)

	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>
	)

	.template ctor<const Shibboleth::HashStringView32<>&>()
	.template ctor<const Shibboleth::HashString32<>&>()
	.template ctor<const Shibboleth::U8String&>()
	.template ctor<>()

	.template opAssignment< Shibboleth::HashStringView32<> >()
	.template opAssignment< Shibboleth::HashString32<> >()
	.template opAssignment<Shibboleth::U8String>()

	.template opGreaterThanOrEqual< Shibboleth::HashStringView32<> >()
	.template opGreaterThan< Shibboleth::HashStringView32<> >()

	.template opLessThanOrEqual< Shibboleth::HashStringView32<> >()
	.template opLessThan< Shibboleth::HashStringView32<> >()

	.template opNotEqual< Shibboleth::HashStringView32<> >()
	.template opEqual< Shibboleth::HashStringView32<> >()

	.template opComparison< Shibboleth::HashStringView32<> >()
	.template opGreaterThanOrEqual<>()
	.template opGreaterThan<>()

	.template opLessThanOrEqual<>()
	.template opLessThan<>()

	.template opNotEqual<>()
	.template opEqual<>()

	.template opComparison<>()

	.func("getHash", &Shibboleth::HashString32<>::getHash)
	.func("clear", &Shibboleth::HashString32<>::clear)
	.func("empty", &Shibboleth::HashString32<>::empty)
	.func("size", &Shibboleth::HashString32<>::size)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashString32<>)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashString64<>)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute{
			Shibboleth::ScriptFlagsAttribute::Flag::ValueType
		}
	)

	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>
	)

	.template ctor<const Shibboleth::HashStringView64<>&>()
	.template ctor<const Shibboleth::HashString64<>&>()
	.template ctor<const Shibboleth::U8String&>()
	.template ctor<>()

	.template opAssignment< Shibboleth::HashStringView64<> >()
	.template opAssignment< Shibboleth::HashString64<> >()
	.template opAssignment<Shibboleth::U8String>()

	.template opGreaterThanOrEqual< Shibboleth::HashStringView64<> >()
	.template opGreaterThan< Shibboleth::HashStringView64<> >()

	.template opLessThanOrEqual< Shibboleth::HashStringView64<> >()
	.template opLessThan< Shibboleth::HashStringView64<> >()

	.template opNotEqual< Shibboleth::HashStringView64<> >()
	.template opEqual< Shibboleth::HashStringView64<> >()

	.template opComparison< Shibboleth::HashStringView64<> >()

	.template opGreaterThanOrEqual<>()
	.template opGreaterThan<>()

	.template opLessThanOrEqual<>()
	.template opLessThan<>()

	.template opNotEqual<>()
	.template opEqual<>()

	.template opComparison<>()

	.func("getHash", &Shibboleth::HashString64<>::getHash)
	.func("clear", &Shibboleth::HashString64<>::clear)
	.func("empty", &Shibboleth::HashString64<>::empty)
	.func("size", &Shibboleth::HashString64<>::size)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashString64<>)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashStringNoString32<>)
	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashStringNoString32<>)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashStringNoString64<>)
	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashStringNoString64<>)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashStringView32<>)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute{
			Shibboleth::ScriptFlagsAttribute::Flag::ValueType
		}
	)

	.setInstanceHash(Shibboleth::HashStringViewInstanceHash< char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32> >)
	.serialize(
		Shibboleth::LoadHashStringView< char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32> >,
		Shibboleth::SaveHashStringView< char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32> >
	)

	.template ctor<const Shibboleth::HashStringView32<>&>()
	.template ctor<const Shibboleth::HashString32<>&>()
	.template ctor<const Shibboleth::U8String&>()
	.template ctor<>()

	.template opGreaterThanOrEqual< Shibboleth::HashString32<> >()
	.template opGreaterThan< Shibboleth::HashString32<> >()

	.template opLessThanOrEqual< Shibboleth::HashString32<> >()
	.template opLessThan< Shibboleth::HashString32<> >()

	.template opNotEqual< Shibboleth::HashString32<> >()
	.template opEqual< Shibboleth::HashString32<> >()

	.template opComparison< Shibboleth::HashString32<> >()

	.template opGreaterThanOrEqual<>()
	.template opGreaterThan<>()

	.template opLessThanOrEqual<>()
	.template opLessThan<>()

	.template opNotEqual<>()
	.template opEqual<>()

	.template opComparison<>()

	.func("getHash", &Shibboleth::HashStringView32<>::getHash)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashStringView32<>)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashStringView64<>)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute{
			Shibboleth::ScriptFlagsAttribute::Flag::ValueType
		}
	)

	.setInstanceHash(Shibboleth::HashStringViewInstanceHash< char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64> >)
	.serialize(
		Shibboleth::LoadHashStringView< char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64> >,
		Shibboleth::SaveHashStringView< char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64> >
	)

	.template ctor<const Shibboleth::HashStringView64<>&>()
	.template ctor<const Shibboleth::HashString64<>&>()
	.template ctor<const Shibboleth::U8String&>()
	.template ctor<>()

	.template opGreaterThanOrEqual< Shibboleth::HashString64<> >()
	.template opGreaterThan< Shibboleth::HashString64<> >()

	.template opLessThanOrEqual< Shibboleth::HashString64<> >()
	.template opLessThan< Shibboleth::HashString64<> >()

	.template opNotEqual< Shibboleth::HashString64<> >()
	.template opEqual< Shibboleth::HashString64<> >()

	.template opComparison< Shibboleth::HashString64<> >()

	.template opGreaterThanOrEqual<>()
	.template opGreaterThan<>()

	.template opLessThanOrEqual<>()
	.template opLessThan<>()

	.template opNotEqual<>()
	.template opEqual<>()

	.template opComparison<>()

	.func("getHash", &Shibboleth::HashStringView64<>::getHash)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashStringView64<>)
