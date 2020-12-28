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

#include "Shibboleth_CommonHelpers.h"
#include <Shibboleth_ReflectionInterfaces.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_Vector.h>
#include <Shibboleth_String.h>
#include <janet.h>

NS_SHIBBOLETH

class JanetManager;

//struct TableState final
//{
//	~TableState(void);
//
//	Vector< eastl::pair<int32_t, Gaff::FunctionStackEntry> > array_entries{ ProxyAllocator("Lua") };
//	VectorMap<U8String, Gaff::FunctionStackEntry> key_values{ ProxyAllocator("Lua") };
//};

static constexpr Gaff::Hash32 k_janet_log_channel = Gaff::FNV1aHash32Const("Janet");

Janet PushUserTypeReference(const void* value, const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info);
Janet PushUserType(const void* value, const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info, UserData** out);
Janet PushUserType(const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info, UserData** out);
UserData* PushUserType(const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info);

void FillArgumentStack(int32_t num_args, Janet* args, Vector<Gaff::FunctionStackEntry>& stack, int32_t start = -1, int32_t end = -1);
void FillEntry(const Janet& arg, Gaff::FunctionStackEntry& entry, bool clone_non_janet);
Janet PushReturnValue(const Gaff::FunctionStackEntry& ret, bool create_user_data);

//void RestoreTable(lua_State* state, const TableState& table);
//void SaveTable(lua_State* state, TableState& table);

void RegisterEnum(JanetTable* env, const Gaff::IEnumReflectionDefinition& enum_ref_def);
void RegisterType(JanetTable* env, const Gaff::IReflectionDefinition& ref_def, JanetManager& janet_mgr);
void RegisterTypeFinalize(JanetTable* env, const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info);
void RegisterBuiltIns(JanetTable* env);

Janet UserTypeFunctionCall(void* data, int32_t num_args, Janet* args);
void UserTypeToString(void* data, JanetBuffer* buffer);
int UserTypeDestroy(void* data, size_t len);
//int UserTypeNewIndex(lua_State* state);
int UserTypeIndex(void* data, Janet key, Janet* out);
Janet UserTypeNew(int32_t num_args, Janet* args);

template <class T>
Janet PushUserTypeReference(const T& value, const JanetManager& janet_mgr)
{
	const JanetAbstractType* const type_info = janet_mgr.getType(*ret.ref_def);
	GAFF_ASSERT(type_info);

	return PushUserTypeReference(&value, Reflection<T>::GetReflectionDefinition(), type_info);
}

template <class T>
Janet PushUserTypeReference(const T& value)
{
	const JanetManager& janet_mgr = GetApp().getManagerTFast<JanetManager>();
	return PushUserTypeReference(value, janet_mgr);
}

template <class T>
Janet PushUserType(const T& value, const JanetManager& janet_mgr)
{
	const JanetAbstractType* const type_info = janet_mgr.getType(*ret.ref_def);
	GAFF_ASSERT(type_info);

	UserData* const user_data = reinterpret_cast<UserData*>(janet_abstract(type_info, sizeof(T) + k_alloc_size_no_reference));
	new(user_data) UserData::MetaData();
	new(user_data->getData()) T(value);

	const auto& ref_def = Reflection<T>::GetReflectionDefinition();
	user_data->ref_def = &ref_def;

	return janet_wrap_abstract(user_data);
}

template <class T>
Janet PushUserType(const T& value)
{
	JanetManager& janet_mgr = GetApp().getManagerTFast<JanetManager>();
	return PushUserType(value, janet_mgr);
}

NS_END
