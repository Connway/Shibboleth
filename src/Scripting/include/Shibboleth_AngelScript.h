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

#include <Shibboleth_Reflection.h>
#include <angelscript.h>

NS_SHIBBOLETH

template <class T>
class AngelScriptClassRegister
{
public:
	AngelScriptClassRegister(asIScriptEngine* engine);

	template <class Base>
	AngelScriptClassRegister& base(const char* name, Gaff::ReflectionHash hash);

	template <class Base>
	AngelScriptClassRegister& base(void);

	template <class... Args>
	AngelScriptClassRegister& ctor(void);

	template <class Var, size_t size>
	AngelScriptClassRegister& var(const char(&name)[size], Var T::*ptr);

	template <class Ret, class Var, size_t size>
	AngelScriptClassRegister& var(const char(&name)[size], Ret(T::*getter)(void) const, void (T::*setter)(Var));

	template <size_t size, class Ret, class... Args>
	AngelScriptClassRegister& func(const char(&name)[size], Ret(T::*ptr)(Args...) const);

	template <size_t size, class Ret, class... Args>
	AngelScriptClassRegister& func(const char(&name)[size], Ret(T::*ptr)(Args...));

	void finish(void);

private:
	asIScriptEngine* _engine = nullptr;
	asDWORD _flags = asGetTypeTraits<T>();
};

template <class T, class B>
class AngelScriptClassBaseRegister
{
public:
	AngelScriptClassBaseRegister(asIScriptEngine* engine);

	template <class Base>
	AngelScriptClassBaseRegister& base(const char* name, Gaff::ReflectionHash hash);

	template <class Base>
	AngelScriptClassBaseRegister& base(void);

	template <class... Args>
	AngelScriptClassBaseRegister& ctor(void);

	template <class Var, size_t size>
	AngelScriptClassBaseRegister& var(const char(&name)[size], Var T::*ptr);

	template <class Ret, class Var, size_t size>
	AngelScriptClassBaseRegister& var(const char(&name)[size], Ret(T::*getter)(void) const, void (T::*setter)(Var));

	template <size_t size, class Ret, class... Args>
	AngelScriptClassBaseRegister& func(const char(&name)[size], Ret(T::*ptr)(Args...) const);

	template <size_t size, class Ret, class... Args>
	AngelScriptClassBaseRegister& func(const char(&name)[size], Ret(T::*ptr)(Args...));

	void finish(void);

private:
	asIScriptEngine* _engine = nullptr;
	asDWORD _flags = asGetTypeTraits<T>();
};

NS_END

#include"Shibboleth_AngelScript.inl"
