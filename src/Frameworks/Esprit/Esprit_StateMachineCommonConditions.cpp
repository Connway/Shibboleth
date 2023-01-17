/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Esprit_StateMachineCommonConditions.h"
#include "Esprit_StateMachine.h"

NS_ESPRIT

template <class T>
static bool IsFalse(const T& value)
{
	return !value;
}

template <>
bool IsFalse(const U8String& value)
{
	return value.empty();
}

template <class T>
static bool IsTrue(const T& value)
{
	return value;
}

template <>
bool IsTrue(const U8String& value)
{
	return !value.empty();
}

template <class T>
static bool NotEqualTo(const T& value, const T& other_value)
{
	return value != other_value;
}

template <class T>
static bool EqualTo(const T& value, const T& other_value)
{
	return value == other_value;
}

template <class T>
static bool LessThan(const T& value, const T& other_value)
{
	return value < other_value;
}

template <class T>
static bool GreaterThan(const T& value, const T& other_value)
{
	return value > other_value;
}

template <class T>
static bool LessThanOrEqualTo(const T& value, const T& other_value)
{
	return value <= other_value;
}

template <class T>
static bool GreaterThanOrEqualTo(const T& value, const T& other_value)
{
	return value >= other_value;
}

template <class T>
static bool Evaluate(const T& value, const T& other_value, CheckVariableCondition::Operation operation)
{
	switch (operation) {
		case CheckVariableCondition::Operation::IsFalse:
			return IsFalse(value);

		case CheckVariableCondition::Operation::IsTrue:
			return IsTrue(value);

		case CheckVariableCondition::Operation::NotEqualTo:
			return NotEqualTo(value, other_value);

		case CheckVariableCondition::Operation::EqualTo:
			return EqualTo(value, other_value);

		case CheckVariableCondition::Operation::LessThan:
			return LessThan(value, other_value);

		case CheckVariableCondition::Operation::GreaterThan:
			return GreaterThan(value, other_value);

		case CheckVariableCondition::Operation::LessThanOrEqualTo:
			return LessThanOrEqualTo(value, other_value);

		case CheckVariableCondition::Operation::GreaterThanOrEqualTo:
			return GreaterThanOrEqualTo(value, other_value);

	default:
		break;
	}

	return false;
}

CheckVariableCondition::CheckVariableCondition(void)
{
}

CheckVariableCondition::~CheckVariableCondition(void)
{
	if (static_cast<int32_t>(_operation) >= static_cast<int32_t>(Operation::Count)) {
		_var_name.~OptimizedHashString32<>();
	}

	if (_var_type == VariableSet::VariableType::String) {
		_string.~U8String();
	}
}

bool CheckVariableCondition::init(const StateMachine& owner)
{
	if (_var_type == VariableSet::VariableType::Count) {
		// $TODO: Log error.
		return false;
	}

	if (_operation == Operation::Count) {
		// $TODO: Log error.
		return false;
	}

	const VariableSet& variables = owner.getVariables();
	const int32_t index = variables.getVariableIndex(_var_name, _var_type);

	if (index < 0) {
		_operation = static_cast<Operation>(static_cast<int32_t>(_operation) + static_cast<int32_t>(Operation::Count));
		// $TODO: Log error.
		return false;
	}

	_var_name.~OptimizedHashString32<>();
	_var_index = index;
	return true;
}

bool CheckVariableCondition::evaluate(const StateMachine& owner, VariableSet::Instance& variables) const
{
	GAFF_REF(owner);

	switch (_var_type) {
		case VariableSet::VariableType::String:
			return Evaluate(variables.strings[_var_index], _string, _operation);

		case VariableSet::VariableType::Float:
			return Evaluate(variables.floats[_var_index], _float, _operation);

		case VariableSet::VariableType::Integer:
			return Evaluate(variables.integers[_var_index], _integer, _operation);

		case VariableSet::VariableType::Bool:
			return Evaluate<bool>(variables.bools[_var_index], _bool, _operation);

		case VariableSet::VariableType::Reference:
			// Not supported.

		default:
			break;
	}

	return false;
}

// These two functions are only safe to call before init() is called.
const OptimizedHashString32<>& CheckVariableCondition::getVariableName(void) const
{
	return _var_name;
}

void CheckVariableCondition::setVariableName(const HashStringView32<>& name)
{
	_var_name = name;
}

VariableSet::VariableType CheckVariableCondition::getVariableType(void) const
{
	return _var_type;
}

void CheckVariableCondition::setVariableType(VariableSet::VariableType type)
{
	_var_type = type;
}

void CheckVariableCondition::setOperation(Operation operation)
{
	_operation = operation;
}

CheckVariableCondition::Operation CheckVariableCondition::getOperation(void) const
{
	return _operation;
}

const U8String& CheckVariableCondition::getString(void) const
{
	return _string;
}

void CheckVariableCondition::setString(const U8String& value)
{
	_string = value;
}

float CheckVariableCondition::getFloat(void) const
{
	return _float;
}

void CheckVariableCondition::setFloat(float value)
{
	_float = value;
}

int64_t CheckVariableCondition::getInteger(void) const
{
	return _integer;
}

void CheckVariableCondition::setInteger(int64_t value)
{
	_integer = value;
}

bool CheckVariableCondition::getBool(void) const
{
	return _bool;
}

void CheckVariableCondition::setBool(bool value)
{
	_bool = value;
}

NS_END
