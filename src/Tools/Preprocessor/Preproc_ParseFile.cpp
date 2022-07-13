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

#include "Preproc_ParseFile.h"

namespace
{
	static bool IsWithinBlockRange(const BlockRange& range, size_t index)
	{
		// Neither range is established.
		if (index == std::string_view::npos || range.start == std::string_view::npos) {
			return false;
		}

		if ((range.end == std::string_view::npos && index > range.start) ||
			(range.end != std::string_view::npos && index > range.start && index < range.end)) {

			return true;
		}

		return false;
	}

	static bool ProcessBlockRange(std::string_view substr, const ParseData& parse_data, BlockRange& range, BlockRangeType type, size_t offset = 0)
	{
		const auto marker = k_range_markers[static_cast<size_t>(type)];

		if (range.start == std::string_view::npos) {
			range.start = substr.find(marker[0], offset);

			if (range.start != std::string_view::npos) {
				if (marker[1]) {
					range.end = substr.find(marker[1], range.start + 1);

					if (range.end != std::string_view::npos) {
						range.end += parse_data.start_index + strlen(marker[1]);
					}

				// Currently only used for line comments.
				} else if (parse_data.next_index != std::string_view::npos) {
					if (parse_data.file_text.substr(parse_data.next_index, 1).find_first_of(k_newline_chars) != std::string_view::npos) {
						range.end = parse_data.next_index;
					}
				}

				range.start += parse_data.start_index;
				return true;
			}

		} else {
			if (marker[1]) {
				range.end = substr.find(marker[1], offset);

				if (range.end != std::string_view::npos) {
					range.end += parse_data.start_index + strlen(marker[1]);
				}

			// Currently only used for line comments.
			} else if (parse_data.next_index != std::string_view::npos) {
				if (parse_data.file_text.substr(parse_data.next_index, 1).find_first_of(k_newline_chars) != std::string_view::npos) {
					range.end = parse_data.next_index;
				}
			}

			if (range.end != std::string_view::npos) {
				return true;
			}
		}

		return false;
	}

	static BlockRange ProcessIgnoreRange(std::string_view substr, ParseData& parse_data)
	{
		// Get all ignore ranges and calculate the largest ignore range size.
		BlockRange ignore_range;
		ignore_range.end = 0;

		for (int32_t i = 0; i < static_cast<int32_t>(BlockRangeType::IgnoreBlocksCount); ++i) {
			BlockRange& range = parse_data.block_ranges[i];
			ProcessBlockRange(substr, parse_data, range, static_cast<BlockRangeType>(i));

			ignore_range.start = std::min(ignore_range.start, range.start);
			ignore_range.end = std::max(
				ignore_range.end,
				(parse_data.block_ranges[i].end != std::string_view::npos) ? range.end : 0
			);
		}

		// Did not find an end to the range.
		if (ignore_range.start >= ignore_range.end) {
			ignore_range.end = std::string_view::npos;
		}

		// Clear any ranges that are within another ignore range.
		for (int32_t i = 0; i < static_cast<int32_t>(BlockRangeType::IgnoreBlocksCount); ++i) {
			BlockRange& range_a = parse_data.block_ranges[i];

			for (int32_t j = 0; j < static_cast<int32_t>(BlockRangeType::IgnoreBlocksCount); ++j) {
				if (i == j) {
					continue;
				}

				const BlockRange& range_b = parse_data.block_ranges[j];

				if (IsWithinBlockRange(range_b, range_a.start)) {
					if (IsWithinBlockRange(range_b, range_a.end)) {
						range_a.start = std::string_view::npos;
					} else {
						range_a.start = range_a.end;
					}

					range_a.end = std::string_view::npos;
				}
			}
		}

		const size_t ignore_start = (ignore_range.start != std::string_view::npos) ?
			std::max(ignore_range.start, parse_data.start_index) :
			parse_data.start_index;

		const size_t ignore_end = (ignore_range.end != std::string_view::npos) ?
			std::min(ignore_range.end, parse_data.start_index + substr.size()) :
			(ignore_range.start != std::string_view::npos) ? parse_data.start_index + substr.size() : parse_data.start_index;

		// Clear block ranges if we found the end range in this substring.
		for (int32_t i = 0; i < static_cast<int32_t>(BlockRangeType::IgnoreBlocksCount); ++i) {
			if (parse_data.block_ranges[i].end != std::string_view::npos) {
				parse_data.block_ranges[i].start = std::string_view::npos;
				parse_data.block_ranges[i].end = std::string_view::npos;
			}
		}

		ignore_range.start = ignore_start;
		ignore_range.end = ignore_end;

		return ignore_range;
	}
}

void Preproc_ParseSubstring(std::string_view substr, ParseData& parse_data, int32_t depth)
{
	if (substr.empty()) {
		return;
	}

	// Get all ignore ranges and calculate the largest ignore range size.
	const BlockRange ignore_range = ProcessIgnoreRange(substr, parse_data);

	// Nothing to ignore.
	if (ignore_range.start == ignore_range.end) {
		bool append_text = true;

		const auto marker = k_range_markers[static_cast<size_t>(BlockRangeType::Scope)];
		size_t open_pos = substr.find(marker[0]);
		size_t close_pos = substr.find(marker[1]);

		if (open_pos != std::string_view::npos && close_pos != std::string_view::npos) {
			// Only process one at a time. Take the one that is earliest in the substring.
			if (open_pos < close_pos) {
				close_pos = std::string_view::npos;
			} else {
				open_pos = std::string_view::npos;
			}
		}

		const size_t start_index = parse_data.start_index;
		const size_t end_index = parse_data.next_index;

		if (open_pos != std::string_view::npos) {
			const std::string_view before_scope = substr.substr(0, open_pos);
			const std::string_view after_scope = substr.substr(open_pos + strlen(marker[0]));

			// Process before new scope.
			parse_data.next_index = start_index + before_scope.size();
			Preproc_ParseSubstring(before_scope, parse_data, depth + 1);

			// Open new scope.
			parse_data.scope_ranges.emplace_back(BlockRange{ start_index + open_pos, std::string_view::npos });

			// Preserve open scope marker.
			parse_data.out_text += marker[0];

			// Process rest.
			parse_data.start_index = start_index + open_pos + strlen(marker[0]);
			parse_data.next_index = end_index;
			Preproc_ParseSubstring(after_scope, parse_data, depth + 1);

			parse_data.start_index = start_index;
			parse_data.next_index = end_index;

			append_text = false;

		} else if (close_pos != std::string_view::npos) {
			const std::string_view before_scope = substr.substr(0, close_pos);
			const std::string_view after_scope = substr.substr(close_pos + strlen(marker[1]));

			// Process before closing scope.
			parse_data.next_index = start_index + before_scope.size();
			Preproc_ParseSubstring(before_scope, parse_data, depth + 1);

			// Close previous scope.
			parse_data.scope_ranges.pop_back();
			// Process class scopes.

			// Preserve close scope marker.
			parse_data.out_text += marker[1];

			// Process rest.
			parse_data.start_index = start_index + close_pos + strlen(marker[1]);
			parse_data.next_index = end_index;
			Preproc_ParseSubstring(after_scope, parse_data, depth + 1);

			parse_data.start_index = start_index;
			parse_data.next_index = end_index;

			append_text = false;
		}

		// Process 'class'/'struct' tokens.
		// Found the name of the class.
		//if (!parse_data.class_stack.empty() && parse_data.class_stack.back().name.empty()) {
		//	// Check if class name has an opening bracket directly after it.
		//	size_t class_name_count = std::string_view::npos;

		//	if (!parse_data.scope_ranges.empty() &&
		//		parse_data.scope_ranges.back().start > parse_data.start_index &&
		//		parse_data.scope_ranges.back().start < (parse_data.start_index + substr.size())) {

		//		class_name_count = parse_data.scope_ranges.back().start - parse_data.start_index;
		//	}

		//	ClassRange& class_range = parse_data.class_stack.back();
		//	class_range.name = substr.substr(0, class_name_count);

		//	// Add class data to parse data.
		//	parse_data.class_data[std::hash<std::string>{}(class_range.name)] = ClassData{ class_range.name };
		//}

		//const size_t struct_index = substr.find("struct");
		//const size_t class_index = substr.find("class");

		//if (struct_index != std::string_view::npos || class_index != std::string_view::npos) {
		//	parse_data.class_stack.emplace_back();
		//}

		//const size_t mixin_index = substr.find("mixin");

		//if (mixin_index != std::string_view::npos) {
		//	append_text = false;
		//}

		if (append_text) {
			parse_data.out_text += substr;
		}

	} else {
		const std::string_view left_substr = substr.substr(0, ignore_range.start - parse_data.start_index);
		const std::string_view ignore_substr = substr.substr(ignore_range.start - parse_data.start_index, ignore_range.end - ignore_range.start);
		const std::string_view right_substr = substr.substr(ignore_range.end - parse_data.start_index);

		const size_t start_index = parse_data.start_index;
		const size_t end_index = parse_data.next_index;

		parse_data.next_index = start_index + left_substr.size();
		Preproc_ParseSubstring(left_substr, parse_data, depth + 1);

		parse_data.out_text += ignore_substr;

		parse_data.start_index = parse_data.next_index + ignore_substr.size();
		parse_data.next_index = end_index;
		Preproc_ParseSubstring(right_substr, parse_data, depth + 1);

		parse_data.start_index = start_index;
		parse_data.next_index = end_index;
	}

	if (!depth && parse_data.next_index != std::string_view::npos) {
		parse_data.out_text += parse_data.file_text[parse_data.next_index]; // Preserve token character.
	}
}

bool Preproc_ParseFile(ParseData& parse_data)
{
	parse_data.next_index = parse_data.file_text.find_first_of(k_parse_until_tokens);

	while (parse_data.next_index != std::string::npos) {
		// Started on whitespace. Append it to the output and move on.
		if (parse_data.start_index == parse_data.next_index) {
			// Don't append whitespace characters if we're stripping from the beginning of the file.
			if (!parse_data.out_text.empty()) {
				parse_data.out_text += parse_data.file_text[parse_data.start_index];
			}

		} else {
			const std::string_view substr = parse_data.file_text.substr(parse_data.start_index, parse_data.next_index - parse_data.start_index);
			Preproc_ParseSubstring(substr, parse_data);
		}

		parse_data.start_index = parse_data.next_index + 1;
		parse_data.next_index = parse_data.file_text.find_first_of(k_parse_until_tokens, parse_data.start_index);
	}

	const std::string_view substr = parse_data.file_text.substr(parse_data.start_index);
	Preproc_ParseSubstring(substr, parse_data);

	return true;
}