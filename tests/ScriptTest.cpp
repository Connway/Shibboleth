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
#include <Shibboleth_ChaiScript.h>
#include <Shibboleth_App.h>

Shibboleth::ChaiScriptModulePtr m;
Shibboleth::App g_app;

class ScriptTest
{
public:
	int test_var = 10;

	void testSet(int val) { test_var = val; }
	int testGet(void) const { return test_var; }

	SHIB_REFLECTION_CLASS_DECLARE(ScriptTest);
};

SHIB_REFLECTION_DECLARE(ScriptTest);

SHIB_REFLECTION_DEFINE_BEGIN(ScriptTest)
	ChaiScriptClassRegister<ScriptTest> csr(*m);
	//AngelScriptClassRegister<ScriptTest> asr();

	ScriptTest::BuildReflection(csr);
	//ScriptTest::BuildReflection(asr);
SHIB_REFLECTION_DEFINE_END(ScriptTest)

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ScriptTest)
	.ctor<ScriptTest()>()
	.var("test_var", &ScriptTest::test_var)
	.func("testSet", &ScriptTest::testSet)
	.func("testGet", &ScriptTest::testGet)
SHIB_REFLECTION_CLASS_DEFINE_END(ScriptTest)


TEST_CASE("reflection_script_test", "[shibboleth_reflection_script]")
{
	g_app.init(0, nullptr);
	Shibboleth::SetApp(g_app);

	m = Shibboleth::CreateChaiScriptModule();
	Shibboleth::Reflection<ScriptTest>::Init();

	chaiscript::ChaiScript* chai = Shibboleth::CreateChaiScriptInstance();
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
		)""
	);
}
