/************************************************************************************
Copyright (C) by Nicholas LaCroix

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
VarDeferredResourcePtr<T, VarType>::VarDeferredResourcePtr(DeferredResourcePtr<VarType> T::* ptr):
	Refl::IVar<T>(ptr)
{
}

template <class T, class VarType>
const Refl::Reflection<typename VarDeferredResourcePtr<T, VarType>::ReflectionType>& VarDeferredResourcePtr<T, VarType>::GetReflection(void)
{
	return Refl::Reflection<ReflectionType>::GetInstance();
}

template <class T, class VarType>
const Refl::IReflection& VarDeferredResourcePtr<T, VarType>::getReflection(void) const
{
	return GetReflection();
}

template <class T, class VarType>
const void* VarDeferredResourcePtr<T, VarType>::getData(const void* object) const
{
	return const_cast<VarDeferredResourcePtr<T, VarType>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class VarType>
void* VarDeferredResourcePtr<T, VarType>::getData(void* object)
{
	return Refl::IVar<T>::template get< DeferredResourcePtr<VarType> >(object);
}

template <class T, class VarType>
void VarDeferredResourcePtr<T, VarType>::setData(void* object, const void* data)
{
	if (Refl::IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	DeferredResourcePtr<VarType>* const var = Refl::IVar<T>::template get< DeferredResourcePtr<VarType> >(object);
	*var = const_cast<VarType*>(reinterpret_cast<const VarType*>(data));
}

template <class T, class VarType>
void VarDeferredResourcePtr<T, VarType>::setDataMove(void* object, void* data)
{
	setData(object, data);
}

template <class T, class VarType>
bool VarDeferredResourcePtr<T, VarType>::load(const ISerializeReader& reader, void* object)
{
	GAFF_ASSERT(reader.isNull() || reader.isObject() || reader.isString());

	if (reader.isNull()) {
		return true;
	}

	const char8_t* resource_path = nullptr;

	if (reader.isString()) {
		resource_path = reader.readString();
	} else {
		const auto guard = reader.enterElementGuard(u8"path");
		resource_path = reader.readString();
	}

	if (!resource_path || !resource_path[0]) {
		// $TODO: Log error.
		return false;
	}

	const char8_t* type_name = nullptr;

	if (reader.isObject()) {
		const auto guard = reader.enterElementGuard(u8"type");
		type_name = reader.readString();

		if (!type_name || !type_name[0]) {
			// $TODO: Log error.
			return false;
		}
	}

	const Refl::IReflectionDefinition* const ref_def = type_name ?
		GetApp().getReflectionManager().getReflection(Gaff::FNV1aHash64String(type_name)) :
		GetApp().getReflectionManager().getReflection(Refl::Reflection<ReflectionType>::GetNameHash());

	DeferredResourcePtr<VarType>* const var = reinterpret_cast<DeferredResourcePtr<VarType>*>(object);

	if (ref_def) {
		*var = DeferredResourcePtr<VarType>(HashString64<>(resource_path), ref_def);
	} else {
		// $TODO: Log warning.
		*var = DeferredResourcePtr<VarType>(HashString64<>(resource_path));
	}

	reader.freeString(resource_path);

	if (type_name) {
		reader.freeString(type_name);
	}

	return *var;
}

template <class T, class VarType>
void VarDeferredResourcePtr<T, VarType>::save(ISerializeWriter& writer, const void* object)
{
	const DeferredResourcePtr<VarType>* const var = reinterpret_cast<const DeferredResourcePtr<VarType>*>(object);

	if (var->getFilePath().empty()) {
		writer.writeNull();

	} else {
		writer.startObject(2);

		writer.writeString(u8"type", var->getReflectionDefinition()->getReflectionInstance().getName());
		writer.writeString(u8"path", var->getFilePath().getBuffer());

		writer.endObject();

	}
}

template <class T, class VarType>
bool VarDeferredResourcePtr<T, VarType>::load(const ISerializeReader& reader, T& object)
{
	DeferredResourcePtr<VarType>* const var = Refl::IVar<T>::template get< DeferredResourcePtr<VarType> >(&object);
	const bool success = load(reader, var);

	if constexpr (std::derived_from<T, IResourceTracker>) {
		if (*var) {
			(*var)->addIncomingReference(object);
		}
	}

	return success;
}

template <class T, class VarType>
void VarDeferredResourcePtr<T, VarType>::save(ISerializeWriter& writer, const T& object)
{
	const DeferredResourcePtr<VarType>* const var = Refl::IVar<T>::template get< DeferredResourcePtr<VarType> >(&object);
	save(writer, var);
}

NS_END
