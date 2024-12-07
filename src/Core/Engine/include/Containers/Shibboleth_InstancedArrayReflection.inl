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
VarInstancedArray<T, VarType>::VarInstancedArray(InstancedArray<VarType> T::* ptr):
	Refl::IVar<T>(ptr)
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
bool VarInstancedArray<T, VarType>::isVector(void) const
{
	return true;
}

template <class T, class VarType>
bool VarInstancedArray<T, VarType>::isFixedArray(void) const
{
	return false;
}

template <class T, class VarType>
int32_t VarInstancedArray<T, VarType>::size(const void* object) const
{
	const InstancedArray<VarType>* const var = Refl::IVar<T>::template get< InstancedArray<VarType> >(object);
	return var->size();
}

template <class T, class VarType>
const void* VarInstancedArray<T, VarType>::getElement(const void* object, int32_t index) const
{
	return const_cast<VarInstancedArray<T, VarType>*>(this)->getElement(const_cast<void*>(object), index);
}

template <class T, class VarType>
void* VarInstancedArray<T, VarType>::getElement(void* object, int32_t index)
{
	InstancedArray<VarType>* const var = Refl::IVar<T>::template get< InstancedArray<VarType> >(object);
	return var->at(index);
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
void VarInstancedArray<T, VarType>::swap(void* object, int32_t index_a, int32_t index_b)
{
	InstancedArray<VarType>* const var = Refl::IVar<T>::template get< InstancedArray<VarType> >(object);
	var->swap(index_a, index_b);
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::resize(void* object, size_t new_size)
{
	InstancedArray<VarType>* const var = Refl::IVar<T>::template get< InstancedArray<VarType> >(object);
	var->resize(static_cast<int32_t>(new_size));

	const size_t old_size = _elements.size();

	_cached_element_vars.resize(new_size);
	_elements.resize(new_size);

	if (new_size > old_size) {
		regenerateSubVars(object, static_cast<int32_t>(old_size + 1), static_cast<int32_t>(new_size));
	}
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::remove(void* object, int32_t index)
{
	InstancedArray<VarType>* const var = Refl::IVar<T>::template get< InstancedArray<VarType> >(object);
	var->erase(index);

	_cached_element_vars.erase(_cached_element_vars.begin() + index);
	_elements.erase(_elements.begin() + index);

	regenerateSubVars(object, index, static_cast<int32_t>(_elements.size()));
}

template <class T, class VarType>
bool VarInstancedArray<T, VarType>::load(const ISerializeReader& reader, void* object)
{
	GAFF_ASSERT(reader.isNull() || reader.isArray());

	if (reader.isNull()) {
		return true;
	}

	const InstancedOptionalAttribute* const optional_attr = getOptionalAttribute();
	InstancedArray<VarType>* const var = reinterpret_cast<InstancedArray<VarType>*>(object);
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
				auto& instance = var->push(*ref_def);

				if (reader.isObject()) {
					const auto guard = reader.enterElementGuard(u8"data");
					GAFF_ASSERT(reader.isNull() || reader.isObject());

					if (!reader.isNull()) {
						if constexpr (std::is_same_v<VarType, void>) {
							success = ref_def->load(reader, ref_def->getBasePointer(instance)) && success;
						} else {
							success = ref_def->load(reader, ref_def->getBasePointer(&instance)) && success;
						}
					}
				}

			} else {
				// This entry is not optional.
				if (!optional_attr || stripped_class_name.size() != eastl::CharStrlen(class_name)) {
					// $TODO: Log error.
					reader.freeString(class_name);
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

	return true;
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::save(ISerializeWriter& writer, const void* object)
{
	const InstancedArray<VarType>* const var = reinterpret_cast<const InstancedArray<VarType>*>(object);

	if (var->empty()) {
		writer.writeNull();
		return;
	}

	const int32_t array_size = var->size();

	writer.startArray(static_cast<uint32_t>(array_size));

	for (int32_t i = 0; i < array_size; ++i) {
		const VarType* const element = var->at(i);

		if (element) {
			const Refl::IReflectionDefinition* const ref_def = var->getReflectionDefinition(i);

			writer.startObject(2);

			writer.writeString(u8"class", ref_def->getReflectionInstance().getName());
			// $TODO: If no data has been overridden from default, just write the class name instead.
			writer.writeKey(u8"data");

			ref_def->save(writer, ref_def->getBasePointer(element));

			writer.endObject();

		} else {
			writer.writeNull();
		}
	}

	writer.endArray();
}

template <class T, class VarType>
bool VarInstancedArray<T, VarType>::load(const ISerializeReader& reader, T& object)
{
	InstancedArray<VarType>* const var = Refl::IVar<T>::template get< InstancedArray<VarType> >(&object);
	return load(reader, var);
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::save(ISerializeWriter& writer, const T& object)
{
	const InstancedArray<VarType>* const var = Refl::IVar<T>::template get< InstancedArray<VarType> >(&object);
	save(writer, var);
}

template <class T, class VarType>
const InstancedOptionalAttribute* VarInstancedArray<T, VarType>::getOptionalAttribute(void) const
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

template <class T, class VarType>
const Vector<Refl::IReflectionVar::SubVarData>& VarInstancedArray<T, VarType>::getSubVars(void)
{
	return _cached_element_vars;
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::setSubVarBaseName(eastl::u8string_view base_name)
{
	_base_name = base_name;
}

template <class T, class VarType>
void VarInstancedArray<T, VarType>::regenerateSubVars(const void* object, int32_t range_begin, int32_t range_end)
{
	const InstancedArray<VarType>* const var = Refl::IVar<T>::template get< InstancedArray<VarType> >(&object);

	for (int32_t i = range_begin; i < range_end; ++i) {
		U8String element_path(REFLECTION_ALLOCATOR);
		element_path.append_sprintf(u8"%s[%i]", _base_name.data(), i);

		_elements[i].setReflection(var->getReflectionDefinition(i)->getReflectionInstance());
		_elements[i].setOffset(static_cast<ptrdiff_t>(var->_metadata[i].start));
		_elements[i].setParent(this);
		_elements[i].setNoSerialize(true);

		_cached_element_vars[i].first = HashString32<>(element_path);
		_cached_element_vars[i].second = &_elements[i];
	}
}

NS_END
