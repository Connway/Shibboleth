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

NS_REFLECTION

template <class T>
struct IsVectorHelper final
{
	using type = T;
	static constexpr bool value = false;
};

template <class T, class Allocator>
struct IsVectorHelper< Gaff::Vector<T, Allocator> > final
{
	using type = typename std::remove_const<T>::type;
	static constexpr bool value = true;
};

template <class T>
static constexpr bool IsVector = IsVectorHelper<T>::value;

template <class T>
using IsVectorType = typename IsVectorHelper<T>::type;


template <class T>
struct IsU8StringRefHelper final
{
	static constexpr bool value = false;
};

template <class Allocator>
struct IsU8StringRefHelper<const Gaff::U8String<Allocator>&> final
{
	static constexpr bool value = true;
};

template <class T>
struct IsU8StringHelper final
{
	static constexpr bool value = false;
};

template <class Allocator>
struct IsU8StringHelper< Gaff::U8String<Allocator> > final
{
	static constexpr bool value = true;
};

template <class T>
static constexpr bool IsU8StringRef = IsU8StringRefHelper<T>::value;

template <class T>
static constexpr bool IsU8String = IsU8StringHelper<T>::value;


template <class T>
struct IsHashStringViewHelper final
{
	static constexpr bool value = false;
};

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc>
struct IsHashStringViewHelper< const Gaff::HashStringView<T, HashType, HashingFunc>& > final
{
	static constexpr bool value = true;
};

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc>
struct IsHashStringViewHelper< Gaff::HashStringView<T, HashType, HashingFunc> > final
{
	static constexpr bool value = true;
};

template <class T>
static constexpr bool IsHashStringView = IsHashStringViewHelper<T>::value;


template <class First, class... Rest>
static constexpr bool IsLastRefDef(void)
{
	if constexpr (sizeof...(Rest) > 0) {
		return IsLastRefDef<Rest...>();
	} else {
		return std::is_same<const IReflectionDefinition&, First>::value || std::is_same<const IReflectionDefinition*, First>::value;
	}
}

template <class First, class... Rest>
static constexpr bool IsLastReference(void)
{
	if constexpr (sizeof...(Rest) > 0) {
		return IsLastReference<Rest...>();
	} else {
		return std::is_reference<First>::value;
	}
}

template <class First, class... Rest>
static auto&& GetLastArg(First&& first, Rest&&... rest)
{
	if constexpr (sizeof...(Rest) > 0) {
		GAFF_REF(first);
		return GetLastArg<Rest...>(std::forward<Rest>(rest)...);
	} else {
		return std::forward<First>(first);
	}
}


template <class T>
static bool CastNumberToType(const IReflectionDefinition& ref_def, const void* in, T& out)
{
	return CastFloatToType(ref_def, in, out) || CastIntegerToType(ref_def, in, out);
}

template <class T>
static bool CastFloatToType(const IReflectionDefinition& ref_def, const void* in, T& out)
{
	if (&ref_def == &Reflection<double>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const double*>(in));
		return true;
	} else if (&ref_def == &Reflection<float>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const float*>(in));
		return true;
	}

	return false;
}

template <class T>
static bool CastUnsignedIntegerToType(const IReflectionDefinition& ref_def, const void* in, T& out)
{
	if (&ref_def == &Reflection<uint64_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint64_t*>(in));
		return true;
	} else if (&ref_def == &Reflection<uint32_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint32_t*>(in));
		return true;
	} else if (&ref_def == &Reflection<uint16_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint16_t*>(in));
		return true;
	} else if (&ref_def == &Reflection<uint8_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint8_t*>(in));
		return true;
	}

	return false;
}

template <class T>
static bool CastSignedIntegerToType(const IReflectionDefinition& ref_def, const void* in, T& out)
{
	if (&ref_def == &Reflection<int64_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int64_t*>(in));
		return true;
	} else if (&ref_def == &Reflection<int32_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int32_t*>(in));
		return true;
	} else if (&ref_def == &Reflection<int16_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int16_t*>(in));
		return true;
	} else if (&ref_def == &Reflection<int8_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int8_t*>(in));
		return true;
	}

	return false;
}

template <class T>
static bool CastIntegerToType(const IReflectionDefinition& ref_def, const void* in, T& out)
{
	return CastUnsignedIntegerToType(ref_def, in, out) || CastSignedIntegerToType(ref_def, in, out);
}

template <class T>
static bool CastNumberToType(const FunctionStackEntry& entry, T& out)
{
	return CastNumberToType(*entry.ref_def, &entry.value, out);
}

template <class T>
static bool CastFloatToType(const FunctionStackEntry& entry, T& out)
{
	return CastFloatToType(*entry.ref_def, &entry.value, out);
}

template <class T>
static bool CastUnsignedIntegerToType(const FunctionStackEntry& entry, T& out)
{
	return CastUnsignedIntegerToType(*entry.ref_def, &entry.value, out);
}

template <class T>
static bool CastSignedIntegerToType(const FunctionStackEntry& entry, T& out)
{
	return CastSignedIntegerToType(*entry.ref_def, &entry.value, out);
}

template <class T>
static bool CastIntegerToType(const FunctionStackEntry& entry, T& out)
{
	return CastIntegerToType(*entry.ref_def, &entry.value, out);
}


template <class T>
template <class Callable, class Ret, class First, class... Rest, class... CurrentArgs>
bool ReflectionDefinition<T>::CallFuncStackHelper(
	const Callable& callable,
	void* object,
	const FunctionStackEntry* args,
	FunctionStackEntry& ret,
	int32_t arg_index,
	IFunctionStackAllocator& allocator,
	CurrentArgs&&... current_args)
{
	using ArgType = std::remove_pointer_t< std::decay_t<First> >;
	using FinalType = IsVectorType<ArgType>;
	constexpr bool is_vector = IsVector<ArgType>;

	// $TODO: Add vector support.

	if constexpr (
		std::is_same<const char*, First>::value ||
		std::is_same<const char8_t*, First>::value ||
		IsU8StringRef<First> ||
		IsU8String<First> ||
		IsHashStringView<First>) {

		// Get current value from arg stack.
		const FunctionStackEntry& entry = args[arg_index];
		const char8_t* str_u8 = nullptr;
		const char* str = nullptr;

		if (entry.flags.testAll(FunctionStackEntry::Flag::IsStringU8)) {
			str_u8 = reinterpret_cast<char8_t*>(entry.value.vp);
			str = reinterpret_cast<const char*>(str_u8);

		} else if (entry.flags.testAll(FunctionStackEntry::Flag::IsString)) {
			str = reinterpret_cast<char*>(entry.value.vp);

		// It's a U8String.
		} else if (Gaff::Find(entry.ref_def->getReflectionInstance().getName(), u8"U8String") != GAFF_SIZE_T_FAIL) {
			// I think this *SHOULD* be safe, since the string data is stored in the first part. So even if the allocator is a different size,
			// the memory for the actual string should be in the same offset.
			str_u8 = reinterpret_cast<Shibboleth::U8String*>(entry.value.vp)->data();
		}

		if (!str_u8 && !str) {
			// $TODO: Log error.
			return false;
		}

		if constexpr (sizeof...(Rest) > 0) {
			if constexpr (std::is_same<const char8_t*, First>::value) {
				if (str_u8) {
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(str));
				} else {
					CONVERT_STRING(char8_t, temp_str, str);
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(temp_str));
				}

			} else if constexpr (std::is_same<const char*, First>::value) {
				return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(str));

			} else {
				if constexpr (std::is_constructible<ArgType, const char8_t*>::value) {
					if (str_u8) {
						ArgType value(str_u8);
						return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
					} else {
						CONVERT_STRING(char8_t, temp_str, str);
						const char8_t* const beg = temp_str;
						ArgType value(beg);
						return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
					}
				} else {
					ArgType value(str);
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				}
			}

		} else {
			if constexpr (std::is_same<const char8_t*, First>::value) {
				if (str_u8) {
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(str));
				} else {
					CONVERT_STRING(char8_t, temp_str, str);
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(temp_str));
				}

			} else if constexpr (std::is_same<const char*, First>::value) {
				return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(str));

			} else {
				if constexpr (std::is_constructible<ArgType, const char8_t*>::value) {
					if (str_u8) {
						ArgType value(str_u8);
						return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
					} else {
						CONVERT_STRING(char8_t, temp_str, str);
						const char8_t* const beg = temp_str;
						ArgType value(beg);
						return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
					}
				} else {
					ArgType value(str);
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				}
			}
		}

	} else if constexpr (std::is_same<const IReflectionDefinition&, First>::value || std::is_same<const IReflectionDefinition*, First>::value) {
		// Get current value from arg stack.
		const FunctionStackEntry& entry = args[arg_index];

		if (!entry.ref_def) {
			// $TODO: Log error.
			return false;
		}

		if constexpr (std::is_reference<First>::value) {
			return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(*entry.ref_def));
		} else {
			return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(entry.ref_def));
		}

	// We don't support passing in void pointers or unreflected types as arguments.
	} else if constexpr (std::is_void<FinalType>::value || !Reflection<FinalType>::HasReflection) {
		GAFF_REF(callable, object, args, ret, arg_index, allocator);
		Gaff::VarArgRef<CurrentArgs...>(std::forward<CurrentArgs>(current_args)...);

		// $TODO: Log error.
		return false;

	} else {
		// Get current value from arg stack.
		const FunctionStackEntry& entry = args[arg_index];

		if constexpr (!is_vector && std::is_enum<FinalType>::value) {
			//if (entry.enum_ref_def != &Reflection<FinalType>::GetReflectionDefinition()) {
			//	// $TODO: Log error.
			//	return FunctionStackEntry();
			//}

			FinalType value;

			if (!CastNumberToType<FinalType>(entry, value)) {
				// $TOOD: Log error.
				return false;
			}

			if constexpr (sizeof...(Rest) > 0) {
				if constexpr (std::is_pointer<First>::value) {
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(&value));
				} else {
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				}

			} else {
				if constexpr (std::is_pointer<First>::value) {
					return CallFuncStackHelper<Callable, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(&value));
				} else {
					return CallFuncStackHelper<Callable, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				}
			}

		} else if constexpr (!is_vector && ReflectionDefinition<FinalType>::IsBuiltIn()) {
			FinalType value;

			if (entry.ref_def == &Reflection<bool>::GetReflectionDefinition()) {
				value = static_cast<FinalType>(entry.value.b);

			} else {
				if (!CastNumberToType<FinalType>(entry, value)) {
					// $TOOD: Log error.
					return false;
				}
			}

			if constexpr (sizeof...(Rest) > 0) {
				if constexpr (std::is_pointer<First>::value) {
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(&value));
				} else {
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				}

			} else {
				if constexpr (std::is_pointer<First>::value) {
					return CallFuncStackHelper<Callable, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(&value));
				} else {
					return CallFuncStackHelper<Callable, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				}
			}

		} else if constexpr (is_vector) {
			GAFF_TEMPLATE_STATIC_ASSERT(false, "Currently can't pass vector's to functions from script.");

		} else {
			if (entry.ref_def != &Reflection<FinalType>::GetReflectionDefinition()) {
				// $TOOD: Log error.
				return false;
			}

			FinalType* value = reinterpret_cast<FinalType*>(entry.value.vp);

			if constexpr (sizeof...(Rest) > 0) {
				if constexpr (std::is_pointer<First>::value) {
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				} else {
					return CallFuncStackHelper<Callable, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(*value));
				}

			} else {
				if constexpr (std::is_pointer<First>::value) {
					return CallFuncStackHelper<Callable, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				} else {
					return CallFuncStackHelper<Callable, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(*value));
				}
			}
		}
	}
}

template <class T>
template <class Callable, class Ret, class... CurrentArgs>
bool ReflectionDefinition<T>::CallFuncStackHelper(
	const Callable& callable,
	void* object,
	FunctionStackEntry& ret,
	IFunctionStackAllocator& allocator,
	CurrentArgs&&... current_args)
{
	if constexpr (std::is_void<Ret>::value) {
		CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...);
		GAFF_REF(ret);

	} else if constexpr (std::is_same<const void*, Ret>::value || std::is_same<void*, Ret>::value) {
		if constexpr (IsLastRefDef<CurrentArgs...>()) {
			if constexpr (IsLastReference<CurrentArgs...>()) {
				ret.ref_def = &GetLastArg(std::forward<CurrentArgs>(current_args)...);
			} else {
				ret.ref_def = GetLastArg(std::forward<CurrentArgs>(current_args)...);
			}

			ret.value.vp = CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...);

		} else {
			GAFF_TEMPLATE_STATIC_ASSERT(false, "Cannot return void* when we don't know what to cast to. Last arg to function must be a const IReflectionDefinition[&/*].");
		}

	} else if constexpr (std::is_same<const char8_t*, Ret>::value) {
		ret.flags.set(true, FunctionStackEntry::Flag::IsStringU8, FunctionStackEntry::Flag::IsString, FunctionStackEntry::Flag::IsReference);
		ret.value.vp = const_cast<char8_t*>(CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...));

	} else if constexpr (std::is_same<const char*, Ret>::value) {
		ret.flags.set(true, FunctionStackEntry::Flag::IsString, FunctionStackEntry::Flag::IsReference);
		ret.value.vp = const_cast<char*>(CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...));

	} else if constexpr (IsU8StringRef<Ret> || IsU8String<Ret>) {
		Ret string = CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...);
		ret.flags.set(true, FunctionStackEntry::Flag::IsStringU8, FunctionStackEntry::Flag::IsString);

		if constexpr (IsU8StringRef<Ret>) {
			ret.flags.set(true, FunctionStackEntry::Flag::IsStringU8, FunctionStackEntry::Flag::IsString, FunctionStackEntry::Flag::IsReference);
			ret.value.vp = const_cast<char8_t*>(string.data());

		} else {
			ret.value.vp = GAFF_ALLOC(string.size() + 1, allocator);
			memcpy(ret.value.vp, string.data(), string.size() + 1);
		}

	} else if constexpr (std::is_same<const IReflectionDefinition&, Ret>::value || std::is_same<const IReflectionDefinition*, Ret>::value) {
		Ret ref_def = CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...);

		if constexpr (std::is_reference<Ret>::value) {
			ret.ref_def = &ref_def;
		} else {
			ret.ref_def = ref_def;
		}

	} else {
		using RetType = std::remove_pointer_t< std::decay_t<Ret> >;
		using FinalType = IsVectorType<RetType>;
		constexpr bool is_vector = IsVector<RetType>;

		if constexpr (!is_vector && std::is_enum<FinalType>::value) {
			RetType value;

			if constexpr (std::is_pointer<Ret>::value) {
				value = *CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...);
			} else {
				value = CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...);
			}

			ret.enum_ref_def = &Reflection<RetType>::GetReflectionDefinition();
			ret.value.i64 = static_cast<int64_t>(value);

		} else if constexpr (!is_vector && ReflectionDefinition<FinalType>::IsBuiltIn()) {
			RetType value;

			if constexpr (std::is_pointer<Ret>::value) {
				value = *CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...);
			} else {
				value = CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...);
			}

			ret.ref_def = &Reflection<RetType>::GetReflectionDefinition();
			*reinterpret_cast<RetType*>(&ret.value.vp) = value;

		} else {
			if constexpr (std::is_enum<FinalType>::value) {
				ret.enum_ref_def = &Reflection<FinalType>::GetReflectionDefinition();
			} else {
				ret.ref_def = &Reflection<FinalType>::GetReflectionDefinition();
			}

			if constexpr (std::is_pointer_v<Ret> || std::is_reference_v<Ret>) {
				ret.flags.set(true, FunctionStackEntry::Flag::IsReference);

				if constexpr (std::is_pointer<Ret>::value) {
					ret.value.vp = const_cast<RetType*>(CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...));

				// Is a reference.
				} else {
					ret.value.vp = const_cast<RetType*>(&CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...));
				}

				if constexpr (is_vector) {
					const size_t size = reinterpret_cast<RetType*>(ret.value.vp)->size();
					ret.value.arr.data = reinterpret_cast<RetType*>(ret.value.vp)->data();
					ret.value.arr.size = static_cast<int32_t>(size);

					ret.flags.set(true, FunctionStackEntry::Flag::IsArray);

				} else if constexpr (std::is_base_of<IReflectionObject, RetType>::value) {
					ret.ref_def = &reinterpret_cast<RetType*>(ret.value.vp)->getReflectionDefinition();
					ret.value.vp = reinterpret_cast<RetType*>(ret.value.vp)->getBasePointer();
				}

			} else {
				RetType value = CallCallableStackHelper(callable, object, std::forward<CurrentArgs>(current_args)...);

				if constexpr (is_vector) {
					GAFF_TEMPLATE_STATIC_ASSERT(false, "Stack functions do not support returning a vector by value.");

					//ret.value.arr.data = GAFF_ALLOC(sizeof(FinalType) * value.size(), allocator);
					//ret.flags.set(true, FunctionStackEntry::Flag::IsArray);

					//FinalType* data = reinterpret_cast<FinalType*>(ret.value.arr.data);

					//for (auto& v : value) {
					//	new(data) FinalType(std::move(v));
					//	++data;
					//}

				} else {
					ret.value.vp = allocator.alloc(*ret.ref_def);
					new(ret.value.vp) RetType(value);
				}
			}
		}
	}

	return true;
}

template <class T>
template <bool is_const, class Ret, class... Args, class... CurrentArgs>
Ret ReflectionDefinition<T>::CallCallableStackHelper(const ReflectionExtensionFunction<is_const, Ret, Args...>& func, void* object, CurrentArgs&&... current_args)
{
	return func.call(object, std::forward<CurrentArgs>(current_args)...);
}

template <class T>
template <bool is_const, class Ret, class... Args, class... CurrentArgs>
Ret ReflectionDefinition<T>::CallCallableStackHelper(const ReflectionFunction<is_const, Ret, Args...>& func, void* object, CurrentArgs&&... current_args)
{
	return func.call(object, std::forward<CurrentArgs>(current_args)...);
}

template <class T>
template <class Ret, class... Args, class... CurrentArgs>
Ret ReflectionDefinition<T>::CallCallableStackHelper(const ReflectionStaticFunction<Ret, Args...>& func, void*, CurrentArgs&&... current_args)
{
	return func.call(std::forward<CurrentArgs>(current_args)...);
}

NS_END
