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

template <class T, class R, R T::*M>
constexpr ptrdiff_t OffsetOfMember(void)
{
	return reinterpret_cast<ptrdiff_t>(&(((T*)0)->*M));
}

template <class T, class R>
constexpr ptrdiff_t OffsetOfMember(R T::*m)
{
	return reinterpret_cast<ptrdiff_t>(&(((T*)0)->*m));
}

template <class Derived, class Base>
constexpr ptrdiff_t OffsetOfClass(void)
{
	return ((ptrdiff_t)(Base*)(Derived*)1) - 1;
}

template <size_t SizeA, size_t SizeB>
constexpr eastl::array<char, SizeA + SizeB - 1> ConcatConst(const eastl::array<char, SizeA>& lhs, const char(&rhs)[SizeB])
{
	// Leave room for null terminator.
	eastl::array<char, SizeA + SizeB - 1> result{};
	size_t outIndex = 0;

	for (size_t index = 0; index < (SizeA - 1); ++index, ++outIndex) {
		result[outIndex] = lhs[index];
	}

	for (size_t index = 0; index < (SizeB - 1); ++index, ++outIndex) {
		result[outIndex] = rhs[index];
	}

	result[outIndex] = 0;
	return result;
}

template <size_t SizeA, size_t SizeB>
constexpr eastl::array<char, SizeA + SizeB - 1> ConcatConst(const char (&lhs)[SizeA], const char (&rhs)[SizeB])
{
	// Leave room for null terminator.
	eastl::array<char, SizeA + SizeB - 1> result{};
	size_t outIndex = 0;

	for (size_t index = 0; index < (SizeA - 1); ++index, ++outIndex) {
		result[outIndex] = lhs[index];
	}

	for (size_t index = 0; index < (SizeB - 1); ++index, ++outIndex) {
		result[outIndex] = rhs[index];
	}

	result[outIndex] = 0;
	return result;
}
