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
#include <Shibboleth_App.h>

NS_SHIBBOLETH

App g_app;

class Base /*: public Gaff::IReflectionObject*/
{
public:
	int a = 1;
	SHIB_REFLECTION_CLASS_DECLARE(Base)
};

class Base2
{
	int b = 2;
};

class Derived : public Base, public Base2
{
public:
	const int& getCRef(void) const { return c; }
	int getC(void) const { return c; }
	void setC(int v) { c = v; }

	int c = 3;
	SHIB_REFLECTION_CLASS_DECLARE(Derived)
};

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Base)
	.var("a", &Base::a)
SHIB_REFLECTION_CLASS_DEFINE_END(Base)

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Derived)
	.baseClass<Base>()
	.BASE_CLASS(Base2)

	.var("c", &Derived::c)
	.var("cFunc", &Derived::getC, &Derived::setC)
	.var("cRef", &Derived::getCRef, &Derived::setC)
SHIB_REFLECTION_CLASS_DEFINE_END(Derived)



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
}

TEST_CASE("reflection class test", "[shibboleth_reflection_class]")
{
	Shibboleth::Reflection<Shibboleth::Derived>::SetAllocator(Shibboleth::ProxyAllocator("Reflection"));
	Shibboleth::Reflection<Shibboleth::Base>::SetAllocator(Shibboleth::ProxyAllocator("Reflection"));

	Shibboleth::g_app.init(0, nullptr);
	Shibboleth::SetApp(Shibboleth::g_app);

	printf("Reflection Class: %s\n", Shibboleth::Reflection<Shibboleth::Derived>::GetName());
	printf("Reflection Class: %s\n", Shibboleth::Reflection<Shibboleth::Base>::GetName());
	REQUIRE(!strcmp(Shibboleth::Reflection<Shibboleth::Derived>::GetName(), "Derived"));
	REQUIRE(!strcmp(Shibboleth::Reflection<Shibboleth::Base>::GetName(), "Base"));

	Shibboleth::Reflection<Shibboleth::Derived>::Init();
	Shibboleth::Reflection<Shibboleth::Base>::Init();
	Shibboleth::Derived test;

	Shibboleth::Base& base = test;
	Shibboleth::Base2& base2 = test;
	Shibboleth::Derived* ref_result = Gaff::ReflectionCast<Shibboleth::Derived>(base);
	Shibboleth::Base2* ref_result2 = REFLECTION_CAST_PTR_NAME(Shibboleth::Base2, "Base2", &base);

	REQUIRE(ref_result == &test);
	REQUIRE(ref_result2 == &base2);

	Gaff::Hash64 hash = Shibboleth::Reflection<Shibboleth::Derived>::GetReflectionDefinition().getVersionHash();
	printf("Version Hash: %llu\n", hash);

	int test_get_func_ref = Shibboleth::Reflection<Shibboleth::Derived>::GetReflectionDefinition().getVar(Gaff::FNV1aHash32Const("cRef"))->getDataT<int>(*ref_result);
	int test_get_func = Shibboleth::Reflection<Shibboleth::Derived>::GetReflectionDefinition().getVar(Gaff::FNV1aHash32Const("cFunc"))->getDataT<int>(*ref_result);
	int test_get = Shibboleth::Reflection<Shibboleth::Derived>::GetReflectionDefinition().getVar(Gaff::FNV1aHash32Const("c"))->getDataT<int>(*ref_result);

	printf(
		"GetFuncRef: %i\n"
		"GetFunc: %i\n"
		"GetVar: %i\n",
		test_get_func_ref,
		test_get_func,
		test_get
	);

	REQUIRE(test_get_func_ref == ref_result->c);
	REQUIRE(test_get_func == ref_result->c);
	REQUIRE(test_get == ref_result->c);

	auto* ref_var = Shibboleth::Reflection<Shibboleth::Derived>::GetReflectionDefinition().getVar(Gaff::FNV1aHash32Const("a"));
	int test_base_get = ref_var->getDataT<int>(*ref_result);
	printf("GetBase: %i\n", test_base_get);

	REQUIRE(test_base_get == base.a);

	ref_var->setDataT(*ref_result, 20);
	test_base_get = ref_var->getDataT<int>(*ref_result);
	printf("SetBase: %i\n", test_base_get);

	REQUIRE(test_base_get == base.a);
	REQUIRE(test_base_get == 20);

	Shibboleth::g_app.destroy();
}

NS_SHIBBOLETH
//namespace Foo {

template <class T>
class Test1
{
	double a = 20.0;
	T t;

	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(Test1, T);
};

SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(Test1, T)
	.var("a", &Test1::a)
	.var("t", &Test1::t)
SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_END(Test1, T)


template <class T1, class T2>
class Test2
{
	float a = 11.0f;
	T1 t1;
	T2 t2;

	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(Test2, T1, T2);
};

SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(Test2, T1, T2)
	.var("a", &Test2::a)
	.var("t1", &Test2::t1)
	.var("t2", &Test2::t2)
SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_END(Test2, T1, T2)

//}
NS_END

TEST_CASE("reflection template class test", "[shibboleth_reflection_template_class]")
{
	Shibboleth::Reflection< Shibboleth::Test1<int32_t> >::Init();
	Shibboleth::Reflection< Shibboleth::Test1<double> >::Init();
	Shibboleth::Reflection< Shibboleth::Test2<int32_t, int8_t> >::Init();
	Shibboleth::Reflection< Shibboleth::Test2<float, double> >::Init();

	printf("\nReflection Class: %s\n", Shibboleth::Reflection< Shibboleth::Test1<int32_t> >::GetName());
	printf("Reflection Class: %s\n", Shibboleth::Reflection< Shibboleth::Test1<double> >::GetName());
	printf("Reflection Class: %s\n", Shibboleth::Reflection< Shibboleth::Test2<int32_t, int8_t> >::GetName());
	printf("Reflection Class: %s\n", Shibboleth::Reflection< Shibboleth::Test2<float, double> >::GetName());

	REQUIRE(!strcmp(Shibboleth::Reflection< Shibboleth::Test1<int32_t> >::GetName(), "Test1<int32_t>"));
	REQUIRE(!strcmp(Shibboleth::Reflection< Shibboleth::Test1<double> >::GetName(), "Test1<double>"));
	REQUIRE(!strcmp(Shibboleth::Reflection< Shibboleth::Test2<int32_t, int8_t> >::GetName(), "Test2<int32_t, int8_t>"));
	REQUIRE(!strcmp(Shibboleth::Reflection< Shibboleth::Test2<float, double> >::GetName(), "Test2<float, double>"));
}
