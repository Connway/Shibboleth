/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#define OP_ADD_NAME u8"__add"
#define OP_SUB_NAME u8"__sub"
#define OP_MUL_NAME u8"__mul"
#define OP_DIV_NAME u8"__div"
#define OP_MOD_NAME u8"__mod"
#define OP_BIT_AND_NAME u8"__band"
#define OP_BIT_OR_NAME u8"__bor"
#define OP_BIT_XOR_NAME u8"__bxor"
#define OP_BIT_NOT_NAME u8"__bnot"
#define OP_BIT_SHIFT_LEFT_NAME u8"__shl"
#define OP_BIT_SHIFT_RIGHT_NAME u8"__shr"
#define OP_LOGIC_AND_NAME u8"__and"
#define OP_LOGIC_OR_NAME u8"__or"
#define OP_EQUAL_NAME u8"__eq"
#define OP_NOT_EQUAL_NAME u8"__neq"
#define OP_LESS_THAN_NAME u8"__lt"
#define OP_GREATER_THAN_NAME u8"__gt"
#define OP_LESS_THAN_OR_EQUAL_NAME u8"__le"
#define OP_GREATER_THAN_OR_EQUAL_NAME u8"__ge"
#define OP_MINUS_NAME u8"__unm"
#define OP_PLUS_NAME u8"__unp"
#define OP_CALL_NAME u8"__call"
#define OP_INDEX_NAME u8"__idx"
#define OP_TO_STRING_NAME u8"__tostring"
#define OP_COMP_NAME u8"__comp"
#define OP_PRE_INC_NAME u8"__preinc"
#define OP_POST_INC_NAME u8"__preinc"
#define OP_PRE_DEC_NAME u8"__predec"
#define OP_POST_DEC_NAME u8"__predec"
#define OP_ASSIGN_NAME u8"__assign"
#define OP_ADD_ASSIGN_NAME u8"__add_assign"
#define OP_SUB_ASSIGN_NAME u8"__sub_assign"
#define OP_MUL_ASSIGN_NAME u8"__mul_assign"
#define OP_DIV_ASSIGN_NAME u8"__div_assign"
#define OP_MOD_ASSIGN_NAME u8"__mod_assign"
#define OP_BIT_AND_ASSIGN_NAME u8"__band_assign"
#define OP_BIT_OR_ASSIGN_NAME u8"__bor_assign"
#define OP_BIT_XOR_ASSIGN_NAME u8"__bxor_assign"
#define OP_BIT_SHIFT_LEFT_ASSIGN_NAME u8"__shl_assign"
#define OP_BIT_SHIFT_RIGHT_ASSIGN_NAME u8"__shr_assign"

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
	NotEqual,
	LessThan,
	GreaterThan,
	LessThanOrEqual,
	GreaterThanOrEqual,

	Minus,
	Plus,

	Call,
	Index,

	ToString,

	Comparison,

	PreIncrement,
	PostIncrement,
	PreDecrement,
	PostDecrement,

	Assignment,
	AddAssignment,
	SubAssignment,
	MulAssignment,
	DivAssignment,
	ModAssignment,
	BitAndAssignment,
	BitOrAssignment,
	BitXorAssignment,
	BitShiftLeftAssignment,
	BitShiftRightAssignment,

	Count
};

static constexpr const char8_t* k_op_names[] = {
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
	OP_NOT_EQUAL_NAME,
	OP_LESS_THAN_NAME,
	OP_GREATER_THAN_NAME,
	OP_LESS_THAN_OR_EQUAL_NAME,
	OP_GREATER_THAN_OR_EQUAL_NAME,
	OP_MINUS_NAME,
	OP_PLUS_NAME,
	OP_CALL_NAME,
	OP_INDEX_NAME,
	OP_TO_STRING_NAME,
	OP_COMP_NAME,
	OP_PRE_INC_NAME,
	OP_POST_INC_NAME,
	OP_PRE_DEC_NAME,
	OP_POST_DEC_NAME,
	OP_ASSIGN_NAME,
	OP_ADD_ASSIGN_NAME,
	OP_SUB_ASSIGN_NAME,
	OP_MUL_ASSIGN_NAME,
	OP_DIV_ASSIGN_NAME,
	OP_MOD_ASSIGN_NAME,
	OP_BIT_AND_ASSIGN_NAME,
	OP_BIT_OR_ASSIGN_NAME,
	OP_BIT_XOR_ASSIGN_NAME,
	OP_BIT_SHIFT_LEFT_ASSIGN_NAME,
	OP_BIT_SHIFT_RIGHT_ASSIGN_NAME
};
static_assert(std::size(k_op_names) == static_cast<size_t>(Operator::Count));

static constexpr const Hash32 k_op_hashes[] = {
	FNV1aHash32Const(OP_ADD_NAME, eastl::CharStrlen(OP_ADD_NAME)),
	FNV1aHash32Const(OP_SUB_NAME, eastl::CharStrlen(OP_SUB_NAME)),
	FNV1aHash32Const(OP_MUL_NAME, eastl::CharStrlen(OP_MUL_NAME)),
	FNV1aHash32Const(OP_DIV_NAME, eastl::CharStrlen(OP_DIV_NAME)),
	FNV1aHash32Const(OP_MOD_NAME, eastl::CharStrlen(OP_MOD_NAME)),
	FNV1aHash32Const(OP_BIT_AND_NAME, eastl::CharStrlen(OP_BIT_AND_NAME)),
	FNV1aHash32Const(OP_BIT_OR_NAME, eastl::CharStrlen(OP_BIT_OR_NAME)),
	FNV1aHash32Const(OP_BIT_XOR_NAME, eastl::CharStrlen(OP_BIT_XOR_NAME)),
	FNV1aHash32Const(OP_BIT_NOT_NAME, eastl::CharStrlen(OP_BIT_NOT_NAME)),
	FNV1aHash32Const(OP_BIT_SHIFT_LEFT_NAME, eastl::CharStrlen(OP_BIT_SHIFT_LEFT_NAME)),
	FNV1aHash32Const(OP_BIT_SHIFT_RIGHT_NAME, eastl::CharStrlen(OP_BIT_SHIFT_RIGHT_NAME)),
	FNV1aHash32Const(OP_LOGIC_AND_NAME, eastl::CharStrlen(OP_LOGIC_AND_NAME)),
	FNV1aHash32Const(OP_LOGIC_OR_NAME, eastl::CharStrlen(OP_LOGIC_OR_NAME)),
	FNV1aHash32Const(OP_EQUAL_NAME, eastl::CharStrlen(OP_EQUAL_NAME)),
	FNV1aHash32Const(OP_NOT_EQUAL_NAME, eastl::CharStrlen(OP_NOT_EQUAL_NAME)),
	FNV1aHash32Const(OP_LESS_THAN_NAME, eastl::CharStrlen(OP_LESS_THAN_NAME)),
	FNV1aHash32Const(OP_GREATER_THAN_NAME, eastl::CharStrlen(OP_GREATER_THAN_NAME)),
	FNV1aHash32Const(OP_LESS_THAN_OR_EQUAL_NAME, eastl::CharStrlen(OP_LESS_THAN_OR_EQUAL_NAME)),
	FNV1aHash32Const(OP_GREATER_THAN_OR_EQUAL_NAME, eastl::CharStrlen(OP_GREATER_THAN_OR_EQUAL_NAME)),
	FNV1aHash32Const(OP_MINUS_NAME, eastl::CharStrlen(OP_MINUS_NAME)),
	FNV1aHash32Const(OP_PLUS_NAME, eastl::CharStrlen(OP_PLUS_NAME)),
	FNV1aHash32Const(OP_CALL_NAME, eastl::CharStrlen(OP_CALL_NAME)),
	FNV1aHash32Const(OP_INDEX_NAME, eastl::CharStrlen(OP_INDEX_NAME)),
	FNV1aHash32Const(OP_TO_STRING_NAME, eastl::CharStrlen(OP_TO_STRING_NAME)),
	FNV1aHash32Const(OP_COMP_NAME, eastl::CharStrlen(OP_COMP_NAME)),
	FNV1aHash32Const(OP_PRE_INC_NAME, eastl::CharStrlen(OP_PRE_INC_NAME)),
	FNV1aHash32Const(OP_POST_INC_NAME, eastl::CharStrlen(OP_POST_INC_NAME)),
	FNV1aHash32Const(OP_PRE_DEC_NAME, eastl::CharStrlen(OP_PRE_DEC_NAME)),
	FNV1aHash32Const(OP_POST_DEC_NAME, eastl::CharStrlen(OP_POST_DEC_NAME)),
	FNV1aHash32Const(OP_ASSIGN_NAME, eastl::CharStrlen(OP_ASSIGN_NAME)),
	FNV1aHash32Const(OP_ADD_ASSIGN_NAME, eastl::CharStrlen(OP_ADD_ASSIGN_NAME)),
	FNV1aHash32Const(OP_SUB_ASSIGN_NAME, eastl::CharStrlen(OP_SUB_ASSIGN_NAME)),
	FNV1aHash32Const(OP_MUL_ASSIGN_NAME, eastl::CharStrlen(OP_MUL_ASSIGN_NAME)),
	FNV1aHash32Const(OP_DIV_ASSIGN_NAME, eastl::CharStrlen(OP_DIV_ASSIGN_NAME)),
	FNV1aHash32Const(OP_MOD_ASSIGN_NAME, eastl::CharStrlen(OP_MOD_ASSIGN_NAME)),
	FNV1aHash32Const(OP_BIT_AND_ASSIGN_NAME, eastl::CharStrlen(OP_BIT_AND_ASSIGN_NAME)),
	FNV1aHash32Const(OP_BIT_OR_ASSIGN_NAME, eastl::CharStrlen(OP_BIT_OR_ASSIGN_NAME)),
	FNV1aHash32Const(OP_BIT_XOR_ASSIGN_NAME, eastl::CharStrlen(OP_BIT_XOR_ASSIGN_NAME)),
	FNV1aHash32Const(OP_BIT_SHIFT_LEFT_ASSIGN_NAME, eastl::CharStrlen(OP_BIT_SHIFT_LEFT_ASSIGN_NAME)),
	FNV1aHash32Const(OP_BIT_SHIFT_RIGHT_ASSIGN_NAME, eastl::CharStrlen(OP_BIT_SHIFT_RIGHT_ASSIGN_NAME))
};
static_assert(std::size(k_op_hashes) == static_cast<size_t>(Operator::Count));

static constexpr const char8_t* GetOpName(Operator op) { return k_op_names[static_cast<size_t>(op)]; }
static constexpr Hash32 GetOpNameHash(Operator op) { return k_op_hashes[static_cast<size_t>(op)]; }


template <class LHS, class RHS>
static decltype(auto) Add(const LHS& lhs, const RHS& rhs)
{
	return lhs + rhs;
}

template <class LHS, class RHS>
static decltype(auto) Sub(const LHS& lhs, const RHS& rhs)
{
	return lhs - rhs;
}

template <class LHS, class RHS>
static decltype(auto) Mul(const LHS& lhs, const RHS& rhs)
{
	return lhs * rhs;
}

template <class LHS, class RHS>
static decltype(auto) Div(const LHS& lhs, const RHS& rhs)
{
	return lhs / rhs;
}

template <class LHS, class RHS>
static decltype(auto) Mod(const LHS& lhs, const RHS& rhs)
{
	return lhs % rhs;
}

template <class LHS, class RHS>
static decltype(auto) BitAnd(const LHS& lhs, const RHS& rhs)
{
	return lhs & rhs;
}

template <class LHS, class RHS>
static decltype(auto) BitOr(const LHS& lhs, const RHS& rhs)
{
	return lhs | rhs;
}

template <class LHS, class RHS>
static decltype(auto) BitXor(const LHS& lhs, const RHS& rhs)
{
	return lhs ^ rhs;
}

template <class T>
static decltype(auto) BitNot(const T& value)
{
	return ~value;
}

template <class LHS, class RHS>
static decltype(auto) BitShiftLeft(const LHS& lhs, const RHS& rhs)
{
	return lhs << rhs;
}

template <class LHS, class RHS>
static decltype(auto) BitShiftRight(const LHS& lhs, const RHS& rhs)
{
	return lhs >> rhs;
}

template <class LHS, class RHS>
static decltype(auto) LogicAnd(const LHS& lhs, const RHS& rhs)
{
	return lhs && rhs;
}

template <class LHS, class RHS>
static decltype(auto) LogicOr(const LHS& lhs, const RHS& rhs)
{
	return lhs || rhs;
}

template <class LHS, class RHS>
static decltype(auto) Equal(const LHS& lhs, const RHS& rhs)
{
	return lhs == rhs;
}

template <class LHS, class RHS>
static decltype(auto) NotEqual(const LHS& lhs, const RHS& rhs)
{
	return lhs != rhs;
}

template <class LHS, class RHS>
static decltype(auto) LessThan(const LHS& lhs, const RHS& rhs)
{
	return lhs < rhs;
}

template <class LHS, class RHS>
static decltype(auto) GreaterThan(const LHS& lhs, const RHS& rhs)
{
	return lhs > rhs;
}

template <class LHS, class RHS>
static decltype(auto) LessThanOrEqual(const LHS& lhs, const RHS& rhs)
{
	return lhs <= rhs;
}

template <class LHS, class RHS>
static decltype(auto) GreaterThanOrEqual(const LHS& lhs, const RHS& rhs)
{
	return lhs >= rhs;
}

template <class T>
static decltype(auto) Minus(const T& value)
{
	return -value;
}

template <class T>
static decltype(auto) Plus(const T& value)
{
	return +value;
}

template <class T, class... Args>
static decltype(auto) Call(T& value, Args&&... args)
{
	return value(std::forward<Args>(args)...);
}

template <class T, class Idx>
static decltype(auto) Index(T& value, const Idx& index)
{
	return value[index];
}

template <class T, int32_t (*to_string_func)(const T&, char8_t*, int32_t)>
static int32_t ToStringHelper(const void* value, char8_t* buffer, int32_t size)
{
	return to_string_func(*reinterpret_cast<const T*>(value), buffer, size);
}

template <class LHS, class RHS>
static int32_t Comparison(const LHS& lhs, const RHS& rhs)
{
	if (lhs < rhs) {
		return -1;
	} else if (lhs > rhs) {
		return 1;
	} else {
		return 0;
	}
}

template <class T>
static decltype(auto) PreIncrement(T& value)
{
	return ++value;
}

template <class T>
static decltype(auto) PostIncrement(T& value)
{
	return value++;
}

template <class T>
static decltype(auto) PreDecrement(T& value)
{
	return --value;
}

template <class T>
static decltype(auto) PostDecrement(T& value)
{
	return value--;
}

template <class LHS, class RHS>
static decltype(auto) Assignment(LHS& lhs, const RHS& rhs)
{
	return (lhs = rhs);
}

template <class LHS, class RHS>
static decltype(auto) AddAssignment(LHS& lhs, const RHS& rhs)
{
	return (lhs += rhs);
}

template <class LHS, class RHS>
static decltype(auto) SubAssignment(LHS& lhs, const RHS& rhs)
{
	return (lhs -= rhs);
}

template <class LHS, class RHS>
static decltype(auto) MulAssignment(LHS& lhs, const RHS& rhs)
{
	return (lhs *= rhs);
}

template <class LHS, class RHS>
static decltype(auto) DivAssignment(LHS& lhs, const RHS& rhs)
{
	return (lhs /= rhs);
}

template <class LHS, class RHS>
static decltype(auto) ModAssignment(LHS& lhs, const RHS& rhs)
{
	return (lhs %= rhs);
}

template <class LHS, class RHS>
static decltype(auto) BitAndAssignment(LHS& lhs, const RHS& rhs)
{
	return (lhs &= rhs);
}

template <class LHS, class RHS>
static decltype(auto) BitOrAssignment(LHS& lhs, const RHS& rhs)
{
	return (lhs |= rhs);
}

template <class LHS, class RHS>
static decltype(auto) BitXorAssignment(LHS& lhs, const RHS& rhs)
{
	return (lhs ^= rhs);
}

template <class LHS, class RHS>
static decltype(auto) BitShiftLeftAssignment(LHS& lhs, const RHS& rhs)
{
	return (lhs <<= rhs);
}

template <class LHS, class RHS>
static decltype(auto) BitShiftRightAssignment(LHS& lhs, const RHS& rhs)
{
	return (lhs >>= rhs);
}

NS_END
