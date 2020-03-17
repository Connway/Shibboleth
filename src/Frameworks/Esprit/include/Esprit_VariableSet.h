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

class VariableSet
{
public:
	bool getVariable(Gaff::Hash32 name, const U8String*& result) const;
	bool getVariable(const char* name, const U8String*& result) const;
	bool getVariable(Gaff::Hash32 name, U8String& result) const;
	bool getVariable(const char* name, U8String& result) const;
	void setVariable(Gaff::Hash32 name, const U8String& value);
	void setVariable(const char* name, const U8String& value);
	void setVariable(Gaff::Hash32 name, U8String&& value);
	void setVariable(const char* name, U8String&& value);

	bool getVariable(Gaff::Hash32 name, float& result) const;
	bool getVariable(const char* name, float& result) const;
	void setVariable(Gaff::Hash32 name, float value);
	void setVariable(const char* name, float value);

	bool getVariable(Gaff::Hash32 name, int64_t& result) const;
	bool getVariable(const char* name, int64_t& result) const;
	void setVariable(Gaff::Hash32 name, int64_t value);
	void setVariable(const char* name, int64_t value);

	bool getVariable(Gaff::Hash32 name, bool& result) const;
	bool getVariable(const char* name, bool& result) const;
	void setVariable(Gaff::Hash32 name, bool value);
	void setVariable(const char* name, bool value);

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
	static const Variable<T>* GetVariableEntry(Gaff::Hash32 name, const Vector< Variable<T> >& variables);

	template <class T>
	static Variable<T>* GetVariableEntry(Gaff::Hash32 name, Vector< Variable<T> >& variables);
};

NS_END
