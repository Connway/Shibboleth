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

#include "Shibboleth_LuaHelpers.h"
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Flags.h>

namespace
{
	constexpr int32_t k_ref_def_index = lua_upvalueindex(1);
	constexpr int32_t k_metatable_index = lua_upvalueindex(2);
	constexpr int32_t k_func_index_index = lua_upvalueindex(2);
	constexpr int32_t k_func_is_static_index = lua_upvalueindex(3);

	constexpr const char* const k_ref_def_field_name = "__ref_def";

	static Shibboleth::ProxyAllocator g_allocator("Lua");

	class LuaTypeInstanceAllocator final : public Gaff::IAllocator
	{
	public:
		LuaTypeInstanceAllocator(lua_State* state): _state(state) {}

		void* realloc(void* old_ptr, size_t new_size, size_t alignment, const char* file, int line);
		void* realloc(void* old_ptr, size_t new_size, const char* file, int line);

		// For EASTL support.
		void* allocate(size_t, size_t, size_t, int flags = 0)
		{
			GAFF_ASSERT(false);
			GAFF_REF(flags);
			return nullptr;
		}

		void* allocate(size_t, int flags = 0)
		{
			GAFF_ASSERT(false);
			GAFF_REF(flags);
			return nullptr;
		}

		void deallocate(void*, size_t)
		{
			// Should never happen.
			GAFF_ASSERT(false);
		}

		const char* get_name() const
		{
			// Should never happen.
			GAFF_ASSERT(false);
			return nullptr;
		}

		void set_name(const char*)
		{
			// Should never happen.
			GAFF_ASSERT(false);
		}

		void* alloc(size_t, size_t, const char*, int)
		{
			GAFF_ASSERT(false);
			return nullptr;
		}

		void* alloc(size_t size_bytes, const char*, int)
		{
			Shibboleth::UserData* const value = reinterpret_cast<Shibboleth::UserData*>(lua_newuserdata(_state, sizeof(Shibboleth::UserData::MetaData) + size_bytes));
			new(value) Shibboleth::UserData::MetaData();

			return &value->reference;
		}

		void free(void*)
		{
			// Should never happen.
			GAFF_ASSERT(false);
		}

	private:
		lua_State* _state = nullptr;
	};


	int Print(lua_State* state)
	{
		const int32_t num_args = lua_gettop(state);
		Shibboleth::U8String final_string;

		for (int32_t i = 0; i < num_args; ++i) {
			switch (lua_type(state, i + 1)) {
				case LUA_TNONE:
				case LUA_TNIL:
					final_string += "nil";
					break;

				case LUA_TBOOLEAN:
					final_string += (lua_toboolean(state, i + 1)) ? "true" : "false";
					break;

				case LUA_TNUMBER:
					if (lua_isinteger(state, i + 1)) {
						final_string.append_sprintf("%lli", luaL_checkinteger(state, i + 1));
					} else {
						final_string.append_sprintf("%f", luaL_checknumber(state, i + 1));
					}
					break;

				case LUA_TSTRING:
					final_string += luaL_checkstring(state, i + 1); 
					break;

				case LUA_TTABLE:
					// $TODO: Log error.
					break;

				case LUA_TFUNCTION:
					// $TODO: Log error.
					break;

				case LUA_TLIGHTUSERDATA:
				case LUA_TUSERDATA: {
					lua_getmetatable(state, i + 1);
					lua_getfield(state, -1, k_ref_def_field_name);

					const auto& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
					lua_pop(state, 2);

					const Shibboleth::UserData& user_data = *reinterpret_cast<Shibboleth::UserData*>(lua_touserdata(state, i + 1));
					const void* const object = user_data.getData();

					auto* const func = ref_def.getStaticFunc<int32_t, const void*, char*, int32_t>(Gaff::GetOpNameHash(Gaff::Operator::ToString));

					if (func) {
						char buffer[256];
						func->getFunc()(object, buffer, sizeof(buffer));

						final_string += buffer;

					} else {
						final_string.append_sprintf("%p", object);
					}

				} break;

				case LUA_TTHREAD:
					// $TODO: Log error.
					break;
			}
		}

		Shibboleth::GetApp().getLogManager().logMessage(Shibboleth::LogType::Normal, Shibboleth::k_lua_log_channel, final_string.data());
		Gaff::DebugPrintf(final_string.data());
		Gaff::DebugPrintf("\n");
		return 0;
	}

	int GetManager(lua_State* state)
	{
		if (!lua_istable(state, 1)) {
			// $TODO: Log error.
			return 0;
		}

		lua_getfield(state, 1, k_ref_def_field_name);

		if (!lua_isuserdata(state, -1)) {
			// $TODO: Log error.
			return 0;
		}

		const auto* const ref_def = reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
		Shibboleth::IManager* const manager = Shibboleth::GetApp().getManager(ref_def->getReflectionInstance().getHash());

		if (!manager) {
			// $TODO: Log error.
			return 0;
		}

		Shibboleth::PushUserTypeReference(state, manager->getBasePointer(), *ref_def);
		return 1;
	}

	void FreeDifferentType(Gaff::FunctionStackEntry& entry, const Gaff::IReflectionDefinition& new_ref_def, bool new_is_reference)
	{
		const bool is_reference = entry.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference);

		if (entry.ref_def && (entry.ref_def != &new_ref_def) || (is_reference != new_is_reference)) {
			if (!is_reference && !entry.ref_def->isBuiltIn()) {
				entry.ref_def->destroyInstance(entry.value.vp);
				SHIB_FREE(entry.value.vp, g_allocator);
			}

			entry.value.vp = nullptr;
			entry.ref_def = nullptr;
			entry.flags.clear();
		}
	}

	void CopyUserType(const Gaff::FunctionStackEntry& entry, void* dest)
	{
		Shibboleth::U8String ctor_sig(g_allocator);
		ctor_sig.append_sprintf("const %s&", entry.ref_def->getReflectionInstance().getName());

		const Shibboleth::HashStringTemp64<> hash(ctor_sig);

		auto ctor = entry.ref_def->getConstructor(hash.getHash());

		if (!ctor) {
			// $TODO: Log error.
			return;
		}

		// Deconstruct old value.
		entry.ref_def->destroyInstance(dest);

		// Construct new value.
		const auto cast_ctor = reinterpret_cast<void (*)(void*, const void*)>(ctor);
		cast_ctor(dest, entry.value.vp);
	}

	bool PushOrUpdateTableValue(lua_State* state, const Gaff::FunctionStackEntry& entry)
	{
		const int32_t type = lua_type(state, -1);

		// Built-in types, just push onto the stack.
		if (entry.enum_ref_def || (entry.ref_def && entry.ref_def->isBuiltIn())) {
			Shibboleth::PushReturnValue(state, entry, true);
			return true;
		}

		// Nil type.
		if (!entry.enum_ref_def && !entry.ref_def) {
			lua_pushnil(state);
			return true;
		}

		// Get the reflection definition of the Lua type.
		const Gaff::IReflectionDefinition* ref_def = nullptr;

		if (type == LUA_TUSERDATA) {
			lua_getmetatable(state, -1);
			lua_getfield(state, -1, k_ref_def_field_name);

			ref_def = reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
			lua_pop(state, 2);
		}

		// Not the same type, just push the value onto the stack.
		if (entry.ref_def != ref_def) {
			Shibboleth::PushReturnValue(state, entry, true);
			return true;
		}

		Shibboleth::UserData* const old_data = reinterpret_cast<Shibboleth::UserData*>(lua_touserdata(state, -1));

		// Just create and push the new value.
		CopyUserType(entry, old_data->getData());
		return false;
	}
}


NS_SHIBBOLETH

TableState::~TableState(void)
{
	for (auto& pair : array_entries) {
		if (pair.second.ref_def && !pair.second.ref_def->isBuiltIn() && !pair.second.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference)) {
			pair.second.ref_def->destroyInstance(pair.second.value.vp);
			SHIB_FREE(pair.second.value.vp, GetAllocator());
		}
	}

	for (auto& pair : key_values) {
		if (pair.second.ref_def && !pair.second.ref_def->isBuiltIn() && !pair.second.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference)) {
			pair.second.ref_def->destroyInstance(pair.second.value.vp);
			SHIB_FREE(pair.second.value.vp, GetAllocator());
		}
	}
}



void PushUserTypeReference(lua_State* state, const void* value, const Gaff::IReflectionDefinition& ref_def)
{
	UserData* const user_data = reinterpret_cast<UserData*>(lua_newuserdata(state, sizeof(UserData)));

	new(user_data) UserData();
	user_data->meta.flags.set(true, UserData::MetaData::HeaderFlag::IsReference);
	user_data->reference = const_cast<void*>(value);

	luaL_getmetatable(state, ref_def.getFriendlyName());
	lua_setmetatable(state, -2);
}

void FillArgumentStack(lua_State* state, Vector<Gaff::FunctionStackEntry>& stack, int32_t start, int32_t end)
{
	start = Gaff::Max(start, 1);
	end = (end < 1) ? lua_gettop(state) : Gaff::Min(end, lua_gettop(state));

	stack.resize(static_cast<size_t>(end - start + 1));

	for (int32_t i = start - 1; i < end; ++i) {
		FillEntry(state, i + 1, stack[i - start + 1], false);
	}
}

void FillEntry(lua_State* state, int32_t stack_index, Gaff::FunctionStackEntry& entry, bool clone_non_lua)
{
	const int32_t type = lua_type(state, stack_index);

	switch (type) {
		case LUA_TNONE:
		case LUA_TNIL:
			entry.value.vp = nullptr;
			break;

		case LUA_TBOOLEAN:
			if (clone_non_lua) {
				FreeDifferentType(entry, Reflection<bool>::GetReflectionDefinition(), false);
			}

			entry.ref_def = &Reflection<bool>::GetReflectionDefinition();
			entry.value.b = lua_toboolean(state, stack_index);
			break;

		case LUA_TNUMBER:
			if (lua_isinteger(state, stack_index)) {
				if (clone_non_lua) {
					FreeDifferentType(entry, Reflection<int64_t>::GetReflectionDefinition(), false);
				}

				entry.ref_def = &Reflection<int64_t>::GetReflectionDefinition();
				entry.value.i64 = luaL_checkinteger(state, stack_index);

			} else {
				if (clone_non_lua) {
					FreeDifferentType(entry, Reflection<double>::GetReflectionDefinition(), false);
				}

				entry.ref_def = &Reflection<double>::GetReflectionDefinition();
				entry.value.d = luaL_checknumber(state, stack_index);
			}
			break;

		case LUA_TSTRING:
			if (clone_non_lua) {
				FreeDifferentType(entry, Reflection<U8String>::GetReflectionDefinition(), false);

				if (!entry.value.vp) {
					entry.value.vp = SHIB_ALLOCT(U8String, g_allocator);
				}

				*reinterpret_cast<U8String*>(entry.value.vp) = luaL_checkstring(state, stack_index);
				entry.ref_def = &Reflection<U8String>::GetReflectionDefinition();

			} else {
				entry.flags.set(true, Gaff::FunctionStackEntry::Flag::IsString);
				entry.value.vp = const_cast<char*>(luaL_checkstring(state, stack_index));
			}

			break;

		case LUA_TTABLE:
			// Fill vector or vectormap.
			// $TODO: Log error.
			break;

		case LUA_TFUNCTION:
			// $TODO: Log error.
			break;

		case LUA_TLIGHTUSERDATA:
		case LUA_TUSERDATA: {
			lua_getmetatable(state, stack_index);
			lua_getfield(state, -1, k_ref_def_field_name);

			const Gaff::IReflectionDefinition* const ref_def = reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
			lua_pop(state, 2);

			UserData* const user_data = reinterpret_cast<UserData*>(lua_touserdata(state, stack_index));

			if (clone_non_lua) {
				const bool other_is_reference = user_data->meta.flags.testAll(UserData::MetaData::HeaderFlag::IsReference);
				const bool is_reference = entry.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference);

				FreeDifferentType(entry, *ref_def, other_is_reference);

				// If it's just a reference, don't worry about creating a copy of the data.
				if (other_is_reference) {
					entry.flags.set(true, Gaff::FunctionStackEntry::Flag::IsReference);
					entry.value.vp = user_data->getData();
					entry.ref_def = ref_def;

				// Is not a reference, we should clone the data.
				} else {
					U8String ctor_sig(g_allocator);
					ctor_sig.append_sprintf("const %s&", ref_def->getReflectionInstance().getName());

					const HashStringTemp64<> hash(ctor_sig);

					// If we already have a non-reference value, just re-construct in place.
					if (entry.value.vp) {
						auto ctor = entry.ref_def->getConstructor(hash.getHash());

						if (!ctor) {
							// $TODO: Log error.
							break;
						}

						// Deconstruct old value.
						entry.ref_def->destroyInstance(entry.value.vp);

						// Construct new value.
						const auto cast_ctor = reinterpret_cast<void (*)(void*, const void*)>(ctor);
						cast_ctor(entry.value.vp, user_data->getData());

					// Need to make a copy.
					} else {
						auto factory = ref_def->getFactory(hash.getHash());

						if (!factory) {
							// $TODO: Log error.
							break;
						}

						const auto cast_factory = reinterpret_cast<void* (*)(Gaff::IAllocator&, const void*)>(factory);
						entry.value.vp = cast_factory(g_allocator, user_data->getData());
					}

					entry.ref_def = ref_def;
				}

			} else {
				// Unnecessary, but for posterity.
				entry.flags.set(
					user_data->meta.flags.testAll(UserData::MetaData::HeaderFlag::IsReference),
					Gaff::FunctionStackEntry::Flag::IsReference
				);

				entry.value.vp = user_data->getData();
				entry.ref_def = ref_def;
			}

		} break;

		case LUA_TTHREAD:
			// $TODO: Log error.
			break;
	}
}

int32_t PushReturnValue(lua_State* state, const Gaff::FunctionStackEntry& ret, bool create_user_data)
{
	if (ret.enum_ref_def) {
		if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsArray)) {
			GAFF_ASSERT_MSG(ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference), "Do not support returning arrays by value.");

			const int8_t* begin = reinterpret_cast<int8_t*>(ret.value.arr.data);
			const int32_t data_size = ret.enum_ref_def->size();
			lua_createtable(state, ret.value.arr.size, 0);

			for (int32_t i = 0; i < ret.value.arr.size; ++i) {
				int64_t value = 0;

				if (data_size <= sizeof(int8_t)) {
					value = static_cast<int64_t>(*begin);
				} else if (data_size <= sizeof(int16_t)) {
					value = static_cast<int64_t>(*reinterpret_cast<const int16_t*>(begin));
				} else if (data_size <= sizeof(int32_t)) {
					value = static_cast<int64_t>(*reinterpret_cast<const int32_t*>(begin));
				} else if (data_size <= sizeof(int64_t)) {
					value = *reinterpret_cast<const int64_t*>(begin);
				} else {
					GAFF_ASSERT_MSG(false, "Enum is larger than 64-bits.");
				}

				lua_pushinteger(state, value);
				lua_seti(state, -1, i);
				begin += data_size;
			}

			//if (!ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference)) {
			//	SHIB_FREE(ret.value.vp, g_allocator);
			//}

		} else if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsMap)) {
			// $TODO: impl

		} else {
			lua_pushinteger(state, ret.value.i64);
		}

		return 1;

	// Is a string.
	} else if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsString)) {
		lua_pushstring(state, reinterpret_cast<char*>(ret.value.vp));

		if (!ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference)) {
			SHIB_FREET(ret.value.vp, GetAllocator());
		}

		return 1;

	} else if (ret.ref_def) {
		if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsArray)) {
			GAFF_ASSERT_MSG(ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference), "Do not support returning arrays by value.");

			const int8_t* begin = reinterpret_cast<int8_t*>(ret.value.arr.data);
			const int32_t data_size = ret.ref_def->size();
			lua_createtable(state, ret.value.arr.size, 0);

			for (int32_t i = 0; i < ret.value.arr.size; ++i) {
				if (ret.ref_def->isBuiltIn()) {
					if (lua_Number num_value; Gaff::CastFloatToType<lua_Number>(ret, num_value)) {
						lua_pushnumber(state, num_value);

					} else if (lua_Integer int_value; Gaff::CastIntegerToType<lua_Integer>(ret, int_value)) {
						lua_pushinteger(state, int_value);

					// Value is a boolean.
					} else {
						lua_pushboolean(state, ret.value.b);
					}

				} else {
					UserData* const value = reinterpret_cast<UserData*>(lua_newuserdata(state, sizeof(UserData)));

					new(value) UserData();
					value->meta.flags.set(true, UserData::MetaData::HeaderFlag::IsReference);
					value->reference = const_cast<int8_t*>(begin);

					luaL_getmetatable(state, ret.ref_def->getFriendlyName());
					lua_setmetatable(state, -2);
				}

				lua_seti(state, -1, i);
				begin += data_size;
			}

		} else if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsMap)) {
			// $TODO: impl

		} else if (ret.ref_def->isBuiltIn()) {
			if (lua_Number num_value; Gaff::CastFloatToType<lua_Number>(ret, num_value)) {
				lua_pushnumber(state, num_value);

			} else if (lua_Integer int_value; Gaff::CastIntegerToType<lua_Integer>(ret, int_value)) {
				lua_pushinteger(state, int_value);

			// Value is a boolean.
			} else {
				lua_pushboolean(state, ret.value.b);
			}

		// Is a user defined type.
		} else {
			if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference)) {
				UserData* const value = reinterpret_cast<UserData*>(lua_newuserdata(state, sizeof(UserData)));

				new(value) UserData();
				value->meta.flags.set(true, UserData::MetaData::HeaderFlag::IsReference);
				value->reference = ret.value.vp;

			} else if (create_user_data) {
				UserData* const value = reinterpret_cast<UserData*>(lua_newuserdata(state, k_alloc_size_no_reference + ret.ref_def->size()));
				new(value) UserData::MetaData();

				CopyUserType(ret, value->getData());
			}
			// Else allocator already pushed it onto the stack.

			luaL_getmetatable(state, ret.ref_def->getFriendlyName());
			lua_setmetatable(state, -2);
		}

		return 1;

	} else {
		return 0;
	}
}

void RestoreTable(lua_State* state, const TableState& table)
{
	for (const auto& pair : table.array_entries) {
		lua_geti(state, -1, pair.first);
		
		if (PushOrUpdateTableValue(state, pair.second)) {
			lua_remove(state, -2);
			lua_seti(state, -2, pair.first);
		} else {
			lua_pop(state, 1);
		}
	}

	for (const auto& pair : table.key_values) {
		lua_getfield(state, -1, pair.first.data());

		if (PushOrUpdateTableValue(state, pair.second)) {
			lua_remove(state, -2);
			lua_setfield(state, -2, pair.first.data());
		} else {
			lua_pop(state, 1);
		}
	}
}

void SaveTable(lua_State* state, TableState& table)
{
	lua_pushnil(state);

	while (lua_next(state, -2) != 0) {
		const int32_t type = lua_type(state, -1);

		// We don't save threads or functions.
		if (type == LUA_TTHREAD || type == LUA_TFUNCTION) {
			lua_pop(state, 1);
			continue;
		}

		// Key is a string.
		if (lua_type(state, -2) == LUA_TSTRING) {
			size_t len = 0;
			const char* const string = lua_tolstring(state, -2, &len);
			U8String key = U8String(string, len, g_allocator);

			FillEntry(state, -1, table.key_values[std::move(key)], true);

		// Key is an integer index.
		} else {
			const lua_Integer index = lua_tointeger(state, -2);
			auto& pair = table.array_entries.emplace_back();
			pair.first = static_cast<int32_t>(index);

			FillEntry(state, -1, pair.second, true);
		}

		lua_pop(state, 1);
	}
}

void RegisterEnum(lua_State* state, const Gaff::IEnumReflectionDefinition& enum_ref_def)
{
	const U8String name = enum_ref_def.getReflectionInstance().getName();

	size_t prev_index = 0;
	size_t curr_index = name.find_first_of(':');

	int32_t table_count = 0;

	// Create all sub-tables.
	do {
		const U8String substr = name.substr(prev_index, curr_index - prev_index);

		// Create first, global table.
		if (table_count == 0) {
			if (lua_getglobal(state, substr.data()) <= 0) {
				lua_pop(state, 1);

				lua_createtable(state, 0, 0);
				lua_pushvalue(state, -1);
				lua_setglobal(state, substr.data());
			}

		// Create sub-table.
		} else {
			if (lua_getfield(state, -1, substr.data()) <= 0) {
				lua_pop(state, 1);

				lua_createtable(state, 0, 0);
				lua_pushvalue(state, -1);
				lua_setfield(state, -3, substr.data());
			}
		}

		prev_index = (curr_index != SIZE_T_FAIL) ? curr_index + 2 : SIZE_T_FAIL;
		curr_index = name.find_first_of(':', prev_index);

		++table_count;
	} while (prev_index != SIZE_T_FAIL);

	// Add all the enum entries.
	const int32_t num_entries = enum_ref_def.getNumEntries();

	for (int32_t i = 0; i < num_entries; ++i) {
		const char* const entry_name = enum_ref_def.getEntryNameFromIndex(i);
		const int32_t entry_value = enum_ref_def.getEntryValue(i);

		lua_pushinteger(state, entry_value);
		lua_setfield(state, -2, entry_name);

		// For reverse lookup.
		lua_pushstring(state, entry_name);
		lua_seti(state, -2, entry_value);
	}

	lua_pop(state, table_count);
}

void RegisterType(lua_State* state, const Gaff::IReflectionDefinition& ref_def)
{
	// We do not need to register attributes.
	if (ref_def.hasInterface(CLASS_HASH(Gaff::IAttribute))) {
		return;
	}

	const char* const friendly_name = ref_def.getFriendlyName();
	const auto* const flags = ref_def.getClassAttr<ScriptFlagsAttribute>();

	if (flags && flags->getFlags().testAll(ScriptFlagsAttribute::Flag::NoRegister)) {
		return;
	}

	// Type Metatable
	luaL_newmetatable(state, friendly_name);

	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
	lua_setfield(state, -2, k_ref_def_field_name);

	// Register operators.
	const int32_t num_static_funcs = ref_def.getNumStaticFuncs();
	int32_t num_operators = 0;

	Vector<luaL_Reg> mt(g_allocator);

	for (int32_t i = 0; i < num_static_funcs; ++i) {
		const char* const name = ref_def.getStaticFuncName(i);

		// Is not an operator function.
		if (Gaff::FindFirstOf(name, "__") != 0) {
			continue;
		}

		// Is the tostring function.
		if (Gaff::FindFirstOf(name, OP_TO_STRING_NAME) == 0) {
			mt.emplace_back(luaL_Reg{ "__tostring", UserTypeToString });
			continue;
		}

		lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
		lua_pushinteger(state, i);
		lua_pushboolean(state, true); // Is static.

		lua_pushcclosure(state, UserTypeFunctionCall, 3);
		lua_setfield(state, -2, name);

		++num_operators;
	}

	mt.emplace_back(luaL_Reg{ "__newindex", UserTypeNewIndex });
	mt.emplace_back(luaL_Reg{ "__index", UserTypeIndex });
	mt.emplace_back(luaL_Reg{ "__gc", UserTypeDestroy });
	mt.emplace_back(luaL_Reg{ nullptr, nullptr });

	// Register funcs with up values.
	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
	luaL_setfuncs(state, mt.data(), 1);

	lua_pop(state, 1);


	// Library Table.
	Vector<luaL_Reg> reg(g_allocator);

	// Add constructor.
	if ((!flags || !flags->getFlags().testAll(ScriptFlagsAttribute::Flag::ReferenceOnly))) {
		reg.emplace_back(luaL_Reg{ "new", UserTypeNew });
	}

	reg.emplace_back(luaL_Reg{ nullptr, nullptr });

	lua_createtable(state, 0, static_cast<int32_t>(reg.size()) + num_static_funcs - num_operators);

	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
	lua_setfield(state, -2, k_ref_def_field_name);

	// Add static funcs.
	for (int32_t i = 0; i < num_static_funcs; ++i) {
		const char* const name = ref_def.getStaticFuncName(i);

		// Is an operator function.
		if (!strncmp(name, "__", 2)) {
			continue;
		}

		lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
		lua_pushinteger(state, i);
		lua_pushboolean(state, true); // Is static.

		lua_pushcclosure(state, UserTypeFunctionCall, 3);
		lua_setfield(state, -2, name);
	}

	// Push up values.
	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
	luaL_getmetatable(state, friendly_name);

	luaL_setfuncs(state, reg.data(), 2);
	lua_setglobal(state, friendly_name);
}

void RegisterBuiltIns(lua_State* state)
{
	lua_pushcclosure(state, Print, 0);
	lua_setglobal(state, "print");

	lua_pushcclosure(state, GetManager, 0);
	lua_setglobal(state, "GetManager");
}

int UserTypeFunctionCall(lua_State* state)
{
	const Gaff::IReflectionDefinition& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, k_ref_def_index));
	const int32_t func_index = static_cast<int32_t>(luaL_checkinteger(state, k_func_index_index));
	const bool is_static = lua_toboolean(state, k_func_is_static_index);

	const int32_t num_overrides = (is_static) ? ref_def.getNumStaticFuncOverrides(func_index) : ref_def.getNumFuncOverrides(func_index);
	const int32_t num_args = (is_static) ? lua_gettop(state) : lua_gettop(state) - 1;

	Vector<Gaff::FunctionStackEntry> args(g_allocator);
	LuaTypeInstanceAllocator allocator(state);
	Gaff::FunctionStackEntry ret;

	for (int32_t i = 0; i < num_overrides; ++i) {
		const Gaff::IReflectionStaticFunctionBase* const static_func = (is_static) ? ref_def.getStaticFunc(func_index, i) : nullptr;
		const Gaff::IReflectionFunctionBase* const func = (is_static) ? nullptr : ref_def.getFunc(func_index, i);
		const int32_t func_args = (is_static) ? static_func->numArgs() : func->numArgs();

		if (func_args == num_args) {
			if (num_args > 0 && args.empty()) {
				FillArgumentStack(state, args, (is_static) ? 1 : 2, (is_static) ? num_args : (num_args + 1));
			}

			if (is_static) {
				if (!static_func->call(args.data(), static_cast<int32_t>(args.size()), ret, allocator)) {
					continue;
				}
			} else {
				// First element on the stack is our object instance.
				UserData* const object = reinterpret_cast<UserData*>(luaL_checkudata(state, 1, ref_def.getFriendlyName()));

				if (!func->call(object->getData(), args.data(), static_cast<int32_t>(args.size()), ret, allocator)) {
					continue;
				}
			}

			return PushReturnValue(state, ret, false);
		}
	}

	// $TODO: Log error. Can't find function with the correct number of arguments or argument type mismatch.
	return 0;
}

int UserTypeToString(lua_State* state)
{
	const auto& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, k_ref_def_index));
	const UserData& user_data = *reinterpret_cast<UserData*>(lua_touserdata(state, -1));

	auto* const func = ref_def.getStaticFunc<int32_t, const void*, char*, int32_t>(Gaff::GetOpNameHash(Gaff::Operator::ToString));

	if (func) {
		char buffer[256];
		const int32_t size = func->getFunc()(user_data.getData(), buffer, sizeof(buffer));

		lua_pushlstring(state, buffer, static_cast<size_t>(size));
		return 1;
	}

	// $TODO: Log error. Can't find function with the correct number of arguments or argument type mismatch.
	return 0;
}

int UserTypeDestroy(lua_State* state)
{
	const Gaff::IReflectionDefinition& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, k_ref_def_index));
	UserData* const user_data = reinterpret_cast<UserData*>(luaL_checkudata(state, 1, ref_def.getFriendlyName()));

	if (!user_data->meta.flags.testAll(UserData::MetaData::HeaderFlag::IsReference)) {
		ref_def.destroyInstance(user_data->getData());
	}

	return 0;
}

int UserTypeNewIndex(lua_State* state)
{
	const Gaff::IReflectionDefinition& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, k_ref_def_index));
	UserData* const user_data = reinterpret_cast<UserData*>(luaL_checkudata(state, 1, ref_def.getFriendlyName()));
	void* input = user_data->getData();

	if (lua_type(state, 2) == LUA_TSTRING) {
		size_t len = 0;
		const char* const name = luaL_checklstring(state, 2, &len);
		const Gaff::Hash32 hash = Gaff::FNV1aHash32String(name);

		// Find a variable with name.
		if (auto* const var = ref_def.getVar(hash)) {
			if (var->isFixedArray() || var->isVector()) {
				// $TODO: Add support for arrays.
				GAFF_ASSERT_MSG(false, "Currently do not support array variables.");

			} else if (var->isMap()) {
				// $TODO: Add support for maps.
				GAFF_ASSERT_MSG(false, "Currently do not support map variables.");

			} else {
				const Gaff::IReflection& var_refl = var->getReflection();
				const Gaff::IReflectionDefinition& var_ref_def = var_refl.getReflectionDefinition();

				if (var_ref_def.isBuiltIn()) {
					if (&var_ref_def == &Reflection<double>::GetReflectionDefinition()) {
						if (lua_isnumber(state, 3)) {
							const double value = lua_tonumber(state, 3);
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<float>::GetReflectionDefinition()) {
						if (lua_isnumber(state, 3)) {
							const float value = static_cast<float>(lua_tonumber(state, 3));
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<int64_t>::GetReflectionDefinition()) {
						if (lua_isinteger(state, 3)) {
							const int64_t value = lua_tointeger(state, 3);
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<int32_t>::GetReflectionDefinition()) {
						if (lua_isinteger(state, 3)) {
							const int32_t value = static_cast<int32_t>(lua_tointeger(state, 3));
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<int16_t>::GetReflectionDefinition()) {
						if (lua_isinteger(state, 3)) {
							const int16_t value = static_cast<int16_t>(lua_tointeger(state, 3));
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<int8_t>::GetReflectionDefinition()) {
						if (lua_isinteger(state, 3)) {
							const int8_t value = static_cast<int8_t>(lua_tointeger(state, 3));
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<uint64_t>::GetReflectionDefinition()) {
						if (lua_isinteger(state, 3)) {
							const uint64_t value = static_cast<uint64_t>(lua_tointeger(state, 3));
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<uint32_t>::GetReflectionDefinition()) {
						if (lua_isinteger(state, 3)) {
							const uint32_t value = static_cast<uint32_t>(lua_tointeger(state, 3));
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<uint16_t>::GetReflectionDefinition()) {
						if (lua_isinteger(state, 3)) {
							const uint16_t value = static_cast<uint16_t>(lua_tointeger(state, 3));
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<uint8_t>::GetReflectionDefinition()) {
						if (lua_isinteger(state, 3)) {
							const uint8_t value = static_cast<uint8_t>(lua_tointeger(state, 3));
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<bool>::GetReflectionDefinition()) {
						if (lua_isboolean(state, 3)) {
							const bool value = lua_toboolean(state, 3);
							var->setDataT(input, value);

						} else {
							// $TODO: Log error.
						}
					}

				// Is a user defined type.
				} else {
					if (&ref_def == &var_ref_def) {
						const UserData* const value = reinterpret_cast<UserData*>(luaL_checkudata(state, 3, ref_def.getFriendlyName()));
						var->setData(input, value->getData());

					} else {
						// $TODO: Log error.
					}
				}
			}
		}

	// Index function?
	} else {
	}

	return 0;
}

int UserTypeIndex(lua_State* state)
{
	const Gaff::IReflectionDefinition& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, k_ref_def_index));
	UserData* const user_data = reinterpret_cast<UserData*>(luaL_checkudata(state, 1, ref_def.getFriendlyName()));
	const void* input = user_data->getData();

	if (lua_type(state, 2) == LUA_TSTRING) {
		size_t len = 0;
		const char* const name = luaL_checklstring(state, 2, &len);
		const Gaff::Hash32 hash = Gaff::FNV1aHash32String(name);

		// Find a variable with name.
		if (const auto* const var = ref_def.getVar(hash)) {
			if (var->isFixedArray() || var->isVector()) {
				// $TODO: Add support for arrays.
				GAFF_ASSERT_MSG(false, "Currently do not support array variables.");
			} else if (var->isMap()) {
				// $TODO: Add support for maps.
				GAFF_ASSERT_MSG(false, "Currently do not support map variables.");

			} else {
				const Gaff::IReflection& var_refl = var->getReflection();
				const Gaff::IReflectionDefinition& var_ref_def = var_refl.getReflectionDefinition();

				input = var->getData(input);

				// Push floating point number to stack.
				if (lua_Number value_num; Gaff::CastFloatToType<lua_Number>(var_ref_def, input, value_num)) {
					lua_pushnumber(state, value_num);
					return 1;

				// Push integer to stack.
				} else if (lua_Integer value_int; Gaff::CastIntegerToType<lua_Integer>(var_ref_def, input, value_int)) {
					lua_pushinteger(state, value_int);
					return 1;

				// Push bool to stack.
				} else if (&ref_def == &Reflection<bool>::GetReflectionDefinition()) {
					lua_pushboolean(state, *reinterpret_cast<const bool*>(input));
					return 1;

				// Push string to stack.
				} else if (&ref_def == &Reflection<U8String>::GetReflectionDefinition()) {
					const U8String& string = *reinterpret_cast<const U8String*>(input);
					lua_pushlstring(state, string.data(), string.size());
					return 1;

				// Push user defined type reference.
				} else {
					UserData* const value = reinterpret_cast<UserData*>(lua_newuserdata(state, sizeof(UserData)));

					new(value) UserData();
					value->meta.flags.set(true, UserData::MetaData::HeaderFlag::IsReference);
					value->reference = const_cast<void*>(input);

					//if (auto* const root = user_data->meta.getRoot()) {
					//	value->meta.root = root;
					//}

					luaL_getmetatable(state, var_ref_def.getFriendlyName());
					lua_setmetatable(state, -2);
				}
			}

		// Find function with name.
		} else if (int32_t func_index = ref_def.getFuncIndex(hash); func_index > -1) {
			lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
			lua_pushinteger(state, func_index);
			lua_pushboolean(state, false); // Is not static.
			lua_pushcclosure(state, UserTypeFunctionCall, 3);

			return 1;

		// Last resort, use the type's registered index function.
		} else if (func_index = ref_def.getStaticFuncIndex(Gaff::GetOpNameHash(Gaff::Operator::Index)); func_index > -1) {
			const int32_t num_overloads = ref_def.getNumStaticFuncOverrides(func_index);
			const int32_t num_args = lua_gettop(state);

			Vector<Gaff::FunctionStackEntry> args(g_allocator);
			LuaTypeInstanceAllocator allocator(state);
			Gaff::FunctionStackEntry ret;

			for (int32_t i = 0; i < num_overloads; ++i) {
				const Gaff::IReflectionStaticFunctionBase* const static_func = ref_def.getStaticFunc(func_index, i);

				if (static_func->numArgs() == num_args) {
					if (num_args > 0 && args.empty()) {
						FillArgumentStack(state, args);
					}

					if (!static_func->call(args.data(), static_cast<int32_t>(args.size()), ret, allocator)) {
						continue;
					}

					return PushReturnValue(state, ret, false);
				}
			}

			// $TODO: Log error. Can't find index function with the correct number of arguments or argument type mismatch.
		}

		// $TODO: Log error. Can't find anything at index.

	// Non-string type.
	} else {
		// Use the type's registered index function.
		if (const int32_t func_index = ref_def.getStaticFuncIndex(Gaff::GetOpNameHash(Gaff::Operator::Index)); func_index > -1) {
			const int32_t num_overloads = ref_def.getNumStaticFuncOverrides(func_index);
			const int32_t num_args = lua_gettop(state);

			Vector<Gaff::FunctionStackEntry> args(g_allocator);
			LuaTypeInstanceAllocator allocator(state);
			Gaff::FunctionStackEntry ret;

			for (int32_t i = 0; i < num_overloads; ++i) {
				const Gaff::IReflectionStaticFunctionBase* const static_func = ref_def.getStaticFunc(func_index, i);

				if (static_func->numArgs() == num_args) {
					if (num_args > 0 && args.empty()) {
						FillArgumentStack(state, args);
					}

					if (!static_func->call(args.data(), static_cast<int32_t>(args.size()), ret, allocator)) {
						continue;
					}

					return PushReturnValue(state, ret, false);
				}
			}

			// $TODO: Log error. Can't find index function with the correct number of arguments or argument type mismatch.
		}
	}

	return 0;
}

int UserTypeNew(lua_State* state)
{
	const Gaff::IReflectionDefinition& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, k_ref_def_index));

	Vector<Gaff::FunctionStackEntry> arg_stack(g_allocator);
	Gaff::FunctionStackEntry ret;

	FillArgumentStack(state, arg_stack);

	const size_t var_size = static_cast<size_t>(ref_def.size());
	UserData* const value = reinterpret_cast<UserData*>(lua_newuserdata(state, var_size + k_alloc_size_no_reference));

	// Initialize metadata at the beginning.
	new(value) UserData::MetaData();
	//value->root = value;

	Gaff::FunctionStackEntry obj;
	obj.value.vp = value->getData();
	obj.ref_def = &ref_def;

	arg_stack.insert(arg_stack.begin(), obj);

	// Initialize our data.
	const int32_t num_ctors = ref_def.getNumConstructors();
	const int32_t num_args = static_cast<int32_t>(arg_stack.size());

	for (int32_t i = 0; i < num_ctors; ++i) {
		auto* const ctor = ref_def.getConstructor(i);

		if (ctor->numArgs() == num_args) {
			if (ctor->call(arg_stack.data(), num_args, ret, g_allocator)) {
				break;
			}
		}
	}

	lua_pushvalue(state, k_metatable_index);
	lua_setmetatable(state, -2);

	return 1;
}

NS_END
