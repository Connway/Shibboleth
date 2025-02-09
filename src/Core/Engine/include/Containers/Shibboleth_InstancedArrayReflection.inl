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

#include "Containers/Shibboleth_InstancedArrayReflection.h"
#include "Gaff_Assert.h"

NS_SHIBBOLETH

template <class T>
const void* VarInstancedArrayAny<T>::VarElementWrapper::getData(const void* object) const
{
	return const_cast<VarElementWrapper*>(this)->getData(const_cast<void*>(object));
}

template <class T>
void* VarInstancedArrayAny<T>::VarElementWrapper::getData(void* object)
{
	return Refl::IVar<T>::adjust(object);
}

template <class T>
void VarInstancedArrayAny<T>::VarElementWrapper::setData(void* object, const void* data)
{
	GAFF_REF(object, data);

	GAFF_ASSERT_MSG(
		false,
		"VarElementWrapperVar<T, VarType>::setData() was called with ReflectionType of '%s'.",
		reinterpret_cast<const char*>(getReflection().getName())
	);
}

template <class T>
void VarInstancedArrayAny<T>::VarElementWrapper::setDataMove(void* object, void* data)
{
	GAFF_REF(object, data);

	GAFF_ASSERT_MSG(
		false,
		"VarElementWrapperVar<T, VarType>::setDataMove() was called with ReflectionType of '%s'.",
		reinterpret_cast<const char*>(getReflection().getName())
	);

}

template <class T>
bool VarInstancedArrayAny<T>::VarElementWrapper::load(const Shibboleth::ISerializeReader& reader, void* object)
{
	return getReflection().getReflectionDefinition().load(reader, object);
}

template <class T>
void VarInstancedArrayAny<T>::VarElementWrapper::save(Shibboleth::ISerializeWriter& writer, const void* object)
{
	getReflection().getReflectionDefinition().save(writer, object);
}

template <class T>
bool VarInstancedArrayAny<T>::VarElementWrapper::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	void* const data = getData(&object);
	return load(reader, data);
}

template <class T>
void VarInstancedArrayAny<T>::VarElementWrapper::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const void* const data = getData(&object);
	save(writer, data);
}



template <class T>
template <class VarType>
VarInstancedArrayAny<T>::VarInstancedArrayAny(InstancedArray<VarType> T::* ptr):
	Refl::IVar<T>(ptr)
{
}

template <class T>
VarInstancedArrayAny<T>::VarInstancedArrayAny(InstancedArrayAny T::* ptr):
	Refl::IVar<T>(ptr)
{
}

template <class T>
const Refl::IReflection& VarInstancedArrayAny<T>::getReflection(const void* object, int32_t index) const
{
	const InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(object);
	return var->_metadata[index].ref_def->getReflectionInstance();
}

/*template <class T>
const Refl::IReflection& VarInstancedArrayAny<T>::getReflection(const void* object) const
{
	const InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(object);
	GAFF_ASSERT_MSG(var->_base_ref_def, "VarInstancedArrayAny<T>::getReflection: Has no base reflection definition.");

	return var->_base_ref_def->getReflectionInstance();
}*/

template <class T>
const Refl::IReflection& VarInstancedArrayAny<T>::getReflection(void) const
{
	GAFF_ASSERT_MSG(false, "InstancedArrayAny has no base reflection type.");

	const Refl::IReflection* const refl = nullptr;
	return *refl;
}

/*template <class T>
bool VarInstancedArrayAny<T>::hasReflection(const void* object) const
{
	const InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(object);
	return var->_base_ref_def != nullptr;
}*/

template <class T>
bool VarInstancedArrayAny<T>::hasReflection(void) const
{
	return false;
}

template <class T>
const void* VarInstancedArrayAny<T>::getData(const void* object) const
{
	return const_cast<VarInstancedArrayAny<T>*>(this)->getData(const_cast<void*>(object));
}

template <class T>
void* VarInstancedArrayAny<T>::getData(void* object)
{
	return Refl::IVar<T>::template get<InstancedArrayAny>(object);
}

template <class T>
void VarInstancedArrayAny<T>::setData(void* /*object*/, const void* /*data*/)
{
	GAFF_ASSERT_MSG(false, "VarInstancedArrayAny<T>::setData() was called.");
}

template <class T>
void VarInstancedArrayAny<T>::setDataMove(void* object, void* data)
{
	if (Refl::IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(object);
	*var = std::move(*reinterpret_cast<InstancedArrayAny*>(data));
}

template <class T>
bool VarInstancedArrayAny<T>::isVector(void) const
{
	return true;
}

template <class T>
bool VarInstancedArrayAny<T>::isFixedArray(void) const
{
	return false;
}

template <class T>
int32_t VarInstancedArrayAny<T>::size(const void* object) const
{
	const InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(object);
	return var->size();
}

template <class T>
const void* VarInstancedArrayAny<T>::getElement(const void* object, int32_t index) const
{
	return const_cast<VarInstancedArrayAny<T>*>(this)->getElement(const_cast<void*>(object), index);
}

template <class T>
void* VarInstancedArrayAny<T>::getElement(void* object, int32_t index)
{
	InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(object);
	return var->at(index);
}

template <class T>
void VarInstancedArrayAny<T>::setElement(void* /*object*/, int32_t /*index*/, const void* /*data*/)
{
	GAFF_ASSERT_MSG(false, "VarInstancedArrayAny<T>::setElement() was called.");
}

template <class T>
void VarInstancedArrayAny<T>::setElementMove(void* /*object*/, int32_t /*index*/, void* /*data*/)
{
	GAFF_ASSERT_MSG(false, "VarInstancedArrayAny<T>::setElementMove() was called.");
}

template <class T>
void VarInstancedArrayAny<T>::swap(void* object, int32_t index_a, int32_t index_b)
{
	InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(object);
	var->swap(index_a, index_b);
}

template <class T>
void VarInstancedArrayAny<T>::resize(void* object, size_t new_size)
{
	InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(object);
	var->resize(static_cast<int32_t>(new_size));

	const size_t old_size = _elements.size();

	_cached_element_vars.resize(new_size);
	_elements.resize(new_size);

	if (new_size > old_size) {
		regenerateSubVars(object, static_cast<int32_t>(old_size + 1), static_cast<int32_t>(new_size));
	}
}

template <class T>
void VarInstancedArrayAny<T>::remove(void* object, int32_t index)
{
	InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(object);
	var->erase(index);

	_cached_element_vars.erase(_cached_element_vars.begin() + index);
	_elements.erase(_elements.begin() + index);

	regenerateSubVars(object, index, static_cast<int32_t>(_elements.size()));
}

template <class T>
bool VarInstancedArrayAny<T>::load(const ISerializeReader& reader, void* object)
{
	GAFF_ASSERT(reader.isNull() || reader.isArray());

	if (reader.isNull()) {
		return true;
	}

	const InstancedOptionalAttribute* const optional_attr = getOptionalAttribute();
	InstancedArrayAny* const var = reinterpret_cast<InstancedArrayAny*>(object);
	const ReflectionManager& ref_mgr = GetApp().getReflectionManager();
	const int32_t array_size = reader.size();
	bool success = true;

	for (int32_t i = 0; i < array_size; ++i) {
		const auto element_guard = reader.enterElementGuard(i);
		const char8_t* class_name = nullptr;

		GAFF_ASSERT(reader.isNull() || reader.isObject() || reader.isString());

		if (reader.isNull()) {
			if (!optional_attr || optional_attr->shouldLeaveEmptyElement()) {
				var->pushEmpty();
			}

			continue;
		}

		if (reader.isString()) {
			class_name = reader.readString();

		} else {
			const auto guard = reader.enterElementGuard(u8"class");
			GAFF_ASSERT(reader.isNull() || reader.isString());

			if (reader.isString()) {
				class_name = reader.readString();
			}
		}

		if (class_name && class_name[0]) {
			const eastl::u8string_view stripped_class_name = (optional_attr) ? optional_attr->stripPrefixAndSuffix(class_name) : class_name;
			const Refl::IReflectionDefinition* const ref_def = ref_mgr.getReflection(Gaff::FNV1aHash64String(stripped_class_name));

			if (ref_def) {
				if (!var->_base_ref_def || ref_def->hasInterface(*var->_base_ref_def)) {
					void* const instance = var->push(*ref_def);

					if (reader.isObject()) {
						const auto guard = reader.enterElementGuard(u8"data");
						GAFF_ASSERT(reader.isNull() || reader.isObject());

						if (!reader.isNull()) {
							success = ref_def->load(reader, ref_def->getBasePointer(instance)) && success;
						}
					}

				} else {
					if (!optional_attr || optional_attr->shouldLeaveEmptyElement()) {
						var->pushEmpty();
					}

					// $TODO: Log error.
					success = false;
				}

			} else {
				// This entry is not optional.
				if (!optional_attr || stripped_class_name.size() != eastl::CharStrlen(class_name)) {
					// $TODO: Log error.
					success = false;
				}

				if (!optional_attr || optional_attr->shouldLeaveEmptyElement()) {
					var->pushEmpty();
				}
			}
		}

		reader.freeString(class_name);
	}

	_cached_element_vars.resize(static_cast<size_t>(var->size()));
	_elements.resize(static_cast<size_t>(var->size()));
	regenerateSubVars(object, 0, var->size());

	return success;
}

template <class T>
void VarInstancedArrayAny<T>::save(ISerializeWriter& writer, const void* object)
{
	const InstancedArrayAny* const var = reinterpret_cast<const InstancedArrayAny*>(object);

	if (var->empty()) {
		writer.writeNull();
		return;
	}

	const int32_t array_size = var->size();

	writer.startArray(static_cast<uint32_t>(array_size));

	for (int32_t i = 0; i < array_size; ++i) {
		const auto& metadata = var->_metadata[i];

		if (metadata.ref_def) {
			const void* const element = var->_instances.data() + metadata.start;

			writer.startObject(2);

			writer.writeString(u8"class", metadata.ref_def->getReflectionInstance().getName());
			// $TODO: If no data has been overridden from default, just write the class name instead.
			writer.writeKey(u8"data");

			metadata.ref_def->save(writer, element);

			writer.endObject();

		} else {
			writer.writeNull();
		}
	}

	writer.endArray();
}

template <class T>
bool VarInstancedArrayAny<T>::load(const ISerializeReader& reader, T& object)
{
	InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(&object);
	return load(reader, var);
}

template <class T>
void VarInstancedArrayAny<T>::save(ISerializeWriter& writer, const T& object)
{
	const InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(&object);
	save(writer, var);
}

template <class T>
const InstancedOptionalAttribute* VarInstancedArrayAny<T>::getOptionalAttribute(void) const
{
	const auto& ref_def = Refl::Reflection<T>::GetReflectionDefinition();

	const InstancedOptionalAttribute* optional_attr = ref_def.template getVarAttr<InstancedOptionalAttribute>(Refl::IReflectionVar::getName());

	if (!optional_attr) {
		const Refl::IVar<T>* parent = Refl::IVar<T>::getParent();

		while (parent && parent->isContainer()) {
			parent = parent->getParent();
		}

		if (parent) {
			optional_attr = ref_def.template getVarAttr<InstancedOptionalAttribute>(parent->getName());
		}
	}

	return optional_attr;
}

template <class T>
const Vector<Refl::IReflectionVar::SubVarData>& VarInstancedArrayAny<T>::getSubVars(void)
{
	return _cached_element_vars;
}

template <class T>
void VarInstancedArrayAny<T>::setSubVarBaseName(eastl::u8string_view base_name)
{
	_base_name = base_name;
}

template <class T>
void VarInstancedArrayAny<T>::regenerateSubVars(const void* object, int32_t range_begin, int32_t range_end)
{
	const InstancedArrayAny* const var = Refl::IVar<T>::template get<InstancedArrayAny>(&object);

	for (int32_t i = range_begin; i < range_end; ++i) {
		U8String element_path{ REFLECTION_ALLOCATOR };
		element_path.append_sprintf(u8"%s[%i]", _base_name.data(), i);

		_elements[i].setReflection(var->getReflectionDefinition(i)->getReflectionInstance());
		_elements[i].setOffset(static_cast<ptrdiff_t>(var->_metadata[i].start));
		_elements[i].setParent(this);
		_elements[i].setNoSerialize(true);

		_cached_element_vars[i].first = HashString32<>(element_path);
		_cached_element_vars[i].second = &_elements[i];
	}
}





template <class T, class VarType>
VarInstancedArray<T, VarType>::VarInstancedArray(InstancedArray<VarType> T::* ptr):
	VarInstancedArrayAny<T>(ptr)
{
}

template <class T, class VarType>
const Refl::Reflection<typename VarInstancedArray<T, VarType>::ReflectionType>& VarInstancedArray<T, VarType>::GetReflection(void)
{
	return Refl::Reflection<ReflectionType>::GetInstance();
}

template <class T, class VarType>
const Refl::IReflection& VarInstancedArray<T, VarType>::getReflection(void) const
{
	return GetReflection();
}

template <class T, class VarType>
const void* VarInstancedArray<T, VarType>::getData(const void* object) const
{
	return const_cast<VarInstancedArray<T, VarType>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class VarType>
void* VarInstancedArray<T, VarType>::getData(void* object)
{
	return Refl::IVar<T>::template get< InstancedArray<VarType> >(object);
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::setData(void* /*object*/, const void* /*data*/)
{
	GAFF_ASSERT_MSG(
		false,
		"VarInstancedArray<T, VarType>::setData() was called with ReflectionType of '%s'.",
		reinterpret_cast<const char*>(Refl::Reflection<ReflectionType>::GetName())
	);
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::setDataMove(void* object, void* data)
{
	if (Refl::IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	InstancedArray<VarType>* const var = Refl::IVar<T>::template get< InstancedArray<VarType> >(object);
	*var = std::move(*reinterpret_cast<InstancedArray<VarType>*>(data));
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::setElement(void* /*object*/, int32_t /*index*/, const void* /*data*/)
{
	GAFF_ASSERT_MSG(
		false,
		"VarInstancedArray<T, VarType>::setElement() was called with ReflectionType of '%s'.",
		reinterpret_cast<const char*>(Refl::Reflection<ReflectionType>::GetName())
	);
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::setElementMove(void* /*object*/, int32_t /*index*/, void* /*data*/)
{
	GAFF_ASSERT_MSG(
		false,
		"VarInstancedArray<T, VarType>::setElementMove() was called with ReflectionType of '%s'.",
		reinterpret_cast<const char*>(Refl::Reflection<ReflectionType>::GetName())
	);
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::regenerateSubVars(const void* object, int32_t range_begin, int32_t range_end)
{
	const InstancedArray<VarType>* const var = Refl::IVar<T>::template get< InstancedArray<VarType> >(&object);

	_elements_typed.resize(VarInstancedArrayAny<T>::_cached_element_vars.size());

	for (int32_t i = range_begin; i < range_end; ++i) {
		U8String element_path{ REFLECTION_ALLOCATOR };
		element_path.append_sprintf(u8"%s[%i]", VarInstancedArrayAny<T>::_base_name.data(), i);

		_elements_typed[i].setReflection(var->getReflectionDefinition(i)->getReflectionInstance());
		_elements_typed[i].setOffset(static_cast<ptrdiff_t>(var->_metadata[i].start));
		_elements_typed[i].setParent(this);
		_elements_typed[i].setNoSerialize(true);

		VarInstancedArrayAny<T>::_cached_element_vars[i].first = HashString32<>(element_path);
		VarInstancedArrayAny<T>::_cached_element_vars[i].second = &_elements_typed[i];
	}
}

NS_END
