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
	struct Instance final
	{
		Vector<void*> references;
		Vector<U8String> strings;
		Vector<float> floats;
		Vector<int64_t> integers;
		BitVector bools;

		void init(const VariableSet& vars)
		{
			references.resize(vars._names[static_cast<size_t>(VariableType::Reference)].size());
			strings.resize(vars._names[static_cast<size_t>(VariableType::String)].size());
			floats.resize(vars._names[static_cast<size_t>(VariableType::Float)].size());
			integers.resize(vars._names[static_cast<size_t>(VariableType::Integer)].size());
			bools.resize(vars._names[static_cast<size_t>(VariableType::Bool)].size());
		}
	};

	enum class VariableType
	{
		Reference,
		String,
		Float,
		Integer,
		Bool,

		Count
	};

	int32_t getVariableIndex(const HashStringTemp32<>& name, VariableType type) const;
	bool removeVariable(const HashStringTemp32<>& name, VariableType type);
	bool addVariable(const HashStringTemp32<>& name, VariableType type);

	// Once called, add/remove functions should not be called.
	void finalize(void);

	bool getVariable(const Instance& variables, int32_t index, void*& result) const;
	bool setVariable(Instance& variables, int32_t index, void* value) const;

	bool getVariable(const Instance& variables, int32_t index, const U8String*& result) const;
	bool getVariable(const Instance& variables, int32_t index, U8String& result) const;
	bool setVariable(Instance& variables, int32_t index, const U8String& value) const;
	bool setVariable(Instance& variables, int32_t index, U8String&& value) const;

	bool getVariable(const Instance& variables, int32_t index, float& result) const;
	bool setVariable(Instance& variables, int32_t index, float value) const;

	bool getVariable(const Instance& variables, int32_t index, int64_t& result) const;
	bool setVariable(Instance& variables, int32_t index, int64_t value) const;

	bool getVariable(const Instance& variables, int32_t index, bool& result) const;
	bool setVariable(Instance& variables, int32_t index, bool value) const;

	const U8String& getString(const Instance& variables, int32_t index) const;
	float getFloat(const Instance& variables, int32_t index) const;
	int64_t getInteger(const Instance& variables, int32_t index) const;
	bool getBool(const Instance& variables, int32_t index) const;

#ifdef _DEBUG
	// Add debug query functions here.
#endif

private:

	Vector< OptimizedHashString32<> > _names[static_cast<size_t>(VariableType::Count)];
};

NS_END
