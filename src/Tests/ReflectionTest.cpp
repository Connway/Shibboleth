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

#include <Shibboleth_Reflection.h>
#include <Shibboleth_App.h>
#include <Gaff_MessagePackSerializeWriter.h>
#include <Gaff_JSONSerializeWriter.h>
#include <Gaff_SerializeReader.h>
#include <Gaff_DynamicModule.h>
#include <catch_amalgamated.hpp>
#include <filesystem>
#include <fstream>

class Base : public Gaff::IReflectionObject
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
SHIB_REFLECTION_CLASS_DEFINE(Base);
SHIB_REFLECTION_DEFINE_BEGIN(Base)
	.var("a", &Base::a)
SHIB_REFLECTION_DEFINE_END(Base)

SHIB_REFLECTION_DECLARE(Derived);
SHIB_REFLECTION_CLASS_DEFINE(Derived);
SHIB_REFLECTION_DEFINE_BEGIN(Derived)
	.base<Base>()
	.BASE(Base2)

	.var("c", &Derived::c)
	//.var("cFunc", &Derived::getC, &Derived::setC)
	.var("cRef", &Derived::getCRef, &Derived::setC)
SHIB_REFLECTION_DEFINE_END(Derived)


// Namespace Test
namespace Foo
{
	class NamespaceClass : public Gaff::IReflectionObject
	{
		SHIB_REFLECTION_CLASS_DECLARE(NamespaceClass);
	};
}

SHIB_REFLECTION_DECLARE(Foo::NamespaceClass);
SHIB_REFLECTION_CLASS_DEFINE(Foo::NamespaceClass);

SHIB_REFLECTION_DEFINE_BEGIN(Foo::NamespaceClass)
SHIB_REFLECTION_DEFINE_END(Foo::NamespaceClass)

TEST_CASE("shibboleth_reflection_basic")
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

TEST_CASE("shibboleth_reflection_class")
{
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

	Gaff::Hash64 hash = Shibboleth::Reflection<Derived>::GetInstance().getVersion();
	printf("Version Hash: %llu\n", hash.getHash());

	int test_get_func_ref = Shibboleth::Reflection<Derived>::GetReflectionDefinition().getVarT(Gaff::FNV1aHash32Const("cRef"))->getDataT<int>(*ref_result);
	//int test_get_func = Shibboleth::Reflection<Derived>::GetReflectionDefinition().getVarT(Gaff::FNV1aHash32Const("cFunc"))->getDataT<int>(*ref_result);
	int test_get = Shibboleth::Reflection<Derived>::GetReflectionDefinition().getVarT(Gaff::FNV1aHash32Const("c"))->getDataT<int>(*ref_result);

	printf(
		"GetFuncRef: %i\n"
		//"GetFunc: %i\n"
		"GetVar: %i\n",
		test_get_func_ref,
		//test_get_func,
		test_get
	);

	REQUIRE(test_get_func_ref == ref_result->c);
	//REQUIRE(test_get_func == ref_result->c);
	REQUIRE(test_get == ref_result->c);

	auto* ref_var = Shibboleth::Reflection<Derived>::GetReflectionDefinition().getVarT(Gaff::FNV1aHash32Const("a"));
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
class Test1 : public Gaff::IReflectionObject
{
	double a = 20.0;
	T t;

	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(Test1, T);
};

SHIB_TEMPLATE_REFLECTION_DECLARE(Test1, T);
SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE(Test1, T);
SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(Test1, T)
	.var("a", &Test1<T>::a)
	.var("t", &Test1<T>::t)
SHIB_TEMPLATE_REFLECTION_DEFINE_END(Test1, T)


template <class T1, class T2>
class Test2 : public Gaff::IReflectionObject
{
	float a = 11.0f;
	T1 t1;
	T2 t2;

	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(Test2, T1, T2);
};

SHIB_TEMPLATE_REFLECTION_DECLARE(Test2, T1, T2);
SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE(Test2, T1, T2);
SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(Test2, T1, T2)
	.var("a", &Test2<T1, T2>::a)
	.var("t1", &Test2<T1, T2>::t1)
	.var("t2", &Test2<T1, T2>::t2)
SHIB_TEMPLATE_REFLECTION_DEFINE_END(Test2, T1, T2)

TEST_CASE("shibboleth_reflection_template_class")
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
SHIB_REFLECTION_CLASS_DEFINE(VecTest);

SHIB_REFLECTION_DEFINE_BEGIN(VecTest)
	.var("vec", &VecTest::vec)
	.var("arr", &VecTest::arr)
	.var("base", &VecTest::base)
SHIB_REFLECTION_DEFINE_END(VecTest)

TEST_CASE("shibboleth_reflection_array_vector")
{
	Shibboleth::Reflection<VecTest>::Init();

	printf("\nReflection Class: %s\n", Shibboleth::Reflection<VecTest>::GetName());

	VecTest instance;
	Gaff::IReflectionObject* ptr = &instance;

	const Gaff::ReflectionDefinition<VecTest, Shibboleth::ProxyAllocator>& ref_def = Shibboleth::Reflection<VecTest>::GetReflectionDefinition();
	Gaff::IReflectionVar* vec_var = ref_def.getVar(Gaff::FNV1aHash32Const("vec"));
	Gaff::IReflectionVar* arr_var = ref_def.getVar(Gaff::FNV1aHash32Const("arr"));
	Gaff::IReflectionVar* base_var = ref_def.getVar(Gaff::FNV1aHash32Const("base"));
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


class CtorTest : public Gaff::IReflectionObject
{
public:
	CtorTest(void) {}
	CtorTest(int val): a(val) {}

	int a = 200;

	SHIB_REFLECTION_CLASS_DECLARE(CtorTest);
};

SHIB_REFLECTION_DECLARE(CtorTest);
SHIB_REFLECTION_CLASS_DEFINE(CtorTest);
SHIB_REFLECTION_DEFINE_BEGIN(CtorTest)
	.ctor<>()
	.ctor<int>()
	.var("a", &CtorTest::a)
SHIB_REFLECTION_DEFINE_END(CtorTest)

TEST_CASE("shibboleth_factory")
{
	Shibboleth::Reflection<CtorTest>::Init();
	CtorTest* test_a = Shibboleth::Reflection<CtorTest>::GetReflectionDefinition().create();
	CtorTest* test_b = Shibboleth::Reflection<CtorTest>::GetReflectionDefinition().create(100);

	REQUIRE(test_a->a == 200);
	REQUIRE(test_b->a == 100);

	SHIB_FREE(test_a, Shibboleth::GetAllocator());
	SHIB_FREE(test_b, Shibboleth::GetAllocator());
}

//TEST_CASE("shibboleth_reflection_module")
//{
//	Gaff::DynamicModule module;
//	bool ret = module.load("ResourceModule" BIT_EXTENSION);
//
//	REQUIRE(ret);
//
//	if (ret) {
//		using InitFunc = bool (*)(Shibboleth::IApp*);
//		InitFunc init = module.getFunc<InitFunc>("InitModule");
//		init(&Shibboleth::GetApp());
//	}
//}


class AttrTest final : public Gaff::IReflectionObject
{
public:
	AttrTest(void) {}
	AttrTest(int val) : a(val) {}

	int a = 555;

	SHIB_REFLECTION_CLASS_DECLARE(AttrTest);
};

class TestAttr final : public Gaff::IAttribute
{
public:
	Gaff::IAttribute* clone(void) const { return SHIB_ALLOCT(TestAttr, Shibboleth::GetAllocator()); }

	SHIB_REFLECTION_CLASS_DECLARE(TestAttr);
};

SHIB_REFLECTION_DECLARE(TestAttr);
SHIB_REFLECTION_DEFINE(TestAttr);
SHIB_REFLECTION_CLASS_DEFINE(TestAttr);


SHIB_REFLECTION_DECLARE(AttrTest);
SHIB_REFLECTION_CLASS_DEFINE(AttrTest);
SHIB_REFLECTION_DEFINE_BEGIN(AttrTest)
	.classAttrs
	(
		TestAttr(),
		TestAttr(),
		TestAttr()
	)

	.ctor<>()
	.ctor<int>()
	.var("a", &AttrTest::a, TestAttr(), TestAttr())
SHIB_REFLECTION_DEFINE_END(AttrTest)


TEST_CASE("shibboleth_attribute")
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
SHIB_REFLECTION_CLASS_DEFINE(FuncTest);

SHIB_REFLECTION_DEFINE_BEGIN(FuncTest)
	.ctor<>()
	.func("getMyInt", &FuncTest::getMyInt)
	.func("setMyInt", &FuncTest::setMyInt)
	.func("myInt", &FuncTest::getMyInt)
	.func("myInt", &FuncTest::setMyInt)
	.func("getIntRef", &FuncTest::getIntRef)
	.func("setIntRef", &FuncTest::setIntRef)
SHIB_REFLECTION_DEFINE_END(FuncTest)

TEST_CASE("shibboleth_func")
{
	Shibboleth::Reflection<FuncTest>::Init();

	const Gaff::IReflectionDefinition& ref_def = Shibboleth::Reflection<FuncTest>::GetReflectionDefinition();
	Gaff::IReflectionFunction<int>* const get_func = ref_def.getFunc<int>(Gaff::FNV1aHash32Const("getMyInt"));
	Gaff::IReflectionFunction<void, int>* const set_func = ref_def.getFunc<void, int>(Gaff::FNV1aHash32Const("setMyInt"));
	Gaff::IReflectionFunction<int>* const ovl_get_func = ref_def.getFunc<int>(Gaff::FNV1aHash32Const("myInt"));
	Gaff::IReflectionFunction<void, int>* const ovl_set_func = ref_def.getFunc<void, int>(Gaff::FNV1aHash32Const("myInt"));
	Gaff::IReflectionFunction<const int&>* const ref_get_func = ref_def.getFunc<const int&>(Gaff::FNV1aHash32Const("getIntRef"));
	Gaff::IReflectionFunction<void, const int&>* const ref_set_func = ref_def.getFunc<void, const int&>(Gaff::FNV1aHash32Const("setIntRef"));

	void* data = ref_def.create(Shibboleth::ProxyAllocator::GetGlobal());

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

	SHIB_FREE(data, Shibboleth::GetAllocator());
}

enum class TestEnum
{
	MinusOne = -1,
	Zero,
	One,
	Two,
	Twenty = 20
};

SHIB_REFLECTION_DECLARE(TestEnum)

SHIB_REFLECTION_DEFINE_BEGIN(TestEnum)
	.entry("MinusOne", TestEnum::MinusOne)
	.entry("Zero", TestEnum::Zero)
	.entry("One", TestEnum::One)
	.entry("Two", TestEnum::Two)
	.entry("Twenty", TestEnum::Twenty)
SHIB_REFLECTION_DEFINE_END(TestEnum)

TEST_CASE("shibboleth_enum")
{
	Shibboleth::Reflection<TestEnum>::Init();

	const Gaff::IEnumReflectionDefinition& ref_def = Shibboleth::Reflection<TestEnum>::GetReflectionDefinition();
	REQUIRE(ref_def.getNumEntries() == 5);

	printf("Enum Name Index 0: %s\n", ref_def.getEntryNameFromIndex(0).getBuffer());

	REQUIRE(!strcmp("MinusOne", ref_def.getEntryNameFromValue(static_cast<int32_t>(TestEnum::MinusOne)).getBuffer()));
	REQUIRE(!strcmp("Zero", ref_def.getEntryNameFromValue(static_cast<int32_t>(TestEnum::Zero)).getBuffer()));
	REQUIRE(!strcmp("One", ref_def.getEntryNameFromValue(static_cast<int32_t>(TestEnum::One)).getBuffer()));
	REQUIRE(!strcmp("Two", ref_def.getEntryNameFromValue(static_cast<int32_t>(TestEnum::Two)).getBuffer()));
	REQUIRE(!strcmp("Twenty", ref_def.getEntryNameFromValue(static_cast<int32_t>(TestEnum::Twenty)).getBuffer()));

	printf("MinusOne: %s\n", ref_def.getEntryNameFromValue(static_cast<int32_t>(TestEnum::MinusOne)).getBuffer());
	printf("Zero: %s\n", ref_def.getEntryNameFromValue(static_cast<int32_t>(TestEnum::Zero)).getBuffer());
	printf("One: %s\n", ref_def.getEntryNameFromValue(static_cast<int32_t>(TestEnum::One)).getBuffer());
	printf("Two: %s\n", ref_def.getEntryNameFromValue(static_cast<int32_t>(TestEnum::Two)).getBuffer());
	printf("Twenty: %s\n\n", ref_def.getEntryNameFromValue(static_cast<int32_t>(TestEnum::Twenty)).getBuffer());

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

static bool LoadSTCPtr(const Gaff::ISerializeReader& reader, STCPtr& out)
{
	GAFF_REF(out);
	printf("Load STCPtr\n");

	reader.enterElement("test_string");
	char buffer[128] = { 0 };
	printf("Value at 'test_string' is '%s'\n", reader.readString(buffer, sizeof(buffer)));
	reader.exitElement();

	return true;
}

static void SaveSTCPtr(Gaff::ISerializeWriter& writer, const STCPtr& value)
{
	GAFF_REF(value);
	printf("Save STCPtr\n");

	writer.startObject(1);
	writer.writeString("test_string", "This is a test string");
	writer.endObject();
}


SHIB_REFLECTION_DECLARE(STCPtr)

SHIB_REFLECTION_DEFINE_BEGIN(STCPtr)
	.serialize(LoadSTCPtr, SaveSTCPtr)
SHIB_REFLECTION_DEFINE_END(STCPtr)


template <class T, class TT>
class SerializeTestTemplate
{
};

template <class T, class TT>
static bool LoadTest(const Gaff::ISerializeReader& reader, SerializeTestTemplate<T, TT>& out)
{
	GAFF_REF(out);
	printf("Load %s\n", Shibboleth::Reflection< SerializeTestTemplate<T, TT> >::GetName());

	reader.enterElement("test_double");
	printf("Value at 'test_double' is '%f'\n", reader.readDouble());
	reader.exitElement();

	return true;
}

template <class T, class TT>
static void SaveTest(Gaff::ISerializeWriter& writer, const SerializeTestTemplate<T, TT>& value)
{
	GAFF_REF(value);
	printf("Save %s\n", Shibboleth::Reflection< SerializeTestTemplate<T, TT> >::GetName());

	writer.startObject(1);
	writer.writeDouble("test_double", 123.0);
	writer.endObject();
}

SHIB_TEMPLATE_REFLECTION_DECLARE(SerializeTestTemplate, T, TT);
SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(SerializeTestTemplate, T, TT)
	.serialize(LoadTest, SaveTest)
SHIB_TEMPLATE_REFLECTION_DEFINE_END(SerializeTestTemplate, T, TT)



TEST_CASE("reflection serialize test", "[shibboleth_serialize]")
{
	// Test STCPtr with JSON.
	{
		Gaff::JSONSerializeWriter<> json_writer;
		STCPtr stc;

		Shibboleth::Reflection<STCPtr>::Save(json_writer, stc);

		Gaff::SerializeReader<Gaff::JSON> json_reader(json_writer.getRootNode());
		REQUIRE(Shibboleth::Reflection<STCPtr>::Load(json_reader, stc));
	}

	// Test STT with JSON.
	{
		Gaff::JSONSerializeWriter<> json_writer;
		SerializeTestTemplate<int32_t, double> stt;

		Shibboleth::Reflection< SerializeTestTemplate<int32_t, double> >::Save(json_writer, stt);

		Gaff::SerializeReader<Gaff::JSON> json_reader(json_writer.getRootNode());
		REQUIRE(Shibboleth::Reflection< SerializeTestTemplate<int32_t, double> >::Load(json_reader, stt));
	}

	// Test STCPtr with MessagePack.
	{
		Gaff::MessagePackSerializeWriter mpack_writer;
		STCPtr stc;

		char buffer[1024] = { 0 };

		REQUIRE(mpack_writer.init(buffer, sizeof(buffer)));
		Shibboleth::Reflection<STCPtr>::Save(mpack_writer, stc);
		mpack_writer.finish();

		// Write buffer to a file.
		const std::filesystem::path path("tests/test.mpack");
		std::fstream file(path, std::ios_base::out | std::ios_base::trunc);

		REQUIRE(file.is_open());
		file.write(buffer, mpack_writer.size());
		file.close();

		REQUIRE(std::filesystem::is_regular_file(path)); // Check that file was written.

		// Test buffer we wrote to with mpack_writer.
		{
			Gaff::MessagePackReader reader;

			REQUIRE(reader.parse(buffer, sizeof(buffer)));
			Gaff::SerializeReader<Gaff::MessagePackNode> mpack_reader(reader.getRoot());
			REQUIRE(Shibboleth::Reflection<STCPtr>::Load(mpack_reader, stc));
		}

		// Test binary file written to disk.
		{
			Gaff::MessagePackReader reader;

			REQUIRE(reader.openFile("tests/test.mpack"));
			Gaff::SerializeReader<Gaff::MessagePackNode> mpack_reader(reader.getRoot());
			REQUIRE(Shibboleth::Reflection<STCPtr>::Load(mpack_reader, stc));
		}

		std::filesystem::remove(path); // Don't leave it on disk.
	}

	// Test STT with MessagePack.
	{
		Gaff::MessagePackSerializeWriter mpack_writer;
		SerializeTestTemplate<int32_t, double> stt;

		char buffer[1024] = {0};

		REQUIRE(mpack_writer.init(buffer, sizeof(buffer)));
		Shibboleth::Reflection< SerializeTestTemplate<int32_t, double> >::Save(mpack_writer, stt);
		mpack_writer.finish();

		// Write buffer to a file.
		const std::filesystem::path path("tests/test2.mpack");
		std::fstream file(path, std::ios_base::out | std::ios_base::trunc);

		REQUIRE(file.is_open());
		file.write(buffer, mpack_writer.size());
		file.close();

		REQUIRE(std::filesystem::is_regular_file(path)); // Check that file was written.

		// Test buffer we wrote to with mpack_writer.
		{
			Gaff::MessagePackReader reader;

			REQUIRE(reader.parse(buffer, sizeof(buffer)));
			Gaff::SerializeReader<Gaff::MessagePackNode> mpack_reader(reader.getRoot());
			REQUIRE(Shibboleth::Reflection< SerializeTestTemplate<int32_t, double> >::Load(mpack_reader, stt));
		}

		// Test binary file written to disk.
		{
			Gaff::MessagePackReader reader;

			REQUIRE(reader.openFile("tests/test2.mpack"));
			Gaff::SerializeReader<Gaff::MessagePackNode> mpack_reader(reader.getRoot());
			REQUIRE(Shibboleth::Reflection< SerializeTestTemplate<int32_t, double> >::Load(mpack_reader, stt));
		}

		std::filesystem::remove(path); // Don't leave it on disk.
	}
}
