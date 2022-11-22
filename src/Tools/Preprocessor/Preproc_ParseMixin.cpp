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

#include "Preproc_ParseMixin.h"
#include "Preproc_ParseFile.h" 
#include <iostream>

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
		const ClassRuntimeData& class_runtime_data = parse_data.class_stack.back();
		const size_t hash = std::hash<std::string>{}(class_runtime_data.name);

		GAFF_ASSERT(parse_data.global_runtime->class_data.contains(hash));

		if (substr.back() == ';') {
			substr = substr.substr(0, substr.size() - 1);
		}

		ClassData& class_data = parse_data.global_runtime->class_data[hash];
		class_data.mixin_classes.emplace_back(substr);

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

	// $TODO: Class declaration parsing does not strip out commented out lines. Should strip those out before processing.
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

	// Trim newlines in back.
	while (!class_data.declaration_text.empty() &&
			k_newline_substr.find(class_data.declaration_text.back()) != std::string_view::npos) {

		class_data.declaration_text.pop_back();
	}

	// Trim newlines in front.
	while (!class_data.declaration_text.empty() &&
			k_newline_substr.find(class_data.declaration_text.front()) != std::string_view::npos) {

		class_data.declaration_text.erase(0, 1);
	}

	std::string extra_definitions;

	for (const std::string& mixin_class_name : class_data.mixin_classes) {
		const size_t hash = std::hash<std::string>{}(mixin_class_name);
		const auto it = global_runtime_data.class_data.find(hash);

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
		std::string_view mixin_name = mixin_class_name;
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
			class_data.mixin_definition_text.replace(index, mixin_name.size(), name);
			index = class_data.mixin_definition_text.find(mixin_name);
		}
	}
}
