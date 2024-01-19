/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include <array>

NS_GAFF

template <class EnumType>
class EnumIterator;

NS_END

#define GAFF_DEFINE_ENUM_ITERATOR_RANGE(EnumType, First, Last) \
	NS_GAFF \
		template <> \
		class EnumIterator<EnumType> final \
		{ \
		public: \
			EnumIterator(const EnumIterator<EnumType>& iterator) = default; \
			EnumIterator(void) = default; \
			static EnumIterator<EnumType> begin(void) \
			{ \
				return EnumIterator<EnumType>(First); \
			} \
			static EnumIterator<EnumType> end(void) \
			{ \
				return EnumIterator<EnumType>(Last); \
			} \
			EnumIterator<EnumType>& operator++(void) \
			{ \
				++_value; \
				return *this; \
			} \
			EnumIterator<EnumType> operator++(int) \
			{ \
				return EnumIterator<EnumType>(_value++); \
			} \
			EnumType operator*() const \
			{ \
				return static_cast<EnumType>(_value); \
			} \
			EnumIterator<EnumType>& operator=(const EnumIterator<EnumType>& rhs) = default; \
			auto operator<=>(const EnumIterator<EnumType>& rhs) const = default; \
		private: \
			using ValueType = __underlying_type(EnumType); \
			ValueType _value = static_cast<ValueType>(First); \
			explicit EnumIterator(EnumType value): _value(static_cast<ValueType>(value)) {} \
			explicit EnumIterator(ValueType value): _value(value) {} \
		}; \
	NS_END

#define GAFF_DEFINE_ENUM_ITERATOR_COUNT(EnumType, Count) GAFF_DEFINE_ENUM_ITERATOR_RANGE(EnumType, 0, static_cast<__underlying_type(EnumType)>(Count) - 1)
#define GAFF_DEFINE_ENUM_ITERATOR(EnumType) GAFF_DEFINE_ENUM_ITERATOR_COUNT(EnumType, EnumType::Count)

#define GAFF_DEFINE_ENUM_ITERATOR_VALUES(EnumType, ...) \
	NS_GAFF \
		template <> \
		class EnumIterator<EnumType> final \
		{ \
		public: \
			EnumIterator(const EnumIterator<EnumType>& iterator) = default; \
			EnumIterator(void) = default; \
			static EnumIterator<EnumType> begin(void) \
			{ \
				return EnumIterator<EnumType>(GetFirst()); \
			} \
			static EnumIterator<EnumType> end(void) \
			{ \
				return EnumIterator<EnumType>(GetLast()); \
			} \
			EnumIterator<EnumType>& operator++(void) \
			{ \
				++_ptr; \
				return *this; \
			} \
			EnumIterator<EnumType> operator++(int) \
			{ \
				return EnumIterator<EnumType>(_ptr++); \
			} \
			EnumType operator*() const \
			{ \
				return *_ptr; \
			} \
			EnumIterator<EnumType>& operator=(const EnumIterator<EnumType>& rhs) = default; \
			auto operator<=>(const EnumIterator<EnumType>& rhs) const = default; \
		private: \
			using ValueType = __underlying_type(EnumType); \
			static constexpr const EnumType k_values[] = { __VA_ARGS__ }; \
			const EnumType* _ptr = &k_values[0]; \
			EnumIterator(const EnumType* ptr): _ptr(ptr) {} \
			static constexpr const EnumType* GetFirst(void) { return &k_values[0]; } \
			static constexpr const EnumType* GetLast(void) { return &k_values[std::size(k_values) - 1]; } \
		}; \
	NS_END
