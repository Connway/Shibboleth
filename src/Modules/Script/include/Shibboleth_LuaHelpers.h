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

#pragma once

#include <Shibboleth_IReflectionFunction.h>
#include <Shibboleth_IReflection.h>
#include <Shibboleth_Vector.h>
#include <Gaff_Hash.h>
#include <lua.hpp>

struct lua_State;

NS_SHIBBOLETH

struct TableState;
struct UserData;

UserData* PushUserTypeReference(lua_State* state, const void* value, const Refl::IReflectionDefinition& ref_def);
UserData* PushUserType(lua_State* state, const Refl::IReflectionDefinition& ref_def);

void FillArgumentStack(lua_State* state, Vector<Refl::FunctionStackEntry>& stack, int32_t start = -1, int32_t end = -1);
void FillEntry(lua_State* state, int32_t stack_index, Refl::FunctionStackEntry& entry, bool clone_non_lua);
int32_t PushReturnValue(lua_State* state, const Refl::FunctionStackEntry& ret, bool create_user_data);

void RestoreTable(lua_State* state, const TableState& table);
void SaveTable(lua_State* state, TableState& table);

void RegisterEnum(lua_State* state, const Refl::IEnumReflectionDefinition& enum_ref_def);
void RegisterType(lua_State* state, const Refl::IReflectionDefinition& ref_def);
void RegisterBuiltIns(lua_State* state);

int UserTypeFunctionCall(lua_State* state);
int UserTypeToString(lua_State* state);
int UserTypeDestroy(lua_State* state);
int UserTypeNewIndex(lua_State* state);
int UserTypeIndex(lua_State* state);
int UserTypeNew(lua_State* state);

template <class T>
UserData* PushUserTypeReference(lua_State* state, const T& value)
{
	return PushUserTypeReference(state, &value, Refl::Reflection<T>::GetReflectionDefinition());
}

template <class T>
UserData* PushUserType(lua_State* state, const T& value)
{
	UserData* const user_data = PushUserType(state, Refl::Reflection<T>::GetReflectionDefinition());
	new(user_data->getData()) T(value);

	return user_data;
}

NS_END
