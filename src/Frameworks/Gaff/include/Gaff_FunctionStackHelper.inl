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

NS_GAFF

template <class T>
static bool CastNumberToType(const IReflectionDefinition& ref_def, const void* in, T& out)
{
	if (&ref_def == &Shibboleth::Reflection<double>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const double*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<float>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const float*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<uint64_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint64_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<uint32_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint32_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<uint16_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint16_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<uint8_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint8_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<int64_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int64_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<int32_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int32_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<int16_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int16_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<int8_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int8_t*>(in));
		return true;
	}

	return false;
}

template <class T>
static bool CastFloatToType(const IReflectionDefinition& ref_def, const void* in, T& out)
{
	if (&ref_def == &Shibboleth::Reflection<double>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const double*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<float>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const float*>(in));
		return true;
	}

	return false;
}

template <class T>
static bool CastIntegerToType(const IReflectionDefinition& ref_def, const void* in, T& out)
{
	if (&ref_def == &Shibboleth::Reflection<uint64_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint64_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<uint32_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint32_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<uint16_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint16_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<uint8_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const uint8_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<int64_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int64_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<int32_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int32_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<int16_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int16_t*>(in));
		return true;
	} else if (&ref_def == &Shibboleth::Reflection<int8_t>::GetReflectionDefinition()) {
		out = static_cast<T>(*reinterpret_cast<const int8_t*>(in));
		return true;
	}

	return false;
}

template <class T>
static bool CastNumberToType(const FunctionStackEntry& entry, T& out)
{
	return CastNumberToType<T>(*entry.ref_def, &entry.value, out);
}

template <class T>
static bool CastFloatToType(const FunctionStackEntry& entry, T& out)
{
	return CastFloatToType<T>(*entry.ref_def, &entry.value, out);
}

template <class T>
static bool CastIntegerToType(const FunctionStackEntry& entry, T& out)
{
	return CastIntegerToType<T>(*entry.ref_def, &entry.value, out);
}


template <class Callable, class Allocator, class Ret, class First, class... Rest, class... CurrentArgs>
bool CallFunc(
	const Callable& callable,
	void* object,
	const FunctionStackEntry* args, 
	FunctionStackEntry& ret,
	int32_t arg_index,
	IAllocator& allocator,
	CurrentArgs&&... current_args)
{
	using ArgType = typename std::remove_const< typename std::remove_pointer< typename std::remove_reference<First>::type >::type >::type;

	// We don't support passing in void pointers or unreflected types as arguments.
	if constexpr (std::is_void<ArgType>::value || !GAFF_REFLECTION_NAMESPACE::Reflection<ArgType>::HasReflection) {
		GAFF_REF(callable, object, args, ret, arg_index, allocator);
		VarArgRef<CurrentArgs...>(std::forward<CurrentArgs>(current_args)...);

		// $TODO: Log error.
		return false;

	} else {
		// Get current value from arg stack.
		const FunctionStackEntry& entry = args[arg_index];

		if constexpr (std::is_enum<ArgType>::value) {
			//if (entry.enum_ref_def != &GAFF_REFLECTION_NAMESPACE::Reflection<ArgType>::GetReflectionDefinition()) {
			//	// $TODO: Log error.
			//	return FunctionStackEntry();
			//}

			ArgType value;

			if (!CastNumberToType<ArgType>(entry, value)) {
				// $TOOD: Log error.
				return false;
			}

			if constexpr (sizeof...(Rest) > 0) {
				if constexpr (std::is_pointer<First>::value) {
					return CallFunc<Callable, Allocator, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(&value));
				} else {
					return CallFunc<Callable, Allocator, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				}

			} else {
				if constexpr (std::is_pointer<First>::value) {
					return CallFunc<Callable, Allocator, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(&value));
				} else {
					return CallFunc<Callable, Allocator, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				}
			}

		} else if constexpr (ReflectionDefinition<ArgType, Allocator>::IsBuiltIn()) {
			ArgType value;

			if (entry.ref_def == &GAFF_REFLECTION_NAMESPACE::Reflection<bool>::GetReflectionDefinition()) {
				value = static_cast<ArgType>(entry.value.b);

			} else {
				if (!Gaff::CastNumberToType<ArgType>(entry, value)) {
					// $TOOD: Log error.
					return false;
				}
			}

			if constexpr (sizeof...(Rest) > 0) {
				if constexpr (std::is_pointer<First>::value) {
					return CallFunc<Callable, Allocator, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(&value));
				} else {
					return CallFunc<Callable, Allocator, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				}

			} else {
				if constexpr (std::is_pointer<First>::value) {
					return CallFunc<Callable, Allocator, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(&value));
				} else {
					return CallFunc<Callable, Allocator, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				}
			}

		} else {
			if (entry.ref_def != &GAFF_REFLECTION_NAMESPACE::Reflection<ArgType>::GetReflectionDefinition()) {
				// $TOOD: Log error.
				return false;
			}

			ArgType* const value = reinterpret_cast<ArgType*>(entry.value.vp);

			if constexpr (sizeof...(Rest) > 0) {
				if constexpr (std::is_pointer<First>::value) {
					return CallFunc<Callable, Allocator, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				} else {
					return CallFunc<Callable, Allocator, Ret, Rest...>(callable, object, args, ret, arg_index + 1, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(*value));
				}

			} else {
				if constexpr (std::is_pointer<First>::value) {
					return CallFunc<Callable, Allocator, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(value));
				} else {
					return CallFunc<Callable, Allocator, Ret>(callable, object, ret, allocator, std::forward<CurrentArgs>(current_args)..., std::forward<First>(*value));
				}
			}
		}
	}
}

template <class Callable, class Allocator, class Ret, class... CurrentArgs>
bool CallFunc(
	const Callable& callable,
	void* object,
	FunctionStackEntry& ret,
	IAllocator& allocator,
	CurrentArgs&&... current_args)
{
	if constexpr (std::is_void<Ret>::value) {
		CallCallable(callable, object, std::forward<CurrentArgs>(current_args)...);
		GAFF_REF(ret);

	} else {
		using RetType = typename std::remove_const< typename std::remove_pointer< typename std::remove_reference<Ret>::type >::type >::type;
		RetType return_value;

		if constexpr (std::is_pointer<Ret>::value) {
			return_value = *CallCallable(callable, object, std::forward<CurrentArgs>(current_args)...);
		} else {
			return_value = CallCallable(callable, object, std::forward<CurrentArgs>(current_args)...);
		}

		if constexpr (std::is_enum<RetType>::value) {
			ret.enum_ref_def = &GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::GetReflectionDefinition();
			ret.value.i64 = static_cast<int64_t>(return_value);

		} else if constexpr (ReflectionDefinition<RetType, Allocator>::IsBuiltIn()) {
			ret.ref_def = &GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::GetReflectionDefinition();
			*reinterpret_cast<RetType*>(&ret.value.vp) = return_value;

		} else {
			ret.ref_def = &GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::GetReflectionDefinition();
			ret.value.vp = GAFF_ALLOCT(RetType, allocator, return_value);
		}
	}

	return true;
}

//template <class Callable, class Allocator, class Ret, class... CurrentArgs>
//bool CallFunc(
//	const Callable* callable,
//	void* object,
//	FunctionStackEntry& ret,
//	IAllocator& allocator)
//{
//	if constexpr (std::is_void<Ret>::value) {
//		call(object);
//		GAFF_REF(ret);
//
//	} else {
//		using RetType = typename std::remove_const< typename std::remove_pointer< typename std::remove_reference<Ret>::type >::type >::type;
//
//		Ret return_value = call(object);
//		GAFF_REF(ret, return_value);
//
//		if constexpr (std::is_enum<RetType>::value) {
//			ret.enum_ref_def = &GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::GetReflectionDefinition();
//			*reinterpret_cast<RetType*>(&ret.value.vp) = return_value;
//
//		} else if constexpr (ReflectionDefinition<RetType, Allocator>::IsBuiltIn()) {
//			ret.ref_def = &GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::GetReflectionDefinition();
//			*reinterpret_cast<RetType*>(&ret.value.vp) = return_value;
//
//		} else {
//			ret.ref_def = &GAFF_REFLECTION_NAMESPACE::Reflection<RetType>::GetReflectionDefinition();
//			reinterpret_cast<RetType*>(ret.value.vp) = GAFF_ALLOCT(RetType, const_cast<IReflectionDefinition*>(ret.ref_def)->getAllocator(), return_value);
//		}
//	}
//
//	return true;
//}

template <class Ret, class... Args, class... CurrentArgs>
Ret CallCallable(const IReflectionFunction<Ret, Args...>& func, void* object, CurrentArgs&&... current_args)
{
	return func.call(object, std::forward<CurrentArgs>(current_args)...);
}
template <class Ret, class... Args, class... CurrentArgs>
Ret CallCallable(const IReflectionStaticFunction<Ret, Args...>& func, void*, CurrentArgs&&... current_args)
{
	return func.call(std::forward<CurrentArgs>(current_args)...);
}

NS_END
