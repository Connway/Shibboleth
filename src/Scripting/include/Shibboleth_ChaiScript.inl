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

NS_SHIBBOLETH

template <class T>
ChaiScriptClassRegister<T>::ChaiScriptClassRegister(chaiscript::Module& module):
	_module(module)
{
}

template <class T>
ChaiScriptClassRegister<T>& ChaiScriptClassRegister<T>::base(const char*, Gaff::ReflectionHash interface_name, ptrdiff_t offset)
{
	return *this;
}

template <class T>
template <class Base>
ChaiScriptClassRegister<T>& ChaiScriptClassRegister<T>::base(void)
{
	return *this;
}

template <class T>
template <class Constructor>
ChaiScriptClassRegister<T>& ChaiScriptClassRegister<T>::ctor(void)
{
	ChaiScriptModuleAdd(_module, Reflection<T>::GetName(), chaiscript::constructor<Constructor>());
	return *this;
}

template <class T>
template <class Var, size_t size>
ChaiScriptClassRegister<T>& ChaiScriptClassRegister<T>::var(const char (&name)[size], Var T::*ptr)
{
	ChaiScriptModuleAdd(_module, name, chaiscript::fun(ptr));
	return *this;
}

template <class T>
template <class Ret, class Var, size_t size>
ChaiScriptClassRegister<T>& ChaiScriptClassRegister<T>::var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var))
{
	ChaiScriptModuleAdd(_module, name, chaiscript::fun(getter));

	if (setter) {
		ChaiScriptModuleAdd(_module, name, chaiscript::fun(setter));
	}

	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args>
ChaiScriptClassRegister<T>& ChaiScriptClassRegister<T>::func(const char (&name)[size], Ret (T::*ptr)(Args...) const)
{
	ChaiScriptModuleAdd(_module, name, chaiscript::fun(ptr));
	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args>
ChaiScriptClassRegister<T>& ChaiScriptClassRegister<T>::func(const char (&name)[size], Ret (T::*ptr)(Args...))
{
	ChaiScriptModuleAdd(_module, name, chaiscript::fun(ptr));
	return *this;
}

template <class T>
void ChaiScriptClassRegister<T>::finish(void)
{
}

NS_END
