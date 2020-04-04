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
	constexpr int32_t k_metatable_index = lua_upvalueindex(2);
	constexpr int32_t k_func_index_index = lua_upvalueindex(2);
	constexpr int32_t k_func_is_static_index = lua_upvalueindex(3);
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

		const void* getData(void) const
		{
			return const_cast<UserData*>(this)->getData();
		}

		void* getData(void)
		{
			// If it's a reference, read the pointer stored in the user data.
			// If it's not a reference, the memory after the metadata is our object, so take a pointer to it.
			return (meta.flags.testAll(UserDataMetadata::HeaderFlag::IsReference)) ?
				reference :
				&reference;
		}
	};

	constexpr size_t k_alloc_size_no_reference = sizeof(UserData) - sizeof(void*);

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
			UserData* const value = reinterpret_cast<UserData*>(lua_newuserdata(_state, sizeof(UserData) + size_bytes));
			new(value) UserData();

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

	int32_t Print(lua_State* state)
	{
		const int32_t num_args = lua_gettop(state);
		Shibboleth::U8String final_string;

		for (int32_t i = 0; i < num_args; ++i) {
			switch (lua_type(state, i + 1)) {
				case LUA_TNIL:
					final_string += "nil";
					break;

				case LUA_TBOOLEAN:
					final_string += (lua_toboolean(state, i + 1)) ? "true" : "false";
					break;

				case LUA_TNUMBER:
					if (lua_isinteger(state, i + 1)) {
						final_string.append_sprintf("%ill", luaL_checkinteger(state, i + 1));
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
					lua_getfield(state, -1, "__ref_def");

					const auto& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
					lua_pop(state, 2);

					const UserData& user_data = *reinterpret_cast<UserData*>(lua_touserdata(state, -1));
					const void* const object = user_data.getData();

					Gaff::IReflectionStaticFunction<Shibboleth::U8String, const void*>* const func =
						ref_def.getStaticFunc<Shibboleth::U8String, const void*>(Gaff::GetOpNameHash(Gaff::Operator::ToString));

					if (func) {
						final_string += func->getFunc()(object);
					} else {
						final_string.append_sprintf("%p", object);
					}

				} break;

				case LUA_TTHREAD:
					// $TODO: Log error.
					break;
			}
		}

		return 0;
	}
}

NS_SHIBBOLETH

void FillArgumentStack(lua_State* state, Vector<Gaff::FunctionStackEntry>& stack, int32_t start, int32_t end)
{
	start = Gaff::Max(start, 1);
	end = (end < 1) ? lua_gettop(state) : Gaff::Min(end, lua_gettop(state));

	stack.resize(static_cast<size_t>(end - start + 1));

	for (int32_t i = start - 1; i < end; ++i) {
		const int32_t type = lua_type(state, i + 1);

		switch (type) {
			case LUA_TNIL:
				stack[i - start + 1].value.vp = nullptr;
				break;

			case LUA_TBOOLEAN:
				stack[i - start + 1].ref_def = &Reflection<bool>::GetReflectionDefinition();
				stack[i - start + 1].value.b = lua_toboolean(state, i + 1);
				break;

			//case LUA_TLIGHTUSERDATA:
			//	// $TODO: Log error.
			//	break;

			case LUA_TNUMBER:
				if (lua_isinteger(state, i + 1)) {
					stack[i - start + 1].ref_def = &Reflection<int64_t>::GetReflectionDefinition();
					stack[i - start + 1].value.i64 = luaL_checkinteger(state, i + 1);
				} else {
					stack[i - start + 1].ref_def = &Reflection<double>::GetReflectionDefinition();
					stack[i - start + 1].value.d = luaL_checknumber(state, i + 1);
				}
				break;

			case LUA_TSTRING:
				stack[i - start + 1].ref_def = &Reflection<U8String>::GetReflectionDefinition();
				stack[i - start + 1].value.vp = const_cast<char*>(luaL_checkstring(state, i + 1));
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
				lua_getfield(state, -1, "__ref_def");

				stack[i - start + 1].ref_def = reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
				lua_pop(state, 2);

				UserData* const user_data = reinterpret_cast<UserData*>(lua_touserdata(state, i + 1));
				stack[i - start + 1].value.vp = user_data->getData();

			} break;

			case LUA_TTHREAD:
				// $TODO: Log error.
				break;
		}
	}
}

int32_t PushReturnValue(lua_State* state, const Gaff::FunctionStackEntry& ret)
{
	if (ret.enum_ref_def) {
		lua_pushinteger(state, ret.value.i64);
		return 1;

	} else if (ret.ref_def) {
		if (ret.ref_def->isBuiltIn()) {
			if (lua_Number num_value; Gaff::CastFloatToType<lua_Number>(ret, num_value)) {
				lua_pushnumber(state, num_value);
				return 1;

			} else if (lua_Integer int_value; Gaff::CastIntegerToType<lua_Integer>(ret, int_value)) {
				lua_pushinteger(state, int_value);
				return 1;

			// Value is a boolean.
			} else {
				lua_pushboolean(state, ret.value.b);
				return 1;
			}

		// Is a user defined type.
		} else {
			//UserData* const value = reinterpret_cast<UserData*>(reinterpret_cast<int8_t*>(ret.value.vp) - k_alloc_size_no_reference);

			// Allocator already pushed it into the stack.
			luaL_getmetatable(state, ret.ref_def->getFriendlyName());
			lua_setmetatable(state, -2);

			return 1;
		}

	} else {
		return 0;
	}
}

void RegisterType(lua_State* state, const Gaff::IReflectionDefinition& ref_def)
{
	const char* const friendly_name = ref_def.getFriendlyName();

	// Type Metatable
	luaL_newmetatable(state, friendly_name);

	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
	lua_setfield(state, -2, "__ref_def");

	// Register operators.
	const int32_t num_static_funcs = ref_def.getNumStaticFuncs();
	int32_t num_operators = 0;

	for (int32_t i = 0; i < num_static_funcs; ++i) {
		const char* const name = ref_def.getStaticFuncName(i);

		// Is not an operator function.
		if (name[0] != '_' || name[1] != '_') {
			continue;
		}

		lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
		lua_pushinteger(state, i);
		lua_pushboolean(state, true); // Is static.

		lua_pushcclosure(state, UserTypeFunctionCall, 3);
		lua_setfield(state, -2, name);

		++num_operators;
	}

	Vector<luaL_Reg> mt(g_allocator);
	mt.emplace_back(luaL_Reg{ "__newindex", UserTypeNewIndex });
	mt.emplace_back(luaL_Reg{ "__index", UserTypeIndex });
	mt.emplace_back(luaL_Reg{ "__gc", UserTypeDestroy });
	mt.emplace_back(luaL_Reg{ nullptr, nullptr });

	// Register funcs with up values.
	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
	luaL_setfuncs(state, mt.data(), 1);


	// Library Table.
	Vector<luaL_Reg> reg(g_allocator);

	// Add constructor.
	if (ref_def.getCtorStackFunc()) {
		reg.emplace_back(luaL_Reg{ "new", UserTypeNew });
	}

	reg.emplace_back(luaL_Reg{ nullptr, nullptr });

	lua_createtable(state, 0, static_cast<int32_t>(reg.size()) + num_static_funcs - num_operators);

	// Add static funcs.
	for (int32_t i = 0; i < num_static_funcs; ++i) {
		const char* const name = ref_def.getStaticFuncName(i);

		// Is an operator function.
		if (name[0] == '_' && name[1] == '_') {
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
}

int UserTypeFunctionCall(lua_State* state)
{
	const Gaff::IReflectionDefinition& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, k_ref_def_index));
	const int32_t func_index = static_cast<int32_t>(luaL_checkinteger(state, k_func_index_index));
	const bool is_static = lua_toboolean(state, k_func_is_static_index);

	const int32_t num_overrides = (is_static) ? ref_def.getNumStaticFuncOverrides(func_index) : ref_def.getNumFuncOverrides(func_index);
	const int32_t num_args = (is_static) ? lua_gettop(state) : lua_gettop(state) - 1;

	LuaTypeInstanceAllocator allocator(state);
	Vector<Gaff::FunctionStackEntry> args(g_allocator);
	Gaff::FunctionStackEntry ret;

	for (int32_t i = 0; i < num_overrides; ++i) {
		const Gaff::IReflectionStaticFunctionBase* const static_func = (is_static) ? ref_def.getStaticFunc(func_index, i) : nullptr;
		const Gaff::IReflectionFunctionBase* const func = (is_static) ? nullptr : ref_def.getFunc(func_index, i);
		const int32_t func_args = (is_static) ? static_func->numArgs() : func->numArgs();

		if (func_args == num_args) {
			if (num_args > 0 && args.empty()) {
				FillArgumentStack(state, args, (is_static) ? 1 : 2, num_args);
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

			return PushReturnValue(state, ret);
		}
	}

	// $TODO: Log error. Can't find function with the correct number of arguments or argument type mismatch.
	return 0;
}

int UserTypeDestroy(lua_State* state)
{
	const Gaff::IReflectionDefinition& ref_def = *reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, k_ref_def_index));
	UserData* const user_data = reinterpret_cast<UserData*>(luaL_checkudata(state, 1, ref_def.getFriendlyName()));

	if (!user_data->meta.flags.testAll(UserDataMetadata::HeaderFlag::IsReference)) {
		ref_def.destroyInstance(user_data->getData());
	}

	return 0;
}

int UserTypeNewIndex(lua_State* state)
{
	GAFF_REF(state);
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
			} else if (var->isMap()) {
				// $TODO: Add support for maps.

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
					const size_t var_size = static_cast<size_t>(ref_def.size());
					UserData* const value = reinterpret_cast<UserData*>(lua_newuserdata(state, sizeof(UserData)));

					new(value) UserData();
					value->meta.flags.set(true, UserDataMetadata::HeaderFlag::IsReference);
					value->reference = const_cast<void*>(input);

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

			LuaTypeInstanceAllocator allocator(state);
			Vector<Gaff::FunctionStackEntry> args(g_allocator);
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

					return PushReturnValue(state, ret);
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

			LuaTypeInstanceAllocator allocator(state);
			Vector<Gaff::FunctionStackEntry> args(g_allocator);
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

					return PushReturnValue(state, ret);
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
	FillArgumentStack(state, arg_stack);

	const size_t var_size = static_cast<size_t>(ref_def.size());
	void* const value = lua_newuserdata(state, var_size + k_alloc_size_no_reference);

	// Initialize metadata at the beginning.
	new(value) UserDataMetadata();

	// Initialize our data.
	ref_def.getCtorStackFunc()(&reinterpret_cast<UserData*>(value)->reference, arg_stack.data(), static_cast<int32_t>(arg_stack.size()));

	lua_pushvalue(state, k_metatable_index);
	lua_setmetatable(state, -2);

	return 1;
}

NS_END
