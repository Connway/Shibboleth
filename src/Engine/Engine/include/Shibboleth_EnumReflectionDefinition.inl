/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

NS_REFLECTION

template <class Enum>
const IReflection& EnumReflectionDefinition<Enum>::getReflectionInstance(void) const
{
	return Reflection<Enum>::GetInstance();
}

template <class Enum>
int32_t EnumReflectionDefinition<Enum>::size(void) const
{
	return Size();
}

template <class Enum>
bool EnumReflectionDefinition<Enum>::load(const Shibboleth::ISerializeReader& reader, void* value) const
{
	return load(reader, *reinterpret_cast<Enum*>(value));
}

template <class Enum>
void EnumReflectionDefinition<Enum>::save(Shibboleth::ISerializeWriter& writer, const void* value) const
{
	save(writer, *reinterpret_cast<const Enum*>(value));
}

template <class Enum>
bool EnumReflectionDefinition<Enum>::load(const Shibboleth::ISerializeReader& reader, Enum& value) const
{
	const char8_t* const name = reader.readString();
	const int32_t intValue = getEntryValue(name);

	if (intValue == std::numeric_limits<int32_t>::min()) {
		return false;
	}

	value = static_cast<Enum>(intValue);
	return true;
}

template <class Enum>
void EnumReflectionDefinition<Enum>::save(Shibboleth::ISerializeWriter& writer, Enum value) const
{
	const Shibboleth::HashStringView32<> name = getEntryName(value);
	GAFF_ASSERT(name.getBuffer());

	writer.writeString(name.getBuffer());
}

template <class Enum>
int32_t EnumReflectionDefinition<Enum>::getNumEntries(void) const
{
	return static_cast<int32_t>(_entries.size());
}

template <class Enum>
Shibboleth::HashStringView32<> EnumReflectionDefinition<Enum>::getEntryNameFromValue(int32_t value) const
{
	return getEntryName(static_cast<Enum>(value));
}

template <class Enum>
Shibboleth::HashStringView32<> EnumReflectionDefinition<Enum>::getEntryNameFromIndex(int32_t index) const
{
	GAFF_ASSERT(static_cast<size_t>(index) < _entries.size());
	return Shibboleth::HashStringView32<>((_entries.begin() + index)->first);
}

template <class Enum>
int32_t EnumReflectionDefinition<Enum>::getEntryValue(const char8_t* name) const
{
	return getEntryValue(Gaff::FNV1aHash32String(name));
}

template <class Enum>
int32_t EnumReflectionDefinition<Enum>::getEntryValue(int32_t index) const
{
	GAFF_ASSERT(static_cast<size_t>(index) < _entries.size());
	return static_cast<int32_t>((_entries.begin() + index)->second);
}

template <class Enum>
int32_t EnumReflectionDefinition<Enum>::getEntryValue(Gaff::Hash32 name) const
{
	auto it = Gaff::Find(_entries, name, [](const Shibboleth::HashString32<>& lhs, Gaff::Hash32 rhs) -> bool
	{
		return lhs.getHash() < rhs;
	});

	return (it == _entries.end()) ? std::numeric_limits<int32_t>::min() : static_cast<int32_t>(it->second);
}

template <class Enum>
Enum EnumReflectionDefinition<Enum>::getEntry(const char8_t* name) const
{
	return getEntry(Gaff::FNV1aHash32String(name));
}

template <class Enum>
Enum EnumReflectionDefinition<Enum>::getEntry(int32_t index) const
{
	GAFF_ASSERT(static_cast<size_t>(index) < _entries.size());
	return static_cast<int32_t>((_entries.begin() + index)->second);
}

template <class Enum>
Enum EnumReflectionDefinition<Enum>::getEntry(Gaff::Hash32 name) const
{
	auto it = Gaff::Find(_entries, name, [](const Shibboleth::HashString32<>& lhs, Gaff::Hash32 rhs) -> bool
	{
		return lhs.getHash() < rhs;
	});

	return (it == _entries.end()) ? static_cast<Enum>(std::numeric_limits<int32_t>::min()) : it->second;
}

template <class Enum>
Shibboleth::HashStringView32<> EnumReflectionDefinition<Enum>::getEntryName(Enum value) const
{
	for (const auto& entry : _entries) {
		if (entry.second == value) {
			return Shibboleth::HashStringView32<>(entry.first);
		}
	}

	return Shibboleth::HashStringView32<>();
}

template <class Enum>
bool EnumReflectionDefinition<Enum>::entryExists(const char8_t* name) const
{
	return entryExists(Gaff::FNV1aHash32String(name));
}

template <class Enum>
bool EnumReflectionDefinition<Enum>::entryExists(Gaff::Hash32 name) const
{
	auto it = Gaff::Find(_entries, name, [](const Shibboleth::HashString32<>& lhs, Gaff::Hash32 rhs) -> bool
	{
		return lhs.getHash() < rhs;
	});

	return it != _entries.end();
}

template <class Enum>
int32_t EnumReflectionDefinition<Enum>::getNumEnumAttrs(void) const
{
	return static_cast<int32_t>(_enum_attrs.size());
}

template <class Enum>
const IAttribute* EnumReflectionDefinition<Enum>::getEnumAttr(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_enum_attrs.size()));
	return _enum_attrs[index].get();
}

template <class Enum>
void EnumReflectionDefinition<Enum>::setAllocator(const Shibboleth::ProxyAllocator& allocator)
{
	_entries.set_allocator(allocator);
	_allocator = allocator;
}

template <class Enum>
template <class... Attrs>
EnumReflectionDefinition<Enum>& EnumReflectionDefinition<Enum>::enumAttrs(const Attrs&... attrs)
{
	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(_enum_attrs, attrs...);
	}

	return *this;
}

template <class Enum>
template <size_t name_size, class... Attrs>
EnumReflectionDefinition<Enum>& EnumReflectionDefinition<Enum>::entry(const char8_t (&name)[name_size], Enum value)
{
	eastl::pair<Shibboleth::HashString32<>, Enum> pair(
		Shibboleth::HashString32<>(name, name_size - 1, _allocator),
		value
	);

	GAFF_ASSERT(_entries.find(pair.first) == _entries.end());
	_entries.insert(std::move(pair));

	return *this;
}

template <class Enum>
template <size_t name_size, class... Attrs>
EnumReflectionDefinition<Enum>& EnumReflectionDefinition<Enum>::entry(const char (&name)[name_size], Enum value)
{
	CONVERT_STRING_ARRAY(char8_t, temp_name, name);
	return entry(temp_name, value);
}

template <class Enum>
void EnumReflectionDefinition<Enum>::finish(void)
{
	for (IAttributePtr& attr : _enum_attrs) {
		attr->finish(*this);
	}
}

template <class Enum>
template <class First, class... Rest>
EnumReflectionDefinition<Enum>& EnumReflectionDefinition<Enum>::addAttributes(Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest)
{
	attrs.emplace_back(IAttributePtr(first->clone()));

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(attrs, rest...);
	} else {
		return *this;
	}
}

NS_END
