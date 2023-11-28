/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

NS_REFLECTION

// Var Hash
template <class T, class HashStorage>
VarHash<T, HashStorage>::VarHash(Gaff::Hash<HashStorage> T::* ptr) :
	IVar<T>(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class HashStorage>
const IReflection& VarHash<T, HashStorage>::getReflection(void) const
{
	// Should this be a U8String, or a HashString?
	return Reflection<Shibboleth::U8String>::GetInstance();
}

template <class T, class HashStorage>
const void* VarHash<T, HashStorage>::getData(const void* object) const
{
	GAFF_REF(object);
	return &_string;
}

template <class T, class HashStorage>
void* VarHash<T, HashStorage>::getData(void* object)
{
	GAFF_REF(object);
	return &_string;
}

template <class T, class HashStorage>
void VarHash<T, HashStorage>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	_string = *reinterpret_cast<const Shibboleth::U8String*>(data);

	Gaff::Hash<HashStorage>& hash = *IVar<T>::template get< Gaff::Hash<HashStorage> >(object);

	if constexpr (std::is_same_v<Gaff::Hash<HashStorage>, Gaff::Hash32>) {
		hash = Gaff::FNV1aHash32String(_string.data());
	} else if constexpr (std::is_same_v<Gaff::Hash<HashStorage>, Gaff::Hash64>) {
		hash = Gaff::FNV1aHash64String(_string.data());
	}
}

template <class T, class HashStorage>
void VarHash<T, HashStorage>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	_string = std::move(*reinterpret_cast<Shibboleth::U8String*>(data));

	Gaff::Hash<HashStorage>& hash = *IVar<T>::template get< Gaff::Hash<HashStorage> >(object);

	if constexpr (std::is_same_v<Gaff::Hash<HashStorage>, Gaff::Hash32>) {
		hash = Gaff::FNV1aHash32String(_string.data());
	} else if constexpr (std::is_same_v<Gaff::Hash<HashStorage>, Gaff::Hash64>) {
		hash = Gaff::FNV1aHash64String(_string.data());
	}
}

template <class T, class HashStorage>
bool VarHash<T, HashStorage>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	const Shibboleth::ScopeGuard guard = reader.enterElementGuard("string");

	const bool ret = Reflection<Shibboleth::U8String>::GetInstance().load(reader, _string);

	if (ret) {
		Gaff::Hash<HashStorage>& hash = *IVar<T>::template get< Gaff::Hash<HashStorage> >(object);

		if constexpr (std::is_same_v<Gaff::Hash<HashStorage>, Gaff::Hash32>) {
			hash = Gaff::FNV1aHash32String(_string.data());
		}  else if constexpr (std::is_same_v<Gaff::Hash<HashStorage>, Gaff::Hash64>) {
			hash = Gaff::FNV1aHash64String(_string.data());
		}
	}

	return ret;
}

template <class T, class HashStorage>
void VarHash<T, HashStorage>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const Gaff::Hash<HashStorage>& hash = *IVar<T>::template get< Gaff::Hash<HashStorage> >(object);

	writer.startObject(3);

	writer.writeUInt64(u8"version", Reflection<Gaff::Hash32>::GetInstance().getVersion().getHash());

	writer.writeKey(u8"string");
	Reflection<Shibboleth::U8String>::GetInstance().save(writer, _string);

	writer.writeKey(u8"hash");
	Reflection<HashStorage>::GetInstance().save(writer, hash.getHash());

	writer.endObject();
}

NS_END
