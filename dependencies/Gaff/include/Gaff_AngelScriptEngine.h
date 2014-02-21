/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#pragma once

#include "Gaff_String.h"
#include "Gaff_RefPtr.h"
#include "Gaff_AngelScriptContext.h"
#include "Gaff_AngelScriptModule.h"
#include <add_on/scriptbuilder/scriptbuilder.h>

NS_GAFF

typedef void (*MSGCALLBACK)(const asSMessageInfo* msg);

class AngelScriptModule;

class AngelScriptEngine
{
public:
	// Cause dat shit is LOOOOOOOOOONG 
	#include "Gaff_AngelScriptEngineTemplateFunctions.inl"

	AngelScriptEngine(void);
	~AngelScriptEngine(void);

	bool init(void);
	void destroy(void);

	bool setMessageCallback(MSGCALLBACK callback, asECallConvTypes call_type = asCALL_CDECL);
	bool setMemoryFunctions(asALLOCFUNC_t alloc, asFREEFUNC_t free);

	// Calls must be in the order of startModule(), addScript(), and buildModule() otherwise things will get messed up and it's not my problem.
	// addScript() can be called as many times as necessary before buildModule() is called.
	bool startModule(const char* module_name);
	bool addScript(const char* file_name);
	bool buildModule(void);

#ifdef _UNICODE
	bool addScript(const wchar_t* file_name, const char* section_name);
#endif

	AngelScriptModule getModule(const char* module_name);

	bool registerEnum(const char* type);
	bool registerEnumValue(const char* type, const char* name, int value);
	bool registerGlobalProperty(const char* declaration, void* object);

	bool registerObjectType(const char* object, int size_bytes, unsigned long type_flags);
	bool registerObjectProperty(const char* object, const char* declaration, int byte_offset);

	bool registerInterface(const char* name);
	bool registerInterfaceMethod(const char* interface_name, const char* declaration);

	bool registerFunctionDefinition(const char* declaration);
	bool registerTypedef(const char* type, const char* declaration);
	bool registerDefaultArrayType(const char* type);

	AngelScriptContext createContext(void);

private:
	COMRefPtr<asIScriptEngine> _engine;
	CScriptBuilder _builder;

#ifdef _UNICODE
	asALLOCFUNC_t _alloc;
	asFREEFUNC_t _free;
#endif

	void messageCallback(const asSMessageInfo* msg) const;
	bool registerGlobalFunctionHelper(const char* declaration, const asSFuncPtr& func_ptr, asECallConvTypes call_type, void* this_obj);
	bool registerObjectMethodHelper(const char* object, const char* declaration, const asSFuncPtr& func_ptr, asECallConvTypes call_type);
	bool registerObjectBehaviourHelper(const char* object, const char* declaration, const asSFuncPtr& func_ptr,
										asEBehaviours behaviour, asECallConvTypes call_type, void* this_obj);
	bool registerStringFactoryHelper(const char* type, const asSFuncPtr& factory_func, asECallConvTypes call_type, void* this_obj);
};

NS_END
