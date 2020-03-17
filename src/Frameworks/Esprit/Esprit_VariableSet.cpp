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

#include "Esprit_VariableSet.h"

NS_ESPRIT

bool VariableSet::getVariable(Gaff::Hash32 name, const U8String*& result) const
{
	const auto* const entry = GetVariableEntry(name, _strings);

	if (entry) {
		result = &entry->value;
	}

	return entry != nullptr;
}

bool VariableSet::getVariable(const char* name, const U8String*& result) const
{
	return getVariable(Gaff::FNV1aHash32String(name), result);
}

bool VariableSet::getVariable(Gaff::Hash32 name, U8String& result) const
{
	const auto* const entry = GetVariableEntry(name, _strings);

	if (entry) {
		result = entry->value;
	}

	return entry != nullptr;
}

bool VariableSet::getVariable(const char* name, U8String& result) const
{
	return getVariable(Gaff::FNV1aHash32String(name), result);
}

void VariableSet::setVariable(Gaff::Hash32 name, const U8String& value)
{
	auto* const entry = GetVariableEntry(name, _strings);

	if (entry) {
		entry->value = value;
	}
}

void VariableSet::setVariable(const char* name, const U8String& value)
{
	setVariable(Gaff::FNV1aHash32String(name), value);
}

void VariableSet::setVariable(Gaff::Hash32 name, U8String&& value)
{
	auto* const entry = GetVariableEntry(name, _strings);

	if (entry) {
		entry->value = std::move(value);
	}
}

void VariableSet::setVariable(const char* name, U8String&& value)
{
	setVariable(Gaff::FNV1aHash32String(name), std::move(value));
}

bool VariableSet::getVariable(Gaff::Hash32 name, float& result) const
{
	const auto* const entry = GetVariableEntry(name, _floats);

	if (entry) {
		result = entry->value;
	}

	return entry != nullptr;
}

bool VariableSet::getVariable(const char* name, float& result) const
{
	return getVariable(Gaff::FNV1aHash32String(name), result);
}

void VariableSet::setVariable(Gaff::Hash32 name, float value)
{
	auto* const entry = GetVariableEntry(name, _floats);

	if (entry) {
		entry->value = value;
	}
}

void VariableSet::setVariable(const char* name, float value)
{
	setVariable(Gaff::FNV1aHash32String(name), value);
}

bool VariableSet::getVariable(Gaff::Hash32 name, int64_t& result) const
{
	const auto* const entry = GetVariableEntry(name, _integers);

	if (entry) {
		result = entry->value;
	}

	return entry != nullptr;
}

bool VariableSet::getVariable(const char* name, int64_t& result) const
{
	return getVariable(Gaff::FNV1aHash32String(name), result);
}

void VariableSet::setVariable(Gaff::Hash32 name, int64_t value)
{
	auto* const entry = GetVariableEntry(name, _integers);

	if (entry) {
		entry->value = value;
	}
}

void VariableSet::setVariable(const char* name, int64_t value)
{
	setVariable(Gaff::FNV1aHash32String(name), value);
}

bool VariableSet::getVariable(Gaff::Hash32 name, bool& result) const
{
	const auto it = Gaff::Find(_booleans.names, name);

	if (it == _booleans.names.end()) {
		// $TODO: Log error.
		return false;
	}

	const size_t index = eastl::distance(_booleans.names.begin(), it);
	result = _booleans.values[index];
	return true;
}

bool VariableSet::getVariable(const char* name, bool& result) const
{
	return getVariable(Gaff::FNV1aHash32String(name), result);
}

void VariableSet::setVariable(Gaff::Hash32 name, bool value)
{
	const auto it = Gaff::Find(_booleans.names, name);

	if (it == _booleans.names.end()) {
		// $TODO: Log error.
		return;
	}

	const size_t index = eastl::distance(_booleans.names.begin(), it);
	_booleans.values.set(index, value);
}

void VariableSet::setVariable(const char* name, bool value)
{
	setVariable(Gaff::FNV1aHash32String(name), value);
}

template <class T>
const VariableSet::Variable<T>* VariableSet::GetVariableEntry(Gaff::Hash32 name, const Vector< Variable<T> >& variables)
{
	return GetVariableEntry(name, const_cast<Vector< Variable<T> >&>(variables));
}

template <class T>
VariableSet::Variable<T>* VariableSet::GetVariableEntry(Gaff::Hash32 name, Vector< Variable<T> >& variables)
{
	auto it = Gaff::Find(variables, name, [](const Variable<T>& lhs, Gaff::Hash32 rhs) -> bool
	{
		return lhs.name == rhs;
	});

	return (it == variables.end()) ? nullptr : it;
}

NS_END
