/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

NS_GAFF

template <class Enum, class Allocator>
const IReflection& EnumReflectionDefinition<Enum, Allocator>::getReflectionInstance(void) const
{
	return GAFF_REFLECTION_NAMESPACE::Reflection<Enum>::GetInstance();
}

template <class Enum, class Allocator>
int32_t EnumReflectionDefinition<Enum, Allocator>::size(void) const
{
	return Size();
}

template <class Enum, class Allocator>
bool EnumReflectionDefinition<Enum, Allocator>::load(const ISerializeReader& reader, void* value) const
{
	return load(reader, *reinterpret_cast<Enum*>(value));
}

template <class Enum, class Allocator>
void EnumReflectionDefinition<Enum, Allocator>::save(ISerializeWriter& writer, const void* value) const
{
	save(writer, *reinterpret_cast<const Enum*>(value));
}

template <class Enum, class Allocator>
bool EnumReflectionDefinition<Enum, Allocator>::load(const ISerializeReader& reader, Enum& value) const
{
	const char* const name = reader.readString();
	const int32_t intValue = getEntryValue(name);

	if (intValue == std::numeric_limits<int32_t>::min()) {
		return false;
	}

	value = static_cast<Enum>(intValue);
	return true;
}

template <class Enum, class Allocator>
void EnumReflectionDefinition<Enum, Allocator>::save(ISerializeWriter& writer, Enum value) const
{
	const char* const name = getEntryName(value);
	GAFF_ASSERT(name);

	writer.writeString(name);
}

template <class Enum, class Allocator>
int32_t EnumReflectionDefinition<Enum, Allocator>::getNumEntries(void) const
{
	return static_cast<int32_t>(_entries.size());
}

template <class Enum, class Allocator>
const char* EnumReflectionDefinition<Enum, Allocator>::getEntryNameFromValue(int32_t value) const
{
	return getEntryName(static_cast<Enum>(value));
}

template <class Enum, class Allocator>
const char* EnumReflectionDefinition<Enum, Allocator>::getEntryNameFromIndex(int32_t index) const
{
	GAFF_ASSERT(static_cast<size_t>(index) < _entries.size());
	return (_entries.begin() + index)->first.getBuffer();
}

template <class Enum, class Allocator>
int32_t EnumReflectionDefinition<Enum, Allocator>::getEntryValue(int32_t index) const
{
	GAFF_ASSERT(static_cast<size_t>(index) < _entries.size());
	return static_cast<int32_t>((_entries.begin() + index)->second);
}

template <class Enum, class Allocator>
int32_t EnumReflectionDefinition<Enum, Allocator>::getEntryValue(const char* name) const
{
	return getEntryValue(FNV1aHash32String(name));
}

template <class Enum, class Allocator>
int32_t EnumReflectionDefinition<Enum, Allocator>::getEntryValue(Hash32 name) const
{
	auto it = Find(_entries, name, [](const HashString32<Allocator>& lhs, Hash32 rhs) -> bool
	{
		return lhs.getHash() < rhs;
	});

	return (it == _entries.end()) ? std::numeric_limits<int32_t>::min() : static_cast<int32_t>(it->second);
}

template <class Enum, class Allocator>
const char* EnumReflectionDefinition<Enum, Allocator>::getEntryName(Enum value) const
{
	for (const auto& entry : _entries) {
		if (entry.second == value) {
			return entry.first.getBuffer();
		}
	}

	return nullptr;
}

template <class Enum, class Allocator>
int32_t EnumReflectionDefinition<Enum, Allocator>::getNumEnumAttrs(void) const
{
	return static_cast<int32_t>(_enum_attrs.size());
}

template <class Enum, class Allocator>
const IAttribute* EnumReflectionDefinition<Enum, Allocator>::getEnumAttr(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_enum_attrs.size()));
	return _enum_attrs[index].get();
}

template <class Enum, class Allocator>
void EnumReflectionDefinition<Enum, Allocator>::setAllocator(const Allocator& allocator)
{
	_entries.set_allocator(allocator);
	_allocator = allocator;
}

template <class Enum, class Allocator>
template <class... Attrs>
EnumReflectionDefinition<Enum, Allocator>& EnumReflectionDefinition<Enum, Allocator>::enumAttrs(const Attrs&... attrs)
{
	if constexpr (sizeof...(Attrs) > 0) {
		addAttributes(_entry_attrs, attrs...);
	}

	return *this;
}

template <class Enum, class Allocator>
template <size_t name_size, class... Attrs>
EnumReflectionDefinition<Enum, Allocator>& EnumReflectionDefinition<Enum, Allocator>::entry(const char (&name)[name_size], Enum value)
{
	eastl::pair<HashString32<Allocator>, Enum> pair(
		HashString32<Allocator>(name, name_size - 1, _allocator),
		value
	);

	GAFF_ASSERT(_entries.find(pair.first) == _entries.end());
	_entries.insert(std::move(pair));

	return *this;
}

template <class Enum, class Allocator>
void EnumReflectionDefinition<Enum, Allocator>::finish(void)
{
	for (IAttributePtr& attr : _enum_attrs) {
		attr->finish(*this);
	}
}

template <class Enum, class Allocator>
template <class First, class... Rest>
EnumReflectionDefinition<Enum, Allocator>& EnumReflectionDefinition<Enum, Allocator>::addAttributes(Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	attrs.emplace_back(IAttributePtr(first->clone()));

	if constexpr (sizeof...(Rest) > 0) {
		return addAttributes(attrs, rest...);
	} else {
		return *this;
	}
}

NS_END
