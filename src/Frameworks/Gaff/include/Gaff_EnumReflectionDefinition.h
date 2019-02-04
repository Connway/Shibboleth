/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
public:
	using IAttributePtr = UniquePtr<IAttribute, Allocator>;

	GAFF_STRUCTORS_DEFAULT(EnumReflectionDefinition);
	GAFF_NO_COPY(EnumReflectionDefinition);
	GAFF_NO_MOVE(EnumReflectionDefinition);

	const IEnumReflection& getReflectionInstance(void) const override;

	void load(ISerializeReader& reader, void* object) const override;
	void save(ISerializeWriter& writer, const void* object) const override;

	int32_t getNumEntries(void) const override;
	const char* getEntryNameFromValue(int32_t value) const override;
	const char* getEntryNameFromIndex(int32_t index) const override;
	int32_t getEntryValue(int32_t index) const override;
	int32_t getEntryValue(const char* name) const override;
	int32_t getEntryValue(Hash32 name) const override;

	const char* getEntryName(Enum value) const;

	int32_t getNumEnumAttrs(void) const override;
	const IAttribute* getEnumAttr(int32_t index) const override;

	int32_t getNumEntryAttrs(Hash32 name) const override;
	const IAttribute* getEntryAttr(Hash32 name, int32_t index) const override;

	void setAllocator(const Allocator& allocator);
	Hash64 getVersion(void) const;

	template <size_t size, class... Attrs>
	EnumReflectionDefinition& entry(const char (&name)[size], Enum value, const Attrs&... attrs);

	template <class... Attrs>
	EnumReflectionDefinition& enumAttrs(const Attrs&... attrs);

	template <size_t size, class... Attrs>
	EnumReflectionDefinition& entryAttrs(const char (&name)[size], const Attrs&... attrs);

	void finish(void);

private:
	VectorMap<HashString32<Allocator>, Enum, Allocator> _entries;
	VectorMap<Hash32, Vector<IAttributePtr, Allocator>, Allocator> _entry_attrs;
	Vector<IAttributePtr, Allocator> _enum_attrs;

	Hash64 _version = INIT_HASH64;
	Allocator _allocator;

	template <class First, class... Rest>
	EnumReflectionDefinition& addAttributes(Enum value, Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);
	EnumReflectionDefinition& addAttributes(Enum, Vector<IAttributePtr, Allocator>&);

	template <class First, class... Rest>
	EnumReflectionDefinition& addAttributes(Vector<IAttributePtr, Allocator>& attrs, const First& first, const Rest&... rest);
	EnumReflectionDefinition& addAttributes(Vector<IAttributePtr, Allocator>&);
};

NS_END

#include "Gaff_EnumReflectionDefinition.inl"
