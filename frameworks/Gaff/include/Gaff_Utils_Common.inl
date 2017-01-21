	/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

/*!
	\brief Parses the command line into option/value(s) pairs using a hashmap.
	\return A HashMap of option/value strings.
	\note
		Options start with either a '-' or '--'. Option values are space separated.

		Example (using JSON style notation for result):

		(cmdline): --option1 value1 value2 -option2 value3 -flag0

		(result): { "option1": "value1 value2", "option2": "value3", "flag0": "" }
*/
#ifndef GAFF_UTILS_NO_CONTAINERS
template <class Allocator>
VectorMap<HashString32<Allocator>, U8String<Allocator>, Allocator> ParseCommandLine(int argc, char** argv)
{
	VectorMap<HashString32<Allocator>, U8String<Allocator>, Allocator> opt_flag_map;
	ParseCommandLine(argc, argv, opt_flag_map);
	return opt_flag_map;
}

/*!
	\brief Same as ParseCommandLine, but the user provides the HashMap we are outputting to.
*/
template <class Allocator>
void ParseCommandLine(int argc, char** argv, VectorMap<HashString32<Allocator>, U8String<Allocator>, Allocator>& out)
{
	if (argc == 1) {
		return;
	}

	HashString32<Allocator> option;
	U8String<Allocator> values;

	for (int i = 1; i < argc; ++i) {
		unsigned int option_begin = 0;
		const char* value = argv[i];

		// If it doesn't start with - or -- then skip it!
		while (value[option_begin] == '-') {
			++option_begin;
		}

		if (!option_begin) {
			if (option.size()) {
				if (values.size()) {
					values += ' ';
				}

				values += value;
			}

		} else {
			if (option.size()) {
				out[option] = std::move(values);
			}

			option = value + option_begin;
		}
	}

	if (option.size()) {
		out[option] = std::move(values);
	}
}
#endif

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
