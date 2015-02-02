/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gaff_DefaultAllocator.h"

NS_GAFF

template <unsigned int index, class TupleType>
class TupleElement;

template<class... Args>
class Tuple;

template <>
class Tuple<>
{
public:
	Tuple(const Tuple<>&) {}
	Tuple(Tuple<>&&) {}
	Tuple(void) {}
	~Tuple(void) {}

	const Tuple<>& operator=(const Tuple<>&) { return *this; }
	const Tuple<>& operator=(Tuple<>&&) { return *this; }

	bool operator==(const Tuple<>&) const { return true; }
	bool operator!=(const Tuple<>&) const { return false; }
};

template <class First, class... Rest>
class Tuple<First, Rest...> : public Tuple<Rest...>
{
public:
	static const unsigned int Size = 1 + sizeof...(Rest);
	typedef First ValueType;

	Tuple(const First& first, const Rest&... rest);
	Tuple(const Tuple<First, Rest...>& tuple);
	Tuple(First&& first, Rest&&... rest);
	Tuple(Tuple<First, Rest...>&& tuple);
	Tuple(void);

	const Tuple<First, Rest...>& operator=(const Tuple<First, Rest...>& rhs);
	const Tuple<First, Rest...>& operator=(Tuple<First, Rest...>&& rhs);

	bool operator==(const Tuple<First, Rest...>& rhs) const;
	bool operator!=(const Tuple<First, Rest...>& rhs) const;

	void set(const First& first, Rest&&... rest);
	void moveSet(First&& first, Rest&&... rest);

private:
	First _value;

	template <unsigned int index, class... Args> friend const typename TupleElement< index, Tuple<Args...> >::ValueType& TupleGet(const Tuple<Args...>&);
	template <unsigned int index, class... Args> friend typename TupleElement< index, Tuple<Args...> >::ValueType& TupleGet(Tuple<Args...>&);
};

#include "Gaff_Tuple.inl"

NS_END
