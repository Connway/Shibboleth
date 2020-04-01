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
#include <Shibboleth_Reflection.h>
#include <Gaff_Flags.h>
#include <lua.hpp>

namespace
{
	constexpr int32_t k_ref_def_index = lua_upvalueindex(1);
	constexpr int32_t k_metatable_name_index = lua_upvalueindex(2);
	static Shibboleth::ProxyAllocator g_allocator("Lua");

	struct UserDataMetadata final
	{
		enum class HeaderFlag
		{
			IsReference,

			Count
		};

		Gaff::Flags<HeaderFlag> flags;
	};

	struct UserData final
	{
		UserDataMetadata meta;

		// Only valid if the IsReference flag in metadata is set.
		void* reference;

		void* GetData(void)
		{
			// If it's a reference, read the pointer stored in the user data.
			// If it's not a reference, the memory after the metadata is our object, so take a pointer to it.
			return (meta.flags.testAll(UserDataMetadata::HeaderFlag::IsReference)) ?
				reference :
				&reference;
		}
	};

	struct FunctionWrapper final
	{
		void (*func)(void);
	};
}

NS_SHIBBOLETH

void FillArgumentStack(lua_State* state, Vector<Gaff::FunctionStackEntry>& stack)
{
	const int32_t num_args = lua_gettop(state);
	stack.resize(static_cast<size_t>(num_args));

	for (int32_t i = 0; i < num_args; ++i) {
		const int32_t type = lua_type(state, i + 1);

		switch (type) {
			case LUA_TNIL:
				stack[i].value.vp = nullptr;
				break;

			case LUA_TBOOLEAN:
				stack[i].ref_def = &Reflection<bool>::GetReflectionDefinition();
				stack[i].value.b = lua_toboolean(state, i + 1);
				break;

			case LUA_TLIGHTUSERDATA:
				// $TODO: Log error.
				break;

			case LUA_TNUMBER:
				if (lua_isinteger(state, i + 1)) {
					stack[i].ref_def = &Reflection<int64_t>::GetReflectionDefinition();
					stack[i].value.i64 = luaL_checkinteger(state, i + 1);
				} else {
					stack[i].ref_def = &Reflection<double>::GetReflectionDefinition();
					stack[i].value.d = luaL_checknumber(state, i + 1);
				}
				break;

			case LUA_TSTRING:
				stack[i].ref_def = &Reflection<U8String>::GetReflectionDefinition();
				stack[i].value.vp = const_cast<char*>(luaL_checkstring(state, i + 1));
				break;

			case LUA_TTABLE:
				// $TODO: Log error.
				break;

			case LUA_TFUNCTION:
				// $TODO: Log error.
				break;

			case LUA_TUSERDATA: {
				lua_getmetatable(state, i + 1);
				lua_getfield(state, -1, "__ref_def");

				stack[i].ref_def = reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
				lua_pop(state, 2);

				UserData* const user_data = reinterpret_cast<UserData*>(lua_touserdata(state, i + 1));
				stack[i].value.vp = user_data->GetData();

			} break;

			case LUA_TTHREAD:
				// $TODO: Log error.
				break;
		}
	}
}

void RegisterType(lua_State* state, const Gaff::IReflectionDefinition& ref_def)
{
	if (!ref_def.getCtorStackFunc()) {
		return;
	}

	luaL_Reg mt[] = {
		{ "__index", UserTypeIndex },
		{ NULL, NULL }
	};

	const char* const friendly_name = ref_def.getFriendlyName();

	// Type Metatable
	luaL_newmetatable(state, friendly_name);

	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
	lua_setfield(state, -2, "__ref_def");

	// Push up values.
	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
	luaL_setfuncs(state, mt, 1);


	// Library Table.
	Vector<luaL_Reg> reg(g_allocator);
	// Add other static funcs here.

	reg.emplace_back(luaL_Reg{ "new", UserTypeNew });
	reg.emplace_back(luaL_Reg{ NULL, NULL });

	luaL_checkversion(state);
	lua_createtable(state, 0, static_cast<int32_t>(reg.size()));

	// Push up values.
	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
	lua_pushstring(state, friendly_name);

	luaL_setfuncs(state, reg.data(), 2);
	lua_setglobal(state, friendly_name);
}

void RegisterBuiltIns(lua_State* state)
{
	GAFF_REF(state);
}

//int UserTypeFunctionCall(lua_State* state)
//{
//
//	return 0;
//}

int UserTypeIndex(lua_State* state)
{
	const Gaff::IReflectionDefinition& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, k_ref_def_index));
	UserData* const user_data = reinterpret_cast<UserData*>(luaL_checkudata(state, 1, ref_def.getFriendlyName()));
	void* const input = user_data->GetData();

	size_t len = 0;
	const char* const name = luaL_checklstring(state, 2, &len);
	const Gaff::Hash32 hash = Gaff::FNV1aHash32String(name);

	if (const auto* const var = ref_def.getVar(hash)) {
		if (var->isFixedArray() || var->isVector()) {
			// $TODO: Add support for arrays.
		} else if (var->isMap()) {
			// $TODO: Add support for maps.
		} else {
			const Gaff::IReflection& var_refl = var->getReflection();
			const Gaff::IReflectionDefinition& var_ref_def = var_refl.getReflectionDefinition();

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
				const size_t var_size = static_cast<size_t>(ref_def.size());
				UserData* const value = reinterpret_cast<UserData*>(lua_newuserdata(state, sizeof(UserData)));

				new(value) UserData();
				value->meta.flags.set(true, UserDataMetadata::HeaderFlag::IsReference);
				value->reference = input;

				luaL_getmetatable(state, var_ref_def.getFriendlyName());
				lua_setmetatable(state, -2);
			}
		}

	} /*else if (const auto* const func = ref_def.getFunc(name)) {
	}*/

	return 0;
}

int UserTypeNew(lua_State* state)
{
	const Gaff::IReflectionDefinition& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, k_ref_def_index));
	const char* const metatable_name = lua_tostring(state, k_metatable_name_index);

	Vector<Gaff::FunctionStackEntry> arg_stack(g_allocator);
	FillArgumentStack(state, arg_stack);

	const size_t var_size = static_cast<size_t>(ref_def.size());
	void* const value = lua_newuserdata(state, var_size + sizeof(UserDataMetadata));

	// Initialize metadata at the beginning.
	new(value) UserDataMetadata();

	// Initialize our data.
	ref_def.getCtorStackFunc()(&reinterpret_cast<UserData*>(value)->reference, arg_stack.data(), static_cast<int32_t>(arg_stack.size()));

	luaL_getmetatable(state, metatable_name);
	lua_setmetatable(state, -2);

	return 1;
}

NS_END
