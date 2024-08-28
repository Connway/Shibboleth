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

#include "Shibboleth_ReflectionDefines.h"
#include "Shibboleth_HashString.h"
#include <Gaff_Hash.h>

NS_SHIBBOLETH
	class ISerializeReader;
	class ISerializeWriter;
NS_END

NS_REFLECTION

class IReflection;
class IAttribute;

class IEnumReflectionDefinition
{
public:
	enum class LoadFlags
	{
		Count
	};

	enum class SaveFlags
	{
		Count
	};

	virtual ~IEnumReflectionDefinition(void) {}

	virtual const IReflection& getReflectionInstance(void) const = 0;
	virtual int32_t size(void) const = 0;

	virtual Gaff::Hash64 getInstanceHash(const void* object, Gaff::Hash64 init = Gaff::k_init_hash64) const = 0;

	virtual bool load(const Shibboleth::ISerializeReader& reader, void* object, Gaff::Flags<LoadFlags> flags = Gaff::Flags<LoadFlags>{}) const = 0;
	virtual void save(Shibboleth::ISerializeWriter& writer, const void* object, Gaff::Flags<SaveFlags> flags = Gaff::Flags<SaveFlags>{}) const = 0;

	virtual int32_t getNumEntries(void) const = 0;
	virtual Shibboleth::HashStringView32<> getEntryNameFromValue(int32_t value) const = 0;
	virtual Shibboleth::HashStringView32<> getEntryNameFromIndex(int32_t index) const = 0;
	virtual int32_t getEntryValue(int32_t index) const = 0;
	virtual int32_t getEntryValue(const char8_t* name) const = 0;
	virtual int32_t getEntryValue(Gaff::Hash32 name) const = 0;

	virtual int32_t getNumEnumAttrs(void) const = 0;
	virtual const IAttribute* getEnumAttr(int32_t index) const = 0;
};

NS_END
