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

template <class T, class VarType>
VarInstancedPtr<T, VarType>::VarInstancedPtr(Shibboleth::InstancedPtr<VarType> T::* ptr):
	IVar<T>(ptr)
{
}

template <class T, class VarType>
const Reflection<typename VarInstancedPtr<T, VarType>::ReflectionType>& VarInstancedPtr<T, VarType>::GetReflection(void)
{
	return Reflection<ReflectionType>::GetInstance();
}

template <class T, class VarType>
const IReflection& VarInstancedPtr<T, VarType>::getReflection(void) const
{
	return GetReflection();
}

template <class T, class VarType>
const void* VarInstancedPtr<T, VarType>::getData(const void* object) const
{
	return const_cast<VarInstancedPtr<T, VarType>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class VarType>
void* VarInstancedPtr<T, VarType>::getData(void* object)
{
	return IVar<T>::template get< Shibboleth::InstancedPtr<VarType> >(object);
}

template <class T, class VarType>
void VarInstancedPtr<T, VarType>::setData(void* object, const void* data)
{
	if constexpr (VarTypeHelper<T, VarType>::k_can_copy) {
		if (IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		Shibboleth::InstancedPtr<VarType>* var = IVar<T>::template get< Shibboleth::InstancedPtr<VarType> >(object);
		*var = *reinterpret_cast<const VarType*>(data);

	} else {
		GAFF_REF(object, data);

		GAFF_ASSERT_MSG(
			false,
			"VarInstancedPtr<T, VarType>::setData() was called with ReflectionType of '%s'.",
			reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName())
		);
	}
}

template <class T, class VarType>
void VarInstancedPtr<T, VarType>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	Shibboleth::InstancedPtr<VarType>* var = IVar<T>::template get< Shibboleth::InstancedPtr<VarType> >(object);
	*var = std::move(*reinterpret_cast<VarType*>(data));
}

template <class T, class VarType>
bool VarInstancedPtr<T, VarType>::load(const Shibboleth::ISerializeReader& reader, void* object)
{
	const char8_t* class_name = nullptr;

	{
		const auto guard = reader.enterElementGuard(u8"class");
		GAFF_ASSERT(reader.isNull() || reader.isString());

		if (reader.isNull()) {
			return true;
		}

		class_name = reader.readString();
	}

	if (class_name && class_name[0]) {
		const Refl::IReflectionDefinition* const ref_def = GetApp().getReflectionManager().getReflection(Gaff::FNV1aHash64String(class_name));

		if (ref_def) {
			Shibboleth::InstancedPtr<VarType>* const var = reinterpret_cast<Shibboleth::InstancedPtr<VarType>*>(object);
			var->reset(ref_def->CREATET(VarType, out._allocator));

			const auto guard = reader.enterElementGuard(u8"data");
			GAFF_ASSERT(reader.isNull() || reader.isObject());

			if (!reader.isNull()) {
				ref_def->load(reader, ref_def->getBasePointer(var->get()));
			}

		} else {
			// $TODO: Log error.
			reader.freeString(class_name);
			return false;
		}
	}

	reader.freeString(class_name);
	return true;
}

template <class T, class VarType>
void VarInstancedPtr<T, VarType>::save(Shibboleth::ISerializeWriter& writer, const void* object)
{
	const Shibboleth::InstancedPtr<VarType>* const var = reinterpret_cast<const Shibboleth::InstancedPtr<VarType>*>(object);

	if (*var) {
		const Refl::IReflectionDefinition* const ref_def = var->getReflectionDefinition();

		writer.writeString(u8"class", ref_def->getReflectionInstance().getName());
		writer.writeKey(u8"data");

		ref_def->save(writer, ref_def->getBasePointer(var->get()));

	} else {
		writer.writeNull(u8"class");
		writer.writeNull(u8"data");
	}
}

template <class T, class VarType>
bool VarInstancedPtr<T, VarType>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	Shibboleth::InstancedPtr<VarType>* const var = IVar<T>::template get< Shibboleth::InstancedPtr<VarType> >(&object);
	return load(reader, var);
}

template <class T, class VarType>
void VarInstancedPtr<T, VarType>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const Shibboleth::InstancedPtr<VarType>* const var = IVar<T>::template get< Shibboleth::InstancedPtr<VarType> >(&object);
	save(writer, var);
}

NS_END
