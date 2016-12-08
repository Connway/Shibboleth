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

#pragma once

#include <Shibboleth_Defines.h>
#include <Shibboleth_Reflection.h>
#include <algorithm>
#include <memory>

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable : 4541)
#endif

// For defining bare minimum needed to register classes.
#include <dispatchkit/proxy_constructors.hpp>
#include <dispatchkit/register_function.hpp>
#include <dispatchkit/type_info.hpp>

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif



namespace chaiscript
{
	class ChaiScript;
	class Module;
}

// These pretty much exist only because ChaiScript uses RTTI and I'm isolating it.
NS_SHIBBOLETH

using ChaiScriptModulePtr = std::shared_ptr<chaiscript::Module>;

chaiscript::ChaiScript* CreateChaiScriptInstance(void);
void CreateChaiScriptInstance(chaiscript::ChaiScript* chai);
void ChaiScriptEval(chaiscript::ChaiScript* chai, const char* script);
void ChaiScriptAdd(chaiscript::ChaiScript* chai, const ChaiScriptModulePtr& module);
ChaiScriptModulePtr CreateChaiScriptModule(void);

void ChaiScriptModuleEval(chaiscript::Module& module, const char* script);
void ChaiScriptModuleAdd(chaiscript::Module& module, const char* name, const chaiscript::Type_Info& info);
void ChaiScriptModuleAdd(chaiscript::Module& module, const char* name, const chaiscript::Proxy_Function& function);
void ChaiScriptModuleAdd(chaiscript::Module& module, const chaiscript::Type_Conversion& type_conversion);



template <class T>
class ChaiScriptClassRegister
{
public:
	ChaiScriptClassRegister(chaiscript::Module& module);

	template <class Base>
	ChaiScriptClassRegister& base(const char*, Gaff::ReflectionHash);

	template <class Base>
	ChaiScriptClassRegister& base(void);

	template <class... Args>
	ChaiScriptClassRegister& ctor(void);

	template <class Var, size_t size>
	ChaiScriptClassRegister& var(const char (&name)[size], Var T::*ptr);

	template <class Ret, class Var, size_t size>
	ChaiScriptClassRegister& var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var));

	template <size_t size, class Ret, class... Args>
	ChaiScriptClassRegister& func(const char (&name)[size], Ret (T::*ptr)(Args...) const);

	template <size_t size, class Ret, class... Args>
	ChaiScriptClassRegister& func(const char (&name)[size], Ret (T::*ptr)(Args...));

	void finish(void);

private:
	chaiscript::Module& _module;
};

NS_END

#include "Shibboleth_ChaiScript.inl"
