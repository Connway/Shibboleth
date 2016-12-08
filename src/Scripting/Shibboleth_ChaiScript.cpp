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

#include "Shibboleth_ChaiScript.h"
#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>
#include <chaiscript_stdlib.hpp>
#include <chaiscript.hpp>

NS_SHIBBOLETH

chaiscript::ChaiScript* CreateChaiScriptInstance(void)
{
	chaiscript::ChaiScript* chai = SHIB_ALLOCT(chaiscript::ChaiScript, *GetAllocator(), chaiscript::Std_Lib::library());
	return chai;
}

void CreateChaiScriptInstance(chaiscript::ChaiScript* chai)
{
	SHIB_FREET(chai, *GetAllocator());
}

void ChaiScriptEval(chaiscript::ChaiScript* chai, const char* code)
{
	chai->eval(code);
}

void ChaiScriptAdd(chaiscript::ChaiScript* chai, const ChaiScriptModulePtr& module)
{
	chai->add(module);
}

ChaiScriptModulePtr CreateChaiScriptModule(void)
{
	return chaiscript::ModulePtr(new chaiscript::Module);
}

void ChaiScriptModuleEval(chaiscript::Module& module, const char* script)
{
	module.eval(script);
}

void ChaiScriptModuleAdd(chaiscript::Module& module, const char* name, const chaiscript::Type_Info& info)
{
	module.add(info, name);
}

void ChaiScriptModuleAdd(chaiscript::Module& module, const char* name, const chaiscript::Proxy_Function& function)
{
	module.add(function, name);
}

void ChaiScriptModuleAdd(chaiscript::Module& module, const chaiscript::Type_Conversion& type_conversion)
{
	module.add(type_conversion);
}

NS_END
