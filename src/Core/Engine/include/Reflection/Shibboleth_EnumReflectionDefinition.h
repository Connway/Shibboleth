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

#include "Shibboleth_IEnumReflectionDefinition.h"
#include "Containers/Shibboleth_VectorMap.h"
#include "Shibboleth_SerializeInterfaces.h"
#include "Containers/Shibboleth_Vector.h"
#include "Ptrs/Shibboleth_SmartPtrs.h"
#include "Shibboleth_IAttribute.h"
#include "Shibboleth_HashString.h"
#include <Shibboleth_ProxyAllocator.h>
#include <Gaff_ContainerAlgorithm.h>
#include <Gaff_Hash.h>

NS_REFLECTION

class IReflection;

template <class Enum>
class EnumReflectionDefinition : public IEnumReflectionDefinition
{
	static_assert(std::is_enum<Enum>::value, "Template type Enum is not an enum.");

public:
	using IAttributePtr = Shibboleth::UniquePtr<IAttribute>;

	GAFF_STRUCTORS_DEFAULT(EnumReflectionDefinition);
	GAFF_NO_COPY(EnumReflectionDefinition);
	GAFF_NO_MOVE(EnumReflectionDefinition);

	constexpr static int32_t Size(void) { return static_cast<int32_t>(sizeof(Enum)); }

	const IReflection& getReflectionInstance(void) const override;
	int32_t size(void) const override;

	Gaff::Hash64 getInstanceHash(const void* object, Gaff::Hash64 init = Gaff::k_init_hash64) const override { return Gaff::FNV1aHash64(reinterpret_cast<const char*>(object), sizeof(Enum), init); }
	Gaff::Hash64 getInstanceHash(Enum value, Gaff::Hash64 init = Gaff::k_init_hash64) const { return getInstanceHash(&value, init); }

	bool load(const Shibboleth::ISerializeReader& reader, void* value, Gaff::Flags<LoadFlags> flags = Gaff::Flags<LoadFlags>{}) const override { return load(reader, *reinterpret_cast<Enum*>(value), flags); }
	void save(Shibboleth::ISerializeWriter& writer, const void* value, Gaff::Flags<SaveFlags> flags = Gaff::Flags<SaveFlags>{}) const override { save(writer, *reinterpret_cast<const Enum*>(value), flags); }
	bool load(const Shibboleth::ISerializeReader& reader, Enum& value, Gaff::Flags<LoadFlags> flags = Gaff::Flags<LoadFlags>{}) const;
	void save(Shibboleth::ISerializeWriter& writer, Enum value, Gaff::Flags<SaveFlags> flags = Gaff::Flags<SaveFlags>{}) const;

	int32_t getNumEntries(void) const override;
	Shibboleth::HashStringView32<> getEntryNameFromValue(int32_t value) const override;
	Shibboleth::HashStringView32<> getEntryNameFromIndex(int32_t index) const override;
	int32_t getEntryValue(const char8_t* name) const override;
	int32_t getEntryValue(int32_t index) const override;
	int32_t getEntryValue(Gaff::Hash32 name) const override;

	Enum getEntry(const char8_t* name) const;
	Enum getEntry(int32_t index) const;
	Enum getEntry(Gaff::Hash32 name) const;

	Shibboleth::HashStringView32<> getEntryName(Enum value) const;

	bool entryExists(const char8_t* name) const;
	bool entryExists(Gaff::Hash32 name) const;

	int32_t getNumEnumAttrs(void) const override;
	const IAttribute* getEnumAttr(Gaff::Hash64 attr_name) const override;
	const IAttribute* getEnumAttr(int32_t index) const override;

	void setAllocator(const Shibboleth::ProxyAllocator& allocator);

	template <class... Attrs>
	EnumReflectionDefinition& enumAttrs(const Attrs&... attrs);

	template <size_t name_size, class... Attrs>
	EnumReflectionDefinition& entry(const char8_t (&name)[name_size], Enum value);

	template <size_t name_size, class... Attrs>
	EnumReflectionDefinition& entry(const char (&name)[name_size], Enum value);

	void finish(void);

private:
	Shibboleth::VectorMap<Shibboleth::HashString32<>, Enum> _entries{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::Vector<IAttributePtr> _enum_attrs{ Shibboleth::ProxyAllocator("Reflection") };

	Shibboleth::ProxyAllocator _allocator;

	template <class First, class... Rest>
	EnumReflectionDefinition& addAttributes(Shibboleth::Vector<IAttributePtr>& attrs, const First& first, const Rest&... rest);

	EnumReflectionDefinition& getInitialBuilder(void);

	template <class RefT>
	friend class Reflection;
};

NS_END

#include "Shibboleth_EnumReflectionDefinition.inl"
