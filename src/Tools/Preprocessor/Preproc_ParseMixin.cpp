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

#include "Preproc_ParseMixin.h"
#include "Preproc_ParseFile.h" 
#include <iostream>

namespace
{
	static void ModifyDeclaration(ParseData& parse_data, const ClassData& class_data)
	{
		const size_t last_namespace_delimeter = class_data.name.rfind("::");

		const std::string class_text_to_find =
			((class_data.is_struct) ? "struct " : "class ") +
			((last_namespace_delimeter != std::string::npos) ? class_data.name.substr(last_namespace_delimeter + 2) : class_data.name) +
			((class_data.is_final) ? " final" : "");

		size_t class_decl_index = parse_data.out_text.find(class_text_to_find);

		// If we're calling this function, this class declaration should be in this file.
		GAFF_ASSERT(class_decl_index != std::string::npos);

		// This is a forward declaration, skip this and find the next declaration.
		if (parse_data.out_text[GetNextNonWhitespaceIndex(parse_data.out_text, class_decl_index + class_text_to_find.size())] == ';') {
			class_decl_index = parse_data.out_text.find(class_text_to_find, class_decl_index + class_text_to_find.size());

			// If we're calling this function, this class declaration should be in this file.
			GAFF_ASSERT(class_decl_index != std::string::npos);
		}

		// Parse all the scopes until we find the scope that closes out the class/struct.
		size_t open_scope_index = parse_data.out_text.find('{', class_decl_index);
		size_t close_scope_index = parse_data.out_text.find('}', class_decl_index);
		const size_t first_open_scope_index = open_scope_index;
		size_t prev_close_scope_index = close_scope_index;
		int32_t open_scopes = 0;

		GAFF_ASSERT(open_scope_index != std::string::npos);

		do {
			while (open_scope_index != std::string::npos && open_scope_index < close_scope_index) {
				++open_scopes;
				open_scope_index = parse_data.out_text.find('{', open_scope_index + 1);
			}

			prev_close_scope_index = close_scope_index;
			close_scope_index = parse_data.out_text.find('}', close_scope_index + 1);

			--open_scopes;
		} while (open_scopes > 0);

		open_scope_index = first_open_scope_index + 1;

		// Skip past newlines.
		while (k_newline_substr.find(parse_data.out_text[open_scope_index]) != std::string_view::npos) {
			++open_scope_index;
		}

		parse_data.out_text.replace(open_scope_index, prev_close_scope_index - open_scope_index - strlen(k_newline), class_data.declaration_text);

		// Add ineheritance from mixins.
		if (!class_data.mixin_inheritance.empty()) {
			size_t inherit_index = first_open_scope_index - 2; // One before open bracket.

			// Find the first non-whitespace character prior to the opening bracket.
			while (k_parse_until_substr.find(parse_data.out_text[inherit_index]) != std::string_view::npos) {
				--inherit_index;
			}

			const std::string final_inheritance = ((class_data.inherits.empty()) ? " : " : ", ") + class_data.mixin_inheritance;
			parse_data.out_text.insert(inherit_index + 1, final_inheritance);
		}
	}

	static void ModifyDefinition(ParseData& parse_data, const ClassData& class_data)
	{
		parse_data.out_text += k_newline;
		parse_data.out_text += class_data.mixin_definition_text;
	}
}

bool ParseMixin(std::string_view substr, ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return false;
	}

	if (parse_data.flags.testRangeAny(ParseData::Flag::FirstRuntimeFlag, ParseData::Flag::LastRuntimeFlag) &&
		!parse_data.flags.testAny(ParseData::Flag::MixinName)) {

		return false;
	}

	// Found the name of a mixin.
	if (parse_data.flags.testAll(ParseData::Flag::MixinName)) {
		ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();

		GAFF_ASSERT(parse_data.global_runtime->class_data.contains(class_runtime_data.name));

		if (substr.back() == ';') {
			substr = substr.substr(0, substr.size() - 1);
		}

		if (substr == "no_inherit") {
			class_runtime_data.flags.set(ClassRuntimeData::Flag::MixinNoInherit);
			return true;
		}

		ClassData& class_data = parse_data.global_runtime->class_data[class_runtime_data.name];
		class_data.mixin_classes.emplace_back(ClassData::MixinData{ std::string(substr), class_runtime_data.flags.testAll(ClassRuntimeData::Flag::MixinNoInherit) });

		parse_data.flags.clear(ParseData::Flag::MixinName);
		return true;
	}

	// Parse "mixin" token.
	if (substr == "mixin") {
		if (parse_data.class_stack.empty()) {
			std::cerr << "'mixin' used outside of a class or struct scope." << std::endl;
			return true;
		}

		parse_data.flags.set(ParseData::Flag::MixinName);
		return true;
	}

	return false;
}

void ProcessClassStructMixin(GlobalRuntimeData& global_runtime_data, ClassData& class_data)
{
	if (class_data.finished) {
		return;
	}

	if (class_data.mixin_classes.empty()) {
		return;
	}

	size_t start_index = class_data.declaration_text.find("mixin");

	while (start_index != std::string::npos) {
		size_t end_index = start_index;

		// Find beginning of line.
		while (start_index > 0 && k_newline_substr.find(class_data.declaration_text[start_index - 1]) == std::string_view::npos) {
			--start_index;
		}

		if (start_index > 0) {
			start_index -= strlen(k_newline);
		}

		// Find end of line.
		while (end_index < class_data.declaration_text.size() && k_newline_substr.find(class_data.declaration_text[end_index]) == std::string_view::npos) {
			++end_index;
		}

		class_data.declaration_text.erase(start_index, end_index - start_index + strlen(k_newline));

		start_index = class_data.declaration_text.find("mixin");
	}

	TrimNewlines(class_data.declaration_text);
	TrimNewlines(class_data.definition_text);
	TrimNewlines(class_data.mixin_definition_text);

	//StripComments(class_data.declaration_text);
	//StripComments(class_data.definition_text);
	//StripComments(class_data.mixin_definition_text);

	std::string extra_definitions;

	for (const ClassData::MixinData& mixin_data : class_data.mixin_classes) {
		const auto it = global_runtime_data.class_data.find(mixin_data.class_struct_name);

		GAFF_ASSERT(it != global_runtime_data.class_data.end());

		ProcessClassStructMixin(global_runtime_data, it->second);

		const size_t start_index_declaration = class_data.declaration_text.size();
		const size_t start_index_definition = class_data.mixin_definition_text.size();

		class_data.declaration_text += k_newline;
		class_data.declaration_text += k_newline;
		class_data.declaration_text += it->second.GetAccessText(ClassData::Access::Default);
		class_data.declaration_text += ':';
		class_data.declaration_text += k_newline;

		class_data.declaration_text += it->second.declaration_text;

		class_data.mixin_definition_text += it->second.definition_text;
		class_data.mixin_definition_text += it->second.mixin_definition_text;

		// $TODO: This does not handle code that would have nested scopes, such as namespaces or other nested classes.
		// eg: Shibboleth::ClassName or Shibboleth::ClassA::ClassB.
		// Replace all instances of the class name with our name.
		std::string_view mixin_name = mixin_data.class_struct_name;
		std::string_view name = class_data.name;

		if (const size_t index = mixin_name.rfind("::"); index != std::string_view::npos) {
			mixin_name = mixin_name.substr(index + 2);
		}

		if (const size_t index = name.rfind("::"); index != std::string_view::npos) {
			name = name.substr(index + 2);
		}

		size_t index = class_data.declaration_text.find(mixin_name, start_index_declaration);

		// Replace name in declaration text.
		while (index != std::string::npos) {
			class_data.declaration_text.replace(index, mixin_name.size(), name);
			index = class_data.declaration_text.find(mixin_name);
		}

		// Replace name in definition text.
		index = class_data.mixin_definition_text.find(mixin_name, start_index_definition);

		while (index != std::string::npos) {
			// Using fully namespaced class name as we will put definition text outside of namespace scopes.
			class_data.mixin_definition_text.replace(index, mixin_name.size(), class_data.name);
			index = class_data.mixin_definition_text.find(mixin_name);
		}

		if (!mixin_data.no_inherit) {
			for (auto& inherit_data : it->second.inherits) {
				if (!global_runtime_data.class_data.contains(inherit_data.class_struct_name)) {
					size_t namespace_index = class_data.name.rfind("::");

					while (namespace_index != std::string::npos) {
						const std::string_view name_substr = std::string_view(class_data.name).substr(0, namespace_index + 2);
						const std::string full_name = std::string(name_substr) + inherit_data.class_struct_name;

						if (global_runtime_data.class_data.contains(full_name)) {
							inherit_data.class_struct_name = full_name;
							break;
						}

						namespace_index = class_data.name.rfind("::", namespace_index - 1);
					}
				}

				if (!class_data.mixin_inheritance.empty()) {
					class_data.mixin_inheritance += ", ";
				}

				if (inherit_data.is_virtual) {
					class_data.mixin_inheritance += "virtual ";
				} else {
					class_data.mixin_inheritance += it->second.GetAccessText(inherit_data.access);
				}

				class_data.mixin_inheritance += " ";
				class_data.mixin_inheritance += inherit_data.class_struct_name;
			}
		}
	}
}

void ModifyOutputMixin(ParseData& parse_data)
{
	if (!parse_data.global_runtime->class_file_map.contains(parse_data.file_path)) {
		return;
	}

	const auto& class_map = parse_data.global_runtime->class_file_map[parse_data.file_path];

	for (const auto& entry : class_map) {
		GAFF_ASSERT(parse_data.global_runtime->class_data.contains(entry.first));

		const ClassData& class_data = parse_data.global_runtime->class_data[entry.first];

		if (class_data.mixin_classes.empty()) {
			continue;
		}

		if (entry.second.has_decl) {
			ModifyDeclaration(parse_data, class_data);
		}

		if (entry.second.has_impl) {
			ModifyDefinition(parse_data, class_data);
		}
	}
}
