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

#pragma once

#include <Shibboleth_ReflectionInterfaces.h>
#include <Shibboleth_ProxyAllocator.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_Vector.h>
#include <Shibboleth_String.h>
#include <Gaff_Flags.h>

NS_GAFF
	class IReflectionDefinition;
	struct FunctionStackEntry;
NS_END

NS_SHIBBOLETH

struct UserData final
{
	struct MetaData final
	{
		enum class HeaderFlag
		{
			IsReference,

			Count
		};

		Gaff::Flags<HeaderFlag> flags;
		//UserDataMetadata* root = nullptr;

		//UserDataMetadata* getRoot(void)
		//{
		//	return (flags.testAll(HeaderFlag::IsReference)) ? root : nullptr;
		//}
	};

	MetaData meta;
	const Gaff::IReflectionDefinition* ref_def = nullptr;
	// Only valid if the IsReference flag in metadata is set.
	void* reference;

	const void* getData(void) const
	{
		return const_cast<UserData*>(this)->getData();
	}

	void* getData(void)
	{
		// If it's a reference, read the pointer stored in the user data.
		// If it's not a reference, the memory after the metadata is our object, so take a pointer to it.
		return (meta.flags.testAll(MetaData::HeaderFlag::IsReference)) ?
			reference :
			&reference;
	}
};

struct TableState final
{
	~TableState(void);

	Vector< eastl::pair<int32_t, Gaff::FunctionStackEntry> > array_entries{ ProxyAllocator("Script") };
	VectorMap<U8String, Gaff::FunctionStackEntry> key_values{ ProxyAllocator("Script") };
};


constexpr size_t k_alloc_size_no_reference = sizeof(UserData) - sizeof(void*);

void FreeDifferentType(Gaff::FunctionStackEntry& entry, const Gaff::IReflectionDefinition& new_ref_def, bool new_is_reference);
void CopyUserType(const Gaff::IReflectionDefinition& ref_def, const void* value, void* dest, bool old_value_is_valid, ProxyAllocator allocator);
void CopyUserType(const Gaff::FunctionStackEntry& entry, void* dest, bool old_value_is_valid, ProxyAllocator allocator);

NS_END
