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

#include "Shibboleth_JanetHelpers.h"
#include "Shibboleth_JanetManager.h"
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Flags.h>

namespace
{
	static Shibboleth::ProxyAllocator g_allocator("Janet");

	class JanetTypeInstanceAllocator final : public Gaff::IFunctionStackAllocator
	{
	public:
		// For EASTL support.
		void* allocate(size_t, size_t, size_t, int flags = 0) override
		{
			GAFF_ASSERT(false);
			GAFF_REF(flags);
			return nullptr;
		}

		void* allocate(size_t, int flags = 0) override
		{
			GAFF_ASSERT(false);
			GAFF_REF(flags);
			return nullptr;
		}

		void deallocate(void*, size_t) override
		{
			// Should never happen.
			GAFF_ASSERT(false);
		}

		const char* get_name() const override
		{
			// Should never happen.
			GAFF_ASSERT(false);
			return nullptr;
		}

		void set_name(const char*) override
		{
			// Should never happen.
			GAFF_ASSERT(false);
		}

		void* alloc(size_t, size_t, const char*, int) override
		{
			GAFF_ASSERT(false);
			return nullptr;
		}

		void* alloc(size_t size_bytes, const char* file, int line) override
		{
			return g_allocator.alloc(size_bytes, file, line);
		}

		void* alloc(const Gaff::IReflectionDefinition& ref_def) override
		{
			Shibboleth::JanetManager& janet_mgr = Shibboleth::GetApp().getManagerTFast<Shibboleth::JanetManager>();
			const JanetAbstractType* const type_info = janet_mgr.getType(ref_def);

			GAFF_ASSERT(type_info);

			Shibboleth::UserData* const value = Shibboleth::PushUserType(ref_def, *type_info);
			return value->getData();
		}

		void free(void*) override
		{
			// Should never happen.
			GAFF_ASSERT(false);
		}
	};

	struct ReflectionDataJanet final
	{
		static constexpr char* k_type_info_name = "ReflectionDefinition";

		const Gaff::IReflectionDefinition* ref_def;
		const JanetAbstractType* type_info;
	};

	struct ReflectionFunctionJanet final
	{
		static constexpr char* k_type_info_name = "ReflectionFunction";

		const Gaff::IReflectionDefinition* ref_def;
		const JanetAbstractType* type_info;
		int32_t func_index;
		bool is_static;
	};

	static const JanetAbstractType g_ref_def_type_info =
	{
		ReflectionDataJanet::k_type_info_name,
		JANET_ATEND_NAME
	};

	static const JanetAbstractType g_ref_func_type_info =
	{
		ReflectionFunctionJanet::k_type_info_name,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		Shibboleth::UserTypeFunctionCall
	};

	inline Janet WrapBoolean(bool value)
	{
#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable : 4806)
#endif

		return janet_wrap_boolean(value);

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif
	}

	void PrintHelper(int32_t num_args, Janet* args, Shibboleth::U8String& out)
	{
		for (int32_t i = 0; i < num_args; ++i) {
			switch (janet_type(args[i])) {
				case JANET_NIL:
					out += "nil";
					break;

				case JANET_BOOLEAN:
					out += (janet_unwrap_boolean(args[i])) ? "true" : "false";
					break;

				case JANET_NUMBER:
					if (const JanetIntType int_type = janet_is_int(args[i]); int_type != JANET_INT_NONE) {
						switch (int_type) {
							case JANET_INT_S64:
								out.append_sprintf("%lli", janet_unwrap_s64(args[i]));
								break;

							case JANET_INT_U64:
								out.append_sprintf("%llu", janet_unwrap_u64(args[i]));
								break;
						}

					} else {
						out.append_sprintf("%f", janet_unwrap_number(args[i]));
					}
					break;

				case JANET_STRING:
					out += reinterpret_cast<const char*>(janet_unwrap_string(args[i]));
					break;

				case JANET_TABLE: {
					const JanetTable* const table = janet_unwrap_table(args[i]);
					bool first = true;
					out += "@{";

					for (int32_t j = 0; j < table->capacity; ++j) {
						JanetKV& kv = table->data[j];

						if (janet_checktype(kv.key, JANET_NIL) || janet_checktype(kv.value, JANET_NIL)) {
							continue;
						}

						if (!first) {
							out += ",";
						}

						PrintHelper(1, &kv.key, out);
						out += ":";
						PrintHelper(1, &kv.value, out);

						first = false;
					}

					out += "}";
				} break;

				case JANET_STRUCT: {
					const JanetStruct janet_struct = janet_unwrap_struct(args[i]);
					const int32_t len = janet_struct_capacity(janet_struct);
					bool first = true;
					out += "{";

					for (int32_t j = 0; j < len; ++j) {
						const JanetKV& kv = janet_struct[j];

						if (janet_checktype(kv.key, JANET_NIL) || janet_checktype(kv.value, JANET_NIL)) {
							continue;
						}

						if (!first) {
							out += ",";
						}

						PrintHelper(1, const_cast<Janet*>(&kv.key), out);
						out += ":";
						PrintHelper(1, const_cast<Janet*>(&kv.value), out);

						first = false;
					}

					out += "}";
				} break;

				case JANET_ARRAY: {
					const JanetArray* const array = janet_unwrap_array(args[i]);
					out += "@[";

					for (int32_t j = 0; j < array->count; ++j) {
						PrintHelper(1, &array->data[j], out);

						if (j < (array->count - 1)) {
							out += ",";
						}
					}

					out += "]";
				} break;

				case JANET_TUPLE: {
					const JanetTuple tuple = janet_unwrap_tuple(args[i]);
					const int32_t len = janet_tuple_length(tuple);

					out += "[";

					for (int32_t j = 0; j < len; ++j) {
						PrintHelper(1, const_cast<Janet*>(&tuple[j]), out);

						if (j < (len - 1)) {
							out += ",";
						}
					}

					out += "]";
				} break;

				case JANET_CFUNCTION:
					out.append_sprintf("cfunc:%p", janet_unwrap_cfunction(args[i]));
					break;

				case JANET_FUNCTION:
					out.append_sprintf("func:%s", reinterpret_cast<const char*>(janet_unwrap_function(args[i])->def->name));
					break;

				case JANET_FIBER:
					// $TODO: Log error.
					break;

				case JANET_ABSTRACT: {
					Shibboleth::UserData* const value = reinterpret_cast<Shibboleth::UserData*>(janet_unwrap_abstract(args[i]));
					auto* const to_string = value->ref_def->getStaticFunc<int32_t, const void*, char*, int32_t>(
						Gaff::GetOpNameHash(Gaff::Operator::ToString)
					);

					if (to_string) {
						char buffer[256] = { 0 };
						char* ptr = buffer;

						if (to_string->call(value->getData(), std::forward<char*>(ptr), 256)) {
							out += buffer;
							break;
						}

					}
					
					out += value->ref_def->getReflectionInstance().getName();
				} break;

				case JANET_POINTER:
					// $TODO: Log error.
					break;

				case JANET_BUFFER: {
					const JanetBuffer* const buffer = janet_unwrap_buffer(args[i]);

					for (int32_t j = 0; j < buffer->count; ++i) {
						out += buffer->data[j];
					}
				} break;

				case JANET_KEYWORD:
					out.append_sprintf(":%s", reinterpret_cast<const char*>(janet_unwrap_keyword(args[i])));
					break;

				case JANET_SYMBOL:
					out += reinterpret_cast<const char*>(janet_unwrap_symbol(args[i]));
					break;
			}
		}
	}

	Janet Print(int32_t num_args, Janet* args)
	{
		Shibboleth::U8String final_string(g_allocator);
		PrintHelper(num_args, args, final_string);

		Shibboleth::GetApp().getLogManager().logMessage(Shibboleth::LogType::Normal, Shibboleth::k_janet_log_channel, final_string.data());
		Gaff::DebugPrintf(final_string.data());
		Gaff::DebugPrintf("\n");

		return janet_wrap_nil();
	}

	Janet GetManager(int32_t num_args, Janet* args)
	{
		janet_arity(num_args, 1, -1);

		Janet* const tuple = (num_args > 1) ? janet_tuple_begin(num_args) : nullptr;

		for (int32_t i = 0; i < num_args; ++i) {
			const ReflectionDataJanet* const ref_data = reinterpret_cast<ReflectionDataJanet*>(janet_checkabstract(args[i], &g_ref_def_type_info));

			// Not an ReflectionDefinition.
			if (!ref_data) {
				// $TODO: Log error.

				if (tuple) {
					tuple[i] = janet_wrap_nil();
					continue;
				} else {
					return janet_wrap_nil();
				}
			}

			if (!ref_data->ref_def->hasInterface(CLASS_HASH(IManager))) {
				// $TODO: Log error.

				if (tuple) {
					tuple[i] = janet_wrap_nil();
					continue;
				} else {
					return janet_wrap_nil();
				}
			}

			Shibboleth::IManager* const manager = Shibboleth::GetApp().getManager(ref_data->ref_def->getReflectionInstance().getHash());
			const Janet value = Shibboleth::PushUserTypeReference(manager->getBasePointer(), *ref_data->ref_def, *ref_data->type_info);

			if (tuple) {
				tuple[i] = value;
			} else {
				return value;
			}
		}

		return janet_wrap_nil();
	}

	Janet PushOrUpdateTableValue(JanetTable& table, Janet key, const Gaff::FunctionStackEntry& entry, const Shibboleth::JanetManager& janet_mgr)
	{
		// Nil type.
		if (!entry.enum_ref_def && !entry.ref_def) {
			return janet_wrap_nil();
		}

		// Built-in types, just push onto the stack.
		if (entry.enum_ref_def || (entry.ref_def && entry.ref_def->isBuiltIn())) {
			return Shibboleth::PushReturnValue(entry, true);
		}

		const JanetAbstractType* const type_info = janet_mgr.getType(*entry.ref_def);
		const Janet value = janet_table_get(&table, key);
		Shibboleth::UserData* const old_data = reinterpret_cast<Shibboleth::UserData*>(janet_checkabstract(value, type_info));

		// Types are not the same.
		if (!old_data) {
			return Shibboleth::PushReturnValue(entry, true);
		}

		Shibboleth::CopyUserType(entry, old_data->getData(), true, g_allocator);
		return value;
	}
}


NS_SHIBBOLETH

Janet PushUserTypeReference(const void* value, const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info)
{
	UserData* const user_data = reinterpret_cast<UserData*>(janet_abstract(&type_info, sizeof(UserData)));
	new(user_data) UserData();

	user_data->meta.flags.set(true, UserData::MetaData::HeaderFlag::IsReference);
	user_data->reference = const_cast<void*>(value);
	user_data->ref_def = &ref_def;

	return janet_wrap_abstract(user_data);
}

Janet PushUserType(const void* value, const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info, UserData** out)
{
	UserData* user_data = nullptr;
	const Janet ret_value = PushUserType(ref_def, type_info, &user_data);

	CopyUserType(ref_def, value, user_data->getData(), false, g_allocator);

	if (out) {
		*out = user_data;
	}

	return ret_value;
}

Janet PushUserType(const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info, UserData** out)
{
	UserData* const user_data = PushUserType(ref_def, type_info);

	if (out) {
		*out = user_data;
	}

	return janet_wrap_abstract(user_data);
}

UserData* PushUserType(const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info)
{
	const size_t var_size = static_cast<size_t>(ref_def.size());
	UserData* const user_data = reinterpret_cast<UserData*>(janet_abstract(&type_info, var_size + k_alloc_size_no_reference));

	// Initialize metadata at the beginning.
	new(user_data) UserData::MetaData();
	user_data->ref_def = &ref_def;
	//user_data->root = value;

	return user_data;
}

void FillArgumentStack(int32_t num_args, Janet* args, Vector<Gaff::FunctionStackEntry>& stack, int32_t start, int32_t end)
{
	start = Gaff::Max(start, 0);
	end = (end < 0) ? num_args : Gaff::Min(end, num_args);

	stack.resize(static_cast<size_t>(end - start));

	for (int32_t i = start; i < end; ++i) {
		FillEntry(args[i], stack[i - start], false);
	}
}

void FillEntry(const Janet& arg, Gaff::FunctionStackEntry& entry, bool clone_non_janet)
{
	switch (janet_type(arg)) {
		case JANET_NIL:
			entry.value.vp = nullptr;
			break;

		case JANET_BOOLEAN:
			if (clone_non_janet) {
				FreeDifferentType(entry, Reflection<bool>::GetReflectionDefinition(), false);
			}
					
			entry.ref_def = &Reflection<bool>::GetReflectionDefinition();
			entry.value.b = janet_unwrap_boolean(arg);
			break;

		case JANET_NUMBER:
			if (const JanetIntType int_type = janet_is_int(arg); int_type != JANET_INT_NONE) {
				if (clone_non_janet) {
					FreeDifferentType(entry, Reflection<int64_t>::GetReflectionDefinition(), false);
				}

				switch (int_type) {
					case JANET_INT_S64:
						entry.ref_def = &Reflection<int64_t>::GetReflectionDefinition();
						entry.value.i64 = janet_unwrap_s64(arg);
						break;

					case JANET_INT_U64:
						entry.ref_def = &Reflection<uint64_t>::GetReflectionDefinition();
						entry.value.u64 = janet_unwrap_u64(arg);
						break;
				}

			} else {
				if (clone_non_janet) {
					FreeDifferentType(entry, Reflection<double>::GetReflectionDefinition(), false);
				}

				entry.ref_def = &Reflection<double>::GetReflectionDefinition();
				entry.value.d = janet_unwrap_number(arg);
			}
			break;

		case JANET_STRING:
			if (clone_non_janet) {
				FreeDifferentType(entry, Reflection<U8String>::GetReflectionDefinition(), false);
					
				if (!entry.value.vp) {
					entry.value.vp = SHIB_ALLOCT(U8String, g_allocator);
				}
					
				*reinterpret_cast<U8String*>(entry.value.vp) = reinterpret_cast<const char*>(janet_unwrap_string(arg));
				entry.ref_def = &Reflection<U8String>::GetReflectionDefinition();
					
			} else {
				entry.flags.set(true, Gaff::FunctionStackEntry::Flag::IsString);
				entry.value.vp = const_cast<char*>(reinterpret_cast<const char*>(janet_unwrap_string(arg)));
			}
			break;

		case JANET_TABLE: {
			//lua_getfield(state, stack_index, k_is_type_table_field_name);
			//const bool is_type_table = lua_isboolean(state, -1);
			//
			//lua_pop(state, 1);
			//
			//// This is a type table, only pass the ReflectionDefinition.
			//if (is_type_table) {
			//	lua_getfield(state, stack_index, k_ref_def_field_name);
			//	entry.ref_def = reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
			//
			//	lua_pop(state, 1);
			//	break;
			//}
					
			// Fill vectormap.
			// $TODO: Log error.
		} break;

		case JANET_STRUCT: {
			//lua_getfield(state, stack_index, k_is_type_table_field_name);
			//const bool is_type_table = lua_isboolean(state, -1);
			//
			//lua_pop(state, 1);
			//
			//// This is a type table, only pass the ReflectionDefinition.
			//if (is_type_table) {
			//	lua_getfield(state, stack_index, k_ref_def_field_name);
			//	entry.ref_def = reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
			//
			//	lua_pop(state, 1);
			//	break;
			//}

			// Fill vectormap.
			// $TODO: Log error.
		} break;

		case JANET_ARRAY: {
			//lua_getfield(state, stack_index, k_is_type_table_field_name);
			//const bool is_type_table = lua_isboolean(state, -1);
			//
			//lua_pop(state, 1);
			//
			//// This is a type table, only pass the ReflectionDefinition.
			//if (is_type_table) {
			//	lua_getfield(state, stack_index, k_ref_def_field_name);
			//	entry.ref_def = reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
			//
			//	lua_pop(state, 1);
			//	break;
			//}

			// Fill vector.
			// $TODO: Log error.

			//const JanetArray* const array = janet_unwrap_array(arg);

			//for (int32_t j = 0; j < array->count; ++j) {
			//}
		} break;

		case JANET_TUPLE: {
			//lua_getfield(state, stack_index, k_is_type_table_field_name);
			//const bool is_type_table = lua_isboolean(state, -1);
			//
			//lua_pop(state, 1);
			//
			//// This is a type table, only pass the ReflectionDefinition.
			//if (is_type_table) {
			//	lua_getfield(state, stack_index, k_ref_def_field_name);
			//	entry.ref_def = reinterpret_cast<Gaff::IReflectionDefinition*>(lua_touserdata(state, -1));
			//
			//	lua_pop(state, 1);
			//	break;
			//}

			// Fill vector.
			// $TODO: Log error.

			//const JanetTuple tuple = janet_unwrap_tuple(arg);
			//const int32_t len = janet_tuple_length(tuple);

			//for (int32_t j = 0; j < len; ++j) {
			//}
		} break;

		case JANET_CFUNCTION:
			// $TODO: Log error.
			break;

		case JANET_FUNCTION:
			// $TODO: Log error.
			break;

		case JANET_FIBER:
			// $TODO: Log error.
			break;

		case JANET_ABSTRACT: {
			Shibboleth::UserData* const value = reinterpret_cast<Shibboleth::UserData*>(janet_unwrap_abstract(arg));

			if (clone_non_janet) {
				const bool other_is_reference = value->meta.flags.testAll(UserData::MetaData::HeaderFlag::IsReference);
				const bool is_reference = entry.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference);

				FreeDifferentType(entry, *value->ref_def, other_is_reference);

				// If it's just a reference, don't worry about creating a copy of the data.
				if (other_is_reference) {
					entry.flags.set(true, Gaff::FunctionStackEntry::Flag::IsReference);
					entry.value.vp = value->getData();
					entry.ref_def = value->ref_def;

				// Is not a reference, we should clone the data.
				} else {
					U8String ctor_sig(g_allocator);
					ctor_sig.append_sprintf("const %s&", value->ref_def->getReflectionInstance().getName());

					const HashStringView64<> hash(ctor_sig);

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
						cast_ctor(entry.value.vp, value->getData());

					// Need to make a copy.
					} else {
						auto factory = value->ref_def->getFactory(hash.getHash());

						if (!factory) {
							// $TODO: Log error.
							break;
						}

						const auto cast_factory = reinterpret_cast<void* (*)(Gaff::IAllocator&, const void*)>(factory);
						entry.value.vp = cast_factory(g_allocator, value->getData());
					}

					entry.ref_def = value->ref_def;
				}

			} else {
				// Unnecessary, but for posterity.
				entry.flags.set(
					value->meta.flags.testAll(UserData::MetaData::HeaderFlag::IsReference),
					Gaff::FunctionStackEntry::Flag::IsReference
				);

				entry.value.vp = value->getData();
				entry.ref_def = value->ref_def;
			}
		} break;

		case JANET_POINTER:
			// $TODO: Log error.
			break;

		case JANET_BUFFER:
			// $TODO: Log error.
			break;

		case JANET_KEYWORD:
			if (clone_non_janet) {
				FreeDifferentType(entry, Reflection<U8String>::GetReflectionDefinition(), false);
					
				if (!entry.value.vp) {
					entry.value.vp = SHIB_ALLOCT(U8String, g_allocator);
				}
					
				*reinterpret_cast<U8String*>(entry.value.vp) = reinterpret_cast<const char*>(janet_unwrap_keyword(arg));
				entry.ref_def = &Reflection<U8String>::GetReflectionDefinition();
					
			} else {
				entry.flags.set(true, Gaff::FunctionStackEntry::Flag::IsString);
				entry.value.vp = const_cast<char*>(reinterpret_cast<const char*>(janet_unwrap_keyword(arg)));
			}
			break;

		case JANET_SYMBOL:
			if (clone_non_janet) {
				FreeDifferentType(entry, Reflection<U8String>::GetReflectionDefinition(), false);
					
				if (!entry.value.vp) {
					entry.value.vp = SHIB_ALLOCT(U8String, g_allocator);
				}
					
				*reinterpret_cast<U8String*>(entry.value.vp) = reinterpret_cast<const char*>(janet_unwrap_symbol(arg));
				entry.ref_def = &Reflection<U8String>::GetReflectionDefinition();
					
			} else {
				entry.flags.set(true, Gaff::FunctionStackEntry::Flag::IsString);
				entry.value.vp = const_cast<char*>(reinterpret_cast<const char*>(janet_unwrap_symbol(arg)));
			}
			break;
	}
}

Janet PushReturnValue(const Gaff::FunctionStackEntry& ret, bool create_user_data)
{
	if (ret.enum_ref_def) {
		if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsArray)) {
			GAFF_ASSERT_MSG(ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference), "Do not support returning arrays by value.");

			const int8_t* begin = reinterpret_cast<int8_t*>(ret.value.arr.data);
			const int32_t data_size = ret.enum_ref_def->size();

			Janet* const tuple = janet_tuple_begin(ret.value.arr.size);

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

				tuple[i] = janet_wrap_number(static_cast<double>(value));
				begin += data_size;
			}

			return janet_wrap_tuple(janet_tuple_end(tuple));

		} else if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsMap)) {
			// $TODO: impl

		} else {
			return janet_wrap_number(static_cast<double>(ret.value.i64));
		}

	// Is a string.
	} else if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsString)) {
		const Janet value = janet_cstringv(reinterpret_cast<char*>(ret.value.vp));

		if (!ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference)) {
			SHIB_FREET(ret.value.vp, GetAllocator());
		}

		return value;

	} else if (ret.ref_def) {
		if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsArray)) {
			GAFF_ASSERT_MSG(ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference), "Do not support returning arrays by value.");

			const JanetManager& janet_mgr = GetApp().getManagerTFast<JanetManager>();
			const int8_t* begin = reinterpret_cast<int8_t*>(ret.value.arr.data);
			const int32_t data_size = ret.ref_def->size();

			Janet* const tuple = janet_tuple_begin(ret.value.arr.size);

			for (int32_t i = 0; i < ret.value.arr.size; ++i) {
				if (ret.ref_def->isBuiltIn()) {
					if (double num_value; Gaff::CastNumberToType<double>(ret, num_value)) {
						tuple[i] = janet_wrap_number(num_value);

					// Value is a boolean.
					} else {
						tuple[i] = WrapBoolean(ret.value.b);
					}

				} else {
					const JanetAbstractType* const type_info = janet_mgr.getType(*ret.ref_def);

					if (type_info) {
						tuple[i] = PushUserTypeReference(reinterpret_cast<const void*>(begin), *ret.ref_def, *type_info);
					} else {
						// $TODO: Log error.
						tuple[i] = janet_wrap_nil();
					}
				}

				begin += data_size;
			}

			return janet_wrap_tuple(janet_tuple_end(tuple));

		} else if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsMap)) {
			// $TODO: impl

		} else if (ret.ref_def->isBuiltIn()) {
			if (double num_value; Gaff::CastNumberToType<double>(ret, num_value)) {
				return janet_wrap_number(num_value);

			// Value is a boolean.
			} else {
				return WrapBoolean(ret.value.b);
			}

		// Is a user defined type.
		} else {
			if (ret.flags.testAll(Gaff::FunctionStackEntry::Flag::IsReference)) {
				JanetManager& janet_mgr = GetApp().getManagerTFast<JanetManager>();
				const JanetAbstractType* const type_info = janet_mgr.getType(*ret.ref_def);

				if (!type_info) {
					// $TODO: Log error.
					return janet_wrap_nil();
				}

				return PushUserTypeReference(ret.value.vp, *ret.ref_def, *type_info);

			} else if (create_user_data) {
				JanetManager& janet_mgr = GetApp().getManagerTFast<JanetManager>();
				const JanetAbstractType* const type_info = janet_mgr.getType(*ret.ref_def);

				if (!type_info) {
					// $TODO: Log error.
					return janet_wrap_nil();
				}

				return PushUserType(ret.value.vp, *ret.ref_def, *type_info, nullptr);

			// Return value was already created by allocator.
			} else {
				return janet_wrap_abstract(ret.value.vp);
			}
		}
	}

	return janet_wrap_nil();
}

void RestoreTable(JanetTable& table, const TableState& state)
{
	const JanetManager& janet_mgr = GetApp().getManagerTFast<JanetManager>();

	for (const auto& pair : state.array_entries) {
		const Janet key = janet_wrap_number(static_cast<double>(pair.first));
		Janet value = janet_table_get(&table, key);

		//if (state.key_values.empty()) {
		//} else {
			value = PushOrUpdateTableValue(table, value, pair.second, janet_mgr);
			janet_table_put(&table, key, value);
		//}
	}

	for (const auto& pair : state.key_values) {
		const Janet key = janet_cstringv(pair.first.data());
		Janet value = janet_table_get(&table, key);

		//if (state.key_values.empty()) {
		//} else {
			value = PushOrUpdateTableValue(table, value, pair.second, janet_mgr);
			janet_table_put(&table, key, value);
		//}
	}
}

void SaveTable(const JanetTable& table, TableState& state)
{
	for (int32_t i = 0; i < table.capacity; ++i) {
		JanetKV& kv = table.data[i];

		if (janet_checktype(kv.key, JANET_NIL)) {

			continue;
		}

		if (janet_checktype(kv.key, JANET_STRING)) {
			U8String key(reinterpret_cast<const char*>(janet_unwrap_string(kv.key)), g_allocator);
			FillEntry(kv.value, state.key_values[std::move(key)], true);

		} else if (janet_checktype(kv.key, JANET_NUMBER)) {
			auto& pair = state.array_entries.emplace_back();
			pair.first = static_cast<int32_t>(janet_unwrap_number(kv.key));
			FillEntry(kv.value, pair.second, true);
		}
	}
}

void RegisterEnum(JanetTable* env, const Gaff::IEnumReflectionDefinition& enum_ref_def)
{
	U8String enum_name = enum_ref_def.getReflectionInstance().getName();	
	size_t index = enum_name.find_first_of(':');

	while (index != U8String::npos) {
		enum_name.replace(index, 2, 1, '/');
		index = enum_name.find_first_of(':');
	}

	// Add all the enum entries.
	const int32_t num_entries = enum_ref_def.getNumEntries();
	JanetKV* const lookup = janet_struct_begin(num_entries * 2);
	U8String final_name;

	for (int32_t i = 0; i < num_entries; ++i) {
		const HashStringView32<> entry_name = enum_ref_def.getEntryNameFromIndex(i);
		const int32_t entry_value = enum_ref_def.getEntryValue(i);

		final_name = enum_name;
		final_name.append_sprintf("/%s", entry_name.getBuffer());

		const Janet name_keyword = janet_ckeywordv(entry_name.getBuffer());
		const Janet value = janet_wrap_integer(entry_value);

		janet_struct_put(lookup, name_keyword, value);
		janet_struct_put(lookup, value, name_keyword);

		janet_def(env, final_name.data(), value, nullptr);
	}

	final_name = enum_name + "/def";

	janet_def(env, final_name.data(), janet_wrap_struct(janet_struct_end(lookup)), nullptr);
}

void RegisterType(JanetTable* /*env*/, const Gaff::IReflectionDefinition& ref_def, JanetManager& janet_mgr)
{
	// We do not need to register attributes or built-in types.
	if (ref_def.hasInterface(CLASS_HASH(Gaff::IAttribute)) || ref_def.isBuiltIn()) {
		return;
	}

	const auto* const flags = ref_def.getClassAttr<ScriptFlagsAttribute>();

	if (flags && flags->getFlags().testAll(ScriptFlagsAttribute::Flag::NoRegister)) {
		return;
	}

	U8String friendly_name = ref_def.getFriendlyName();

	if (Gaff::EndsWith(friendly_name.data(), "<>")) {
		friendly_name.erase(friendly_name.size() - 2);
	}

	size_t index = friendly_name.find_first_of(':');

	while (index != U8String::npos) {
		friendly_name.replace(index, 2, 1, '/');
		index = friendly_name.find_first_of(':');
	}

	// Type Info
	JanetAbstractType type_info;
	memset(&type_info, 0, sizeof(JanetAbstractType));

	type_info.name = reinterpret_cast<char*>(SHIB_ALLOC(friendly_name.size() * sizeof(char) + 1, g_allocator));
	type_info.tostring = UserTypeToString;
	type_info.put = UserTypeNewIndex;
	type_info.get = UserTypeIndex;
	type_info.gc = UserTypeDestroy;

	strncpy(const_cast<char*>(type_info.name), friendly_name.data(), friendly_name.size() * sizeof(char) + 1);

	janet_mgr.registerType(ref_def, type_info);


//	// Register operators.
//	const int32_t num_static_funcs = ref_def.getNumStaticFuncs();
//	int32_t num_operators = 0;
//
//	Vector<luaL_Reg> mt(g_allocator);
//
//	for (int32_t i = 0; i < num_static_funcs; ++i) {
//		const HashStringView32<> name = ref_def.getStaticFuncName(i);
//
//		// Is not an operator function.
//		if (Gaff::FindFirstOf(name.getBuffer(), "__") != 0) {
//			continue;
//		}
//
//		// Is the tostring function.
//		if (Gaff::FindFirstOf(name.getBuffer(), OP_TO_STRING_NAME) == 0) {
//			mt.emplace_back(luaL_Reg{ "__tostring", UserTypeToString });
//			continue;
//		}
//
//		lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
//		lua_pushinteger(state, i);
//		lua_pushboolean(state, true); // Is static.
//
//		lua_pushcclosure(state, UserTypeFunctionCall, 3);
//		lua_setfield(state, -2, name.getBuffer());
//
//		++num_operators;
//	}
//
//	mt.emplace_back(luaL_Reg{ "__newindex", UserTypeNewIndex });
//	mt.emplace_back(luaL_Reg{ "__index", UserTypeIndex });
//	mt.emplace_back(luaL_Reg{ "__gc", UserTypeDestroy });
//	mt.emplace_back(luaL_Reg{ nullptr, nullptr });
//
//	// Register funcs with up values.
//	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
//	luaL_setfuncs(state, mt.data(), 1);
//
//	lua_pop(state, 1);
//
//
//	// Library Table.
//	size_t prev_index = 0;
//	size_t curr_index = friendly_name.find_first_of(':');
//
//	int32_t table_count = 0;
//
//	// Create all sub-tables.
//	do {
//		const U8String substr = friendly_name.substr(prev_index, curr_index - prev_index);
//
//		// Create first, global table.
//		if (table_count == 0) {
//			if (lua_getglobal(state, substr.data()) <= 0) {
//				lua_pop(state, 1);
//
//				lua_createtable(state, 0, 0);
//				lua_pushvalue(state, -1);
//				lua_setglobal(state, substr.data());
//			}
//
//		// Create sub-table.
//		} else {
//			if (lua_getfield(state, -1, substr.data()) <= 0) {
//				lua_pop(state, 1);
//
//				lua_createtable(state, 0, 0);
//				lua_pushvalue(state, -1);
//				lua_setfield(state, -3, substr.data());
//			}
//		}
//
//		prev_index = (curr_index != SIZE_T_FAIL) ? curr_index + 2 : SIZE_T_FAIL;
//		curr_index = friendly_name.find_first_of(':', prev_index);
//
//		++table_count;
//	} while (prev_index != SIZE_T_FAIL);
//
//
//	Vector<luaL_Reg> reg(g_allocator);
//
//	// Add constructor.
//	if ((!flags || !flags->getFlags().testAll(ScriptFlagsAttribute::Flag::ReferenceOnly))) {
//		reg.emplace_back(luaL_Reg{ "new", UserTypeNew });
//	}
//
//	reg.emplace_back(luaL_Reg{ nullptr, nullptr });
//
//	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
//	lua_setfield(state, -2, k_ref_def_field_name);
//
//	// Add static funcs.
//	for (int32_t i = 0; i < num_static_funcs; ++i) {
//		const HashStringView32<> func_name = ref_def.getStaticFuncName(i);
//
//		// Is an operator function.
//		if (!strncmp(func_name.getBuffer(), "__", 2)) {
//			continue;
//		}
//
//		lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
//		lua_pushinteger(state, i);
//		lua_pushboolean(state, true); // Is static.
//
//		lua_pushcclosure(state, UserTypeFunctionCall, 3);
//		lua_setfield(state, -2, func_name.getBuffer());
//	}
//
//	// Push up values.
//	lua_pushlightuserdata(state, const_cast<Gaff::IReflectionDefinition*>(&ref_def));
//	luaL_getmetatable(state, friendly_name.data());
//
//	luaL_setfuncs(state, reg.data(), 2);
//	lua_pop(state, table_count);
}

void RegisterTypeFinalize(JanetTable* env, const Gaff::IReflectionDefinition& ref_def, const JanetAbstractType& type_info)
{
	janet_register_abstract_type(&type_info);

	ReflectionDataJanet* const value = reinterpret_cast<ReflectionDataJanet*>(
		janet_abstract(&g_ref_def_type_info, sizeof(ReflectionDataJanet))
	);

	value->type_info = &type_info;
	value->ref_def = &ref_def;

	janet_def(env, type_info.name, janet_wrap_abstract(value), nullptr);

	const U8String type_new_func_source(U8String::CtorSprintf(), "(fn [& args] (New %s (splice args)))", type_info.name, type_info.name);
	const U8String type_new_func_name(U8String::CtorSprintf(), "%s/New", type_info.name);

	Janet func;
	janet_dostring(env, type_new_func_source.data(), nullptr, &func);

	janet_def(env, type_new_func_name.data(), func, nullptr);
}

void RegisterBuiltIns(JanetTable* env)
{
	static constexpr JanetReg cfuns[] = {
		{
			"Print", Print, "Prints a debug output string."
		},
		{
			"GetManager", GetManager, "Gets a manager of the given type from the registry."
		},
		{
			"New", UserTypeNew, "Creates an engine defined type."
		},
		{ NULL, NULL, NULL }
	};

	janet_cfuns(env, nullptr, cfuns);

	janet_register_abstract_type(&g_ref_def_type_info);
}

Janet UserTypeFunctionCall(void* data, int32_t num_args, Janet* args)
{
	ReflectionFunctionJanet* const func_wrapper = reinterpret_cast<ReflectionFunctionJanet*>(data);
	const int32_t num_overrides = (func_wrapper->is_static) ?
		func_wrapper->ref_def->getNumStaticFuncOverrides(func_wrapper->func_index) :
		func_wrapper->ref_def->getNumFuncOverrides(func_wrapper->func_index);

	Vector<Gaff::FunctionStackEntry> func_args(g_allocator);
	JanetTypeInstanceAllocator allocator;
	Gaff::FunctionStackEntry ret;

	const int32_t num_passed_args = (func_wrapper->is_static) ? num_args : (num_args - 1);
	UserData* object = nullptr;

	if (!func_wrapper->is_static) {
		// First element on the stack is our object instance.
		object = reinterpret_cast<UserData*>(janet_checkabstract(args[0], func_wrapper->type_info));

		if (!object) {
			// $TODO: Log error.
			return janet_wrap_nil();
		}
	}


	for (int32_t i = 0; i < num_overrides; ++i) {
		const Gaff::IReflectionStaticFunctionBase* const static_func = (func_wrapper->is_static) ? func_wrapper->ref_def->getStaticFunc(func_wrapper->func_index, i) : nullptr;
		const Gaff::IReflectionFunctionBase* const func = (func_wrapper->is_static) ? nullptr : func_wrapper->ref_def->getFunc(func_wrapper->func_index, i);
		const int32_t num_func_args = (func_wrapper->is_static) ? static_func->numArgs() : func->numArgs();

		if (num_func_args == num_passed_args) {
			if (num_passed_args > 0 && func_args.empty()) {
				FillArgumentStack(num_args, args, func_args, (func_wrapper->is_static) ? 0 : 1);
			}

			if (func_wrapper->is_static) {
				if (!static_func->call(func_args.data(), static_cast<int32_t>(func_args.size()), ret, allocator)) {
					continue;
				}
			} else {
				if (!func->call(object->getData(), func_args.data(), static_cast<int32_t>(func_args.size()), ret, allocator)) {
					continue;
				}
			}

			return PushReturnValue(ret, false);
		}
	}

	// $TODO: Log error. Can't find function with the correct number of arguments or argument type mismatch.
	return janet_wrap_nil();
}

void UserTypeToString(void* data, JanetBuffer* buffer)
{
	UserData* const value = reinterpret_cast<UserData*>(data);

	auto* const to_string = value->ref_def->getStaticFunc<int32_t, const void*, char*, int32_t>(
		Gaff::GetOpNameHash(Gaff::Operator::ToString)
	);

	if (to_string) {
		char temp_buffer[256] = { 0 };
		char* ptr = temp_buffer;

		if (to_string->call(value->getData(), std::forward<char*>(ptr), 256)) {
			janet_buffer_push_cstring(buffer, temp_buffer);
			return;
		}
	}

	janet_buffer_push_cstring(buffer, value->ref_def->getReflectionInstance().getName());
}

int UserTypeDestroy(void* data, size_t)
{
	UserData* const value = reinterpret_cast<UserData*>(data);

	if (!value->meta.flags.testAll(UserData::MetaData::HeaderFlag::IsReference)) {
		value->ref_def->destroyInstance(value->getData());
	}

	return 0;
}

void UserTypeNewIndex(void* data, Janet key, Janet value)
{
	UserData* const user_data = reinterpret_cast<UserData*>(data);
	void* input = user_data->getData();

	if (janet_checktype(key, JANET_KEYWORD)) {
		const char* const name = reinterpret_cast<const char*>(janet_unwrap_keyword(key));
		const Gaff::Hash32 hash = Gaff::FNV1aHash32String(name);

		// Find a variable with name.
		if (auto* const var = user_data->ref_def->getVar(hash)) {
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
						if (janet_checktype(value, JANET_NUMBER)) {
							const double value_num = janet_unwrap_number(value);
							var->setDataT(input, value_num);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<float>::GetReflectionDefinition()) {
						if (janet_checktype(value, JANET_NUMBER)) {
							const float value_num = static_cast<float>(janet_unwrap_number(value));
							var->setDataT(input, value_num);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<int64_t>::GetReflectionDefinition()) {
						if (janet_checktype(value, JANET_NUMBER)) {
							const int64_t value_num = static_cast<int64_t>(janet_unwrap_number(value));
							var->setDataT(input, value_num);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<int32_t>::GetReflectionDefinition()) {
						if (janet_checktype(value, JANET_NUMBER)) {
							const int32_t value_num = static_cast<int32_t>(janet_unwrap_number(value));
							var->setDataT(input, value_num);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<int16_t>::GetReflectionDefinition()) {
						if (janet_checktype(value, JANET_NUMBER)) {
							const int16_t value_num = static_cast<int16_t>(janet_unwrap_number(value));
							var->setDataT(input, value_num);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<int8_t>::GetReflectionDefinition()) {
						if (janet_checktype(value, JANET_NUMBER)) {
							const int8_t value_num = static_cast<int8_t>(janet_unwrap_number(value));
							var->setDataT(input, value_num);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<uint64_t>::GetReflectionDefinition()) {
						if (janet_checktype(value, JANET_NUMBER)) {
							const uint64_t value_num = static_cast<uint64_t>(janet_unwrap_number(value));
							var->setDataT(input, value_num);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<uint32_t>::GetReflectionDefinition()) {
						if (janet_checktype(value, JANET_NUMBER)) {
							const uint32_t value_num = static_cast<uint32_t>(janet_unwrap_number(value));
							var->setDataT(input, value_num);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<uint16_t>::GetReflectionDefinition()) {
						if (janet_checktype(value, JANET_NUMBER)) {
							const uint16_t value_num = static_cast<uint16_t>(janet_unwrap_number(value));
							var->setDataT(input, value_num);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<uint8_t>::GetReflectionDefinition()) {
						if (janet_checktype(value, JANET_NUMBER)) {
							const uint8_t value_num = static_cast<uint8_t>(janet_unwrap_number(value));
							var->setDataT(input, value_num);

						} else {
							// $TODO: Log error.
						}

					} else if (&var_ref_def == &Reflection<bool>::GetReflectionDefinition()) {
						if (janet_checktype(value, JANET_BOOLEAN)) {
							const bool value_bool = janet_unwrap_boolean(value);
							var->setDataT(input, value_bool);

						} else {
							// $TODO: Log error.
						}
					}

				// Is a user defined type.
				} else {
					if (user_data->ref_def == &var_ref_def) {
						const UserData* const user_data_value = reinterpret_cast<UserData*>(janet_unwrap_abstract(value));
						var->setData(input, user_data_value->getData());

					} else {
						// $TODO: Log error.
					}
				}
			}
		}

	// Index function?
	} else {
	}
}

int UserTypeIndex(void* data, Janet key, Janet* out)
{
	UserData* const value = reinterpret_cast<UserData*>(data);
	const void* input = value->getData();

	if (janet_checktype(key, JANET_KEYWORD)) {
		const char* const name = reinterpret_cast<const char*>(janet_unwrap_keyword(key));
		const Gaff::Hash32 hash = Gaff::FNV1aHash32String(name);
		
		// Find a variable with name.
		if (const auto* const var = value->ref_def->getVar(hash)) {
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
		
				// Push number to stack.
				// Opting to not differentiate between integers and floating point, as Janet handles 64-bit integers.
				// The 64-bit integer support in Janet doesn't really seem that useful, just using normal numbers.
				if (double value_num; Gaff::CastNumberToType<double>(var_ref_def, input, value_num)) {
					*out = janet_wrap_number(value_num);
					return 1;
				
				// Push bool to stack.
				} else if (&var_ref_def == &Reflection<bool>::GetReflectionDefinition()) {
					*out = WrapBoolean(*reinterpret_cast<const bool*>(input));
					return 1;

				// Push string to stack.
				} else if (&var_ref_def == &Reflection<U8String>::GetReflectionDefinition()) {
					const U8String& string = *reinterpret_cast<const U8String*>(input);
					*out = janet_cstringv(string.data());
					return 1;
		
				// Push user defined type reference.
				} else {
					JanetManager& janet_mgr = GetApp().getManagerTFast<JanetManager>();
					const JanetAbstractType* const type_info = janet_mgr.getType(var_ref_def);

					if (!type_info) {
						// $TODO: Log error.
						return 0;
					}

					*out = PushUserTypeReference(input, var_ref_def, *type_info);
					return 1;
				}
			}
		
		// Find function with name.
		} else if (int32_t func_index = value->ref_def->getFuncIndex(hash); func_index > -1) {
			// $TODO: Push abstract type that handles function calls.
			ReflectionFunctionJanet* const func = reinterpret_cast<ReflectionFunctionJanet*>(
				janet_abstract(&g_ref_func_type_info, sizeof(ReflectionFunctionJanet))
			);

			func->type_info = janet_abstract_type(value);
			func->ref_def = value->ref_def;
			func->func_index = func_index;
			func->is_static = false;

			*out = janet_wrap_abstract(func);

			return 1;
		
		// Last resort, use the type's registered index function.
		} else if (func_index = value->ref_def->getStaticFuncIndex(Gaff::GetOpNameHash(Gaff::Operator::Index)); func_index > -1) {
			const int32_t num_overloads = value->ref_def->getNumStaticFuncOverrides(func_index);
		
			Vector<Gaff::FunctionStackEntry> args(g_allocator);
			JanetTypeInstanceAllocator allocator;
			Gaff::FunctionStackEntry ret;
		
			for (int32_t i = 0; i < num_overloads; ++i) {
				const Gaff::IReflectionStaticFunctionBase* const static_func = value->ref_def->getStaticFunc(func_index, i);
		
				if (static_func->numArgs() == 1) {
					if (args.empty()) {
						FillArgumentStack(1, &key, args);
					}
		
					if (!static_func->call(args.data(), static_cast<int32_t>(args.size()), ret, allocator)) {
						continue;
					}
		
					*out = PushReturnValue(ret, false);
					return 1;
				}
			}
		
			// $TODO: Log error. Can't find index function with the correct number of arguments or argument type mismatch.
		}

		// $TODO: Log error. Can't find anything at index.

	// Non-string type.
	} else {
		// Use the type's registered index function.
		if (const int32_t func_index = value->ref_def->getStaticFuncIndex(Gaff::GetOpNameHash(Gaff::Operator::Index)); func_index > -1) {
			const int32_t num_overloads = value->ref_def->getNumStaticFuncOverrides(func_index);
	
			Vector<Gaff::FunctionStackEntry> args(g_allocator);
			JanetTypeInstanceAllocator allocator;
			Gaff::FunctionStackEntry ret;
	
			for (int32_t i = 0; i < num_overloads; ++i) {
				const Gaff::IReflectionStaticFunctionBase* const static_func = value->ref_def->getStaticFunc(func_index, i);
	
				if (static_func->numArgs() == 1) {
					if (args.empty()) {
						FillArgumentStack(1, &key, args);
					}
	
					if (!static_func->call(args.data(), static_cast<int32_t>(args.size()), ret, allocator)) {
						continue;
					}
	
					*out = PushReturnValue(ret, false);
					return 1;
				}
			}
	
			// $TODO: Log error. Can't find index function with the correct number of arguments or argument type mismatch.
		}
	}

	return 0;
}

Janet UserTypeNew(int32_t num_args, Janet* args)
{
	janet_arity(num_args, 1, -1);

	const ReflectionDataJanet* const ref_data = reinterpret_cast<ReflectionDataJanet*>(janet_checkabstract(args[0], &g_ref_def_type_info));

	// Not an ReflectionDefinition.
	if (!ref_data) {
		// $TODO: Log error.
		return janet_wrap_nil();
	}

	const auto* const flags = ref_data->ref_def->getClassAttr<ScriptFlagsAttribute>();

	if (flags && flags->getFlags().testAll(ScriptFlagsAttribute::Flag::ReferenceOnly)) {
		// $TODO: Log error.
		return janet_wrap_nil();
	}

	UserData* value = nullptr;
	const Janet ret_value = PushUserType(*ref_data->ref_def, *ref_data->type_info, &value);

	Gaff::FunctionStackEntry obj;
	obj.value.vp = value->getData();
	obj.ref_def = ref_data->ref_def;

	Vector<Gaff::FunctionStackEntry> arg_stack(g_allocator);
	JanetTypeInstanceAllocator allocator;
	Gaff::FunctionStackEntry ret;

	FillArgumentStack(num_args, args, arg_stack, 1);

	arg_stack.insert(arg_stack.begin(), obj);

	// Initialize our data.
	const int32_t num_ctor_args = static_cast<int32_t>(arg_stack.size());
	const int32_t num_ctors = ref_data->ref_def->getNumConstructors();

	for (int32_t i = 0; i < num_ctors; ++i) {
		auto* const ctor = ref_data->ref_def->getConstructor(i);

		if (ctor->numArgs() == num_ctor_args) {
			if (ctor->call(arg_stack.data(), num_ctor_args, ret, allocator)) {
				break;
			}
		}
	}

	return ret_value;
}

NS_END
