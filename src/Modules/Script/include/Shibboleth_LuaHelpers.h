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
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_Vector.h>
#include <Shibboleth_String.h>
#include <lua.hpp>

struct lua_State;

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

	Vector< eastl::pair<int32_t, Gaff::FunctionStackEntry> > array_entries{ ProxyAllocator("Lua") };
	VectorMap<U8String, Gaff::FunctionStackEntry> key_values{ ProxyAllocator("Lua") };
};

constexpr size_t k_alloc_size_no_reference = sizeof(UserData) - sizeof(void*);

static constexpr Gaff::Hash32 k_lua_log_channel = Gaff::FNV1aHash32Const("Lua");

void PushUserTypeReference(lua_State* state, const void* value, const Gaff::IReflectionDefinition& ref_def);

void FillArgumentStack(lua_State* state, Vector<Gaff::FunctionStackEntry>& stack, int32_t start = -1, int32_t end = -1);
void FillEntry(lua_State* state, int32_t stack_index, Gaff::FunctionStackEntry& entry, bool clone_non_lua);
int32_t PushReturnValue(lua_State* state, const Gaff::FunctionStackEntry& ret, bool create_user_data);

void RestoreTable(lua_State* state, const TableState& table);
void SaveTable(lua_State* state, TableState& table);

void RegisterType(lua_State* state, const Gaff::IReflectionDefinition& ref_def);
void RegisterBuiltIns(lua_State* state);

int UserTypeFunctionCall(lua_State* state);
int UserTypeToString(lua_State* state);
int UserTypeDestroy(lua_State* state);
int UserTypeNewIndex(lua_State* state);
int UserTypeIndex(lua_State* state);
int UserTypeNew(lua_State* state);

template <class T>
void PushUserTypeReference(lua_State* state, const T& value)
{
	PushUserTypeReference(state, &value, Reflection<T>::GetReflectionDefinition());
}

template <class T>
void PushUserType(lua_State* state, const T& value)
{
	UserData* const user_data = reinterpret_cast<UserData*>(lua_newuserdata(state, k_alloc_size_no_reference + sizeof(T)));
	new(user_data) UserData::MetaData();
	new(&user_data->reference) T(value);

	const auto& ref_def = Reflection<T>::GetReflectionDefinition();
	luaL_getmetatable(state, ref_def.getFriendlyName());
	lua_setmetatable(state, -2);
}

NS_END