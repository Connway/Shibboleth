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

#include "Shibboleth_AngelScriptManager.h"
#include "Shibboleth_AngelScriptComponentWrapper.h"
#include "Shibboleth_AngelScriptBaseTypes.h"
#include <Shibboleth_IncludeAngelScript.h>
#include <Shibboleth_LogManager.h>
#include <scriptarray.h>

SHIB_REFLECTION_DEFINE(AngelScriptManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(AngelScriptManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(AngelScriptManager)

// Ensure creation of pool index.
static int32_t g_as_alloc_pool = GetAllocator()->getPoolIndex("AngelScript");

static void* ASAlloc(size_t size)
{
	return SHIB_ALLOC(size, g_as_alloc_pool, *GetAllocator());
}

static void ScriptPrint(int a)
{
	char tmp[1024] = { 0 };
	snprintf(tmp, 1024, "Script Print: %i\n", a);
	OutputDebugStringA(tmp);
}

static void ScriptPrint(float a)
{
	char tmp[1024] = { 0 };
	snprintf(tmp, 1024, "Script Print: %f\n", a);
	OutputDebugStringA(tmp);
}

bool AngelScriptManager::init(void)
{
	asSetGlobalMemoryFunctions(ASAlloc, ShibbolethFree);
	_engine = asCreateScriptEngine();

	//CScriptArray::SetMemoryFunctions();

	_engine->SetMessageCallback(asMETHOD(AngelScriptManager, messageCallback), this, asCALL_THISCALL);
	_engine->SetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE, 1);
	_engine->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, 1);

	_engine->RegisterGlobalFunction("void print(int)", asFUNCTIONPR(ScriptPrint, (int), void), asCALL_CDECL);
	_engine->RegisterGlobalFunction("void print(float)", asFUNCTIONPR(ScriptPrint, (float), void), asCALL_CDECL);

	RegisterScriptArray(_engine, true);

	// Declare them ahead of time so that we can add functions that reference them without generating errors.
	DeclareTypes(_engine);
	RegisterMath(_engine);
	RegisterString(_engine);
	RegisterObject(_engine);
	RegisterComponent(_engine);

	//AngelScriptComponentWrapper::Register(_engine);

	return true;
}

asIScriptEngine* AngelScriptManager::getEngine(void) const
{
	return _engine;
}

Gaff::SpinLock& AngelScriptManager::getEngineLock(void)
{
	return _lock;
}

void AngelScriptManager::messageCallback(const asSMessageInfo* msg, void* /*param*/)
{
	GAFF_REF(msg);
}

NS_END
