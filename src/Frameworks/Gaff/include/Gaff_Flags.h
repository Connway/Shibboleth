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

#include "Gaff_Assert.h"
#include <EASTL/bitset.h>

NS_GAFF

constexpr size_t ClosestIntegerSize(size_t size_bits)
{
	// Round up to nearest multiple of 8.
	const size_t remainder = size_bits % CHAR_BIT;
	size_bits = size_bits + ((remainder > 0) ? (CHAR_BIT - remainder) : 0);

	size_t size_bytes = size_bits / CHAR_BIT;
	size_t count = 0;

	// First n in the below condition is for the case where size_bytes is 0.
	if (size_bytes && !(size_bytes & (size_bytes - 1))) {
		return size_bytes;
	}

	while (size_bytes != 0) {
		size_bytes >>= 1;
		count += 1;
	}

	return 1ULL << count;
}

template <size_t size_bytes>
struct ClosestSizeFlagTypeHelper final
{
	using Type = uint64_t;
};

template <>
struct ClosestSizeFlagTypeHelper<1> final
{
	using Type = uint8_t;
};

template <>
struct ClosestSizeFlagTypeHelper<2> final
{
	using Type = uint16_t;
};

template <>
struct ClosestSizeFlagTypeHelper<4> final
{
	using Type = uint32_t;
};

template <size_t size_bytes>
using ClosestSizeFlagType = typename ClosestSizeFlagTypeHelper<size_bytes>::Type;


// *Range() functions include the end bit.
template <class Enum>
class Flags final
{
public:
	static_assert(std::is_enum<Enum>::value, "Flags must be given an enum template argument.");
	using StorageType = ClosestSizeFlagType<ClosestIntegerSize(static_cast<size_t>(Enum::Count))>;
	using BitsetType = eastl::bitset<static_cast<size_t>(Enum::Count), StorageType>;

	template <class... Enum2>
	constexpr static StorageType GetBits(Enum flag, Enum2... rest);
	constexpr static StorageType GetBit(Enum flag);
	constexpr static StorageType GetBitsRange(Enum start, Enum end, StorageType bits = 0);

	template <class... Enum2>
	bool testAll(Enum flag, Enum2... rest) const;

	template <class... Enum2>
	bool testAny(Enum flag, Enum2... rest) const;

	template <class... Enum2>
	void toggle(Enum flag, Enum2... rest);

	template <class... Enum2>
	void set(bool value, Enum flag, Enum2... rest);

	template <class... Enum2>
	void set(Enum flag, Enum2... rest);

	template <class... Enum2>
	void clear(Enum flag, Enum2... rest);

	template <class... Enum2>
	Flags(Enum flag, Enum2... rest);

	Flags(BitsetType flags);
	Flags(StorageType flags = 0);
	Flags(const Flags& rhs);

	bool testAll(StorageType flags) const;
	bool testAny(StorageType flags) const;
	bool testRange(Enum start, Enum end) const;
	bool empty(void) const;
	bool any(void) const;

	void setRange(bool value, Enum start, Enum end);
	void clearRange(Enum start, Enum end);
	void setRange(Enum start, Enum end);

	void set(bool value, StorageType flags);
	void set(StorageType flags);
	bool toggle(Enum flag);
	void invert(void);
	void clear(StorageType flags);
	void clear(void);

	int32_t countUnset(void) const;
	int32_t countSet(void) const;

	StorageType getStorage(void) const;

	bool operator==(Flags rhs) const;
	bool operator!=(Flags rhs) const;
	Flags& operator=(Flags rhs);
	Flags& operator&=(Flags rhs);
	Flags& operator^=(Flags rhs);
	Flags& operator|=(Flags rhs);
	Flags operator&(Flags rhs) const;
	Flags operator^(Flags rhs) const;
	Flags operator|(Flags rhs) const;
	Flags operator~(void) const;

private:
	BitsetType _flags;
};

template <class T>
constexpr bool IsFlags(void);

template <class T>
struct GetFlagsEnum;

#include "Gaff_Flags.inl"

NS_END
