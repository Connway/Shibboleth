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

#include "Preproc_ParseClass.h"
#include "Preproc_ParseFile.h"
#include <iostream>

bool ParseClass(std::string_view substr, ParseData& parse_data)
{
	if (parse_data.flags.any() && !parse_data.flags.testAll(ParseData::Flag::ClassStructName)) {
		return false;
	}

	// Found the name of the class or struct.
	if (parse_data.flags.testAll(ParseData::Flag::ClassStructName)) {
		if (substr.back() == ';') {
			// Just a forward declare. Remove the entry from the stack.
			parse_data.class_stack.pop_back();

		} else if (!parse_data.class_stack.empty() && parse_data.class_stack.back().name.empty()) {
			ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

			if (parse_data.class_stack.size() > 1) {
				const auto it = parse_data.class_stack.end() - 2;
				class_runtime_data.name = it->name + "::" + std::string(substr);

			} else {
				class_runtime_data.name = substr;
			}

			// Add class data to parse data.
			ClassData& class_data = parse_data.class_data[std::hash<std::string>{}(class_runtime_data.name)];
			class_data.is_struct = class_runtime_data.flags.testAll(ClassRuntimeData::Flag::Struct);
			class_data.name = class_runtime_data.name;

		} else {
			std::cerr << "Was parsing a class or struct name, but no class data was present." << std::endl;
		}

		parse_data.flags.clear(ParseData::Flag::ClassStructName);
		return true;
	}

	// Process "class" or "struct" tokens.
	const size_t struct_index = substr.find("struct");
	const size_t class_index = (struct_index == std::string_view::npos) ? substr.find("class") : std::string_view::npos;
	const size_t public_index = (class_index == std::string_view::npos) ? substr.find("public") : std::string_view::npos;
	const size_t private_index = (public_index == std::string_view::npos) ? substr.find("private") : std::string_view::npos;
	const size_t protected_index = (private_index == std::string_view::npos) ? substr.find("protected") : std::string_view::npos;

	if (struct_index == 0 || class_index == 0) {
		parse_data.class_stack.emplace_back();

		if (struct_index != std::string_view::npos) {
			parse_data.class_stack.back().curr_visibility = ClassRuntimeData::Visibility::Public;
			parse_data.class_stack.back().flags.set(ClassRuntimeData::Flag::Struct);
		}

		parse_data.flags.set(ParseData::Flag::ClassStructName);
		return true;

	} else if (public_index == 0) {
		ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

		if (class_runtime_data.scope_range_index != SIZE_T_FAIL) {
			class_runtime_data.curr_visibility = ClassRuntimeData::Visibility::Public;
			return true;
		}

	} else if (private_index == 0) {
		ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

		if (class_runtime_data.scope_range_index != SIZE_T_FAIL) {
			class_runtime_data.curr_visibility = ClassRuntimeData::Visibility::Private;
			return true;
		}

	} else if (protected_index == 0) {
		ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

		if (class_runtime_data.scope_range_index != SIZE_T_FAIL) {
			class_runtime_data.curr_visibility = ClassRuntimeData::Visibility::Protected;
			return true;
		}
	}

	return false;
}

void ProcessClassScopeOpen(ParseData& parse_data)
{
	if (!parse_data.class_stack.empty() && parse_data.class_stack.back().scope_range_index == SIZE_T_FAIL) {
		ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

		if (parse_data.scope_ranges.size() > 1) {
			const auto it = parse_data.scope_ranges.end() - 2;

			// Anonymous class/struct. Just absorb the name of the previous scope.
			if (class_runtime_data.name.empty()) {
				class_runtime_data.flags.set(ClassRuntimeData::Flag::Anonymous);
				parse_data.scope_ranges.back().name = it->name;

			} else {
				parse_data.scope_ranges.back().name = it->name + "::" + class_runtime_data.name;
			}

		// We are the only scope. Just take the class name.
		} else {
			parse_data.scope_ranges.back().name = class_runtime_data.name;
		}


		class_runtime_data.scope_range_index = parse_data.scope_ranges.size() - 1;
		parse_data.scope_ranges.back().type = ScopeRuntimeData::Type::Class;
	}
}

void ProcessClassScopeClose(ParseData& parse_data)
{
	if (!parse_data.class_stack.empty() && parse_data.class_stack.back().scope_range_index == (parse_data.scope_ranges.size() - 1)) {
		ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

		// We have an anonymous class/struct.
		if (class_runtime_data.flags.testAll(ClassRuntimeData::Flag::Anonymous)) {
			// $TODO: Handle anonymous class/struct. (eg: ensure mixins are done for the anonymous class/struct)
		}

		ClassData& class_data = parse_data.class_data[std::hash<std::string>{}(class_runtime_data.name)];
		const ScopeRuntimeData& scope_data = parse_data.scope_ranges.back();

		parse_data.class_stack.pop_back();

		class_data.declaration_text = parse_data.file_text.substr(scope_data.range.start + 1, scope_data.range.end - scope_data.range.start - 1);

		while (!class_data.declaration_text.empty() &&
			std::string_view(k_newline_chars).find_first_of(class_data.declaration_text.back()) != std::string_view::npos) {

			class_data.declaration_text.pop_back();
		}

		while (!class_data.declaration_text.empty() &&
			std::string_view(k_newline_chars).find_first_of(class_data.declaration_text.front()) != std::string_view::npos) {

			class_data.declaration_text.erase(0, 1);
		}

		// Process tabs to make the first line match up.
		size_t tab_count = 0;

		while (!class_data.declaration_text.empty() && class_data.declaration_text[tab_count] == '\t') {
			++tab_count;
		}

		size_t newline_index = class_data.declaration_text.find_first_of(k_newline_chars);
		size_t prev_newline_index = 0;

		while (newline_index != std::string::npos) {
			const size_t tab_index = (prev_newline_index == 0) ? 0 : prev_newline_index + 1;

			for (size_t count = 0;
				!class_data.declaration_text.empty() && class_data.declaration_text[tab_index] == '\t' && count < tab_count;
				++count) {

				class_data.declaration_text.erase(tab_index, 1);
			}

			prev_newline_index = newline_index;
			newline_index = class_data.declaration_text.find_first_of(k_newline_chars, newline_index + 1);
		}

		// Don't forget the last line.
		for (size_t count = 0;
			!class_data.declaration_text.empty() && class_data.declaration_text[prev_newline_index + 1] == '\t' && count < tab_count;
			++count) {

			class_data.declaration_text.erase(prev_newline_index + 1, 1);
		}
	}
}
