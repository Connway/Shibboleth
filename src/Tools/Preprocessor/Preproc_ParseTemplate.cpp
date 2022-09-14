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

#include "Preproc_ParseTemplate.h"
#include "Preproc_ParseFile.h"
#include <iostream>

bool ParseTemplate(std::string_view substr, ParseData& parse_data)
{
	// Currently have no inline modifications to do during file writing.
	if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
		return false;
	}

	if (parse_data.flags.any() && !parse_data.flags.testAll(ParseData::Flag::ProcessingTemplate)) {
		return false;
	}

	bool processed = false;

	if (!parse_data.flags.testAll(ParseData::Flag::ProcessingTemplate)) {
		const size_t template_index = substr.find("template");

		// If we find ".template", then this is a function call, we don't are about function calls.
		// Code can be written as ". template", but for now, we have no code written like that.
		// May wish to fix at a later date.
		// If template_index == 0, then this isn't a ".template" function call.
		if (template_index == 0 /*&& substr.find(".template") == std::string_view::npos*/) {
			parse_data.flags.set(ParseData::Flag::ProcessingTemplate);
			parse_data.template_runtime.clear(); // If template hasn't been consumed yet, clear it. We've hit another template declaraction.
			parse_data.template_runtime.is_template = true;

			processed = true;
		}
	}

	if (parse_data.flags.testAll(ParseData::Flag::ProcessingTemplate)) {
		if (parse_data.template_runtime.is_template) {

			const size_t open_bracket_index = substr.find('<');

			if (open_bracket_index != std::string_view::npos) {
				if (parse_data.template_runtime.open_bracket_count == 0) {
					parse_data.template_runtime.open_bracket_index = open_bracket_index + parse_data.start_index;
				}

				++parse_data.template_runtime.open_bracket_count;
				processed = true;
			}

			const size_t close_bracket_index = substr.find('>');

			if (close_bracket_index != std::string_view::npos) {
				--parse_data.template_runtime.open_bracket_count;

				if (parse_data.template_runtime.open_bracket_count == 0) {
					parse_data.template_runtime.close_bracket_index = close_bracket_index + parse_data.start_index;

					// $TODO: Process template arguments.
					parse_data.flags.clear(ParseData::Flag::ProcessingTemplate);
				}

				processed = true;
			}
		}
	}

	return processed;
}
