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

#include "Preproc_ParseNamespace.h"
#include "Preproc_ParseFile.h"

bool ParseNamespace(std::string_view substr, ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return false;
	}

	if (parse_data.flags.any() && !parse_data.flags.testAll(ParseData::Flag::NamespaceName)) {
		return false;
	}

	// Found the name of an namespace.
	if (parse_data.flags.testAll(ParseData::Flag::NamespaceName)) {
		parse_data.flags.clear(ParseData::Flag::NamespaceName);
		parse_data.namespace_runtime.name = substr;

		return true;
	}

	// Process "namespace" token.
	size_t namespace_index = substr.find("namespace");

	if (namespace_index == std::string_view::npos) {
		namespace_index = substr.find("NS_");

		// Might be bad to assume NS_* macros will be by themselves on their own lines, but going to assume that for now.
		if (namespace_index != 0) {
			return false;

		// $TODO (maybe): May want to have higher level logic parse the NS_* and NS_END scope delimeters.
		} else if (substr == "NS_END") {
			parse_data.scope_ranges.back().range.end = parse_data.start_index + 6;

			ProcessNamespaceScopeClose(parse_data);

			parse_data.scope_ranges.pop_back();

			return true;
		}

		// Parse the part that comes after "NS_". We are assuming all words will be _ separated in the macros.
		// For example, "namespace MyCoolNamespace {" would be "NS_MY_COOL_NAMESPACE".

		size_t prev_offset = 2;
		size_t offset = substr.find('_', 3);

		while (offset != std::string_view::npos) {
			std::string name_section(substr.substr(prev_offset + 1, offset - prev_offset - 1));

			// Lowercase all but the first letter of the word.
			for (size_t i = 1; i < name_section.size(); ++i) {
				// $TODO: This is a larger change, but may want to parse these files in UTF-8 so that we can handle names with non-ASCII characters.
				name_section[i] = static_cast<char>(std::tolower(static_cast<int>(name_section[i])));
			}

			prev_offset = offset;
			offset = substr.find('_', offset + 1);

			parse_data.namespace_runtime.name += name_section;
		}

		// Handle last section.
		std::string name_section(substr.substr(prev_offset + 1));

		// Lowercase all but the first letter of the word.
		for (size_t i = 1; i < name_section.size(); ++i) {
			// $TODO: This is a larger change, but may want to parse these files in UTF-8 so that we can handle names with non-ASCII characters.
			name_section[i] = static_cast<char>(std::tolower(static_cast<int>(name_section[i])));
		}

		parse_data.namespace_runtime.name += name_section;

		// $TODO (maybe): May want to have higher level logic parse the NS_* and NS_END scope delimeters.
		ScopeRuntimeData scope_data;
		scope_data.range.start = parse_data.start_index;

		parse_data.scope_ranges.emplace_back(scope_data);
		parse_data.namespace_runtime.valid = true;

		ProcessNamespaceScopeOpen(parse_data);

	} else {
		parse_data.flags.set(ParseData::Flag::NamespaceName);
		parse_data.namespace_runtime.valid = true;
	}

	return true;
}

void ProcessNamespaceScopeOpen(ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return;
	}

	if (parse_data.namespace_runtime.valid) {
		if (parse_data.scope_ranges.size() > 1) {
			const auto it = parse_data.scope_ranges.end() - 2;

			// Anonymous namespace. Just absorb the name of the previous scope.
			if (parse_data.namespace_runtime.name.empty()) {
				parse_data.scope_ranges.back().name = it->name;
			} else {
				parse_data.scope_ranges.back().name = it->name + "::" + parse_data.namespace_runtime.name;
			}

		// We are the only scope. Just take the namespace name.
		} else {
			parse_data.scope_ranges.back().name = parse_data.namespace_runtime.name;
		}

		parse_data.scope_ranges.back().type = ScopeRuntimeData::Type::Namespace;
		parse_data.namespace_runtime.valid = false;
	}
}

void ProcessNamespaceScopeClose(ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return;
	}
}
