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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <Shibboleth_Reflection.h>

NS_SHIBBOLETH

class Test
{
	int i = 0;
	int j = 10;
};

SHIB_REFLECTION_CLASS_DECLARE(Test);

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Test)
SHIB_REFLECTION_CLASS_DEFINE_END(Test)

NS_END

class Foo;

TEST_CASE("reflection basic test", "[shibboleth_reflection_basic]")
{
	printf("Reflection POD: %s\n", Shibboleth::Reflection<int8_t>::GetName());
	printf("Reflection POD: %s\n", Shibboleth::Reflection<int16_t>::GetName());
	printf("Reflection POD: %s\n", Shibboleth::Reflection<int32_t>::GetName());
	printf("Reflection POD: %s\n", Shibboleth::Reflection<int64_t>::GetName());
	printf("Reflection POD: %s\n", Shibboleth::Reflection<uint8_t>::GetName());
	printf("Reflection POD: %s\n", Shibboleth::Reflection<uint16_t>::GetName());
	printf("Reflection POD: %s\n", Shibboleth::Reflection<uint32_t>::GetName());
	printf("Reflection POD: %s\n", Shibboleth::Reflection<uint64_t>::GetName());
	printf("Reflection POD: %s\n", Shibboleth::Reflection<float>::GetName());
	printf("Reflection POD: %s\n\n", Shibboleth::Reflection<double>::GetName());

	REQUIRE(!strcmp(Shibboleth::Reflection<int8_t>::GetName(), "int8_t"));
	REQUIRE(!strcmp(Shibboleth::Reflection<int16_t>::GetName(), "int16_t"));
	REQUIRE(!strcmp(Shibboleth::Reflection<int32_t>::GetName(), "int32_t"));
	REQUIRE(!strcmp(Shibboleth::Reflection<int64_t>::GetName(), "int64_t"));
	REQUIRE(!strcmp(Shibboleth::Reflection<uint8_t>::GetName(), "uint8_t"));
	REQUIRE(!strcmp(Shibboleth::Reflection<uint16_t>::GetName(), "uint16_t"));
	REQUIRE(!strcmp(Shibboleth::Reflection<uint32_t>::GetName(), "uint32_t"));
	REQUIRE(!strcmp(Shibboleth::Reflection<uint64_t>::GetName(), "uint64_t"));
	REQUIRE(!strcmp(Shibboleth::Reflection<float>::GetName(), "float"));
	REQUIRE(!strcmp(Shibboleth::Reflection<double>::GetName(), "double"));

	printf("Reflection Class: %s\n", Shibboleth::Reflection<Shibboleth::Test>::GetName());
	REQUIRE(!strcmp(Shibboleth::Reflection<Shibboleth::Test>::GetName(), "Test"));
}
