/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gen_ReflectionInit.h"
//#include <Shibboleth_RegisterWithScriptAttribute.h>
#include <Shibboleth_Utilities.h>

DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp* app)
{
	Shibboleth::SetApp(*app);
	Gen::InitReflection();
	return true;
}

//DYNAMICEXPORT_C void AllModulesLoaded(void)
//{
//	const Shibboleth::Vector<const Gaff::IEnumReflectionDefinition*> enum_ref_defs = Shibboleth::GetApp().getEnumReflectionWithAttribute(Gaff::FNV1aHash64Const("IRegisterWithScriptAttribute"));
//	const Shibboleth::Vector<const Gaff::IReflectionDefinition*> ref_defs = Shibboleth::GetApp().getReflectionWithAttribute(Gaff::FNV1aHash64Const("IRegisterWithScriptAttribute"));
//
//	Shibboleth::AngelScriptManager& script_mgr = Shibboleth::GetApp().getManagerTUnsafe<Shibboleth::AngelScriptManager>();
//
//	for (const Gaff::IEnumReflectionDefinition* enum_ref_def : enum_ref_defs) {
//		const IRegisterWithScriptAttribute* const attr = enum_ref_def->GET_ENUM_ATTRIBUTE(IRegisterWithScriptAttribute);
//	}
//
//	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
//		const IRegisterWithScriptAttribute* const attr = ref_def->GET_CLASS_ATTRIBUTE(IRegisterWithScriptAttribute);
//
//		//Shibboleth::AngelScriptClassRegister<T> asr(script_mgr.getEngine(), attr->getFlags());
//		//Shibboleth::Reflection<T>::BuildReflection(asr);
//	}
//}
