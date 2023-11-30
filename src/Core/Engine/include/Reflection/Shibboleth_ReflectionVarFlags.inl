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

// VarFlagBit
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
	GAFF_ASSERT(IVar<T>::_parent);

	_cache = IVar<T>::_parent->template get< Gaff::Flags<Enum> >(object)->testAll(static_cast<Enum>(IVar<T>::_offset));
	return &_cache;
}

template <class T, class Enum>
void VarFlagBit<T, Enum>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	GAFF_ASSERT(IVar<T>::_parent);

	Gaff::Flags<Enum>& flags = *IVar<T>::_parent->template get< Gaff::Flags<Enum> >(object);
	flags.set(*reinterpret_cast<const bool*>(data), static_cast<Enum>(IVar<T>::_offset));
}

template <class T, class Enum>
void VarFlagBit<T, Enum>::setDataMove(void* object, void* data)
{
	setData(object, data);
}

template <class T, class Enum>
bool VarFlagBit<T, Enum>::load(const Shibboleth::ISerializeReader& /*reader*/, void* /*object*/)
{
	GAFF_ASSERT_MSG(false, "VarFlagBit::load() should never be called.");
	return false;
}

template <class T, class Enum>
void VarFlagBit<T, Enum>::save(Shibboleth::ISerializeWriter& /*writer*/, const void* /*object*/)
{
	GAFF_ASSERT_MSG(false, "VarFlagBit::save() should never be called.");
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



// VarFlags
template <class T, class Enum>
VarFlags<T, Enum>::VarFlags(Gaff::Flags<Enum> T::*ptr):
	IVar<T>(ptr)
{
	_cached_element_vars.resize(static_cast<size_t>(Enum::Count));

	for (int32_t i = 0; i < static_cast<int32_t>(Enum::Count); ++i) {
		_elements[i].setOffset(static_cast<ptrdiff_t>(i));
		_elements[i].setParent(this);
		_elements[i].setNoSerialize(true);

		_cached_element_vars[i].second = &_elements[i];
	}
}

template <class T, class Enum>
const IReflection& VarFlags<T, Enum>::getReflection(void) const
{
	return Reflection<Enum>::GetInstance();
}

template <class T, class Enum>
const void* VarFlags<T, Enum>::getData(const void* object) const
{
	return const_cast<VarFlags<T, Enum>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class Enum>
void* VarFlags<T, Enum>::getData(void* object)
{
	return IVar<T>::template get< Gaff::Flags<Enum> >(object);
}

template <class T, class Enum>
void VarFlags<T, Enum>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	*IVar<T>::template get< Gaff::Flags<Enum> >(object) = *reinterpret_cast<const Gaff::Flags<Enum>*>(data);
}

template <class T, class Enum>
void VarFlags<T, Enum>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	*IVar<T>::template get< Gaff::Flags<Enum> >(object) = std::move(*reinterpret_cast<Gaff::Flags<Enum>*>(data));
}

template <class T, class Enum>
int32_t VarFlags<T, Enum>::size(const void* /*object*/) const
{
	return static_cast<int32_t>(Enum::Count);
}

template <class T, class Enum>
bool VarFlags<T, Enum>::load(const Shibboleth::ISerializeReader& reader, void* object)
{
	Gaff::Flags<Enum>& flags = *reinterpret_cast<Gaff::Flags<Enum>*>(object);

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
		flags.set(value, static_cast<Enum>(flag_index));
	}

	return success;
}

template <class T, class Enum>
void VarFlags<T, Enum>::save(Shibboleth::ISerializeWriter& writer, const void* object)
{
	const Gaff::Flags<Enum>& flags = *reinterpret_cast<const Gaff::Flags<Enum>*>(object);

	// Iterate over all the flags and write values.
	const IEnumReflectionDefinition& ref_def = getReflection().getEnumReflectionDefinition();
	const int32_t num_entries = ref_def.getNumEntries();

	for (int32_t i = 0; i < num_entries; ++i) {
		const Shibboleth::HashStringView32<> flag_name = ref_def.getEntryNameFromIndex(i);
		const int32_t flag_index = ref_def.getEntryValue(i);
		const bool value = flags.testAll(static_cast<Enum>(flag_index));

		writer.writeBool(flag_name.getBuffer(), value);
	}
}

template <class T, class Enum>
bool VarFlags<T, Enum>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	Gaff::Flags<Enum>& flags = *IVar<T>::template get< Gaff::Flags<Enum> >(&object);
	return load(reader, &flags);
}

template <class T, class Enum>
void VarFlags<T, Enum>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const Gaff::Flags<Enum>& flags = *IVar<T>::template get< Gaff::Flags<Enum> >(&object);
	save(writer, &flags);
}

template <class T, class Enum>
bool VarFlags<T, Enum>::isFlags(void) const
{
	return true;
}

template <class T, class Enum>
void VarFlags<T, Enum>::setFlagValue(void* object, int32_t flag_index, bool value)
{
	IVar<T>::template get< Gaff::Flags<Enum> >(object)->set(value, static_cast<Enum>(flag_index));
}

template <class T, class Enum>
bool VarFlags<T, Enum>::getFlagValue(const void* object, int32_t flag_index) const
{
	return IVar<T>::template get< Gaff::Flags<Enum> >(object)->testAll(static_cast<Enum>(flag_index));
}

template <class T, class Enum>
const Shibboleth::Vector<IReflectionVar::SubVarData>& VarFlags<T, Enum>::getSubVars(void)
{
	return _cached_element_vars;
}

template <class T, class Enum>
void VarFlags<T, Enum>::setSubVarBaseName(eastl::u8string_view base_name)
{
	// For each reflected entry in Enum, add a reflection var for that entry.
	const EnumReflectionDefinition<Enum>& ref_def = Reflection<Enum>::GetReflectionDefinition();
	const int32_t num_entries = ref_def.getNumEntries();

	for (int32_t i = 0; i < num_entries; ++i) {
		const Shibboleth::HashStringView32<> flag_name = ref_def.getEntryNameFromIndex(i);

		Shibboleth::U8String flag_path(Shibboleth::ProxyAllocator("Reflection"));
		flag_path.append_sprintf(u8"%s/%s", base_name.data(), flag_name.getBuffer());

		_cached_element_vars[i].first = Shibboleth::HashString32<>(flag_path);
	}
}

NS_END
