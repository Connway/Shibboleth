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
#include <Shibboleth_AngelScript.h>
#include <Shibboleth_ChaiScript.h>
#include <Shibboleth_App.h>
#include <angelscript.h>
#include <scriptbuilder.h>

Shibboleth::ChaiScriptModulePtr m;
Shibboleth::App g_app;

asIScriptEngine* e = nullptr;

class ScriptTest
{
public:
	int test_var = 10;

	void testSet(int val) { test_var = val; }
	int testGet(void) const { return test_var; }
};

SHIB_REFLECTION_DECLARE(ScriptTest);

SHIB_REFLECTION_DEFINE_BEGIN_CUSTOM_BUILDER(ScriptTest)
	ChaiScriptClassRegister<ScriptTest> csr(*m);
	AngelScriptClassRegister<ScriptTest> asr(e);

	BuildReflection(csr);
	BuildReflection(asr);
SHIB_REFLECTION_DEFINE_END(ScriptTest)

SHIB_REFLECTION_BUILDER_BEGIN(ScriptTest)
	.ctor<>()
	.var("test_var", &ScriptTest::test_var)
	.func("testSet", &ScriptTest::testSet)
	.func("testGet", &ScriptTest::testGet)
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
	//e->SetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE, 1);
	e->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, 1);
	e->RegisterGlobalFunction("void print()", asFUNCTION(ScriptPrint), asCALL_CDECL);
	e->RegisterGlobalFunction("void print(int)", asFUNCTION(ScriptPrintInt), asCALL_CDECL);

	m = Shibboleth::CreateChaiScriptModule();
	Shibboleth::Reflection<ScriptTest>::Init();

	chaiscript::ChaiScript* chai = Shibboleth::CreateChaiScriptInstance();

	Shibboleth::ChaiScriptModuleEval(
		*m,
		R""(
		class Test
		{
			def Test() { this.a = 66 }
			attr a
		}
		)""
	);

	Shibboleth::ChaiScriptAdd(chai, m);

	Shibboleth::ChaiScriptEval(
		chai,
		R""(
		var test = ScriptTest()
		print("test_var: " + to_string(test.test_var))
		print("testGet: " + to_string(test.testGet()))
		test.test_var = 20
		print("test_var after variable assign: " + to_string(test.test_var))
		print("testGet after variable assign: " + to_string(test.testGet()))
		test.testSet(30)
		print("test_var after testSet() call: " + to_string(test.test_var))
		print("testGet after testSet() call: " + to_string(test.testGet()))
		var test2 = Test()
		print("Script class: "+ to_string(test2.a))
		test2.a = 55
		print("Script class: "+ to_string(test2.a))
		)""
	);

	CScriptBuilder builder;

	int r = builder.StartNewModule(e, "testscript");
	//if (r < 0) return -1;

	r = builder.AddSectionFromMemory(
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
		}

		void test3()
		{
			TestB test;
			test.speak();
		}
		)""
	);
	//if (r < 0) return -1;

	r = builder.AddSectionFromMemory(
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

	r = builder.BuildModule();

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

	REQUIRE(test.test_var == 22);

	func = module->GetFunctionByName("test2");
	ctx = e->RequestContext();
	ctx->Prepare(func);
	ctx->Execute();

	func = module->GetFunctionByName("test3");
	ctx = e->RequestContext();
	ctx->Prepare(func);
	ctx->Execute();

	func = module->GetFunctionByName("TestClassFunc");
	ctx = e->RequestContext();
	ctx->Prepare(func);
	ctx->Execute();

	e->ShutDownAndRelease();
}
