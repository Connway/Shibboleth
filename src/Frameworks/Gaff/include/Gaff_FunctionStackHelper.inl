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

NS_END
