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

#include "Esprit_ICondition.h"

NS_ESPRIT

class CheckVariableCondition final : public ICondition
{
public:
	enum class Operation
	{
		IsFalse,
		IsTrue,
		NotEqualTo,
		EqualTo,
		LessThan,
		GreaterThan,
		LessThanOrEqualTo,
		GreaterThanOrEqualTo,

		Count
	};

	CheckVariableCondition(void);
	~CheckVariableCondition(void);

	bool init(const StateMachine& owner) override;
	bool evaluate(const StateMachine& owner, VariableSet::Instance& variables) const override;

	// These two functions are only safe to call before init() is called.
	const OptimizedHashString32<>& getVariableName(void) const;
	void setVariableName(const OptimizedHashString32<>& name);

	VariableSet::VariableType getVariableType(void) const;
	void setVariableType(VariableSet::VariableType type);

	void setOperation(Operation operation);
	Operation getOperation(void) const;

	const U8String& getString(void) const;
	void setString(const U8String& value);

	float getFloat(void) const;
	void setFloat(float value);

	int64_t getInteger(void) const;
	void setInteger(int64_t value);

	bool getBool(void) const;
	void setBool(bool value);

private:
	VariableSet::VariableType _var_type = VariableSet::VariableType::Count;
	Operation _operation = Operation::Count;

	union
	{
		OptimizedHashString32<> _var_name;
		int32_t _var_index;
	};

	union
	{
		U8String _string;
		float _float;
		int64_t _integer;
		bool _bool;
	};
};

NS_END
