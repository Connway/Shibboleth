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

Shibboleth::App g_app;

class Base /*: public Gaff::IReflectionObject*/
{
public:
	int a = 1;
	SHIB_REFLECTION_CLASS_DECLARE(Base);
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
	SHIB_REFLECTION_CLASS_DECLARE(Derived);
};

SHIB_REFLECTION_DECLARE(Base);
SHIB_REFLECTION_DEFINE(Base);
SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Base)
	.var("a", &Base::a)
SHIB_REFLECTION_CLASS_DEFINE_END(Base)

SHIB_REFLECTION_DECLARE(Derived);
SHIB_REFLECTION_DEFINE(Derived);
SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Derived)
	.base<Base>()
	.BASE(Base2)

	.var("c", &Derived::c)
	.var("cFunc", &Derived::getC, &Derived::setC)
	.var("cRef", &Derived::getCRef, &Derived::setC)
SHIB_REFLECTION_CLASS_DEFINE_END(Derived)


// Namespace Test
namespace Foo
{
	class NamespaceClass
	{
		SHIB_REFLECTION_CLASS_DECLARE(NamespaceClass);
	};
}

SHIB_REFLECTION_DECLARE(Foo::NamespaceClass);
SHIB_REFLECTION_DEFINE(Foo::NamespaceClass);

namespace Foo
{
	SHIB_REFLECTION_CLASS_DEFINE_BEGIN(NamespaceClass)
	SHIB_REFLECTION_CLASS_DEFINE_END(NamespaceClass)
}

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
	Shibboleth::Reflection<Derived>::SetAllocator(Shibboleth::ProxyAllocator("Reflection"));
	Shibboleth::Reflection<Base>::SetAllocator(Shibboleth::ProxyAllocator("Reflection"));

	g_app.init(0, nullptr);
	Shibboleth::SetApp(g_app);

	printf("Reflection Class: %s\n", Shibboleth::Reflection<Derived>::GetName());
	printf("Reflection Class: %s\n", Shibboleth::Reflection<Base>::GetName());
	printf("Reflection Class: %s\n", Shibboleth::Reflection<Foo::NamespaceClass>::GetName());
	REQUIRE(!strcmp(Shibboleth::Reflection<Derived>::GetName(), "Derived"));
	REQUIRE(!strcmp(Shibboleth::Reflection<Base>::GetName(), "Base"));
	REQUIRE(!strcmp(Shibboleth::Reflection<Foo::NamespaceClass>::GetName(), "NamespaceClass"));

	Shibboleth::Reflection<Derived>::Init();
	Shibboleth::Reflection<Base>::Init();
	Derived test;

	Base& base = test;
	Base2& base2 = test;
	Derived* ref_result = Gaff::ReflectionCast<Derived>(base);
	Base2* ref_result2 = REFLECTION_CAST_PTR(Base2, &base);

	REQUIRE(ref_result == &test);
	REQUIRE(ref_result2 == &base2);

	Gaff::Hash64 hash = Shibboleth::Reflection<Derived>::GetReflectionDefinition().getVersionHash();
	printf("Version Hash: %llu\n", hash);

	int test_get_func_ref = Shibboleth::Reflection<Derived>::GetReflectionDefinition().getVar(Gaff::FNV1aHash32Const("cRef"))->getDataT<int>(*ref_result);
	int test_get_func = Shibboleth::Reflection<Derived>::GetReflectionDefinition().getVar(Gaff::FNV1aHash32Const("cFunc"))->getDataT<int>(*ref_result);
	int test_get = Shibboleth::Reflection<Derived>::GetReflectionDefinition().getVar(Gaff::FNV1aHash32Const("c"))->getDataT<int>(*ref_result);

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

	auto* ref_var = Shibboleth::Reflection<Derived>::GetReflectionDefinition().getVar(Gaff::FNV1aHash32Const("a"));
	int test_base_get = ref_var->getDataT<int>(*ref_result);
	printf("GetBase: %i\n", test_base_get);

	REQUIRE(test_base_get == base.a);

	ref_var->setDataT(*ref_result, 20);
	test_base_get = ref_var->getDataT<int>(*ref_result);
	printf("SetBase: %i\n", test_base_get);

	REQUIRE(test_base_get == base.a);
	REQUIRE(test_base_get == 20);
}

template <class T>
class Test1
{
	double a = 20.0;
	T t;

	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(Test1, T);
};

SHIB_TEMPLATE_REFLECTION_DECLARE(Test1, T);
SHIB_TEMPLATE_REFLECTION_DEFINE(Test1, T);
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

SHIB_TEMPLATE_REFLECTION_DECLARE(Test2, T1, T2);
SHIB_TEMPLATE_REFLECTION_DEFINE(Test2, T1, T2);
SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(Test2, T1, T2)
	.var("a", &Test2::a)
	.var("t1", &Test2::t1)
	.var("t2", &Test2::t2)
SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_END(Test2, T1, T2)

TEST_CASE("reflection template class test", "[shibboleth_reflection_template_class]")
{
	Shibboleth::Reflection< Test1<int32_t> >::Init();
	Shibboleth::Reflection< Test1<double> >::Init();
	Shibboleth::Reflection< Test2<int32_t, int8_t> >::Init();
	Shibboleth::Reflection< Test2<float, double> >::Init();

	printf("\nReflection Class: %s\n", Shibboleth::Reflection< Test1<int32_t> >::GetName());
	printf("Reflection Class: %s\n", Shibboleth::Reflection< Test1<double> >::GetName());
	printf("Reflection Class: %s\n", Shibboleth::Reflection< Test2<int32_t, int8_t> >::GetName());
	printf("Reflection Class: %s\n", Shibboleth::Reflection< Test2<float, double> >::GetName());

	REQUIRE(!strcmp(Shibboleth::Reflection< Test1<int32_t> >::GetName(), "Test1<int32_t>"));
	REQUIRE(!strcmp(Shibboleth::Reflection< Test1<double> >::GetName(), "Test1<double>"));
	REQUIRE(!strcmp(Shibboleth::Reflection< Test2<int32_t, int8_t> >::GetName(), "Test2<int32_t, int8_t>"));
	REQUIRE(!strcmp(Shibboleth::Reflection< Test2<float, double> >::GetName(), "Test2<float, double>"));
}


class VecTest : public Gaff::IReflectionObject
{
	Shibboleth::Vector<uint32_t> vec;
	float arr[4] = { 0.0f, 1.0f, 2.0f, 3.0f };
	Base base[2];

	SHIB_REFLECTION_CLASS_DECLARE(VecTest);
};

SHIB_REFLECTION_DECLARE(VecTest);
SHIB_REFLECTION_DEFINE(VecTest);

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(VecTest)
	.var("vec", &VecTest::vec)
	.var("arr", &VecTest::arr)
	.var("base", &VecTest::base)
SHIB_REFLECTION_CLASS_DEFINE_END(VecTest)

TEST_CASE("reflection array/vector test", "[shibboleth_reflection_vector_array]")
{
	Shibboleth::Reflection<VecTest>::Init();

	printf("\nReflection Class: %s\n", Shibboleth::Reflection<VecTest>::GetName());

	VecTest instance;
	Gaff::IReflectionObject* ptr = &instance;

	const Gaff::ReflectionDefinition<VecTest, Shibboleth::ProxyAllocator>& ref_def = Shibboleth::Reflection<VecTest>::GetReflectionDefinition();
	Gaff::IReflectionVar* vec_var = ref_def.getVariable(Gaff::FNV1aHash32Const("vec"));
	Gaff::IReflectionVar* arr_var = ref_def.getVariable(Gaff::FNV1aHash32Const("arr"));
	Gaff::IReflectionVar* base_var = ref_def.getVariable(Gaff::FNV1aHash32Const("base"));
	void* base_ptr = ptr->getBasePointer();

	REQUIRE(vec_var->isVector());
	REQUIRE(!vec_var->isFixedArray());
	REQUIRE(vec_var->size(base_ptr) == 0);

	REQUIRE(!arr_var->isVector());
	REQUIRE(arr_var->isFixedArray());
	REQUIRE(arr_var->size(base_ptr) == 4);

	REQUIRE(!base_var->isVector());
	REQUIRE(base_var->isFixedArray());
	REQUIRE(base_var->size(base_ptr) == 2);

	vec_var->resize(base_ptr, 4);
	REQUIRE(vec_var->size(base_ptr) == 4);

	for (int32_t i = 0; i < 4; ++i) {
		float val = arr_var->getElementT<float>(base_ptr, i);
		REQUIRE(val == static_cast<float>(i));

		printf("Fixed Array[%i]: %f\n", i, val);

		vec_var->setElementT(base_ptr, i, static_cast<uint32_t>(4 - i));
	}

	for (int32_t i = 0; i < 4; ++i) {
		uint32_t val = vec_var->getElementT<uint32_t>(base_ptr, i);
		REQUIRE(val == static_cast<uint32_t>(4 - i));
		printf("Vector[%i]: %u\n", i, val);
	}

	for (int32_t i = 0; i < 2; ++i) {
		Base val = base_var->getElementT<Base>(base_ptr, i);
		REQUIRE(val.a == 1);
		printf("Base Array[%i]: %i\n", i, val.a);
	}

	arr_var->swap(base_ptr, 1, 2);
	vec_var->swap(base_ptr, 0, 3);

	for (int32_t i = 0; i < 4; ++i) {
		float val = arr_var->getElementT<float>(base_ptr, i);
		printf("Swapped Fixed Array[%i]: %f\n", i, val);
	}

	for (int32_t i = 0; i < 4; ++i) {
		uint32_t val = vec_var->getElementT<uint32_t>(base_ptr, i);
		printf("Swapped Vector[%i]: %u\n", i, val);
	}

	REQUIRE(arr_var->getElementT<float>(base_ptr, 0) == 0.0f);
	REQUIRE(arr_var->getElementT<float>(base_ptr, 1) == 2.0f);
	REQUIRE(arr_var->getElementT<float>(base_ptr, 2) == 1.0f);
	REQUIRE(arr_var->getElementT<float>(base_ptr, 3) == 3.0f);

	REQUIRE(vec_var->getElementT<uint32_t>(base_ptr, 0) == 1u);
	REQUIRE(vec_var->getElementT<uint32_t>(base_ptr, 1) == 3u);
	REQUIRE(vec_var->getElementT<uint32_t>(base_ptr, 2) == 2u);
	REQUIRE(vec_var->getElementT<uint32_t>(base_ptr, 3) == 4u);

	REQUIRE(base_var->getElementT<Base>(base_ptr, 0).a == 1);
	REQUIRE(base_var->getElementT<Base>(base_ptr, 1).a == 1);

	float test_data[] = { 5.0f, 6.0f, 7.0f, 8.0f };
	arr_var->setDataT(base_ptr, *test_data);

	for (int32_t i = 0; i < 4; ++i) {
		float val = arr_var->getElementT<float>(base_ptr, i);
		printf("SetData Fixed Array[%i]: %f\n", i, val);
		REQUIRE(val == (i + 5.0f));
	}

	Base base_data[2];
	base_data[0].a = 2;
	base_data[1].a = 3;
	base_var->setDataT(base_ptr, *base_data);

	REQUIRE(base_var->getElementT<Base>(base_ptr, 0).a == 2);
	REQUIRE(base_var->getElementT<Base>(base_ptr, 1).a == 3);
	printf("Base Array[0]: %i\n", base_var->getElementT<Base>(base_ptr, 0).a);
	printf("Base Array[1]: %i\n", base_var->getElementT<Base>(base_ptr, 1).a);

	base_data[0].a = 40;
	base_var->setElementT(base_ptr, 1, base_data[0]);

	REQUIRE(base_var->getElementT<Base>(base_ptr, 0).a == 2);
	REQUIRE(base_var->getElementT<Base>(base_ptr, 1).a == 40);
	printf("SetElement Base Array[0]: %i\n", base_var->getElementT<Base>(base_ptr, 0).a);
	printf("SetElement Base Array[1]: %i\n", base_var->getElementT<Base>(base_ptr, 1).a);
}