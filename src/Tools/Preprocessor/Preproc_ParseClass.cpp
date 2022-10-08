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
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return false;
	}

	if (parse_data.flags.testRangeAny(ParseData::Flag::FirstRuntimeFlag, ParseData::Flag::LastRuntimeFlag) &&
		!parse_data.flags.testAll(ParseData::Flag::ClassStructName)) {

		return false;
	}

	// Found the name of the class or struct.
	if (parse_data.flags.testAll(ParseData::Flag::ClassStructName)) {
		if (substr.back() == ';') {
			// Just a forward declare.
			// Clear template data.
			// Remove the entry from the stack.
			parse_data.template_runtime.clear();
			parse_data.class_stack.pop_back();

		} else if (!parse_data.class_stack.empty() && parse_data.class_stack.back().name.empty()) {
			ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();
			class_runtime_data.name = substr;

			if (parse_data.template_runtime.is_template) {
				class_runtime_data.template_args = parse_data.template_runtime.args;
				class_runtime_data.flags.set(ClassRuntimeData::Flag::Template);
				parse_data.template_runtime.clear();
			}

		} else {
			std::cerr << "Was parsing a class or struct name, but no class data was present." << std::endl;
		}

		parse_data.flags.clear(ParseData::Flag::ClassStructName);
		return true;
	}

	// Process "class" or "struct" tokens.
	const bool is_struct = substr == "struct";
	const bool is_class = substr == "class";

	if (is_struct || is_class) {
		parse_data.class_stack.emplace_back();

		if (is_struct) {
			parse_data.class_stack.back().flags.set(ClassRuntimeData::Flag::Struct);
		}

		parse_data.flags.set(ParseData::Flag::ClassStructName);
		return true;

	} else if (!parse_data.class_stack.empty()) {
		if (substr == "public" || substr == "public:") {
			ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();
			class_runtime_data.curr_access = ClassRuntimeData::Access::Public;
			return true;

		} else if (substr == "private" || substr == "private:") {
			ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();
			class_runtime_data.curr_access = ClassRuntimeData::Access::Private;
			return true;

		} else if (substr == "protected" || substr == "protected:") {
			ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();
			class_runtime_data.curr_access = ClassRuntimeData::Access::Protected;

		} else if (parse_data.class_stack.back().scope_range_index == SIZE_T_FAIL) {
			if (substr == "final") {
				parse_data.class_stack.back().flags.set(ClassRuntimeData::Flag::Final);

			} else if (substr == "virtual") {
				parse_data.class_stack.back().flags.set(ClassRuntimeData::Flag::Virtual);

			} else if (substr != ":") {
				ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

				ClassRuntimeData::InheritanceData inherit_data;
				inherit_data.class_struct_name = substr;
				inherit_data.is_virtual = class_runtime_data.flags.testAll(ClassRuntimeData::Flag::Virtual);
				inherit_data.access = class_runtime_data.curr_access;

				class_runtime_data.derived_from.emplace_back(std::move(inherit_data));
				class_runtime_data.curr_access = ClassRuntimeData::Access::Default;
				class_runtime_data.flags.clear(ClassRuntimeData::Flag::Virtual);
			}
		}
	}

	return false;
}

void ProcessClassScopeOpen(ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return;
	}

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

		class_runtime_data.name = parse_data.scope_ranges.back().name;

		// Currently we don't add template classes.
		if (!class_runtime_data.flags.testAll(ClassRuntimeData::Flag::Template)) {
			// Add class data to parse data.
			const size_t hash = std::hash<std::string>{}(class_runtime_data.name);

			if (parse_data.global_runtime->class_data.contains(hash)) {
				std::cerr << "Class/struct data for '" << class_runtime_data.name << "' already exists! Erasing and proceeding from empty class data." << std::endl;
				parse_data.global_runtime->class_data.erase(hash);
			}

			ClassData& class_data = parse_data.global_runtime->class_data[hash];
			class_data.is_struct = class_runtime_data.flags.testAll(ClassRuntimeData::Flag::Struct);
			class_data.name = class_runtime_data.name;
		}

		class_runtime_data.curr_access = ClassRuntimeData::Access::Default;
		class_runtime_data.scope_range_index = parse_data.scope_ranges.size() - 1;
		parse_data.scope_ranges.back().type = ScopeRuntimeData::Type::Class;
	}
}

void ProcessClassScopeClose(ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return;
	}

	if (!parse_data.class_stack.empty() && parse_data.class_stack.back().scope_range_index == (parse_data.scope_ranges.size() - 1)) {
		ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

		const bool is_anonymous = class_runtime_data.flags.testAll(ClassRuntimeData::Flag::Anonymous);
		const bool is_template = class_runtime_data.flags.testAll(ClassRuntimeData::Flag::Template);
		const size_t hash = std::hash<std::string>{}(class_runtime_data.name);

		parse_data.class_stack.pop_back();

		// We have an anonymous class/struct.
		if (is_anonymous) {
			// $TODO: Handle anonymous class/struct. (eg: ensure mixins are done for the anonymous class/struct)
			return;
		}

		// We have a template class/struct.
		if (is_template) {
			// $TODO: Handle anonymous class/struct. (eg: ensure mixins are done for template class/struct)
			return;
		}

		GAFF_ASSERT(parse_data.global_runtime->class_data.contains(hash));

		ClassData& class_data = parse_data.global_runtime->class_data[hash];
		const ScopeRuntimeData& scope_data = parse_data.scope_ranges.back();

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
