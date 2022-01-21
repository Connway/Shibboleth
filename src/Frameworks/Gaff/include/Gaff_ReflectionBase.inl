/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

template <class T, class Allocator>
constexpr int32_t ReflectionBase<T, Allocator>::Size(void)
{
	return sizeof(T);
}

template <class T, class Allocator>
constexpr bool ReflectionBase<T, Allocator>::IsEnum(void)
{
	return std::is_enum<T>::value;
}

template <class T, class Allocator>
ReflectionBase<T, Allocator>::ReflectionBase(void)
{
	if constexpr (std::is_enum<T>::value) {
		AddToEnumReflectionChain(this);

	} else {
		if constexpr (std::is_base_of<IAttribute, T>::value) {
			AddToAttributeReflectionChain(this);
		} else {
			AddToReflectionChain(this);
		}
	}
}

template <class T, class Allocator>
bool ReflectionBase<T, Allocator>::isEnum(void) const
{
	return IsEnum();
}

template <class T, class Allocator>
Hash64 ReflectionBase<T, Allocator>::getVersion(void) const
{
	return _version.getHash();
}

template <class T, class Allocator>
int32_t ReflectionBase<T, Allocator>::size(void) const
{
	return sizeof(T);
}

template <class T, class Allocator>
const IEnumReflectionDefinition& ReflectionBase<T, Allocator>::getEnumReflectionDefinition(void) const
{
	if constexpr (std::is_enum<T>::value) {
		return reinterpret_cast<const IEnumReflectionDefinition&>(*_ref_def); /* To calm the compiler, even though this should be compiled out ... */
	} else {
		return IReflection::getEnumReflectionDefinition();
	}
}

template <class T, class Allocator>
const IReflectionDefinition& ReflectionBase<T, Allocator>::getReflectionDefinition(void) const
{
	if constexpr (std::is_enum<T>::value) {
		return IReflection::getReflectionDefinition();
	} else {
		return reinterpret_cast<const IReflectionDefinition&>(*_ref_def); /* To calm the compiler, even though this should be compiled out ... */
	}
}
