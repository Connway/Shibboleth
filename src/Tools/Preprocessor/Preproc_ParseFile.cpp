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
#include <vector>

namespace
{
	// Stop when we hit a newline or a semi-colon.
	constexpr const char* const k_parse_until_tokens = " \n\f\r\t\v";
	constexpr const char* const k_newline_chars = "\n\f\r";

	enum class BlockRangeType
	{
		BlockComment,
		LineComment,

		StringLiteral,
		CharLiteral,

		Scope,
		
		Count,

		IgnoreBlocksStart = BlockComment,
		IgnoreBlocksEnd = CharLiteral,
		IgnoreBlocksCount = IgnoreBlocksEnd - IgnoreBlocksStart + 1,
	};

	constexpr const char* k_range_markers[static_cast<size_t>(BlockRangeType::Count)][2] =
	{
		{ "/*", "*/" },
		{ "//", nullptr },
		{ "\"", "\"" },
		{ "'", "'" },
		{ "{", "}" }
	};

	struct BlockRange final
	{
		size_t start = std::string_view::npos;
		size_t end = std::string_view::npos;
	};

	struct ClassBlockRange final
	{
		std::string_view name;

		size_t start = std::string_view::npos;
		size_t end = std::string_view::npos;
	};

	struct ParseData final
	{
		std::string_view file_text;

		size_t start_index = 0;
		size_t next_index = 0;

		BlockRange block_ranges[static_cast<size_t>(BlockRangeType::IgnoreBlocksCount)];
		std::vector<ClassBlockRange> class_ranges;
	};

	static bool IsWithinBlockRange(const ParseData& parse_data, BlockRangeType range_type, size_t index)
	{
		const BlockRange& range = parse_data.block_ranges[static_cast<size_t>(range_type)];

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

	static bool ProcessBlockRange(const std::string_view& substr, ParseData& parse_data, BlockRangeType type)
	{
		BlockRange& range = parse_data.block_ranges[static_cast<size_t>(type)];
		const auto marker = k_range_markers[static_cast<size_t>(type)];

		if (range.start == std::string_view::npos) {
			range.start = substr.find(marker[0]);

			if (range.start != std::string_view::npos) {
				if (marker[1]) {
					range.end = substr.find(marker[1], range.start + 1);

					if (range.end != std::string_view::npos) {
						range.end += parse_data.start_index + strlen(marker[1]);
					}

				} else if (parse_data.next_index != std::string_view::npos) {
					range.end = parse_data.file_text.find_first_of(k_newline_chars, parse_data.next_index, 1);
				}

				range.start += parse_data.start_index;
				return true;
			}

		} else {
			if (marker[1]) {
				range.end = substr.find(marker[1]);

				if (range.end != std::string_view::npos) {
					range.end += parse_data.start_index + strlen(marker[1]);
				}

			} else if (parse_data.next_index != std::string_view::npos) {
				range.end = parse_data.file_text.find_first_of(k_newline_chars, parse_data.next_index, 1);
			}

			if (range.end != std::string_view::npos) {
				return true;
			}
		}

		return false;
	}

	// Depth != 0 means we have already stripped out any ignorable blocks from substr, such as comments and string literals.
	static void Preproc_ParseSubstring(std::string_view substr, std::string& out_text, ParseData& parse_data, int32_t depth = 0)
	{
		if (substr.empty()) {
			return;
		}

		// Get all ignore ranges and calculate the largest ignore range size.
		BlockRange ignore_range;
		ignore_range.end = 0;

		for (int32_t i = 0; i < static_cast<int32_t>(BlockRangeType::IgnoreBlocksCount); ++i) {
			ProcessBlockRange(substr, parse_data, static_cast<BlockRangeType>(i));

			ignore_range.start = std::min(ignore_range.start, parse_data.block_ranges[i].start);
			ignore_range.end = std::max(
				ignore_range.end,
				(parse_data.block_ranges[i].end != std::string_view::npos) ? parse_data.block_ranges[i].end : 0
			);
		}

		// Did not find an end to the range.
		if (ignore_range.start >= ignore_range.end) {
			ignore_range.end = std::string_view::npos;
		}

		// Clear any ranges that are within another ignore range.
		for (int32_t i = 0; i < static_cast<int32_t>(BlockRangeType::IgnoreBlocksCount); ++i) {
			BlockRange& range = parse_data.block_ranges[i];

			for (int32_t j = 0; j < static_cast<int32_t>(BlockRangeType::IgnoreBlocksCount); ++j) {
				if (i == j) {
					continue;
				}

				if (IsWithinBlockRange(parse_data, static_cast<BlockRangeType>(j), range.start)) {
					if (!IsWithinBlockRange(parse_data, static_cast<BlockRangeType>(j), range.end)) {
						range.start = range.end;
					} else {
						range.start = std::string_view::npos;
					}

					range.end = std::string_view::npos;
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

		// Nothing to ignore.
		if (ignore_start == ignore_end) {
			//if (!parse_data.class_ranges.empty() && parse_data.class_ranges.back().name.empty()) {
			//	parse_data.class_ranges.back().name = substr;
			//	return;
			//}

			//const size_t struct_index = substr.find("struct");
			//const size_t class_index = substr.find("class");

			//if (struct_index != std::string_view::npos) {
			//	ClassBlockRange class_range;
			//	parse_data.class_ranges.push_back(class_range);
			//}

			//if (class_index != std::string_view::npos) {
			//}

			out_text += substr;

		} else {
			const std::string_view left_substr = substr.substr(0, ignore_start - parse_data.start_index);
			const std::string_view ignore_substr = substr.substr(ignore_start - parse_data.start_index, ignore_end - ignore_start);
			const std::string_view right_substr = substr.substr(ignore_end - parse_data.start_index);

			const size_t start_index = parse_data.start_index;
			const size_t end_index = parse_data.next_index;

			parse_data.next_index = start_index + left_substr.size();
			Preproc_ParseSubstring(left_substr, out_text, parse_data, depth + 1);

			out_text += ignore_substr;

			parse_data.start_index = parse_data.next_index + ignore_substr.size();
			parse_data.next_index = end_index;
			Preproc_ParseSubstring(right_substr, out_text, parse_data, depth + 1);

			parse_data.start_index = start_index;
			parse_data.next_index = end_index;
		}

		if (!depth && parse_data.next_index != std::string_view::npos) {
			out_text += parse_data.file_text[parse_data.next_index]; // Preserve token character.
		}
	}
}

bool Preproc_ParseFile(std::string_view file_text, std::string& out_text)
{
	ParseData parse_data;
	parse_data.next_index = file_text.find_first_of(k_parse_until_tokens);
	parse_data.file_text = file_text;

	while (parse_data.next_index != std::string::npos) {
		// Started on whitespace. Append it to the output and move on.
		if (parse_data.start_index == parse_data.next_index) {
			// Don't append whitespace characters if we're stripping from the beginning of the file.
			if (!out_text.empty()) {
				out_text += file_text[parse_data.start_index];
			}

		} else {
			const std::string_view substr = file_text.substr(parse_data.start_index, parse_data.next_index - parse_data.start_index);
			Preproc_ParseSubstring(substr, out_text, parse_data);
		}

		parse_data.start_index = parse_data.next_index + 1;
		parse_data.next_index = file_text.find_first_of(k_parse_until_tokens, parse_data.start_index);
	}

	const std::string_view substr = file_text.substr(parse_data.start_index);
	Preproc_ParseSubstring(substr, out_text, parse_data);

	return true;
}