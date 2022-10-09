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
		!parse_data.flags.testAll(ParseData::Flag::MixinName)) {

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
	for (const std::string& mixin_class_name : class_data.mixin_classes) {
		const size_t hash = std::hash<std::string>{}(mixin_class_name);
		const auto it = global_runtime_data.class_data.find(hash);

		GAFF_ASSERT(it != global_runtime_data.class_data.end());

		// Ensure that dependency classes have been processed.
		ProcessClassStructMixin(global_runtime_data, it->second);

		const std::string& mixin_declaration = it->second.declaration_text;

		// Find line with 'mixin <mixin_class_name>'.
		// Replace 'mixin <mixin_class_name>' with modified declaration text of dependent class.
		const size_t start_index = class_data.declaration_text.find("mixin " + mixin_class_name);
		GAFF_ASSERT(start_index != std::string::npos);

		class_data.declaration_text.replace(start_index, strlen("mixin ;") + mixin_class_name.size(), mixin_declaration.data());
	}
}
