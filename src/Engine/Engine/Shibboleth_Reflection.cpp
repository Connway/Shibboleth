/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_Reflection.h"
#include "Shibboleth_EngineAttributesCommon.h"

#ifdef LoadString
	#undef LoadString
#endif

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

// $TODO: May want to reflect the Gaff versions of these in the future.
SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::U8String)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(Shibboleth::ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Shibboleth::HashStringInstance<Shibboleth::ProxyAllocator>)
	.serialize(Shibboleth::LoadString<Shibboleth::ProxyAllocator>, Shibboleth::SaveString<Shibboleth::ProxyAllocator>)
SHIB_REFLECTION_DEFINE_END(Shibboleth::U8String)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashString32<>)
	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashString32<>)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashString64<>)
	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashString64<>)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashStringNoString32<>)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(Shibboleth::ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, Shibboleth::ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashStringNoString32<>)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashStringNoString64<>)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(Shibboleth::ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Shibboleth::HashStringInstanceHash<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>)
	.serialize(
		Shibboleth::LoadHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>,
		Shibboleth::SaveHashString<char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, Shibboleth::ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashStringNoString64<>)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashStringView32<>)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(Shibboleth::ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Shibboleth::HashStringViewInstanceHash< char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32> >)
	.serialize(
		Shibboleth::LoadHashStringView< char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32> >,
		Shibboleth::SaveHashStringView< char8_t, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32> >
	)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashStringView32<>)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::HashStringView64<>)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(Shibboleth::ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Shibboleth::HashStringViewInstanceHash< char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64> >)
	.serialize(
		Shibboleth::LoadHashStringView< char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64> >,
		Shibboleth::SaveHashStringView< char8_t, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64> >
	)
SHIB_REFLECTION_DEFINE_END(Shibboleth::HashStringView64<>)

SHIB_REFLECTION_DEFINE_BEGIN(Gaff::Hash32)
	.var("hash", &Gaff::Hash32::getHash, &Gaff::Hash32::setHash)
SHIB_REFLECTION_DEFINE_END(Gaff::Hash32)

SHIB_REFLECTION_DEFINE_BEGIN(Gaff::Hash64)
	.var("hash", &Gaff::Hash64::getHash, &Gaff::Hash64::setHash)
SHIB_REFLECTION_DEFINE_END(Gaff::Hash64)
