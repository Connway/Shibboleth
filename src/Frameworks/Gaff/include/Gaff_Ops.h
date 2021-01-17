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

#pragma once

#include "Gaff_Hash.h"

#define OP_ADD_NAME "__add"
#define OP_SUB_NAME "__sub"
#define OP_MUL_NAME "__mul"
#define OP_DIV_NAME "__div"
#define OP_MOD_NAME "__mod"
#define OP_BIT_AND_NAME "__band"
#define OP_BIT_OR_NAME "__bor"
#define OP_BIT_XOR_NAME "__bxor"
#define OP_BIT_NOT_NAME "__bnot"
#define OP_BIT_SHIFT_LEFT_NAME "__shl"
#define OP_BIT_SHIFT_RIGHT_NAME "__shr"
#define OP_LOGIC_AND_NAME "__and"
#define OP_LOGIC_OR_NAME "__or"
#define OP_EQUAL_NAME "__eq"
#define OP_LESS_THAN_NAME "__lt"
#define OP_GREATER_THAN_NAME "__gt"
#define OP_LESS_THAN_OR_EQUAL_NAME "__le"
#define OP_GREATER_THAN_OR_EQUAL_NAME "__ge"
#define OP_MINUS_NAME "__unm"
#define OP_PLUS_NAME "__unp"
#define OP_CALL_NAME "__call"
#define OP_INDEX_NAME "__idx"
#define OP_TO_STRING_NAME "__tostring"

NS_GAFF

enum class Operator
{
	Add,
	Sub,
	Mul,
	Div,
	Mod,

	BitAnd,
	BitOr,
	BitXor,
	BitNot,
	BitShiftLeft,
	BitShiftRight,

	LogicAnd,
	LogicOr,

	Equal,
	LessThan,
	GreaterThan,
	LessThanOrEqual,
	GreaterThanOrEqual,

	Minus,
	Plus,
	
	Call,
	Index,

	ToString,

	Count
};

static constexpr const char* OpNames[static_cast<size_t>(Operator::Count)] = {
	OP_ADD_NAME,
	OP_SUB_NAME,
	OP_MUL_NAME,
	OP_DIV_NAME,
	OP_MOD_NAME,
	OP_BIT_AND_NAME,
	OP_BIT_OR_NAME,
	OP_BIT_XOR_NAME,
	OP_BIT_NOT_NAME,
	OP_BIT_SHIFT_LEFT_NAME,
	OP_BIT_SHIFT_RIGHT_NAME,
	OP_LOGIC_AND_NAME,
	OP_LOGIC_OR_NAME,
	OP_EQUAL_NAME,
	OP_LESS_THAN_NAME,
	OP_GREATER_THAN_NAME,
	OP_LESS_THAN_OR_EQUAL_NAME,
	OP_GREATER_THAN_OR_EQUAL_NAME,
	OP_MINUS_NAME,
	OP_PLUS_NAME,
	OP_CALL_NAME,
	OP_INDEX_NAME,
	OP_TO_STRING_NAME
};

static constexpr const char* GetOpName(Operator op) { return OpNames[static_cast<size_t>(op)]; }
static constexpr Hash32 GetOpNameHash(Operator op) { return FNV1aHash32Const(GetOpName(op)); }


template <class LHS, class RHS>
static auto Add(const LHS& lhs, const RHS& rhs)
{
	return lhs + rhs;
}

template <class LHS, class RHS>
static auto Sub(const LHS& lhs, const RHS& rhs)
{
	return lhs - rhs;
}

template <class LHS, class RHS>
static auto Mul(const LHS& lhs, const RHS& rhs)
{
	return lhs * rhs;
}

template <class LHS, class RHS>
static auto Div(const LHS& lhs, const RHS& rhs)
{
	return lhs / rhs;
}

template <class LHS, class RHS>
static auto Mod(const LHS& lhs, const RHS& rhs)
{
	return lhs % rhs;
}

template <class LHS, class RHS>
static auto BitAnd(const LHS& lhs, const RHS& rhs)
{
	return lhs & rhs;
}

template <class LHS, class RHS>
static auto BitOr(const LHS& lhs, const RHS& rhs)
{
	return lhs | rhs;
}

template <class LHS, class RHS>
static auto BitXor(const LHS& lhs, const RHS& rhs)
{
	return lhs ^ rhs;
}

template <class T>
static auto BitNot(const T& value)
{
	return ~value;
}

template <class LHS, class RHS>
static auto BitShiftLeft(const LHS& lhs, const RHS& rhs)
{
	return lhs << rhs;
}

template <class LHS, class RHS>
static auto BitShiftRight(const LHS& lhs, const RHS& rhs)
{
	return lhs >> rhs;
}

template <class LHS, class RHS>
static auto LogicAnd(const LHS& lhs, const RHS& rhs)
{
	return lhs && rhs;
}

template <class LHS, class RHS>
static auto LogicOr(const LHS& lhs, const RHS& rhs)
{
	return lhs || rhs;
}

template <class LHS, class RHS>
static auto Equal(const LHS& lhs, const RHS& rhs)
{
	return lhs == rhs;
}

template <class LHS, class RHS>
static auto LessThan(const LHS& lhs, const RHS& rhs)
{
	return lhs < rhs;
}

template <class LHS, class RHS>
static auto GreaterThan(const LHS& lhs, const RHS& rhs)
{
	return lhs > rhs;
}

template <class LHS, class RHS>
static auto LessThanOrEqual(const LHS& lhs, const RHS& rhs)
{
	return lhs <= rhs;
}

template <class LHS, class RHS>
static auto GreaterThanOrEqual(const LHS& lhs, const RHS& rhs)
{
	return lhs >= rhs;
}

template <class T>
static auto Minus(const T& value)
{
	return -value;
}

template <class T>
static auto Plus(const T& value)
{
	return +value;
}

template <class T, class... Args>
static auto Call(T& value, Args&&... args)
{
	return value(std::forward<Args>(args)...);
}

template <class T, class Idx>
static auto Index(T& value, const Idx& index)
{
	return value[index];
}

template <class T>
struct ToStringHelper final
{
	template <int32_t (*to_string_func)(const T&, char*, int32_t)>
	static int32_t ToString(const void* value, char* buffer, int32_t size)
	{
		return to_string_func(*reinterpret_cast<const T*>(value), buffer, size);
	}
};

NS_END
