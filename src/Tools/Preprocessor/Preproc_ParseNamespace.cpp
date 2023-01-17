/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

namespace
{
	// Parse the part that comes after "NS_". We are assuming all words will be _ separated in the macros.
	// For example, "namespace MyCoolNamespace {" would be "NS_MY_COOL_NAMESPACE".
	// This function assumes all namespace macros take the form of "#define NS_MY_NAMESPACE namespace MyNamespace {".
	// If the opening bracket is smooshed together with the namespace name, this function will break.
	static std::string ProcessNamespaceDefine(std::string_view namespace_text, const ParseData& parse_data)
	{
		size_t prev_offset = 2;
		size_t offset = namespace_text.find('_', 3);
		std::string name;

		while (offset != std::string_view::npos) {
			std::string name_section(namespace_text.substr(prev_offset + 1, offset - prev_offset - 1));

			// Lowercase all but the first letter of the word.
			for (size_t i = 1; i < name_section.size(); ++i) {
				// $TODO: This is a larger change, but may want to parse these files in UTF-8 so that we can handle names with non-ASCII characters.
				name_section[i] = static_cast<char>(std::tolower(static_cast<int>(name_section[i])));
			}

			prev_offset = offset;
			offset = namespace_text.find('_', offset + 1);

			name += name_section;
		}

		// Handle last section.
		std::string name_section(namespace_text.substr(prev_offset + 1));

		// Lowercase all but the first letter of the word.
		for (size_t i = 1; i < name_section.size(); ++i) {
			// $TODO: This is a larger change, but may want to parse these files in UTF-8 so that we can handle names with non-ASCII characters.
			name_section[i] = static_cast<char>(std::tolower(static_cast<int>(name_section[i])));
		}

		name += name_section;

		if (const auto it = parse_data.global_runtime->namespace_mappings.find(name); it != parse_data.global_runtime->namespace_mappings.end()) {
			return it->second;
		}

		return name;
	}
}

bool ParseNamespace(std::string_view substr, ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return false;
	}

	if (parse_data.flags.testRangeAny(ParseData::Flag::FirstRuntimeFlag, ParseData::Flag::LastRuntimeFlag) &&
		!parse_data.flags.testAll(ParseData::Flag::NamespaceName)) {

		return false;
	}

	// Found the name of an namespace.
	if (parse_data.flags.testAll(ParseData::Flag::NamespaceName)) {
		parse_data.flags.clear(ParseData::Flag::NamespaceName);
		parse_data.namespace_runtime.name = substr;

		return true;
	}

	// Process "namespace" token.
	if (substr == "namespace") {
		parse_data.flags.set(ParseData::Flag::NamespaceName);
		parse_data.namespace_runtime.valid = true;
		return true;

	// $TODO (maybe): May want to have higher level logic parse the NS_* and NS_END scope delimeters.
	// Might be bad to assume NS_* macros will be by themselves on their own lines, but going to assume that for now.
	} else if (substr.find("NS_") == 0) {
		if (substr == "NS_END") {
			parse_data.scope_ranges.back().range.end = parse_data.start_index + 6;

			ProcessNamespaceScopeClose(parse_data);

			parse_data.scope_ranges.pop_back();

		} else {
			parse_data.namespace_runtime.name = ProcessNamespaceDefine(substr, parse_data);

			// $TODO (maybe): May want to have higher level logic parse the NS_* and NS_END scope delimeters.
			ScopeRuntimeData scope_data;
			scope_data.range.start = parse_data.start_index;

			parse_data.scope_ranges.emplace_back(scope_data);
			parse_data.namespace_runtime.valid = true;

			ProcessNamespaceScopeOpen(parse_data);
		}

		return true;
	}

	return false;
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

void PostProcessNamespaces(ParseData& parse_data)
{
	constexpr std::string_view k_namespace_text = " namespace ";
	constexpr std::string_view k_define_text = "#define NS_";

	const size_t namespace_define_index = parse_data.file_text.find(k_define_text);

	if (namespace_define_index == std::string_view::npos) {
		return;
	}

	size_t start_index = namespace_define_index + 8;
	size_t end_index = start_index;

	while (k_whitespace_substr.find(parse_data.file_text[end_index]) == std::string_view::npos) {
		++end_index;
	}

	const std::string_view unaltered_namespace_name(parse_data.file_text.data() + start_index, end_index - start_index);
	const std::string namespace_name = ProcessNamespaceDefine(unaltered_namespace_name, parse_data);

	start_index = namespace_define_index + k_define_text.size() + namespace_name.size() + k_namespace_text.size();
	end_index = start_index;

	while (k_whitespace_substr.find(parse_data.file_text[end_index]) == std::string_view::npos || parse_data.file_text[end_index] == '{') {
		++end_index;
	}

	const std::string_view actual_namespace_name(parse_data.file_text.data() + start_index, end_index - start_index);

	if (namespace_name == actual_namespace_name) {
		return;
	}

	parse_data.global_runtime->namespace_mappings[namespace_name] = actual_namespace_name;

	// Run through all current namespace usage and update.
	const std::string name_and_delimeter = namespace_name + "::";

	// Class Data
	{
		std::vector< std::pair<std::string, ClassData> > entries_to_update;
		std::vector<size_t> indices;

		for (auto& entry : parse_data.global_runtime->class_data) {
			if (const size_t index = entry.first.find(name_and_delimeter); index != std::string::npos) {
				std::pair<std::string, ClassData> new_entry;
				new_entry.first = entry.first;
				new_entry.second = std::move(entry.second);

				entries_to_update.emplace_back(std::move(new_entry));
				indices.emplace_back(index);
			}
		}

		for (size_t i = 0; i < entries_to_update.size(); ++i) {
			auto& entry = entries_to_update[i];
			parse_data.global_runtime->class_data.erase(entry.first);

			entry.first.replace(indices[i], namespace_name.size(), actual_namespace_name);
			entry.second.name = entry.first;

			parse_data.global_runtime->class_data[entry.first] = std::move(entry.second);
		}
	}

	// Class File Map
	{
		for (auto& entry_file : parse_data.global_runtime->class_file_map) {
			std::vector< std::pair<std::string, GlobalRuntimeData::ClassFileMapData> > entries_to_update;
			std::vector<size_t> indices;

			for (auto& entry_class : entry_file.second) {
				if (const size_t index = entry_class.first.find(name_and_delimeter); index != std::string::npos) {
					entries_to_update.emplace_back(entry_class);
					indices.emplace_back(index);
				}
			}

			for (size_t i = 0; i < entries_to_update.size(); ++i) {
				auto& entry = entries_to_update[i];
				entry_file.second.erase(entry.first);

				entry.first.replace(indices[i], namespace_name.size(), actual_namespace_name);
				entry_file.second[entry.first] = std::move(entry.second);
			}
		}
	}

	// Enum Data
	{
		std::vector< std::pair<std::string, EnumData> > entries_to_update;
		std::vector<size_t> indices;

		for (auto& entry : parse_data.global_runtime->enum_data) {
			if (const size_t index = entry.first.find(name_and_delimeter); index != std::string::npos) {
				std::pair<std::string, EnumData> new_entry;
				new_entry.first = entry.first;
				new_entry.second = std::move(entry.second);

				entries_to_update.emplace_back(std::move(new_entry));
				indices.emplace_back(index);
			}
		}

		for (size_t i = 0; i < entries_to_update.size(); ++i) {
			auto& entry = entries_to_update[i];
			parse_data.global_runtime->enum_data.erase(entry.first);

			entry.first.replace(indices[i], namespace_name.size(), actual_namespace_name);
			entry.second.name = entry.first;

			parse_data.global_runtime->enum_data[entry.first] = std::move(entry.second);
		}
	}
}
