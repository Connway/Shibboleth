	/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

template <class T>
void SetBitsToValue(T& value, T bits, bool set)
{
	if (set) {
		SetBits(value, bits);
	} else {
		ClearBits(value, bits);
	}
}

template <class T>
void SetBits(T& value, T bits)
{
	value |= bits;
}

template <class T>
void ClearBits(T& value, T bits)
{
	value &= ~bits;
}

template <class T>
bool IsAnyBitSet(const T& value, T bits)
{
	return (value & bits) != 0;
}

template <class T>
bool AreAllBitsSet(const T& value, T bits)
{
	return (value & bits) == bits;
}

template <class T, class R, R T::*M>
constexpr ptrdiff_t OffsetOfMember(void)
{
	return reinterpret_cast<ptrdiff_t>(&(((T*)0)->*M));
}

template <class T, class R>
ptrdiff_t OffsetOfMember(R T::*m)
{
	return reinterpret_cast<ptrdiff_t>(&(((T*)0)->*m));
}

template <class Derived, class Base>
ptrdiff_t OffsetOfClass(void)
{
	return ((ptrdiff_t)(Base*)(Derived*)1) - 1;
}
