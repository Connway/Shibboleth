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
bool VarInstancedArray<T, VarType>::load(const ISerializeReader& reader, void* object)
{
	GAFF_ASSERT(reader.isNull() || reader.isArray());

	if (reader.isNull()) {
		return true;
	}

	InstancedArray<VarType>* const var = reinterpret_cast<InstancedArray<VarType>*>(object);
	const ReflectionManager& ref_mgr = GetApp().getReflectionManager();
	const int32_t array_size = reader.size();
	bool success = true;

	for (int32_t i = 0; i < array_size; ++i) {
		const auto element_guard = reader.enterElementGuard(i);
		const char8_t* class_name = nullptr;

		if (reader.isNull()) {
			var->pushEmpty();
			continue;
		}

		{
			const auto guard = reader.enterElementGuard(u8"class");
			GAFF_ASSERT(reader.isNull() || reader.isString());

			if (reader.isNull()) {
				// $TODO: Log error.
				var->pushEmpty();

				continue;
			}

			class_name = reader.readString();
		}

		if (class_name && class_name[0]) {
			const Refl::IReflectionDefinition* const ref_def = ref_mgr.getReflection(Gaff::FNV1aHash64String(class_name));

			if (ref_def) {
				VarType* const instance = var->push(*ref_def);

				const auto guard = reader.enterElementGuard(u8"data");
				GAFF_ASSERT(reader.isNull() || reader.isObject());

				if (!reader.isNull()) {
					ref_def->load(reader, ref_def->getBasePointer(instance));
				}

			} else {
				// $TODO: Log error.
				reader.freeString(class_name);
				success = false;

				var->pushEmpty();
			}
		}
	}

	reader.freeString(class_name);
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
		VarType* const element = var->at(i);

		if (element) {
			const Refl::IReflectionDefinition* const ref_def = var->getReflectionDefinition(i);

			writer.startObject(2);

			writer.writeString(u8"class", ref_def->getReflectionInstance().getName());
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

NS_END
