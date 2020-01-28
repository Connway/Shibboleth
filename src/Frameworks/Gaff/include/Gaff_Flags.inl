/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

template <class Enum>
template <class... Enum2>
constexpr typename Flags<Enum>::StorageType Flags<Enum>::GetBits(Enum flag, Enum2... rest)
{
	if constexpr (sizeof...(rest) > 0) {
		return GetBit(flag) | GetBits(rest...);
	} else {
		return GetBit(flag);
	}
}

template <class Enum>
constexpr typename Flags<Enum>::StorageType Flags<Enum>::GetBit(Enum flag)
{
	return 1 << static_cast<StorageType>(flag);
}

template <class Enum>
template <class... Enum2>
bool Flags<Enum>::testAll(Enum flag, Enum2... rest) const
{
	return testAll(GetBits(flag, rest...));
}

template <class Enum>
template <class... Enum2>
bool Flags<Enum>::testAny(Enum flag, Enum2... rest) const
{
	return testAny(GetBits(flag, rest...));
}

template <class Enum>
template <class... Enum2>
void Flags<Enum>::toggle(Enum flag, Enum2... rest)
{
	_flags ^= GetBits(flag, rest...);
}

template <class Enum>
template <class... Enum2>
void Flags<Enum>::set(bool value, Enum flag, Enum2... rest)
{
	set(value, GetBits(flag, rest...));
}

template <class Enum>
template <class... Enum2>
constexpr Flags<Enum>::Flags(Enum flag, Enum2... rest):
	_flags(GetBits(flag, rest...))
{
}

template <class Enum>
constexpr Flags<Enum>::Flags(typename StorageType flags):
	_flags(flags)
{
}

template <class Enum>
bool Flags<Enum>::testAll(typename StorageType flags) const
{
	return (_flags & flags) == flags;
}
	
template <class Enum>
bool Flags<Enum>::testAny(typename StorageType flags) const
{
	return (_flags & flags) != 0;
}

template <class Enum>
bool Flags<Enum>::testRange(Enum start, Enum end) const
{
	for (int32_t i = static_cast<int32_t>(start); i <= static_cast<int32_t>(end); ++start) {
		return testAll(static_cast<Enum>(i));
	}

	return true;
}

template <class Enum>
bool Flags<Enum>::empty(void) const
{
	return _flags == 0;
}

template <class Enum>
void Flags<Enum>::setRange(bool value, Enum start, Enum end)
{
	for (int32_t i = static_cast<int32_t>(start); i <= static_cast<int32_t>(end); ++start) {
		return set(value, static_cast<Enum>(i));
	}
}

template <class Enum>
void Flags<Enum>::set(bool value, typename StorageType flags)
{
	if (value) {
		_flags |= flags;
	} else {
		_flags &= ~flags;
	}
}

template <class Enum>
void Flags<Enum>::toggle(Enum flag)
{
	_flags ^= GetBit(flag);
}

template <class Enum>
void Flags<Enum>::invert(void)
{
	_flags = ~_flags;
}

template <class Enum>
void Flags<Enum>::clear(void)
{
	_flags = 0;
}

template <class Enum>
int32_t Flags<Enum>::countUnset(void) const
{
	int32_t count = 0;

	for (int32_t i = 0; i < Enum::Count; ++i) {
		if (!testAll(GetBit(static_cast<Enum>(i)))) {
			++count;
		}
	}
	
	return count;
}

template <class Enum>
int32_t Flags<Enum>::countSet(void) const
{
	int32_t count = 0;

	for (int32_t i = 0; i < Enum::Count; ++i) {
		if (testAll(GetBit(static_cast<Enum>(i)))) {
			++count;
		}
	}

	return count;
}

template <class Enum>
const Flags<Enum>& Flags<Enum>::operator&=(Flags rhs)
{
	_flags &= rhs._flags;
	return *this;
}

template <class Enum>
const Flags<Enum>& Flags<Enum>::operator^=(Flags rhs)
{
	_flags ^= rhs._flags;
	return *this;
}

template <class Enum>
const Flags<Enum>& Flags<Enum>::operator|=(Flags rhs)
{
	_flags |= rhs._flags;
	return *this;
}

template <class Enum>
Flags<Enum> Flags<Enum>::operator&(Flags rhs) const
{
	return Flags(_flags & rhs._flags);
}

template <class Enum>
Flags<Enum> Flags<Enum>::operator^(Flags rhs) const
{
	return Flags(_flags ^ rhs._flags);
}

template <class Enum>
Flags<Enum> Flags<Enum>::operator|(Flags rhs) const
{
	return Flags(_flags | rhs._flags);
}

template <class Enum>
Flags<Enum> Flags<Enum>::operator~(void) const
{
	return Flags(~_flags);
}
