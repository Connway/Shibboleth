/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Preproc_ParseEnum.h"
#include "Preproc_ParseFile.h"

// We currently have no special features that require processing enums.
// Does nothing for now other than to mark enums so that "enum class" is not intepreted as a class.

bool ParseEnum(std::string_view substr, ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return false;
	}

	if (parse_data.flags.any() && !parse_data.flags.testAll(ParseData::Flag::EnumName)) {
		return false;
	}

	// Found the name of an enum.
	if (parse_data.flags.testAll(ParseData::Flag::EnumName)) {
		// If marked "enum class", keep parsing.
		if (substr == "class") {
			return true;

		} else if (substr.back() == ';') {
			// Just a forward declare.
			parse_data.flags.clear(ParseData::Flag::EnumName);
			return true;
		}

		// $TODO: Enum forward declares can take the form of "enum <name> : <type>;". Will need to handle that.
		// $TODO: Enum names could (worst case) take the form of "enum <name>:<type>". Will need to handle that case.
		// $TODO: Add enum data.

		parse_data.flags.clear(ParseData::Flag::EnumName);
		return true;
	}

	// Process "enum" token.
	const size_t enum_index = substr.find("enum");

	if (enum_index != 0) {
		return false;
	}

	parse_data.enum_runtime.flags.set(EnumRuntimeData::Flag::Valid);
	parse_data.flags.set(ParseData::Flag::EnumName);
	return true;
}

void ProcessEnumScopeOpen(ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return;
	}

	if (parse_data.enum_runtime.flags.testAll(EnumRuntimeData::Flag::Valid) && parse_data.enum_runtime.scope_range_index == SIZE_T_FAIL) {
		if (parse_data.scope_ranges.size() > 1) {
			const auto it = parse_data.scope_ranges.end() - 2;

			// Anonymous enum. Just absorb the name of the previous scope.
			if (parse_data.enum_runtime.name.empty()) {
				parse_data.enum_runtime.flags.set(EnumRuntimeData::Flag::Anonymous);
				parse_data.scope_ranges.back().name = it->name;

			} else {
				parse_data.scope_ranges.back().name = it->name + "::" + parse_data.enum_runtime.name;
			}

		// We are the only scope. Just take the enum name.
		} else {
			parse_data.scope_ranges.back().name = parse_data.enum_runtime.name;
		}

		parse_data.enum_runtime.scope_range_index = parse_data.scope_ranges.size() - 1;
		parse_data.scope_ranges.back().type = ScopeRuntimeData::Type::Enum;
	}
}

void ProcessEnumScopeClose(ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return;
	}

	if (parse_data.enum_runtime.flags.testAll(EnumRuntimeData::Flag::Valid) && parse_data.enum_runtime.scope_range_index == (parse_data.scope_ranges.size() - 1)) {
		parse_data.enum_runtime.flags.clear(EnumRuntimeData::Flag::Valid);
	}
}
