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
#include <EASTL/sort.h>

NS_ESPRIT

static int32_t GetVariableIndex(const HashStringView32<>& name, const Vector< OptimizedHashString32<> >& variables)
{
	const auto it = Gaff::LowerBound(variables, name);
	return (it == variables.end() || *it != name) ? -1 : static_cast<int32_t>(eastl::distance(variables.begin(), it));
}

int32_t VariableSet::getVariableIndex(const HashStringView32<>& name, VariableType type) const
{
	auto& names = _names[static_cast<int32_t>(type)];
	return GetVariableIndex(name, names);
}

bool VariableSet::removeVariable(const HashStringView32<>& name, VariableType type)
{
	auto& names = _names[static_cast<int32_t>(type)];
	const int32_t index = GetVariableIndex(name, names);

	if (index < 0) {
		// $TODO: Log error.
		return false;
	}

	names.erase_unsorted(names.begin() + index);
	return true;
}

bool VariableSet::addVariable(const HashStringView32<>& name, VariableType type)
{
	auto& names = _names[static_cast<int32_t>(type)];
	const int32_t index = GetVariableIndex(name, names);

	if (index > -1) {
		// $TODO: Log error.
		return false;
	}

	names.emplace_back(name);
	return true;
}

const VariableSet::Instance& VariableSet::getDefaults(void) const
{
	return _defaults;
}

VariableSet::Instance& VariableSet::getDefaults(void)
{
	return _defaults;
}

void VariableSet::finalize(void)
{
	for (int32_t i = 0; i < static_cast<int32_t>(VariableType::Count); ++i) {
		eastl::sort(_names[i].begin(), _names[i].end());
	}

	_defaults.references.resize(_names[static_cast<size_t>(VariableType::Reference)].size(), nullptr);
	_defaults.strings.resize(_names[static_cast<size_t>(VariableType::String)].size());
	_defaults.floats.resize(_names[static_cast<size_t>(VariableType::Float)].size(), 0.0f);
	_defaults.integers.resize(_names[static_cast<size_t>(VariableType::Integer)].size(), 0);
	_defaults.bools.resize(_names[static_cast<size_t>(VariableType::Bool)].size(), false);
}

bool VariableSet::getVariable(const Instance& variables, int32_t index, void*& result) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::Reference)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	result = variables.references[index];
	return true;
}

bool VariableSet::setVariable(Instance& variables, int32_t index, void* value) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::Reference)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	variables.references[index] = value;
	return true;
}

bool VariableSet::getVariable(const Instance& variables, int32_t index, const U8String*& result) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::String)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	result = &variables.strings[index];
	return true;
}

bool VariableSet::getVariable(const Instance& variables, int32_t index, U8String& result) const
{
	const U8String* string_ptr = nullptr;

	if (!getVariable(variables, index, string_ptr)) {
		return false;
	}

	result = *string_ptr;
	return true;
}

bool VariableSet::setVariable(Instance& variables, int32_t index, const U8String& value) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::String)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	variables.strings[index] = value;
	return true;
}

bool VariableSet::setVariable(Instance& variables, int32_t index, U8String&& value) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::String)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	variables.strings[index] = std::move(value);
	return true;
}

bool VariableSet::getVariable(const Instance& variables, int32_t index, float& result) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::Float)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	result = variables.floats[index];
	return true;
}

bool VariableSet::setVariable(Instance& variables, int32_t index, float value) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::Float)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	variables.floats[index] = value;
	return true;
}

bool VariableSet::getVariable(const Instance& variables, int32_t index, int64_t& result) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::Integer)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	result = variables.integers[index];
	return true;
}

bool VariableSet::setVariable(Instance& variables, int32_t index, int64_t value) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::Integer)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	variables.integers[index] = value;
	return true;
}

bool VariableSet::getVariable(const Instance& variables, int32_t index, bool& result) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::Bool)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	result = variables.bools[index];
	return true;
}

bool VariableSet::setVariable(Instance& variables, int32_t index, bool value) const
{
	const auto& names = _names[static_cast<int32_t>(VariableType::Bool)];

	if (!Gaff::Between(index, 0, static_cast<int32_t>(names.size()))) {
		// $TODO: Log error.
		return false;
	}

	variables.bools.set(static_cast<size_t>(index), value);
	return true;
}

const U8String& VariableSet::getString(const Instance& variables, int32_t index) const
{
	const U8String* result = nullptr;
	getVariable(variables, index, result);

	GAFF_ASSERT(result);
	return *result;
}

float VariableSet::getFloat(const Instance& variables, int32_t index) const
{
	float result = 0.0f;
	getVariable(variables, index, result);
	return result;
}

int64_t VariableSet::getInteger(const Instance& variables, int32_t index) const
{
	int64_t result = 0;
	getVariable(variables, index, result);
	return result;
}

bool VariableSet::getBool(const Instance& variables, int32_t index) const
{
	bool result = false;
	getVariable(variables, index, result);
	return result;
}

NS_END
