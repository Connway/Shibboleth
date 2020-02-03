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

#include "Shibboleth_Reflection.h"

#ifdef LoadString
	#undef LoadString
#endif

SHIB_REFLECTION_EXTERNAL_DEFINE(int8_t)
SHIB_REFLECTION_EXTERNAL_DEFINE(int16_t)
SHIB_REFLECTION_EXTERNAL_DEFINE(int32_t)
SHIB_REFLECTION_EXTERNAL_DEFINE(int64_t)
SHIB_REFLECTION_EXTERNAL_DEFINE(uint8_t)
SHIB_REFLECTION_EXTERNAL_DEFINE(uint16_t)
SHIB_REFLECTION_EXTERNAL_DEFINE(uint32_t)
SHIB_REFLECTION_EXTERNAL_DEFINE(uint64_t)
SHIB_REFLECTION_EXTERNAL_DEFINE(float)
SHIB_REFLECTION_EXTERNAL_DEFINE(double)
SHIB_REFLECTION_EXTERNAL_DEFINE(bool)
SHIB_REFLECTION_EXTERNAL_DEFINE(U8String)

NS_SHIBBOLETH

static Gaff::Hash64 HashStringInstance(const U8String& string, Gaff::Hash64 init)
{
	return Gaff::FNV1aHash64String(string.data(), init);
}

static bool LoadString(const Gaff::ISerializeReader& reader, U8String& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char* const str = reader.readString();
	out = str;
	reader.freeString(str);

	return true;
}

static void SaveString(Gaff::ISerializeWriter& writer, const U8String& value) { writer.writeString(value.data()); }

SHIB_REFLECTION_BUILDER_BEGIN(int8_t)
SHIB_REFLECTION_BUILDER_END(int8_t)

SHIB_REFLECTION_BUILDER_BEGIN(int16_t)
SHIB_REFLECTION_BUILDER_END(int16_t)

SHIB_REFLECTION_BUILDER_BEGIN(int32_t)
SHIB_REFLECTION_BUILDER_END(int32_t)

SHIB_REFLECTION_BUILDER_BEGIN(int64_t)
SHIB_REFLECTION_BUILDER_END(int64_t)

SHIB_REFLECTION_BUILDER_BEGIN(uint8_t)
SHIB_REFLECTION_BUILDER_END(uint8_t)

SHIB_REFLECTION_BUILDER_BEGIN(uint16_t)
SHIB_REFLECTION_BUILDER_END(uint16_t)

SHIB_REFLECTION_BUILDER_BEGIN(uint32_t)
SHIB_REFLECTION_BUILDER_END(uint32_t)

SHIB_REFLECTION_BUILDER_BEGIN(uint64_t)
SHIB_REFLECTION_BUILDER_END(uint64_t)

SHIB_REFLECTION_BUILDER_BEGIN(float)
SHIB_REFLECTION_BUILDER_END(float)

SHIB_REFLECTION_BUILDER_BEGIN(double)
SHIB_REFLECTION_BUILDER_END(double)

SHIB_REFLECTION_BUILDER_BEGIN(bool)
SHIB_REFLECTION_BUILDER_END(bool)

SHIB_REFLECTION_BUILDER_BEGIN(U8String)
	.setInstanceHash(HashStringInstance)
	.serialize(LoadString, SaveString)
SHIB_REFLECTION_BUILDER_END(U8String)

NS_END
