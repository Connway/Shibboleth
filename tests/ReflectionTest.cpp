/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include <Shibboleth_EnumReflection.h>
#include <Shibboleth_Reflection.h>
#include <Shibboleth_App.h>
#include <Gaff_DynamicModule.h>

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

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Foo::NamespaceClass)
SHIB_REFLECTION_CLASS_DEFINE_END(Foo::NamespaceClass)

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
	REQUIRE(!strcmp(Shibboleth::Reflection<Foo::NamespaceClass>::GetName(), "Foo::NamespaceClass"));

	Shibboleth::Reflection<Derived>::Init();
	Shibboleth::Reflection<Base>::Init();
	Derived test;

	Base& base = test;
	Base2& base2 = test;
	Derived* ref_result = Gaff::ReflectionCast<Derived>(base);
	Base2* ref_result2 = REFLECTION_CAST_PTR(Base2, &base);

	REQUIRE(ref_result == &test);
	REQUIRE(ref_result2 == &base2);

	Gaff::Hash64 hash = Shibboleth::Reflection<Derived>::GetVersion();
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


class CtorTest
{
public:
	CtorTest(void) {}
	CtorTest(int val): a(val) {}

	int a = 200;

	SHIB_REFLECTION_CLASS_DECLARE(CtorTest);
};

SHIB_REFLECTION_DECLARE(CtorTest);
SHIB_REFLECTION_DEFINE(CtorTest);
SHIB_REFLECTION_CLASS_DEFINE_BEGIN(CtorTest)
	.ctor<>()
	.ctor<int>()
	.var("a", &CtorTest::a)
SHIB_REFLECTION_CLASS_DEFINE_END(CtorTest)

TEST_CASE("reflection factory test", "[shibboleth_factory]")
{
	Shibboleth::Reflection<CtorTest>::Init();
	CtorTest* test_a = Shibboleth::Reflection<CtorTest>::GetReflectionDefinition().create();
	CtorTest* test_b = Shibboleth::Reflection<CtorTest>::GetReflectionDefinition().create(100);

	REQUIRE(test_a->a == 200);
	REQUIRE(test_b->a == 100);
}

TEST_CASE("reflection module test", "[shibboleth_reflection_module]")
{
	Gaff::DynamicModule module;
	bool ret = module.load("ScriptingModule" BIT_EXTENSION);

	REQUIRE(ret);

	if (ret) {
		using InitFunc = bool (*)(Shibboleth::IApp*);
		InitFunc init = module.getFunc<InitFunc>("InitModule");
		init(&g_app);
	}

	g_app.destroy();
}


class AttrTest
{
public:
	AttrTest(void) {}
	AttrTest(int val) : a(val) {}

	int a = 555;

	SHIB_REFLECTION_CLASS_DECLARE(AttrTest);
};

class TestAttr : public Gaff::IAttribute
{
	Gaff::IAttribute* clone(void) const { return SHIB_ALLOCT(TestAttr, *Shibboleth::GetAllocator()); }

	SHIB_REFLECTION_CLASS_DECLARE(TestAttr);
};

SHIB_REFLECTION_DECLARE(TestAttr);
SHIB_REFLECTION_DEFINE(TestAttr);
SHIB_REFLECTION_CLASS_DEFINE(TestAttr);
//SHIB_REFLECTION_CLASS_DEFINE_BEGIN(AttrTest)
//SHIB_REFLECTION_CLASS_DEFINE_END(AttrTest)


SHIB_REFLECTION_DECLARE(AttrTest);
SHIB_REFLECTION_DEFINE(AttrTest);
SHIB_REFLECTION_CLASS_DEFINE_BEGIN(AttrTest)
	.classAttrs
	(
		TestAttr(),
		TestAttr(),
		TestAttr()
	)
	.varAttrs
	(
		"a",
		TestAttr(),
		TestAttr()
	)

	.ctor<>()
	.ctor<int>()
	.var("a", &AttrTest::a)
SHIB_REFLECTION_CLASS_DEFINE_END(AttrTest)


TEST_CASE("reflection attribute test", "[shibboleth_attribute]")
{
	Shibboleth::Reflection<TestAttr>::Init();
	Shibboleth::Reflection<AttrTest>::Init();
}


class FuncTest : public Gaff::IReflectionObject
{
	int getMyInt(void) const { return _my_int; }
	void setMyInt(int i) { _my_int = i; }

	const int& getIntRef(void) const { return _my_int; }
	void setIntRef(const int& i) { _my_int = i; }

	int _my_int = 1238;

	SHIB_REFLECTION_CLASS_DECLARE(FuncTest);
};

SHIB_REFLECTION_DECLARE(FuncTest);
SHIB_REFLECTION_DEFINE(FuncTest);

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(FuncTest)
	.ctor<>()
	.func("getMyInt", &FuncTest::getMyInt)
	.func("setMyInt", &FuncTest::setMyInt)
	.func("myInt", &FuncTest::getMyInt)
	.func("myInt", &FuncTest::setMyInt)
	.func("getIntRef", &FuncTest::getIntRef)
	.func("setIntRef", &FuncTest::setIntRef)
SHIB_REFLECTION_CLASS_DEFINE_END(FuncTest)

TEST_CASE("reflection func test", "[shibboleth_func]")
{
	Shibboleth::Reflection<FuncTest>::Init();

	const Gaff::IReflectionDefinition& ref_def = Shibboleth::Reflection<FuncTest>::GetReflectionDefinition();
	Gaff::IReflectionFunction<int>* const get_func = ref_def.getFunc<int>(Gaff::FNV1aHash32Const("getMyInt"));
	Gaff::IReflectionFunction<void, int>* const set_func = ref_def.getFunc<void, int>(Gaff::FNV1aHash32Const("setMyInt"));
	Gaff::IReflectionFunction<int>* const ovl_get_func = ref_def.getFunc<int>(Gaff::FNV1aHash32Const("myInt"));
	Gaff::IReflectionFunction<void, int>* const ovl_set_func = ref_def.getFunc<void, int>(Gaff::FNV1aHash32Const("myInt"));
	Gaff::IReflectionFunction<const int&>* const ref_get_func = ref_def.getFunc<const int&>(Gaff::FNV1aHash32Const("getIntRef"));
	Gaff::IReflectionFunction<void, const int&>* const ref_set_func = ref_def.getFunc<void, const int&>(Gaff::FNV1aHash32Const("setIntRef"));

	void* data = ref_def.createAlloc(*Shibboleth::GetAllocator());

	REQUIRE(data);
	REQUIRE(get_func);
	REQUIRE(get_func->isConst());
	REQUIRE(get_func->call(data) == 1238);

	REQUIRE(set_func);
	REQUIRE(!set_func->isConst());

	printf("getMyInt: %i\n", get_func->call(data));

	set_func->call(data, 999);
	REQUIRE(get_func->call(data) == 999);

	printf("setMyInt: %i\n", get_func->call(data));

	REQUIRE(ovl_get_func);
	REQUIRE(ovl_get_func->isConst());
	REQUIRE(ovl_get_func->call(data) == 999);
	printf("myInt (get): %i\n", ovl_get_func->call(data));

	REQUIRE(ovl_set_func);
	REQUIRE(!ovl_set_func->isConst());
	ovl_set_func->call(data, 987);

	REQUIRE(ovl_get_func->call(data) == 987);
	printf("myInt (set): %i\n", ovl_get_func->call(data));

	REQUIRE(ref_get_func);
	REQUIRE(ref_get_func->isConst());
	REQUIRE(ref_get_func->call(data) == 987);
	printf("getIntRef: %i\n", ref_get_func->call(data));

	REQUIRE(ref_set_func);
	REQUIRE(!ref_set_func->isConst());
	ref_set_func->call(data, 1111);

	REQUIRE(ref_get_func->call(data) == 1111);
	printf("setIntRef: %i\n", ref_get_func->call(data));

	SHIB_FREE(data, *Shibboleth::GetAllocator());
}

enum TestEnum
{
	TE_MINUS_ONE = -1,
	TE_ZERO,
	TE_ONE,
	TE_TWO,
	TE_TWENTY = 20
};

SHIB_ENUM_REFLECTION_DECLARE(TestEnum)

SHIB_ENUM_REFLECTION_DEFINE_BEGIN(TestEnum)
	.entry("MinusOne", TE_MINUS_ONE)
	.entry("Zero", TE_ZERO)
	.entry("One", TE_ONE)
	.entry("Two", TE_TWO)
	.entry("Twenty", TE_TWENTY)
SHIB_ENUM_REFLECTION_DEFINE_END(TestEnum)

TEST_CASE("reflection enum test", "[shibboleth_enum]")
{
	Shibboleth::EnumReflection<TestEnum>::Init();

	const Gaff::IEnumReflectionDefinition& ref_def = Shibboleth::EnumReflection<TestEnum>::GetReflectionDefinition();
	REQUIRE(ref_def.getNumEntries() == 5);

	printf("Enum Name Index 0: %s\n", ref_def.getEntryNameFromIndex(0));

	REQUIRE(!strcmp("MinusOne", ref_def.getEntryNameFromValue(TE_MINUS_ONE)));
	REQUIRE(!strcmp("Zero", ref_def.getEntryNameFromValue(TE_ZERO)));
	REQUIRE(!strcmp("One", ref_def.getEntryNameFromValue(TE_ONE)));
	REQUIRE(!strcmp("Two", ref_def.getEntryNameFromValue(TE_TWO)));
	REQUIRE(!strcmp("Twenty", ref_def.getEntryNameFromValue(TE_TWENTY)));

	printf("TE_MINUS_ONE: %s\n", ref_def.getEntryNameFromValue(TE_MINUS_ONE));
	printf("TE_ZERO: %s\n", ref_def.getEntryNameFromValue(TE_ZERO));
	printf("TE_ONE: %s\n", ref_def.getEntryNameFromValue(TE_ONE));
	printf("TE_TWO: %s\n", ref_def.getEntryNameFromValue(TE_TWO));
	printf("TE_TWENTY: %s\n", ref_def.getEntryNameFromValue(TE_TWENTY));

	REQUIRE(ref_def.getEntryValue("MinusOne") == -1);
	REQUIRE(ref_def.getEntryValue("Zero") == 0);
	REQUIRE(ref_def.getEntryValue("One") == 1);
	REQUIRE(ref_def.getEntryValue("Two") == 2);
	REQUIRE(ref_def.getEntryValue("Twenty") == 20);
}


class SerializeTestClass
{
};

using STCPtr = Shibboleth::UniquePtr<SerializeTestClass>;

SHIB_REFLECTION_DECLARE_SERIALIZE(STCPtr, 0)

SHIB_REFLECTION_DEFINE_SERIALIZE_INIT(STCPtr)
{
}

SHIB_REFLECTION_DEFINE_SERIALIZE_LOAD(STCPtr)
{
	GAFF_REF(reader);
	GAFF_REF(object);
	printf("Load STCPtr\n");
}

SHIB_REFLECTION_DEFINE_SERIALIZE_SAVE(STCPtr)
{
	GAFF_REF(writer);
	GAFF_REF(object);
	printf("Save STCPtr\n");
}

template <class T, class TT>
class SerializeTestTemplate
{
};

SHIB_TEMPLATE_REFLECTION_DECLARE_SERIALIZE(SerializeTestTemplate, 0, T, TT)

SHIB_TEMPLATE_REFLECTION_DEFINE_SERIALIZE_INIT(SerializeTestTemplate, T, TT)
{
}

SHIB_TEMPLATE_REFLECTION_DEFINE_SERIALIZE_LOAD(SerializeTestTemplate, T, TT)
{
	GAFF_REF(reader);
	GAFF_REF(object);
	printf("Load %s\n", Shibboleth::Reflection< SerializeTestTemplate<T, TT> >::GetName());
}

SHIB_TEMPLATE_REFLECTION_DEFINE_SERIALIZE_SAVE(SerializeTestTemplate, T, TT)
{
	GAFF_REF(writer);
	GAFF_REF(object);
	printf("Save %s\n", Shibboleth::Reflection< SerializeTestTemplate<T, TT> >::GetName());
}



//class EmptyReader : public Gaff::ISerializeReader
//{
//	int8_t readInt8(void) const { return 0; }
//	int16_t readInt16(void) const { return 0; }
//	int32_t readInt32(void) const { return 0; }
//	int64_t readInt64(void) const { return 0; }

//	uint8_t readUInt8(void) const { return 0; }
//	uint16_t readUInt16(void) const { return 0; }
//	uint32_t readUInt32(void) const { return 0; }
//	uint64_t readUInt64(void) const { return 0; }

//	float readFloat(void) const { return 0.0f; }
//	double readDouble(void) const { return 0.0; }

//	size_t readStringLength(void) const { return 0; }
//	const char* readString(void) const { return nullptr; }
//};

//class EmptyWriter : public Gaff::ISerializeWriter
//{
//	void writeInt8(int8_t value) const { GAFF_REF(value); }
//	void writeInt16(int16_t value) const { GAFF_REF(value); }
//	void writeInt32(int32_t value) const { GAFF_REF(value); }
//	void writeInt64(int64_t value) const { GAFF_REF(value); }

//	void writeUInt8(uint8_t value) const { GAFF_REF(value); }
//	void writeUInt16(uint16_t value) const { GAFF_REF(value); }
//	void writeUInt32(uint32_t value) const { GAFF_REF(value); }
//	void writeUInt64(uint64_t value) const { GAFF_REF(value); }

//	void writeFloat(float value) const { GAFF_REF(value); }
//	void writeDouble(double value) const { GAFF_REF(value); }

//	void writeString(const char* value, size_t size) const { GAFF_REF(value); GAFF_REF(size); }
//	void writeString(const char* value) const { GAFF_REF(value); }
//};

//TEST_CASE("reflection serialize test", "[shibboleth_serialize]")
//{
//	EmptyReader reader;
//	EmptyWriter writer;

//	STCPtr stc;
//	Shibboleth::Reflection<STCPtr>::Load(reader, stc);
//	Shibboleth::Reflection<STCPtr>::Save(writer, stc);

//	SerializeTestTemplate<int32_t, double> stt;
//	Shibboleth::Reflection< SerializeTestTemplate<int32_t, double> >::Load(reader, stt);
//	Shibboleth::Reflection< SerializeTestTemplate<int32_t, double> >::Save(writer, stt);
//}
