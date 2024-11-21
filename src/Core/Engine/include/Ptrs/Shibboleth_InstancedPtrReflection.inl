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
VarInstancedPtr<T, VarType>::VarInstancedPtr(InstancedPtr<VarType> T::* ptr):
	Refl::IVar<T>(ptr)
{
}

template <class T, class VarType>
const Refl::IReflection& VarInstancedPtr<T, VarType>::getReflection(void) const
{
	GAFF_ASSERT(_reflection);
	return *_reflection;
}

template <class T, class VarType>
const void* VarInstancedPtr<T, VarType>::getData(const void* object) const
{
	return const_cast<VarInstancedPtr<T, VarType>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class VarType>
void* VarInstancedPtr<T, VarType>::getData(void* object)
{
	return Refl::IVar<T>::template get< InstancedPtr<VarType> >(object);
}

template <class T, class VarType>
void VarInstancedPtr<T, VarType>::setData(void* object, const void* data)
{
	if constexpr (Refl::VarTypeHelper<T, VarType>::k_can_copy) {
		if (Refl::IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		InstancedPtr<VarType>* const var = Refl::IVar<T>::template get< InstancedPtr<VarType> >(object);
		**var = *reinterpret_cast<const VarType*>(data);

	} else {
		GAFF_REF(object, data);

		GAFF_ASSERT_MSG(
			false,
			"VarInstancedPtr<T, VarType>::setData() was called with ReflectionType of '%s'.",
			reinterpret_cast<const char*>(Refl::Reflection<ReflectionType>::GetName())
		);
	}
}

template <class T, class VarType>
void VarInstancedPtr<T, VarType>::setDataMove(void* object, void* data)
{
	if constexpr (Refl::VarTypeHelper<T, VarType>::k_can_move) {
		if (Refl::IReflectionVar::isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		InstancedPtr<VarType>* const var = Refl::IVar<T>::template get< InstancedPtr<VarType> >(object);
		**var = std::move(*reinterpret_cast<VarType*>(data));

	} else {
		GAFF_REF(object, data);

		GAFF_ASSERT_MSG(
			false,
			"VarInstancedPtr<T, VarType>::setDataMove() was called with ReflectionType of '%s'.",
			reinterpret_cast<const char*>(Refl::Reflection<ReflectionType>::GetName())
		);
	}
}

template <class T, class VarType>
bool VarInstancedPtr<T, VarType>::load(const ISerializeReader& reader, void* object)
{
	GAFF_ASSERT(reader.isNull() || reader.isObject() || reader.isString());

	if (reader.isNull()) {
		return true;
	}

	const char8_t* class_name = nullptr;

	if (reader.isString()) {
		class_name = reader.readString();

	} else {
		const auto guard = reader.enterElementGuard(u8"class");
		GAFF_ASSERT(reader.isNull() || reader.isString());

		if (reader.isNull()) {
			// $TODO: Log error.
			return false;
		}

		class_name = reader.readString();
	}

	if (class_name && class_name[0]) {
		const Refl::IReflectionDefinition* const ref_def = GetApp().getReflectionManager().getReflection(Gaff::FNV1aHash64String(class_name));

		if (ref_def) {
			_reflection = &ref_def->getReflectionInstance();

			InstancedPtr<VarType>* const var = reinterpret_cast<InstancedPtr<VarType>*>(object);
			var->reset(ref_def->CREATET(VarType, var->getAllocator()));

			if (reader.isObject()) {
				const auto guard = reader.enterElementGuard(u8"data");
				GAFF_ASSERT(reader.isNull() || reader.isObject());

				if (!reader.isNull() && !ref_def->load(reader, ref_def->getBasePointer(var->get()))) {
					reader.freeString(class_name);
					return false;
				}
			}

		} else if (!checkIsOptional(class_name)) {
			// $TODO: Log error.
			reader.freeString(class_name);
			return false;
		}
	}

	reader.freeString(class_name);
	return true;
}

template <class T, class VarType>
void VarInstancedPtr<T, VarType>::save(ISerializeWriter& writer, const void* object)
{
	const InstancedPtr<VarType>* const var = reinterpret_cast<const InstancedPtr<VarType>*>(object);

	if (!*var) {
		writer.writeNull();
		return;
	}

	writer.startObject(2);

	const Refl::IReflectionDefinition* const ref_def = var->getReflectionDefinition();

	writer.writeString(u8"class", ref_def->getReflectionInstance().getName());
	// $TODO: If no data has been overridden from default, just write the class name instead.
	writer.writeKey(u8"data");

	ref_def->save(writer, ref_def->getBasePointer(var->get()));

	writer.endObject();
}

template <class T, class VarType>
bool VarInstancedPtr<T, VarType>::load(const ISerializeReader& reader, T& object)
{
	InstancedPtr<VarType>* const var = Refl::IVar<T>::template get< InstancedPtr<VarType> >(&object);
	return load(reader, var);
}

template <class T, class VarType>
void VarInstancedPtr<T, VarType>::save(ISerializeWriter& writer, const T& object)
{
	const InstancedPtr<VarType>* const var = Refl::IVar<T>::template get< InstancedPtr<VarType> >(&object);
	save(writer, var);
}

template <class T, class VarType>
bool VarInstancedPtr<T, VarType>::checkIsOptional(const char8_t* class_name) const
{
	const auto& ref_def = Refl::Reflection<T>::GetReflectionDefinition();

	const InstancedOptionalAttribute* optional = ref_def.template getVarAttr<InstancedOptionalAttribute>(Refl::IReflectionVar::getName());

	if (!optional) {
		const Refl::IVar<T>* parent = Refl::IVar<T>::getParent();

		while (parent && parent->isContainer()) {
			parent = parent->getParent();
		}

		if (parent) {
			optional = ref_def.template getVarAttr<InstancedOptionalAttribute>(parent->getName());
		}
	}

	return !optional || optional->matches(class_name);
}

NS_END
