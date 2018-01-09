/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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


NS_SHIBBOLETH

template <class T>
constexpr asETypeIdFlags GetTypeFlag(void)
{
	return static_cast<asETypeIdFlags>(-1);
}

template <>
constexpr asETypeIdFlags GetTypeFlag<bool>(void)
{
	return asTYPEID_BOOL;
}

template <>
constexpr asETypeIdFlags GetTypeFlag<int8_t>(void)
{
	return asTYPEID_INT8;
}

template <>
constexpr asETypeIdFlags GetTypeFlag<int16_t>(void)
{
	return asTYPEID_INT16;
}

template <>
constexpr asETypeIdFlags GetTypeFlag<int32_t>(void)
{
	return asTYPEID_INT32;
}

template <>
constexpr asETypeIdFlags GetTypeFlag<int64_t>(void)
{
	return asTYPEID_INT64;
}

template <>
constexpr asETypeIdFlags GetTypeFlag<uint8_t>(void)
{
	return asTYPEID_UINT8;
}

template <>
constexpr asETypeIdFlags GetTypeFlag<uint16_t>(void)
{
	return asTYPEID_UINT16;
}

template <>
constexpr asETypeIdFlags GetTypeFlag<uint32_t>(void)
{
	return asTYPEID_UINT32;
}

template <>
constexpr asETypeIdFlags GetTypeFlag<uint64_t>(void)
{
	return asTYPEID_UINT64;
}

template <>
constexpr asETypeIdFlags GetTypeFlag<float>(void)
{
	return asTYPEID_FLOAT;
}

template <>
constexpr asETypeIdFlags GetTypeFlag<double>(void)
{
	return asTYPEID_DOUBLE;
}

template <class T>
inline void PushArg(asIScriptContext* /*context*/, int32_t /*index*/, T /*value*/)
{
}

template <>
inline void PushArg<bool>(asIScriptContext* context, int32_t index, bool value)
{
	const asBYTE converted_value = *reinterpret_cast<asBYTE*>(&value);
	context->SetArgByte(static_cast<asUINT>(index), converted_value);
}

template <>
inline void PushArg<int8_t>(asIScriptContext* context, int32_t index, int8_t value)
{
	const asBYTE converted_value = *reinterpret_cast<asBYTE*>(&value);
	context->SetArgByte(static_cast<asUINT>(index), converted_value);
}

template <>
inline void PushArg<int16_t>(asIScriptContext* context, int32_t index, int16_t value)
{
	const asWORD converted_value = *reinterpret_cast<asWORD*>(&value);
	context->SetArgWord(static_cast<asUINT>(index), converted_value);
}

template <>
inline void PushArg<int32_t>(asIScriptContext* context, int32_t index, int32_t value)
{
	const asDWORD converted_value = *reinterpret_cast<asDWORD*>(&value);
	context->SetArgDWord(static_cast<asUINT>(index), converted_value);
}

template <>
inline void PushArg<int64_t>(asIScriptContext* context, int32_t index, int64_t value)
{
	const asQWORD converted_value = *reinterpret_cast<asQWORD*>(&value);
	context->SetArgQWord(static_cast<asUINT>(index), converted_value);
}

template <>
inline void PushArg<uint8_t>(asIScriptContext* context, int32_t index, uint8_t value)
{
	context->SetArgByte(static_cast<asUINT>(index), value);
}

template <>
inline void PushArg<uint16_t>(asIScriptContext* context, int32_t index, uint16_t value)
{
	context->SetArgWord(static_cast<asUINT>(index), value);
}

template <>
inline void PushArg<uint32_t>(asIScriptContext* context, int32_t index, uint32_t value)
{
	context->SetArgDWord(static_cast<asUINT>(index), value);
}

template <>
inline void PushArg<uint64_t>(asIScriptContext* context, int32_t index, uint64_t value)
{
	context->SetArgQWord(static_cast<asUINT>(index), value);
}


template <bool is_ptr, bool is_ref>
struct PushObjectHelper;

template <>
struct PushObjectHelper<true, false>
{
	template <class T>
	static void Push(asIScriptContext* context, int32_t index, T* ptr)
	{
		context->SetArgObject(static_cast<asUINT>(index), ptr);
	}
};

template <>
struct PushObjectHelper<false, true>
{
	template <class T>
	static void Push(asIScriptContext* context, int32_t index, T& ref)
	{
		T* const ptr = &ref;
		context->SetArgAddress(static_cast<asUINT>(index), ptr);
	}
};

template <>
struct PushObjectHelper<false, false>
{
	template <class T>
	static void Push(asIScriptContext* context, int32_t index, T value)
	{
		PushArg(context, index, value);
	}
};


template <class T>
const T& AngelScriptComponent::getProperty(const char* name) const
{
	return getProperty<T>(Gaff::FNV1aHash32String(name));
}

template <class T>
const T& AngelScriptComponent::getProperty(Gaff::Hash32 name) const
{
	auto it = _property_map.find(name);
	GAFF_ASSERT(it != _property_map.end());
	GAFF_ASSERT(it->second.type_id > asTYPEID_DOUBLE || it->second.type_id == GetTypeFlag<T>());
	return *reinterpret_cast<const T*>(it->second.property);
}

template <class T>
const T& AngelScriptComponent::getProperty(int32_t) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_property_map.size()));
	auto it = _property_map.begin() + index;
	GAFF_ASSERT(it->second.type_id > asTYPEID_DOUBLE || it->second.type_id == GetTypeFlag<T>());
	return *reinterpret_cast<const T*>(it->second.property);
}

template <class T>
T& AngelScriptComponent::getProperty(const char* name)
{
	return getProperty<T>(Gaff::FNV1aHash32String(name));
}

template <class T>
T& AngelScriptComponent::getProperty(Gaff::Hash32 name)
{
	auto it = _property_map.find(name);
	GAFF_ASSERT(it != _property_map.end());
	GAFF_ASSERT(it->second.type_id > asTYPEID_DOUBLE || it->second.type_id == GetTypeFlag<T>());
	return *reinterpret_cast<T*>(it->second.property);
}

template <class T>
T& AngelScriptComponent::getProperty(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_property_map.size()));
	auto it = _property_map.begin() + index;
	GAFF_ASSERT(it->second.type_id > asTYPEID_DOUBLE || it->second.type_id == GetTypeFlag<T>());
	return *reinterpret_cast<T*>(it->second.property);
}

template <class T>
void AngelScriptComponent::setArg(int32_t index, T value)
{
	static_assert(
		std::is_arithmetic<T>::value || std::is_reference<T>::value || std::is_pointer<T>::value,
		"Type T must be either a built-in value type, reference or a pointer."
	);

	GAFF_ASSERT(_context->GetFunction());
	GAFF_ASSERT(static_cast<asUINT>(index) < _context->GetFunction()->GetParamCount());

#ifdef GAFF_ASSERT_ENABLED
	int type_id = -1;
	_context->GetFunction()->GetParam(static_cast<asUINT>(index), &type_id);
	GAFF_ASSERT(type_id > asTYPEID_DOUBLE || type_id == GetTypeFlag<T>());
#endif

	PushObjectHelper<std::is_pointer<T>::value, std::is_reference<T>::value>::Push(_context, index, value);
}

template <class T>
T AngelScriptComponent::getReturnValue(void)
{
	GAFF_ASSERT(_context->GetFunction());
	const int type_id = _context->GetFunction()->GetReturnTypeId();
	GAFF_ASSERT(type_id > asTYPEID_DOUBLE || type_id == GetTypeFlag<T>());
	return *reinterpret_cast<T*>(_context->GetAddressOfReturnValue());
}

NS_END
