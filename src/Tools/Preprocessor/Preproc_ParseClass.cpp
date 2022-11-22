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
		!parse_data.flags.testAny(
			ParseData::Flag::ClassStructName,
			ParseData::Flag::ClassFunctionDefinitionLine,
			ParseData::Flag::ClassFunctionDefinition)) {

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

			std::string_view left, right;

			if (const size_t index = SplitSubstr(substr, ':', left, right); index != std::string_view::npos) {
				class_runtime_data.flags.set(ClassRuntimeData::Flag::ParsingInheritance);
				class_runtime_data.name = left;

				// Parse inheritance string after class name.
				if (!right.empty()) {
					parse_data.flags.clear(ParseData::Flag::ClassStructName);

					const size_t start_index = parse_data.start_index;
					parse_data.start_index += index + 1;

					ParseClass(right, parse_data);

					parse_data.start_index = start_index;
				}
			}

		} else {
			std::cerr << "Was parsing a class or struct name, but no class data was present." << std::endl;
		}

		parse_data.flags.clear(ParseData::Flag::ClassStructName);
		return true;

	} else if (parse_data.flags.testAny(ParseData::Flag::ClassFunctionDefinitionLine)) {
		parse_data.curr_class_data->definition_text += " ";

		std::string_view left, right;

		if (const size_t index = SplitSubstr(substr, ';', left, right); index != std::string_view::npos) {
			parse_data.curr_class_data->definition_text += left;
			parse_data.curr_class_data->definition_text += ';';
			parse_data.curr_class_data->definition_text += k_newline;

			if (!right.empty()) {
				const size_t start_index = parse_data.start_index;
				parse_data.start_index += index + 1;

				ParseClass(right, parse_data);

				parse_data.start_index = start_index;
			}

			parse_data.flags.clear(ParseData::Flag::ClassFunctionDefinitionLine);
			parse_data.curr_class_data = nullptr;

		} else {
			parse_data.curr_class_data->definition_text += substr;
		}

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
		// Will fail if formatting doesn't match what we expect.
		// Will revisit later if there are codebases that don't put these on their own lines.
		if (substr == "public" || substr == "public:") {
			ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();
			class_runtime_data.curr_access = ClassData::Access::Public;
			return true;

		} else if (substr == "private" || substr == "private:") {
			ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();
			class_runtime_data.curr_access = ClassData::Access::Private;
			return true;

		} else if (substr == "protected" || substr == "protected:") {
			ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();
			class_runtime_data.curr_access = ClassData::Access::Protected;
			return true;

		// Not opened the class scope yet.
		} else if (parse_data.class_stack.back().scope_range_index == SIZE_T_FAIL) {
			ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

			if (class_runtime_data.flags.testAll(ClassRuntimeData::Flag::ParsingInheritance)) {
				if (substr == "virtual") {
					class_runtime_data.flags.set(ClassRuntimeData::Flag::Virtual);
					return true;

				} else {
					ClassData::InheritanceData inherit_data;
					inherit_data.class_struct_name = substr;
					inherit_data.is_virtual = class_runtime_data.flags.testAll(ClassRuntimeData::Flag::Virtual);
					inherit_data.access = class_runtime_data.curr_access;

					class_runtime_data.inherits.emplace_back(std::move(inherit_data));
					class_runtime_data.curr_access = ClassData::Access::Default;
					class_runtime_data.flags.clear(ClassRuntimeData::Flag::Virtual);

					std::string_view left, right;

					if (const size_t index = SplitSubstr(substr, ',', left, right); index != std::string_view::npos) {
						inherit_data.class_struct_name = left;

						if (!right.empty()) {
							const size_t start_index = parse_data.start_index;
							parse_data.start_index += index + 1;

							ParseClass(right, parse_data);

							parse_data.start_index = start_index;
						}
					}

					return true;
				}

			} else {
				std::string_view left, right;

				if (substr == "final") {
					class_runtime_data.flags.set(ClassRuntimeData::Flag::Final);
					return true;

				} else if (const size_t index = SplitSubstr(substr, ':', left, right); index != std::string_view::npos) {
					if (!left.empty()) {
						ParseClass(left, parse_data);
					}

					class_runtime_data.flags.set(ClassRuntimeData::Flag::ParsingInheritance);

					if (!right.empty()) {
						const size_t start_index = parse_data.start_index;
						parse_data.start_index += index + 1;

						ParseClass(right, parse_data);

						parse_data.start_index = start_index;
					}

					return true;
				}

			}
		}

	} else if (!parse_data.flags.testAny(ParseData::Flag::ClassFunctionDefinitionLine, ParseData::Flag::ClassFunctionDefinition)) {
		// Found a scope delimeter.
		std::string_view left, right;

		if (size_t index = SplitSubstr(substr, "::", left, right, true); index != std::string_view::npos) {
			// If the entire substring is a valid class or enum name, then this substring is probably declaring a typename for a variable or return value.
			std::string name = ((parse_data.scope_ranges.empty()) ? "" : parse_data.scope_ranges.back().name) + "::" + std::string(substr);
			const size_t substr_hash = std::hash<std::string>{}(name);

			if (parse_data.global_runtime->class_data.contains(substr_hash) || parse_data.global_runtime->enum_data.contains(substr_hash)) {
				return false;
			}

			name = ((parse_data.scope_ranges.empty()) ? "" : parse_data.scope_ranges.back().name) + "::" + std::string(left);
			const size_t hash = std::hash<std::string>{}(name);

			// We have found a function implementation or a static class variable.
			if (parse_data.global_runtime->class_data.contains(hash)) {
				size_t type_index = parse_data.start_index - 1;

				// Find first non-whitespace character.
				while (k_parse_until_substr.find(parse_data.file_text[type_index]) != std::string_view::npos) {
					--type_index;
				}

				// Find next whitespace character.
				while (type_index > parse_data.start_of_line_index && k_parse_until_substr.find(parse_data.file_text[type_index]) == std::string_view::npos) {
					--type_index;
				}

				// Start type index on a non-whitespace character.
				if (k_parse_until_substr.find(parse_data.file_text[type_index]) != std::string_view::npos) {
					++type_index;
				}

				if (index = SplitSubstr(substr, ';', left, right); index != std::string_view::npos) {
					++index; // Include ';'
				} else {
					index = substr.size();
				}

				const std::string_view line_until_this_point = parse_data.file_text.substr(type_index, parse_data.start_index - type_index + index);
				ClassData& class_data = parse_data.global_runtime->class_data[hash];

				class_data.definition_text += line_until_this_point;

				// This is a static class variable. Add newline.
				if (line_until_this_point.back() == ';') {
					class_data.definition_text += k_newline;

					if (!right.empty()) {
						const size_t start_index = parse_data.start_index;
						parse_data.start_index += index;

						ParseClass(right, parse_data);

						parse_data.start_index = start_index;
					}

				} else {
					parse_data.flags.set(ParseData::Flag::ClassFunctionDefinitionLine);
					parse_data.curr_class_data = &class_data;
				}
			}
		}
	}

	return false;
}

void ProcessClassScopeOpen(ParseData& parse_data)
{
	if (!parse_data.class_stack.empty()) {
		parse_data.class_stack.back().flags.clear(ClassRuntimeData::Flag::ParsingInheritance);
	}

	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return;
	}

	if (parse_data.flags.testAll(ParseData::Flag::ClassFunctionDefinitionLine)) {
		parse_data.flags.clear(ParseData::Flag::ClassFunctionDefinitionLine);
		parse_data.flags.set(ParseData::Flag::ClassFunctionDefinition);
		parse_data.curr_class_data = nullptr;
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

		class_runtime_data.curr_access = ClassData::Access::Default;
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

	if (parse_data.flags.testAll(ParseData::Flag::ClassFunctionDefinition)) {
		parse_data.flags.clear(ParseData::Flag::ClassFunctionDefinition);

		const BlockRange& range = parse_data.scope_ranges.back().range;

		const std::string_view func_impl = parse_data.file_text.substr(range.start, range.end - range.start);

		parse_data.curr_class_data->definition_text += k_newline;
		parse_data.curr_class_data->definition_text += func_impl;
		parse_data.curr_class_data->definition_text += "}";
		parse_data.curr_class_data->definition_text += k_newline;
	}

	if (!parse_data.class_stack.empty() && parse_data.class_stack.back().scope_range_index == (parse_data.scope_ranges.size() - 1)) {
		ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

		// We have an anonymous class/struct.
		if (class_runtime_data.flags.testAll(ClassRuntimeData::Flag::Anonymous)) {
			// $TODO: Handle anonymous class/struct. (eg: ensure mixins are done for the anonymous class/struct)
			return;
		}

		// We have a template class/struct.
		if (class_runtime_data.flags.testAll(ClassRuntimeData::Flag::Template)) {
			// $TODO: Handle anonymous class/struct. (eg: ensure mixins are done for template class/struct)
			return;
		}

		const size_t hash = std::hash<std::string>{}(class_runtime_data.name);
		GAFF_ASSERT(parse_data.global_runtime->class_data.contains(hash));

		ClassData& class_data = parse_data.global_runtime->class_data[hash];
		const ScopeRuntimeData& scope_data = parse_data.scope_ranges.back();

		class_data.declaration_text = parse_data.file_text.substr(scope_data.range.start + 1, scope_data.range.end - scope_data.range.start - 1);
		class_data.declaration_scope_range = scope_data.range;
		class_data.inherits = std::move(class_runtime_data.inherits);

		// $TODO: Process whitespace so we can align it with the class this is potentially mixed in with.

		//// Trim whitespace in back.
		//while (!class_data.declaration_text.empty() &&
		//	std::string_view(k_newline_chars).find_first_of(class_data.declaration_text.back()) != std::string_view::npos) {

		//	class_data.declaration_text.pop_back();
		//}

		//// Trim whitespace in front.
		//while (!class_data.declaration_text.empty() &&
		//	std::string_view(k_newline_chars).find_first_of(class_data.declaration_text.front()) != std::string_view::npos) {

		//	class_data.declaration_text.erase(0, 1);
		//}

		//// Process tabs to make the first line match up.
		//size_t tab_count = 0;

		//while (parse_data.file_text[scope_data.range.start - tab_count - 1] == '\t') {
		//	++tab_count;
		//}

		//size_t newline_index = class_data.declaration_text.find_first_of(k_newline_chars);
		//size_t prev_newline_index = 0;

		//while (newline_index != std::string::npos) {
		//	const size_t tab_index = (prev_newline_index == 0) ? 0 : prev_newline_index + 1;

		//	for (size_t count = 0;
		//		!class_data.declaration_text.empty() && class_data.declaration_text[tab_index] == '\t' && count < tab_count;
		//		++count) {

		//		class_data.declaration_text.erase(tab_index, 1);
		//	}

		//	prev_newline_index = newline_index;
		//	newline_index = class_data.declaration_text.find_first_of(k_newline_chars, newline_index + 1);
		//}

		//// Don't forget the last line.
		//for (size_t count = 0;
		//	!class_data.declaration_text.empty() && class_data.declaration_text[prev_newline_index + 1] == '\t' && count < tab_count;
		//	++count) {

		//	class_data.declaration_text.erase(prev_newline_index + 1, 1);
		//}

		parse_data.class_stack.pop_back();
	}
}
