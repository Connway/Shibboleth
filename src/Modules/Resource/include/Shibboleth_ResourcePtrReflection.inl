/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

NS_SHIBBOLETH

template <class T, class VarType>
VarResourcePtr<T, VarType>::VarResourcePtr(ResourcePtr<VarType> T::* ptr):
	Refl::IVar<T>(ptr)
{
}

template <class T, class VarType>
const Refl::Reflection<typename VarResourcePtr<T, VarType>::ReflectionType>& VarResourcePtr<T, VarType>::GetReflection(void)
{
	return Refl::Reflection<ReflectionType>::GetInstance();
}

template <class T, class VarType>
const Refl::IReflection& VarResourcePtr<T, VarType>::getReflection(void) const
{
	return GetReflection();
}

template <class T, class VarType>
const void* VarResourcePtr<T, VarType>::getData(const void* object) const
{
	return const_cast<VarResourcePtr<T, VarType>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class VarType>
void* VarResourcePtr<T, VarType>::getData(void* object)
{
	return Refl::IVar<T>::template get< ResourcePtr<VarType> >(object);
}

template <class T, class VarType>
void VarResourcePtr<T, VarType>::setData(void* object, const void* data)
{
	if (Refl::IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	ResourcePtr<VarType>* const var = Refl::IVar<T>::template get< ResourcePtr<VarType> >(object);
	*var = const_cast<VarType*>(reinterpret_cast<const VarType*>(data));
}

template <class T, class VarType>
void VarResourcePtr<T, VarType>::setDataMove(void* object, void* data)
{
	setData(object, data);
}

template <class T, class VarType>
bool VarResourcePtr<T, VarType>::load(const ISerializeReader& reader, void* object)
{
	GAFF_ASSERT(reader.isNull() || reader.isString());

	if (reader.isNull()) {
		return true;
	}

	const char8_t* const resource_path = reader.readString();
	bool result = true;

	if (resource_path && resource_path[0]) {
		ResourcePtr<VarType>* const var = reinterpret_cast<ResourcePtr<VarType>*>(object);
		*var = RequestResourceT<VarType>(HashStringView64<>(resource_path));

		if (!*var) {
			// $TODO: Log error.
			result = false;
		}
	}

	reader.freeString(resource_path);
	return result;
}

template <class T, class VarType>
void VarResourcePtr<T, VarType>::save(ISerializeWriter& writer, const void* object)
{
	const ResourcePtr<VarType>* const var = reinterpret_cast<const ResourcePtr<VarType>*>(object);

	if (*var) {
		writer.writeString((*var)->getFilePath().getBuffer());
	} else {
		writer.writeNull();
	}
}

template <class T, class VarType>
bool VarResourcePtr<T, VarType>::load(const ISerializeReader& reader, T& object)
{
	ResourcePtr<VarType>* const var = Refl::IVar<T>::template get< ResourcePtr<VarType> >(&object);
	const bool result = load(reader, var);

	if constexpr (std::derived_from<T, IResource>) {
		if (*var) {
			(*var)->addIncomingReference(object);
		}
	}

	return result;
}

template <class T, class VarType>
void VarResourcePtr<T, VarType>::save(ISerializeWriter& writer, const T& object)
{
	const ResourcePtr<VarType>* const var = Refl::IVar<T>::template get< ResourcePtr<VarType> >(&object);
	save(writer, var);
}

NS_END
