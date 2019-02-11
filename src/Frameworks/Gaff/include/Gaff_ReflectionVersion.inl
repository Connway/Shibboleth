/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
	_hash = FNV1aHash64T(offset, _hash);
	return *this;
}

template <class T>
template <class Base>
ReflectionVersion<T>& ReflectionVersion<T>::base(void)
{
	const Gaff::Hash64 version = GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetVersion();
	const ptrdiff_t offset = Gaff::OffsetOfClass<T, Base>();

	_hash = FNV1aHash64T(GAFF_REFLECTION_NAMESPACE::Reflection<Base>::GetHash(), _hash);
	_hash = FNV1aHash64T(offset, _hash);
	_hash = FNV1aHash64T(version, _hash);

	return *this;
}

template <class T>
template <class... Args>
ReflectionVersion<T>& ReflectionVersion<T>::ctor(Hash64 factory_hash)
{
	_hash = FNV1aHash64T(factory_hash, _hash);
	return *this;
}

template <class T>
template <class... Args>
ReflectionVersion<T>& ReflectionVersion<T>::ctor(void)
{
	_hash = CalcTemplateHash<Args...>(_hash);
	return *this;
}

template <class T>
template <class Var, size_t size, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::var(const char (&name)[size], Var T::*ptr, const Attrs&... attributes)
{
	_hash = FNV1aHash64(name, size ,_hash);
	_hash = FNV1aHash64T(ptr, _hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = FNV1aHash64(name, size - 1, _hash);
		_hash = CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <class Ret, class Var, size_t size, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::var(const char (&name)[size], Ret (T::* /*getter*/)(void) const, void (T::* /*setter*/)(Var), const Attrs&... attributes)
{
	_hash = FNV1aHash64(name, size - 1, _hash);
	_hash = CalcTemplateHash<Ret, Var>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = FNV1aHash64(name, size - 1, _hash);
		_hash = CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::func(const char (&name)[size], Ret (T::* /*ptr*/)(Args...) const, const Attrs&... attributes)
{
	_hash = FNV1aHash64(name, size - 1, _hash);
	_hash = CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = FNV1aHash64(name, size - 1, _hash);
		_hash = CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::func(const char (&name)[size], Ret (T::* /*ptr*/)(Args...), const Attrs&... attributes)
{
	_hash = FNV1aHash64(name, size - 1, _hash);
	_hash = CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = FNV1aHash64(name, size - 1, _hash);
		_hash = CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <size_t size, class Ret, class... Args, class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::staticFunc(const char (&name)[size], Ret (* /*func*/)(Args...), const Attrs&... attributes)
{
	_hash = FNV1aHash64(name, size - 1, _hash);
	_hash = CalcTemplateHash<Ret, Args...>(_hash);

	if constexpr (sizeof...(Attrs) > 0) {
		_hash = FNV1aHash64(name, size - 1, _hash);
		_hash = CalcTemplateHash<Attrs...>(_hash);
		_hash = getAttributeHashes(_hash, attributes...);
	}

	return *this;
}

template <class T>
template <class... Attrs>
ReflectionVersion<T>& ReflectionVersion<T>::classAttrs(const Attrs&... attributes)
{
	_hash = FNV1aHash64String("class", _hash);
	_hash = CalcTemplateHash<Attrs...>(_hash);
	_hash = getAttributeHashes(_hash, attributes...);
	return *this;
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::version(uint32_t version)
{
	GAFF_ASSERT(_hash == INIT_HASH64);
	_hash = FNV1aHash64T(version, _hash);
	return *this;
}

template <class T>
ReflectionVersion<T>& ReflectionVersion<T>::serialize(LoadFunc serialize_load, SaveFunc serialize_save)
{
	// Unsure if this is a reliable hashing mechanism.
	_hash = FNV1aHash64T(serialize_load, _hash);
	_hash = FNV1aHash64T(serialize_save, _hash);
	return *this;
}

template <class T>
template <class T2>
ReflectionVersion<T>& ReflectionVersion<T>::dependsOn(void)
{
	_hash = FNV1aHash64T(GAFF_REFLECTION_NAMESPACE::Reflection<T2>::GetHash(), _hash);
	return *this;
}

template <class T>
Hash64 ReflectionVersion<T>::getHash(void) const
{
	return _hash;
}

template <class T>
void ReflectionVersion<T>::finish(void)
{
}

template <class T>
template <class First, class... Rest>
Hash64 ReflectionVersion<T>::getAttributeHashes(Hash64 hash, const First& first, const Rest&... rest) const
{
	hash = first.applyVersioning(hash);

	if constexpr (sizeof...(Rest) > 0) {
		return getAttributeHashes(hash, rest...);
	} else {
		return hash;
	}
}

NS_END
