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
	if (parse_data.flags.any() && !parse_data.flags.testAll(ParseFlag::MixinName)) {
		return false;
	}

	// Found the name of a mixin.
	if (parse_data.flags.testAll(ParseFlag::MixinName)) {
		const ClassRange& class_range = parse_data.class_stack.back();
		ClassData& class_data = parse_data.class_data[std::hash<std::string>{}(class_range.name)];

		class_data.mixin_classes.emplace_back(substr);

		parse_data.flags.clear(ParseFlag::MixinName);
		return true;
	}

	const size_t mixin_index = substr.find("mixin");

	if (mixin_index == std::string_view::npos) {
		return false;
	}

	if (parse_data.class_stack.empty()) {
		std::cerr << "'mixin' used outside of a class or struct scope." << std::endl;
		return true;
	}

	parse_data.flags.set(ParseFlag::MixinName);
	return true;
}