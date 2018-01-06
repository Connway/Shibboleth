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

#include <Shibboleth_Reflection.h>
#include <Shibboleth_AngelScript.h>
#include <Shibboleth_RefCounted.h>
#include <Shibboleth_App.h>
#include <angelscript.h>
#include <scriptbuilder.h>

Shibboleth::App g_app;

asIScriptEngine* e = nullptr;

class ScriptTest
{
public:
	int test_var = 10;

	void testSet(int val) { test_var = val; }
	int testGet(void) const { return test_var; }
};

SHIB_REFLECTION_DECLARE(ScriptTest)
SHIB_REFLECTION_EXTERNAL_DEFINE(ScriptTest)

SHIB_REFLECTION_BUILDER_BEGIN(ScriptTest)
	.classAttrs(RegisterAngelScriptAttribute<ScriptTest>(AS_NONE, e))
	.ctor<>()
	.var("test_var", &ScriptTest::test_var)
	.func("testSet", &ScriptTest::testSet)
	.func("testGet", &ScriptTest::testGet)
	.var("test_accessor", &ScriptTest::testGet, &ScriptTest::testSet)
	.var("test_readonly", &ScriptTest::testGet, static_cast<void (ScriptTest::*)(int)>(nullptr))
SHIB_REFLECTION_BUILDER_END(ScriptTest)

static void MessageCallback(const asSMessageInfo* msg, void* /*param*/)
{
	const char *type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";

	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

static void ScriptPrint(void)
{
	printf("AngelScript called something!\n");
}

static void ScriptPrintInt(int v)
{
	printf("Script Print: %i\n", v);
}

TEST_CASE("reflection_script_test", "[shibboleth_reflection_script]")
{
	g_app.init(0, nullptr);
	Shibboleth::SetApp(g_app);

	asSetGlobalMemoryFunctions(Shibboleth::ShibbolethAllocate, Shibboleth::ShibbolethFree);
	e = asCreateScriptEngine();
	e->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
	e->SetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE, 1);
	e->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, 1);
	e->RegisterGlobalFunction("void print()", asFUNCTION(ScriptPrint), asCALL_CDECL);
	e->RegisterGlobalFunction("void print(int)", asFUNCTION(ScriptPrintInt), asCALL_CDECL);

	Shibboleth::Reflection<ScriptTest>::Init();

	CScriptBuilder builder;

	builder.StartNewModule(e, "testscript");

	builder.AddSectionFromMemory(
		"testscript",
		R""(
		class TestA
		{
			int a = 77;
		}

		void main()
		{
			print();
		}

		void test(ScriptTest& v)
		{
			v.test_var = 22;
		}

		void test2()
		{
			ScriptTest test;
			print(test.test_var);
			print(test.testGet());
			test.testSet(500);
			print(test.test_var);

			test.test_accessor = 150;
			print(test.test_accessor);

			test.test_accessor = 12345;
			print(test.test_readonly);
		}

		void test3()
		{
			TestB test;
			test.speak();
		}
		)""
	);

	builder.AddSectionFromMemory(
		"testscript2",
		R""(
			class TestB
			{
				void speak()
				{
					print();
				}
			}

			void TestClassFunc()
			{
				TestA test;
				print(test.a);
				test.a = 32;
				print(test.a);
			}
		)""
	);

	builder.BuildModule();

	asIScriptModule* module = e->GetModule("testscript");
	asIScriptFunction* func = module->GetFunctionByName("main");
	asIScriptContext* ctx = e->RequestContext();
	ctx->Prepare(func);
	ctx->Execute();

	e->ReturnContext(ctx);

	ScriptTest test;

	func = module->GetFunctionByName("test");
	ctx = e->RequestContext();
	ctx->Prepare(func);
	ctx->SetArgObject(0, &test);
	ctx->Execute();

	e->ReturnContext(ctx);

	REQUIRE(test.test_var == 22);

	func = module->GetFunctionByName("test2");
	ctx = e->RequestContext();
	ctx->Prepare(func);
	ctx->Execute();

	e->ReturnContext(ctx);

	func = module->GetFunctionByName("test3");
	ctx = e->RequestContext();
	ctx->Prepare(func);
	ctx->Execute();

	e->ReturnContext(ctx);

	func = module->GetFunctionByName("TestClassFunc");
	ctx = e->RequestContext();
	ctx->Prepare(func);
	ctx->Execute();

	e->ReturnContext(ctx);
	e->ShutDownAndRelease();
}

class BaseTest
{
public:
	int bar = 51;

	SHIB_REFLECTION_CLASS_DECLARE(BaseTest);
};

SHIB_REFLECTION_DECLARE(BaseTest)
SHIB_REFLECTION_DEFINE(BaseTest)

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(BaseTest)
	.classAttrs(Shibboleth::RegisterAngelScriptAttribute<BaseTest>(Shibboleth::AS_VALUE_AS_REF, e))
	.var("bar", &BaseTest::bar)
SHIB_REFLECTION_CLASS_DEFINE_END(BaseTest)


class TestRefCount : public Gaff::IRefCounted, public BaseTest
{
public:
	void addRef(void) const
	{
		int32_t new_count = ++_count;
		printf("AddRef: %i\n", new_count);
	}

	void release(void) const
	{
		int32_t new_count = --_count;
		printf("Release: %i\n", new_count);

		if (!new_count) {
			printf("Freeing TestRefCount instance!\n");
			SHIB_FREET(this, *Shibboleth::GetAllocator());
		}
	}

	int32_t getRefCount(void) const
	{
		return _count;
	}

	int foo = 23;

private:
	mutable std::atomic_int32_t _count = 0;

	SHIB_REFLECTION_CLASS_DECLARE(TestRefCount);
};

SHIB_REFLECTION_DECLARE(TestRefCount)
SHIB_REFLECTION_DEFINE(TestRefCount)

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(TestRefCount)
	.classAttrs(Shibboleth::RegisterAngelScriptAttribute<TestRefCount>(Shibboleth::AS_NONE, e))
	.BASE(Gaff::IRefCounted)
	.base<BaseTest>()
	.ctor<>()
	.var("foo", &TestRefCount::foo)
SHIB_REFLECTION_CLASS_DEFINE_END(TestRefCount)

TEST_CASE("reflection_refcounted_test", "[shibboleth_reflection_refcounted]")
{
	asSetGlobalMemoryFunctions(Shibboleth::ShibbolethAllocate, Shibboleth::ShibbolethFree);
	e = asCreateScriptEngine();
	e->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
	e->SetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE, 1);
	e->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, 1);
	e->RegisterGlobalFunction("void print()", asFUNCTION(ScriptPrint), asCALL_CDECL);
	e->RegisterGlobalFunction("void print(int)", asFUNCTION(ScriptPrintInt), asCALL_CDECL);

	Shibboleth::Reflection<TestRefCount>::Init();
	Shibboleth::Reflection<BaseTest>::Init();

	CScriptBuilder builder;

	builder.StartNewModule(e, "testscript");

	// Test out casting to and from Derived and Base types.
	builder.AddSectionFromMemory(
		"testscript",
		R""(
		void main()
		{
			TestRefCount@ foo = TestRefCount();
			print(foo.foo);
			foo.foo = 5514;
			print(foo.foo);

			print(foo.bar);
			foo.bar = 1234;
			print(foo.bar);

			BaseTest@ b = @foo;
			TestRefCount@ c;
			@c = cast<TestRefCount>(b);

			b.bar = 1235;
			print(b.bar);
			print(c.bar);
			print(foo.bar);
		}
		)""
	);

	builder.BuildModule();

	asIScriptModule* module = e->GetModule("testscript");
	asIScriptFunction* func = module->GetFunctionByName("main");
	asIScriptContext* ctx = e->RequestContext();
	ctx->Prepare(func);
	ctx->Execute();

	e->ReturnContext(ctx);
	e->ShutDownAndRelease();

	g_app.destroy();
}
