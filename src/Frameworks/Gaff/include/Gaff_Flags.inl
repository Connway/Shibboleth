/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

template <class T>
struct IsFlagsHelper final
{
	static constexpr bool IsFlags(void) { return false; }
};

template <class T>
struct IsFlagsHelper< Flags<T> > final
{
	static constexpr bool IsFlags(void) { return true; }
};

template <class T>
constexpr bool IsFlags(void)
{
	return IsFlagsHelper<T>::IsFlags();
}

template <class T>
struct GetFlagsEnum< Flags<T> > final
{
	using Enum = T;
};



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
	return static_cast<StorageType>(1) << static_cast<StorageType>(flag);
}

template <class Enum>
constexpr typename Flags<Enum>::StorageType Flags<Enum>::GetBitsRange(Enum start, Enum end, StorageType bits)
{
	static_assert(static_cast<uint64_t>(start) <= static_cast<uint64_t>(end), "Flags<Enum>::GetBitsRange - start must be <= end.");

	if constexpr (start == end) {
		return bits | GetBit(start);
	} else {
		return GetBitsRange(static_cast<Enum>(static_cast<uint64_t>(start) + 1), end, bits | GetBit(start));
	}
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
	toggle(flag);

	if constexpr (sizeof...(rest) > 0) {
		toggle(rest...);
	}
}

template <class Enum>
template <class... Enum2>
void Flags<Enum>::set(bool value, Enum flag, Enum2... rest)
{
	set(value, GetBits(flag, rest...));
}

template <class Enum>
template <class... Enum2>
void Flags<Enum>::set(Enum flag, Enum2... rest)
{
	set(GetBits(flag, rest...));
}

template <class Enum>
template <class... Enum2>
void Flags<Enum>::clear(Enum flag, Enum2... rest)
{
	clear(GetBits(flag, rest...));
}

template <class Enum>
template <class... Enum2>
Flags<Enum>::Flags(Enum flag, Enum2... rest)
{
	_flags.from_uint64(GetBits(flag, rest...));
}

template <class Enum>
Flags<Enum>::Flags(BitsetType flags):
	_flags(flags)
{
}

template <class Enum>
Flags<Enum>::Flags(StorageType flags)
{
	_flags.from_uint64(flags);
}

template <class Enum>
Flags<Enum>::Flags(const Flags<Enum>& rhs):
	_flags(rhs._flags)
{
}

template <class Enum>
bool Flags<Enum>::testAll(StorageType flags) const
{
	BitsetType temp;
	temp.from_uint64(flags);

	return (_flags & temp) == temp;
}
	
template <class Enum>
bool Flags<Enum>::testAny(StorageType flags) const
{
	BitsetType temp;
	temp.from_uint64(flags);

	return !(_flags & temp).none();
}

template <class Enum>
bool Flags<Enum>::testRange(Enum start, Enum end) const
{
	return testAll(GetBitsRange(start, end));
}

template <class Enum>
bool Flags<Enum>::empty(void) const
{
	return _flags.none();
}

template <class Enum>
bool Flags<Enum>::any(void) const
{
	return _flags.any();
}

template <class Enum>
void Flags<Enum>::setRange(bool value, Enum start, Enum end)
{
	if (value) {
		setRange(start, end);
	} else {
		clearRange(start, end);
	}
}

template <class Enum>
void Flags<Enum>::clearRange(Enum start, Enum end)
{
	GAFF_ASSERT(static_cast<int32_t>(start) <= static_cast<int32_t>(end));

	// $TODO: Optimize this.

	for (int32_t i = static_cast<int32_t>(start); i <= static_cast<int32_t>(end); ++i) {
		clear(static_cast<Enum>(i));
	}
}

template <class Enum>
void Flags<Enum>::setRange(Enum start, Enum end)
{
	GAFF_ASSERT(static_cast<int32_t>(start) <= static_cast<int32_t>(end));

	// $TODO: Optimize this.

	for (int32_t i = static_cast<int32_t>(start); i <= static_cast<int32_t>(end); ++i) {
		set(static_cast<Enum>(i));
	}
}

template <class Enum>
void Flags<Enum>::set(bool value, StorageType flags)
{
	BitsetType temp;
	temp.from_uint64(flags);

	if (value) {
		_flags |= temp;
	} else {
		_flags &= ~temp;
	}
}

template <class Enum>
void Flags<Enum>::set(StorageType flags)
{
	BitsetType temp;
	temp.from_uint64(flags);

	_flags |= temp;
}

template <class Enum>
bool Flags<Enum>::toggle(Enum flag)
{
	_flags.flip(static_cast<size_t>(flag));
	return testAll(flag);
}

template <class Enum>
void Flags<Enum>::invert(void)
{
	_flags.flip();
}

template <class Enum>
void Flags<Enum>::clear(StorageType flags)
{
	BitsetType temp;
	temp.from_uint64(flags);

	_flags &= ~temp;
}

template <class Enum>
void Flags<Enum>::clear(void)
{
	_flags.reset();
}

template <class Enum>
int32_t Flags<Enum>::countUnset(void) const
{
	return static_cast<int32_t>(BitsetType::kSize - _flags.count());
}

template <class Enum>
int32_t Flags<Enum>::countSet(void) const
{
	return static_cast<int32_t>(_flags.count());
}

template <class Enum>
typename Flags<Enum>::StorageType Flags<Enum>::getStorage(void) const
{
	return *_flags.data();
}


template <class Enum>
bool Flags<Enum>::operator==(Flags rhs) const
{
	return _flags == rhs._flags;
}

template <class Enum>
bool Flags<Enum>::operator!=(Flags rhs) const
{
	return _flags != rhs._flags;
}

template <class Enum>
Flags<Enum>& Flags<Enum>::operator=(Flags rhs)
{
	_flags = rhs._flags;
	return *this;
}

template <class Enum>
Flags<Enum>& Flags<Enum>::operator&=(Flags rhs)
{
	_flags &= rhs._flags;
	return *this;
}

template <class Enum>
Flags<Enum>& Flags<Enum>::operator^=(Flags rhs)
{
	_flags ^= rhs._flags;
	return *this;
}

template <class Enum>
Flags<Enum>& Flags<Enum>::operator|=(Flags rhs)
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
