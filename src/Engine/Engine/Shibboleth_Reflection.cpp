/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

SHIB_REFLECTION_DEFINE_BEGIN(U8String)
	.classAttrs(
		ScriptFlagsAttribute(ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Gaff::HashStringInstance<ProxyAllocator>)
	.serialize(Gaff::LoadString<ProxyAllocator>, Gaff::SaveString<ProxyAllocator>)
SHIB_REFLECTION_DEFINE_END(U8String)

SHIB_REFLECTION_DEFINE_BEGIN(HashString32<>)
	.setInstanceHash(Gaff::HashStringInstanceHash<char, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, ProxyAllocator>)
	.serialize(
		Gaff::LoadHashString<char, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, ProxyAllocator>,
		Gaff::SaveHashString<char, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(HashString32<>)

SHIB_REFLECTION_DEFINE_BEGIN(HashString64<>)
	.setInstanceHash(Gaff::HashStringInstanceHash<char, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, ProxyAllocator>)
	.serialize(
		Gaff::LoadHashString<char, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, ProxyAllocator>,
		Gaff::SaveHashString<char, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(HashString64<>)

SHIB_REFLECTION_DEFINE_BEGIN(HashStringNoString32<>)
	.classAttrs(
		ScriptFlagsAttribute(ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Gaff::HashStringInstanceHash<char, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, ProxyAllocator>)
	.serialize(
		Gaff::LoadHashString<char, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, ProxyAllocator>,
		Gaff::SaveHashString<char, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32>, ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(HashStringNoString32<>)

SHIB_REFLECTION_DEFINE_BEGIN(HashStringNoString64<>)
	.classAttrs(
		ScriptFlagsAttribute(ScriptFlagsAttribute::Flag::NoRegister)
	)

	.setInstanceHash(Gaff::HashStringInstanceHash<char, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, ProxyAllocator>)
	.serialize(
		Gaff::LoadHashString<char, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, ProxyAllocator>,
		Gaff::SaveHashString<char, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64>, ProxyAllocator>
	)
SHIB_REFLECTION_DEFINE_END(HashStringNoString64<>)

SHIB_REFLECTION_DEFINE_BEGIN(HashStringView32<>)
	.setInstanceHash(Gaff::HashStringViewInstanceHash< char, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32> >)
	.serialize(
		Gaff::LoadHashStringView< char, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32> >,
		Gaff::SaveHashStringView< char, Gaff::Hash32, Gaff::DefaultHashFunc<Gaff::Hash32> >
	)
SHIB_REFLECTION_DEFINE_END(HashStringView32<>)

SHIB_REFLECTION_DEFINE_BEGIN(HashStringView64<>)
	.setInstanceHash(Gaff::HashStringViewInstanceHash< char, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64> >)
	.serialize(
		Gaff::LoadHashStringView< char, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64> >,
		Gaff::SaveHashStringView< char, Gaff::Hash64, Gaff::DefaultHashFunc<Gaff::Hash64> >
	)
SHIB_REFLECTION_DEFINE_END(HashStringView64<>)

SHIB_REFLECTION_DEFINE_BEGIN(Gaff::Hash32)
	.var("hash", &Gaff::Hash32::getHash, &Gaff::Hash32::setHash)
SHIB_REFLECTION_DEFINE_END(Gaff::Hash32)

SHIB_REFLECTION_DEFINE_BEGIN(Gaff::Hash64)
	.var("hash", &Gaff::Hash64::getHash, &Gaff::Hash64::setHash)
SHIB_REFLECTION_DEFINE_END(Gaff::Hash64)
