/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include "Shibboleth_AngelScriptString.h"
#include "Shibboleth_AngelScriptMath.h"
#include "Shibboleth_IncludeAngelScript.h"

#define CHECK_RESULT(r) if (r < 0) { return false; }

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::AngelScriptManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::AngelScriptManager)

namespace
{
	static bool CheckMemberFunctionPointerSize(size_t mem_func_size)
	{
		return mem_func_size == static_cast<size_t>(SINGLE_PTR_SIZE) ||
			mem_func_size == static_cast<size_t>(SINGLE_PTR_SIZE+1*sizeof(int)) ||
			mem_func_size == static_cast<size_t>(SINGLE_PTR_SIZE+2*sizeof(int)) ||
			mem_func_size == static_cast<size_t>(SINGLE_PTR_SIZE+3*sizeof(int)0 ||
			mem_func_size == static_cast<size_t>(SINGLE_PTR_SIZE+4*sizeof(int));
	}
}


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(AngelScriptManager)

AngelScriptManager::~AngelScriptManager(void)
{
}

bool AngelScriptManager::initAllModulesLoaded(void)
{
	return true;
}

bool AngelScriptManager::init(void)
{
	_engine = asCreateScriptEngine();

	if (!_engine) {
		// $TODO: Log error.
		return false;
	}

	int result = _engine->SetMessageCallback(asMETHOD(AngelScriptManager, messageCallback), this, asCALL_THISCALL);
	CHECK_RESULT(result)

	result = _engine->SetEngineProperty(asEP_ALLOW_MULTILINE_STRINGS, true);
	CHECK_RESULT(result)

	result = _engine->SetEngineProperty(asEP_USE_CHARACTER_LITERALS, true);
	CHECK_RESULT(result)

	// Treat warnings as errors.
	result = _engine->SetEngineProperty(asEP_COMPILER_WARNINGS, 2);
	CHECK_RESULT(result)

	if (!RegisterScriptMath_Native(_engine)) {
		// $TODO: Log error.
		return false;
	}

	if (!RegisterScriptString_Native(_engine)) {
		// $TODO: Log error.
		return false;
	}

	// $TODO: Register array type.
	// $TODO: Register reflected classes.


	// asFunctionPtr((void (*)())func);
	// asSMethodPtr<sizeof(void (c::*)())>::Convert(AS_METHOD_AMBIGUITY_CAST(r (c::*)p)(&c::m))

	// Mark this as a class method
	// asSFuncPtr p(3);
	// p.CopyMethodPtr(&Mthd, method_ptr_size);

	return true;
}

void AngelScriptManager::messageCallback(const asSMessageInfo* msg, void* param)
{
	GAFF_REF(msg, param);

/*	const char *type = "ERR ";
	if( msg->type == asMSGTYPE_WARNING )
		type = "WARN";
	else if( msg->type == asMSGTYPE_INFORMATION )
		type = "INFO";

	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);*/
}

NS_END
