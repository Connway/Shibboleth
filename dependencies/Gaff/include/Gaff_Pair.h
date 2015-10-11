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

/*! \file */

#pragma once

#include "Gaff_Defines.h"

NS_GAFF

template <class T1, class T2>
struct Pair
{
	Pair(const T1& f, const T2& s):
		first(f), second(s)
	{
	}

	//Pair(const T1& f, T2&& s):
	//	first(f), second(std::move(s))
	//{
	//}

	//Pair(T1&& f, const T2& s):
	//	first(std::move(f)), second(s)
	//{
	//}

	//Pair(T1&& f, T2&& s):
	//	first(std::move(f)), second(std::move(s))
	//{
	//}

	Pair(const Pair& pair):
		first(pair.first), second(pair.second)
	{
	}

	Pair(Pair&& pair):
		first(std::move(pair.first)), second(std::move(pair.second))
	{
	}

	Pair(void)
	{
	}

	const Pair& operator=(const Pair& rhs)
	{
		first = rhs.first;
		second = rhs.second;
		return *this;
	}

	const Pair& operator=(Pair&& rhs)
	{
		first = std::move(rhs.first);
		second = std::move(rhs.second);
		return *this;
	}

	T1 first;
	T2 second;
};

template <class T1, class T2>
Pair<T1, T2> MakePair(const T1& first, const T2& second)
{
	return Pair<T1, T2>(first, second);
}

//template <class T1, class T2>
//Pair<T1, T2> MakePair(const T1& first, T2&& second)
//{
//	return Pair<T1, T2>(first, std::move(second));
//}
//
//template <class T1, class T2>
//Pair<T1, T2> MakePair(T1&& first, const T2& second)
//{
//	return Pair<T1, T2>(std::move(first), second);
//}

//template <class T1, class T2>
//Pair<T1, T2> MakePair(T1&& first, T2&& second)
//{
//	return Pair<T1, T2>(std::move(first), std::move(second));
//}

NS_END
