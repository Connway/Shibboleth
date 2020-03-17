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

bool VariableSet::addVariableString(const HashStringTemp32<>& name)
{
	const int32_t index = GetVariableIndex(name, _strings);

	if (index > -1) {
		// $TODO: Log error.
		return false;
	}

	auto& variable = _strings.emplace_back();
	variable.name = name;
	return true;
}

bool VariableSet::removeVariableString(const HashStringTemp32<>& name)
{
	const int32_t index = GetVariableIndex(name, _strings);

	if (index < 0) {
		// $TODO: Log error.
		return false;
	}

	_strings.erase_unsorted(_strings.begin() + index);
	return true;
}

bool VariableSet::addVariableFloat(const HashStringTemp32<>& name)
{
	const int32_t index = GetVariableIndex(name, _floats);

	if (index > -1) {
		// $TODO: Log error.
		return false;
	}

	auto& variable = _floats.emplace_back();
	variable.name = name;
	return true;
}

bool VariableSet::removeVariableFloat(const HashStringTemp32<>& name)
{
	const int32_t index = GetVariableIndex(name, _floats);

	if (index < 0) {
		// $TODO: Log error.
		return false;
	}

	_floats.erase_unsorted(_floats.begin() + index);
	return true;
}

bool VariableSet::addVariableInteger(const HashStringTemp32<>& name)
{
	const int32_t index = GetVariableIndex(name, _integers);

	if (index > -1) {
		// $TODO: Log error.
		return false;
	}

	auto& variable = _integers.emplace_back();
	variable.name = name;
	return true;
}

bool VariableSet::removeVariableInteger(const HashStringTemp32<>& name)
{
	const int32_t index = GetVariableIndex(name, _integers);

	if (index < 0) {
		// $TODO: Log error.
		return false;
	}

	_integers.erase_unsorted(_integers.begin() + index);
	return true;
}

bool VariableSet::addVariableBool(const HashStringTemp32<>& name)
{
	const auto it = Gaff::LowerBound(_booleans.names, name);

	if (it == _booleans.names.end() || *it != name) {
		_booleans.names.emplace_back(name);
		_booleans.values.push_back();
		return true;
	}

	// $TODO: Log error.
	return false;
}

bool VariableSet::removeVariableBool(const HashStringTemp32<>& name)
{
	const auto it = Gaff::LowerBound(_booleans.names, name);

	if (it == _booleans.names.end() || *it != name) {
		// $TODO: Log error.
		return false;
	}

	_booleans.names.erase_unsorted(it);
	_booleans.values.pop_back();
	return true;
}

int32_t VariableSet::getVariableIndexString(const HashStringTemp32<>& name) const
{
	return GetVariableIndex(name, _strings);
}

int32_t VariableSet::getVariableIndexFloat(const HashStringTemp32<>& name) const
{
	return GetVariableIndex(name, _floats);
}

int32_t VariableSet::getVariableIndexInteger(const HashStringTemp32<>& name) const
{
	return GetVariableIndex(name, _integers);
}

int32_t VariableSet::getVariableIndexBool(const HashStringTemp32<>& name) const
{
	const auto it = Gaff::LowerBound(_booleans.names, name);

	if (it == _booleans.names.end() || *it != name) {
		return -1;
	}

	return static_cast<int32_t>(eastl::distance(_booleans.names.begin(), it));
}

void VariableSet::finalize(void)
{
	eastl::sort(_strings.begin(), _strings.end(), VariablePredicate<U8String>);
	eastl::sort(_floats.begin(), _floats.end(), VariablePredicate<float>);
	eastl::sort(_integers.begin(), _integers.end(), VariablePredicate<int64_t>);
	eastl::sort(_booleans.names.begin(), _booleans.names.end());
}

bool VariableSet::getVariable(const HashStringTemp32<>& name, const U8String*& result) const
{
	return getVariable(GetVariableIndex(name, _strings), result);
}

bool VariableSet::getVariable(int32_t index, const U8String*& result) const
{
	if (!Gaff::Between(index, 0, static_cast<int32_t>(_strings.size()))) {
		// $TODO: Log error.
		return false;
	}

	result = &_strings[index].value;
	return true;
}

bool VariableSet::getVariable(const HashStringTemp32<>& name, U8String& result) const
{
	return getVariable(GetVariableIndex(name, _strings), result);
}

bool VariableSet::getVariable(int32_t index, U8String& result) const
{
	if (!Gaff::Between(index, 0, static_cast<int32_t>(_strings.size()))) {
		// $TODO: Log error.
		return false;
	}

	result = _strings[index].value;
	return true;
}

void VariableSet::setVariable(const HashStringTemp32<>& name, const U8String& value)
{
	setVariable(GetVariableIndex(name, _strings), value);
}

void VariableSet::setVariable(int32_t index, const U8String& value)
{
	if (!Gaff::Between(index, 0, static_cast<int32_t>(_strings.size()))) {
		// $TODO: Log error.
		return;
	}

	_strings[index].value = value;
}

void VariableSet::setVariable(const HashStringTemp32<>& name, U8String&& value)
{
	setVariable(GetVariableIndex(name, _strings), std::move(value));
}

void VariableSet::setVariable(int32_t index, U8String&& value)
{
	if (!Gaff::Between(index, 0, static_cast<int32_t>(_strings.size()))) {
		// $TODO: Log error.
		return;
	}

	_strings[index].value = std::move(value);
}

bool VariableSet::getVariable(const HashStringTemp32<>& name, float& result) const
{
	return getVariable(GetVariableIndex(name, _floats), result);
}

bool VariableSet::getVariable(int32_t index, float& result) const
{
	if (!Gaff::Between(index, 0, static_cast<int32_t>(_floats.size()))) {
		// $TODO: Log error.
		return false;
	}

	result = _floats[index].value;
	return true;
}

void VariableSet::setVariable(const HashStringTemp32<>& name, float value)
{
	setVariable(GetVariableIndex(name, _floats), value);
}

void VariableSet::setVariable(int32_t index, float value)
{
	if (!Gaff::Between(index, 0, static_cast<int32_t>(_floats.size()))) {
		// $TODO: Log error.
		return;
	}

	_floats[index].value = value;
}

bool VariableSet::getVariable(const HashStringTemp32<>& name, int64_t& result) const
{
	return getVariable(GetVariableIndex(name, _integers), result);
}

bool VariableSet::getVariable(int32_t index, int64_t& result) const
{
	if (!Gaff::Between(index, 0, static_cast<int32_t>(_integers.size()))) {
		// $TODO: Log error.
		return false;
	}

	result = _integers[index].value;
	return true;
}

void VariableSet::setVariable(const HashStringTemp32<>& name, int64_t value)
{
	setVariable(GetVariableIndex(name, _integers), value);
}

void VariableSet::setVariable(int32_t index, int64_t value)
{
	if (!Gaff::Between(index, 0, static_cast<int32_t>(_integers.size()))) {
		// $TODO: Log error.
		return;
	}

	_integers[index].value = value;
}

bool VariableSet::getVariable(const HashStringTemp32<>& name, bool& result) const
{
	return getVariable(getVariableIndexBool(name), result);
}

bool VariableSet::getVariable(int32_t index, bool& result) const
{
	if (!Gaff::Between(index, 0, static_cast<int32_t>(_booleans.names.size()))) {
		// $TODO: Log error.
		return false;
	}

	result = _booleans.values[index];
	return true;
}

void VariableSet::setVariable(const HashStringTemp32<>& name, bool value)
{
	setVariable(getVariableIndexBool(name), value);
}

void VariableSet::setVariable(int32_t index, bool value)
{
	if (!Gaff::Between(index, 0, static_cast<int32_t>(_booleans.names.size()))) {
		// $TODO: Log error.
		return;
	}

	_booleans.values.set(index, value);
}

template <class T>
bool VariableSet::VariablePredicate(const Variable<T>& lhs, const Variable<T>& rhs)
{
	return lhs.name < rhs.name;
}

template <class T>
int32_t VariableSet::GetVariableIndex(const HashStringTemp32<>& name, const Vector< Variable<T> >& variables)
{
	const auto it = Gaff::LowerBound(variables, name, [](const Variable<T>& lhs, Gaff::Hash32 rhs) -> bool
	{
		return lhs.name < rhs;
	});

	return (it == variables.end() || it->name != name) ? -1 : static_cast<int32_t>(eastl::distance(variables.begin(), it));
}

NS_END
