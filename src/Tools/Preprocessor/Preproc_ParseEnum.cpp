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

	if (parse_data.flags.testRangeAny(ParseData::Flag::FirstRuntimeFlag, ParseData::Flag::LastRuntimeFlag) &&
		!parse_data.flags.testAny(
			ParseData::Flag::EnumName,
			ParseData::Flag::EnumStorage,
			ParseData::Flag::EnumEntries,
			ParseData::Flag::EnumNextEntry)) {

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
			parse_data.enum_runtime.flags.clear();
			return true;
		}

		// Handle "enum class <name>:<storage>" scenarios.
		std::string_view left, right;

		if (const size_t index = SplitSubstr(substr, ':', left, right); index != std::string_view::npos) {
			if (!left.empty()) {
				ParseEnum(left, parse_data);
			}

			if (!right.empty()) {
				const size_t start_index = parse_data.start_index;
				parse_data.start_index += index + 1;

				ParseEnum(right, parse_data);

				parse_data.start_index = start_index;
			}
		}

		parse_data.flags.set(ParseData::Flag::EnumStorage);
		parse_data.flags.clear(ParseData::Flag::EnumName);
		parse_data.enum_runtime.data.name = substr;

		return true;

	// We've parsed an "enum <name>" or "enum class <name>" line, but the scope isn't open yet.
	} else if (parse_data.flags.testAll(ParseData::Flag::EnumStorage)) {
		if (substr.back() == ';') {
			// Just a forward declare.
			parse_data.flags.clear(ParseData::Flag::EnumStorage);
			parse_data.enum_runtime.data.name.clear();
			parse_data.enum_runtime.flags.clear();

			return true;
		}

		std::string_view left, right;

		if (const size_t index = SplitSubstr(substr, ':', left, right); index != std::string_view::npos) {
			// This should be empty if we've reached this point.
			if (!left.empty()) {
				ParseEnum(left, parse_data);
			}

			if (!right.empty()) {
				const size_t start_index = parse_data.start_index;
				parse_data.start_index += index + 1;

				ParseEnum(right, parse_data);

				parse_data.start_index = start_index;
			}

		} else {
			parse_data.flags.clear(ParseData::Flag::EnumStorage);
			parse_data.enum_runtime.data.storage_type = substr;
		}

		return true;

	} else if (parse_data.flags.testAll(ParseData::Flag::EnumEntries)) {
		if (parse_data.flags.testAll(ParseData::Flag::EnumNextEntry)) {
			const size_t comma_index = substr.find(',');
			parse_data.flags.set(comma_index == std::string_view::npos, ParseData::Flag::EnumNextEntry);

			return true;
		}

		const size_t assign_index = substr.find('=');
		const size_t comma_index = substr.find(',');

		std::string_view entry_name;

		if (assign_index != std::string_view::npos && assign_index != 0) {
			entry_name = substr.substr(0, assign_index - 1);

		} else if (assign_index == std::string_view::npos && comma_index != std::string_view::npos) {
			entry_name = substr.substr(0, comma_index - 1);
		}

		// Trim whitespace.
		if (!entry_name.empty()) {
			size_t whitespace_index = entry_name.find_first_of(k_whitespace_chars);

			while (whitespace_index != std::string_view::npos) {
				if (whitespace_index == 0) {
					entry_name = entry_name.substr(1);
				}
				else if (whitespace_index == (entry_name.size() - 1)) {
					entry_name = entry_name.substr(0, whitespace_index - 1);
				}

				whitespace_index = entry_name.find_first_of(k_whitespace_chars);
			}

			parse_data.flags.set(comma_index == std::string_view::npos, ParseData::Flag::EnumNextEntry);
			parse_data.enum_runtime.data.entries.emplace_back(entry_name);
		}

		return true;

	} else if (substr == "enum") {
		parse_data.enum_runtime.flags.set(EnumRuntimeData::Flag::Valid);
		parse_data.flags.set(ParseData::Flag::EnumName);
		return true;
	} 

	return false;
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
			if (parse_data.enum_runtime.data.name.empty()) {
				parse_data.enum_runtime.flags.set(EnumRuntimeData::Flag::Anonymous);
				parse_data.scope_ranges.back().name = it->name;

			} else {
				parse_data.scope_ranges.back().name = it->name + "::" + parse_data.enum_runtime.data.name;
			}

		// We are the only scope. Just take the enum name.
		} else {
			parse_data.scope_ranges.back().name = parse_data.enum_runtime.data.name;
		}

		parse_data.enum_runtime.scope_range_index = parse_data.scope_ranges.size() - 1;
		parse_data.enum_runtime.data.name = parse_data.scope_ranges.back().name;
		parse_data.scope_ranges.back().type = ScopeRuntimeData::Type::Enum;

		parse_data.flags.clear(ParseData::Flag::EnumStorage);
		parse_data.flags.set(ParseData::Flag::EnumEntries);
	}
}

void ProcessEnumScopeClose(ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return;
	}

	if (parse_data.enum_runtime.flags.testAll(EnumRuntimeData::Flag::Valid) && parse_data.enum_runtime.scope_range_index == (parse_data.scope_ranges.size() - 1)) {
		// Add enum data to global runtime data.
		parse_data.global_runtime->enum_data[parse_data.enum_runtime.data.name] = std::move(parse_data.enum_runtime.data);

		parse_data.enum_runtime.scope_range_index = SIZE_T_FAIL;
		parse_data.enum_runtime.flags.clear();

		parse_data.flags.clear(ParseData::Flag::EnumEntries, ParseData::Flag::EnumNextEntry);
	}
}
