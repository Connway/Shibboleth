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

#include "Gaff_Defines.h"

NS_GAFF

// *Range() functions include the end bit.
template <class Enum>
class Flags final
{
	using StorageType = std::underlying_type_t<Enum>;

	static_assert(std::is_enum<Enum>::value, "Flags must be given an enum template argument.");
	static_assert(static_cast<StorageType>(Enum::Count) <= (sizeof(StorageType) * CHAR_BIT), "Not enough bits in enum storage type for flag count.");

public:
	template <class... Enum2>
	constexpr static typename StorageType GetBits(Enum flag, Enum2... rest);
	constexpr static typename StorageType GetBit(Enum flag);

	template <class... Enum2>
	bool testAll(Enum flag, Enum2... rest) const;

	template <class... Enum2>
	bool testAny(Enum flag, Enum2... rest) const;

	template <class... Enum2>
	void toggle(Enum flag, Enum2... rest);

	template <class... Enum2>
	void set(bool value, Enum flag, Enum2... rest);

	template <class... Enum2>
	Flags(Enum flag, Enum2... rest);

	Flags(typename StorageType flags = 0);

	bool testAll(typename StorageType flags) const;
	bool testAny(typename StorageType flags) const;
	bool testRange(Enum start, Enum end) const;
	bool empty(void) const;

	void setRange(bool value, Enum start, Enum end);
	void set(bool value, typename StorageType flags);
	void toggle(Enum flag);
	void invert(void);
	void clear(void);

	int32_t countUnset(void) const;
	int32_t countSet(void) const;

	const Flags& operator&=(Flags rhs);
	const Flags& operator^=(Flags rhs);
	const Flags& operator|=(Flags rhs);
	Flags operator&(Flags rhs) const;
	Flags operator^(Flags rhs) const;
	Flags operator|(Flags rhs) const;
	Flags operator~(void) const;

private:
	typename StorageType _flags = 0;
};

#include "Gaff_Flags.inl"

NS_END
