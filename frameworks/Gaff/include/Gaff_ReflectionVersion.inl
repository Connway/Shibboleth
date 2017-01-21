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

NS_GAFF

template <class T>
template <class Base>
ReflectionVersion<T>& ReflectionVersion<T>::base(const char* name)
{
	const ptrdiff_t offset = Gaff::OffsetOfClass<T, Base>();
	_hash = FNV1aHash64String(name, _hash);
	_hash = FNV1aHash64T(&offset, _hash);
	return *this;
}

template <class T>
template <class Base>
ReflectionVersion<T>& ReflectionVersion<T>::base(void)
{
	return base<Base>(GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetName());
}

template <class T>
template <class... Args>
ReflectionVersion<T>& ReflectionVersion<T>::ctor(void)
{
	_hash = CalcTemplateHash<Args...>(_hash);
	return *this;
}

template <class T>
template <class Var, size_t size>
ReflectionVersion<T>& ReflectionVersion<T>::var(const char(&name)[size], Var T::*ptr)
{
	_hash = FNV1aHash64(name, size ,_hash);
	_hash = FNV1aHash64T(&ptr, _hash);
	return *this;
}

template <class T>
template <class Ret, class Var, size_t size>
ReflectionVersion<T>& ReflectionVersion<T>::var(const char(&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var))
{
	_hash = FNV1aHash64(name, size, _hash);
	_hash = FNV1aHash64T(&getter, _hash);
	_hash = FNV1aHash64T(&setter, _hash);
	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args>
ReflectionVersion<T>& ReflectionVersion<T>::func(const char(&name)[size], Ret (T::*ptr)(Args...) const)
{
	_hash = FNV1aHash64(name, size, _hash);
	_hash = FNV1aHash64T(&ptr, _hash);
	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args>
ReflectionVersion<T>& ReflectionVersion<T>::func(const char(&name)[size], Ret (T::*ptr)(Args...))
{
	_hash = FNV1aHash64(name, size, _hash);
	_hash = FNV1aHash64T(&ptr, _hash);
	return *this;
}

template <class T>
Hash64 ReflectionVersion<T>::getHash(void) const
{
	return _hash;
}

NS_END
