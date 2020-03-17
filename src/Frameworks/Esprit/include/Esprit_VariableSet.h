/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Esprit_HashString.h"
#include "Esprit_BitVector.h"
#include "Esprit_Vector.h"
#include "Esprit_String.h"

NS_ESPRIT

class VariableSet final
{
public:
	bool addVariableString(const HashStringTemp32<>& name);
	bool removeVariableString(const HashStringTemp32<>& name);

	bool addVariableFloat(const HashStringTemp32<>& name);
	bool removeVariableFloat(const HashStringTemp32<>& name);

	bool addVariableInteger(const HashStringTemp32<>& name);
	bool removeVariableInteger(const HashStringTemp32<>& name);

	bool addVariableBool(const HashStringTemp32<>& name);
	bool removeVariableBool(const HashStringTemp32<>& name);

	int32_t getVariableIndexString(const HashStringTemp32<>& name) const;
	int32_t getVariableIndexFloat(const HashStringTemp32<>& name) const;
	int32_t getVariableIndexInteger(const HashStringTemp32<>& name) const;
	int32_t getVariableIndexBool(const HashStringTemp32<>& name) const;

	// Once called, add/remove functions should not be called.
	void finalize(void);

	bool getVariable(const HashStringTemp32<>& name, const U8String*& result) const;
	bool getVariable(int32_t index, const U8String*& result) const;
	bool getVariable(const HashStringTemp32<>& name, U8String& result) const;
	bool getVariable(int32_t index, U8String& result) const;
	void setVariable(const HashStringTemp32<>& name, const U8String& value);
	void setVariable(int32_t index, const U8String& value);
	void setVariable(const HashStringTemp32<>& name, U8String&& value);
	void setVariable(int32_t index, U8String&& value);

	bool getVariable(const HashStringTemp32<>& name, float& result) const;
	bool getVariable(int32_t index, float& result) const;
	void setVariable(const HashStringTemp32<>& name, float value);
	void setVariable(int32_t index, float value);

	bool getVariable(const HashStringTemp32<>& name, int64_t& result) const;
	bool getVariable(int32_t index, int64_t& result) const;
	void setVariable(const HashStringTemp32<>& name, int64_t value);
	void setVariable(int32_t index, int64_t value);

	bool getVariable(const HashStringTemp32<>& name, bool& result) const;
	bool getVariable(int32_t index, bool& result) const;
	void setVariable(const HashStringTemp32<>& name, bool value);
	void setVariable(int32_t index, bool value);

#ifdef _DEBUG
	// Add debug query functions here.
#endif

private:
	struct BooleanVariables final
	{
		Vector< OptimizedHashString32<> > names;
		BitVector values;
	};

	template <class T>
	struct Variable final
	{
		OptimizedHashString32<> name;
		T value;
	};

	Vector< Variable<U8String> > _strings;
	Vector< Variable<float> > _floats;
	Vector< Variable<int64_t> > _integers;
	BooleanVariables _booleans;

	template <class T>
	static bool VariablePredicate(const Variable<T>& lhs, const Variable<T>& rhs);

	template <class T>
	static int32_t GetVariableIndex(const HashStringTemp32<>& name, const Vector< Variable<T> >& variables);
};

NS_END
