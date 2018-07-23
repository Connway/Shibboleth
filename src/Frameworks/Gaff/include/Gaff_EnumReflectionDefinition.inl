/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
const IEnumReflection& EnumReflectionDefinition<Enum, Allocator>::getReflectionInstance(void) const
{
	return GAFF_REFLECTION_NAMESPACE::EnumReflection<Enum>::GetInstance();
}

template <class Enum, class Allocator>
void EnumReflectionDefinition<Enum, Allocator>::load(ISerializeReader& reader, void* object) const
{
	const char* const name = reader.readString();
	const int32_t intValue = getEntryValue(name);
	GAFF_ASSERT(intValue != INT_MIN);

	Enum* const value = reinterpret_cast<Enum*>(object);
	*value = static_cast<Enum>(intValue);
}

template <class Enum, class Allocator>
void EnumReflectionDefinition<Enum, Allocator>::save(ISerializeWriter& writer, const void* object) const
{
	const Enum* const value = reinterpret_cast<const Enum*>(object);
	const char* const name = getEntryName(*value);
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

	return (it == _entries.end()) ? INT_MIN : static_cast<int32_t>(it->second);
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
int32_t EnumReflectionDefinition<Enum, Allocator>::getNumEntryAttrs(Hash32 name) const
{
	auto it = _entry_attrs.find(name);
	GAFF_ASSERT(it != _entry_attrs.end());
	return static_cast<int32_t>(it->second.size());
}

template <class Enum, class Allocator>
const IAttribute* EnumReflectionDefinition<Enum, Allocator>::getEntryAttr(Hash32 name, int32_t index) const
{
	auto it = _entry_attrs.find(name);
	GAFF_ASSERT(it != _entry_attrs.end());
	GAFF_ASSERT(index < static_cast<int32_t>(it->second.size()));
	return it->second[index].get();
}

template <class Enum, class Allocator>
void EnumReflectionDefinition<Enum, Allocator>::setAllocator(const Allocator& allocator)
{
	_entries.set_allocator(allocator);
	_allocator = allocator;
}

template <class Enum, class Allocator>
Hash64 EnumReflectionDefinition<Enum, Allocator>::getVersion(void) const
{
	return _version;
}

template <class Enum, class Allocator>
template <size_t size>
EnumReflectionDefinition<Enum, Allocator>& EnumReflectionDefinition<Enum, Allocator>::entry(const char (&name)[size], Enum value)
{
	eastl::pair<HashString32<Allocator>, Enum> pair(
		HashString32<Allocator>(name, size - 1, nullptr, _allocator),
		value
	);

	GAFF_ASSERT(_entries.find(pair.first) == _entries.end());
	_entries.insert(std::move(pair));

	_version = FNV1aHash64(name, size - 1, _version);
	return *this;
}

template <class Enum, class Allocator>
template <class... Args>
EnumReflectionDefinition<Enum, Allocator>& EnumReflectionDefinition<Enum, Allocator>::enumAttrs(const Args&... args)
{
	return addAttributes(_entry_attrs, args...);
}

template <class Enum, class Allocator>
template <size_t size, class... Args>
EnumReflectionDefinition<Enum, Allocator>& EnumReflectionDefinition<Enum, Allocator>::entryAttrs(const char (&name)[size], const Args&... args)
{
	auto& attrs = _entry_attrs[FNV1aHash32Const(name)];
	attrs.set_allocator(_allocator);
	return addAttributes(attrs, args...);
}

template <class Enum, class Allocator>
void EnumReflectionDefinition<Enum, Allocator>::finish(void)
{
	GAFF_REFLECTION_NAMESPACE::EnumReflection<Enum>::g_defined = true;
}

template <class Enum, class Allocator>
template <class First, class... Rest>
EnumReflectionDefinition<Enum, Allocator>& EnumReflectionDefinition<Enum, Allocator>::addAttributes(Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest)
{
	attrs.emplace_back(IAttributePtr(GAFF_ALLOCT(First, _allocator, first)));
	return addAttributes(attrs, rest...);
}

template <class Enum, class Allocator>
EnumReflectionDefinition<Enum, Allocator>& EnumReflectionDefinition<Enum, Allocator>::addAttributes(Vector<IAttributePtr, Allocator>&)
{
	return *this;
}

NS_END
