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

#pragma once

#include <Shibboleth_EnumReflection.h>
#include <Shibboleth_Reflection.h>
#include <Gaff_IRefCounted.h>
#include <Shibboleth_IncludeAngelScript.h>
#include <Shibboleth_AngelScriptManager.h>
#include <Shibboleth_Function.h>

NS_SHIBBOLETH

enum AngelScriptFlags {
	AS_NONE = 0,
	AS_NO_REF_COUNT = 1 << 0,
	AS_VALUE_AS_REF = 1 << 1
};

template <class T, class B = T>
class AngelScriptClassRegister final
{
public:
	AngelScriptClassRegister(asIScriptEngine* engine, AngelScriptFlags as_flags = AS_NONE);

	template <class Base>
	AngelScriptClassRegister& base(const char*);

	template <class Base>
	AngelScriptClassRegister& base(void);

	template <class... Args>
	AngelScriptClassRegister& ctor(void);

	template <class Var, size_t size>
	AngelScriptClassRegister& var(const char(&name)[size], Var B::*ptr, bool read_only = false);

	template <class Ret, class Var, size_t size>
	AngelScriptClassRegister& var(const char(&name)[size], Ret (B::*getter)(void) const, void (B::*setter)(Var));

	template <size_t size, class Ret, class... Args>
	AngelScriptClassRegister& func(const char(&name)[size], Ret (B::*ptr)(Args...) const);

	template <size_t size, class Ret, class... Args>
	AngelScriptClassRegister& func(const char(&name)[size], Ret (B::*ptr)(Args...));

	template <class... Args>
	AngelScriptClassRegister& classAttrs(const Args&...);

	template <size_t size, class... Args>
	AngelScriptClassRegister& varAttrs(const char (&)[size], const Args&...);

	AngelScriptClassRegister& attrFile(const char*);

	void finish(void);

private:
	static asIScriptEngine* g_engine;
	asDWORD _flags = 0;

	template <class Base>
	static void RegisterBaseClass(void);
};


template <class T>
class RegisterAngelScriptAttribute final : public Gaff::IAttribute
{
public:
	RegisterAngelScriptAttribute(AngelScriptFlags flags = AS_NONE, asIScriptEngine* engine = nullptr);

	Gaff::IAttribute* clone(void) const override;

	void finish(Gaff::IReflectionDefinition*) override;
	void finish(Gaff::IEnumReflectionDefinition*) override;

private:
	asIScriptEngine* _engine;
	AngelScriptFlags _flags;

	void managerReflected(void);

	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(RegisterAngelScriptAttribute, T);
};

NS_END

SHIB_TEMPLATE_REFLECTION_DECLARE(RegisterAngelScriptAttribute, T)
SHIB_TEMPLATE_REFLECTION_DEFINE(RegisterAngelScriptAttribute, 0, T)

#include"Shibboleth_AngelScript.inl"
