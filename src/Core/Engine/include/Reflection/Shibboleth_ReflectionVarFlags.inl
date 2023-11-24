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

// VarFlags
template <class T, class Enum>
VarFlags<T, Enum>::VarFlags(Gaff::Flags<Enum> T::*ptr):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class Enum>
const IReflection& VarFlags<T, Enum>::getReflection(void) const
{
	return Reflection<Enum>::GetInstance();
}

template <class T, class Enum>
const void* VarFlags<T, Enum>::getData(const void* object) const
{
	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class Enum>
void* VarFlags<T, Enum>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr);
}

template <class T, class Enum>
void VarFlags<T, Enum>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = *reinterpret_cast<const Gaff::Flags<Enum>*>(data);
}

template <class T, class Enum>
void VarFlags<T, Enum>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = std::move(*reinterpret_cast<Gaff::Flags<Enum>*>(data));
}

template <class T, class Enum>
bool VarFlags<T, Enum>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	Gaff::Flags<Enum>* const var = &(object.*_ptr);

	// Iterate over all the flags and read values.
	const IEnumReflectionDefinition& ref_def = getReflection().getEnumReflectionDefinition();
	const int32_t num_entries = ref_def.getNumEntries();
	bool success = true;

	for (int32_t i = 0; i < num_entries; ++i) {
		const Shibboleth::HashStringView32<> flag_name = ref_def.getEntryNameFromIndex(i);
		const int32_t flag_index = ref_def.getEntryValue(i);

		const auto guard = reader.enterElementGuard(flag_name.getBuffer());

		if (!reader.isBool() && !reader.isNull()) {
			success = false;
			continue;
		}

		const bool value = reader.readBool(false);
		var->set(value, static_cast<Enum>(flag_index));
	}

	return success;
}

template <class T, class Enum>
void VarFlags<T, Enum>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const Gaff::Flags<Enum>* const var = &(object.*_ptr);

	// Iterate over all the flags and write values.
	const IEnumReflectionDefinition& ref_def = getReflection().getEnumReflectionDefinition();
	const int32_t num_entries = ref_def.getNumEntries();

	for (int32_t i = 0; i < num_entries; ++i) {
		const Shibboleth::HashStringView32<> flag_name = ref_def.getEntryNameFromIndex(i);
		const int32_t flag_index = ref_def.getEntryValue(i);
		const bool value = var->testAll(static_cast<Enum>(flag_index));

		writer.writeBool(flag_name.getBuffer(), value);
	}
}

template <class T, class Enum>
bool VarFlags<T, Enum>::isFlags(void) const
{
	return true;
}

template <class T, class Enum>
void VarFlags<T, Enum>::setFlagValue(void* object, int32_t flag_index, bool value)
{
	(reinterpret_cast<T*>(object)->*_ptr).set(value, static_cast<Enum>(flag_index));
}

template <class T, class Enum>
bool VarFlags<T, Enum>::getFlagValue(void* object, int32_t flag_index) const
{
	return (reinterpret_cast<T*>(object)->*_ptr).testAll(static_cast<Enum>(flag_index));
}



// VarFlagBit
template <class T, class Enum>
VarFlagBit<T, Enum>::VarFlagBit(Gaff::Flags<Enum> T::*ptr, uint8_t flag_index):
	_ptr(ptr), _flag_index(flag_index), _cache(false)
{
	GAFF_ASSERT(ptr);
}

template <class T, class Enum>
const IReflection& VarFlagBit<T, Enum>::getReflection(void) const
{
	return Reflection<bool>::GetInstance();
}

template <class T, class Enum>
const void* VarFlagBit<T, Enum>::getData(const void* object) const
{
	return const_cast<VarFlagBit<T, Enum>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class Enum>
void* VarFlagBit<T, Enum>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);
	_cache = (obj->*_ptr).testAll(static_cast<Enum>(_flag_index));
	return &_cache;
}

template <class T, class Enum>
void VarFlagBit<T, Enum>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr).set(*reinterpret_cast<const bool*>(data), static_cast<Enum>(_flag_index));
}

template <class T, class Enum>
void VarFlagBit<T, Enum>::setDataMove(void* object, void* data)
{
	setData(object, data);
}

template <class T, class Enum>
bool VarFlagBit<T, Enum>::load(const Shibboleth::ISerializeReader& /*reader*/, T& /*object*/)
{
	GAFF_ASSERT_MSG(false, "VarFlagBit::load() should never be called.");
	return false;
}

template <class T, class Enum>
void VarFlagBit<T, Enum>::save(Shibboleth::ISerializeWriter& /*writer*/, const T& /*object*/)
{
	GAFF_ASSERT_MSG(false, "VarFlagBit::save() should never be called.");
}



template <class T, class Enum>
IVar<T>* VarPtrTypeHelper< T, Gaff::Flags<Enum> >::Create(
	eastl::u8string_view name,
	Gaff::Flags<Enum> T::* ptr,
	const Shibboleth::ProxyAllocator& allocator,
	Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& extra_vars)
{
	static_assert(std::is_enum<Enum>::value, "Flags does not contain an enum.");
	static_assert(Reflection<Enum>::HasReflection, "Enum is not reflected!");

	GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string.");

	IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(VarFlags<T, Enum>), allocator, ptr);

	// For each reflected entry in Enum, add a reflection var for that entry.
	const EnumReflectionDefinition<Enum>& ref_def = Reflection<Enum>::GetReflectionDefinition();
	const int32_t num_entries = ref_def.getNumEntries();

	for (int32_t i = 0; i < num_entries; ++i) {
		const Shibboleth::HashStringView32<> flag_name = ref_def.getEntryNameFromIndex(i);
		const int32_t flag_index = ref_def.getEntryValue(i);

		Shibboleth::U8String flag_path(allocator);
		flag_path.append_sprintf(u8"%s/%s", name.data(), flag_name.getBuffer());

		eastl::pair<Shibboleth::HashString32<>, IVar<T>*> flag_pair(
			Shibboleth::HashString32<>(flag_path),
			SHIB_ALLOCT(GAFF_SINGLE_ARG(VarFlagBit<T, Enum>), allocator, ptr, static_cast<uint8_t>(flag_index))
		);

		flag_pair.second->setNoSerialize(true);
		extra_vars.emplace_back(std::move(flag_pair));
	}

	return var_ptr;
}

NS_END
