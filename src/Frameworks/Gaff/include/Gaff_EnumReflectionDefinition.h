/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Gaff_ReflectionInterfaces.h"
#include "Gaff_HashString.h"
#include "Gaff_VectorMap.h"
#include "Gaff_SmartPtrs.h"

NS_GAFF

template <class Enum, class Allocator>
class EnumReflectionDefinition : public IEnumReflectionDefinition
{
	static_assert(std::is_enum<Enum>::value, "Template type Enum is not an enum.");

public:
	using IAttributePtr = UniquePtr<IAttribute, Allocator>;

	GAFF_STRUCTORS_DEFAULT(EnumReflectionDefinition);
	GAFF_NO_COPY(EnumReflectionDefinition);
	GAFF_NO_MOVE(EnumReflectionDefinition);

	constexpr static int32_t Size(void) { return static_cast<int32_t>(sizeof(Enum)); }

	const IReflection& getReflectionInstance(void) const override;
	int32_t size(void) const override;

	Hash64 getInstanceHash(const void* object, Hash64 init = k_init_hash64) const override { return FNV1aHash64(reinterpret_cast<const char*>(object), sizeof(Enum), init); }
	Hash64 getInstanceHash(Enum value, Hash64 init = k_init_hash64) const { return getInstanceHash(&value, init); }

	bool load(const ISerializeReader& reader, void* value, bool) const { return load(reader, value); }
	void save(ISerializeWriter& writer, const void* value, bool) const { save(writer, value); }
	bool load(const ISerializeReader& reader, Enum& value, bool) const { return load(reader, value); }
	void save(ISerializeWriter& writer, Enum value, bool) const { save(writer, value); }

	bool load(const ISerializeReader& reader, void* value) const override;
	void save(ISerializeWriter& writer, const void* value) const override;
	bool load(const ISerializeReader& reader, Enum& value) const;
	void save(ISerializeWriter& writer, Enum value) const;

	int32_t getNumEntries(void) const override;
	HashStringView32<> getEntryNameFromValue(int32_t value) const override;
	HashStringView32<> getEntryNameFromIndex(int32_t index) const override;
	int32_t getEntryValue(const char* name) const override;
	int32_t getEntryValue(int32_t index) const override;
	int32_t getEntryValue(Hash32 name) const override;

	Enum getEntry(const char* name) const;
	Enum getEntry(int32_t index) const;
	Enum getEntry(Hash32 name) const;

	HashStringView32<> getEntryName(Enum value) const;

	bool entryExists(const char* name) const;
	bool entryExists(Hash32 name) const;

	int32_t getNumEnumAttrs(void) const override;
	const IAttribute* getEnumAttr(int32_t index) const override;

	void setAllocator(const Allocator& allocator);

	template <class... Attrs>
	EnumReflectionDefinition& enumAttrs(const Attrs&... attrs);

	template <size_t name_size, class... Attrs>
	EnumReflectionDefinition& entry(const char (&name)[name_size], Enum value);

	void finish(void);

private:
	VectorMap<HashString32<Allocator>, Enum, Allocator> _entries;
	Vector<IAttributePtr, Allocator> _enum_attrs;

	Allocator _allocator;

	template <class First, class... Rest>
	EnumReflectionDefinition& addAttributes(Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);
};

NS_END

#include "Gaff_EnumReflectionDefinition.inl"
