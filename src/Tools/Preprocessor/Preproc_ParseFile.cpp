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
#include "Preproc_ParseNamespace.h"
#include "Preproc_ParseTemplate.h"
#include "Preproc_ParseFunction.h"
#include "Preproc_ParseClass.h"
#include "Preproc_ParseMixin.h"
#include "Preproc_ParseEnum.h"

namespace
{
	static bool IsWithinBlockRange(const BlockRange& range, size_t index)
	{
		// Neither range is established.
		if (index == std::string_view::npos || range.start == std::string_view::npos) {
			return false;
		}

		if ((range.end == std::string_view::npos && index > range.start) ||
			(range.end != std::string_view::npos && index > range.start && index <= range.end)) {

			return true;
		}

		return false;
	}

	static void ProcessBlockRangeStart(std::string_view substr, const ParseData& parse_data, BlockRange& range, BlockRangeType type, size_t offset = 0)
	{
		const auto marker = k_range_markers[static_cast<size_t>(type)];

		range.start = substr.find(marker[0], offset);

		if (range.start != std::string_view::npos) {
			range.start += parse_data.start_index;
		}
	}

	static void ProcessBlockRangeEnd(std::string_view substr, const ParseData& parse_data, BlockRange& range, BlockRangeType type, size_t offset = 0)
	{
		const auto marker = k_range_markers[static_cast<size_t>(type)];

		if (marker[1]) {
			range.end = substr.find(marker[1], offset);

			if (range.end != std::string_view::npos) {
				range.end += parse_data.start_index + strlen(marker[1]);
			}

		} else if (parse_data.next_index != std::string_view::npos) {
			if (parse_data.flags.testAll(ParseData::Flag::LastTokenOnLine)) {
				range.end = parse_data.next_index;
			}

			//if (parse_data.file_text.substr(parse_data.next_index, 1).find_first_of(k_newline_chars) != std::string_view::npos) {
			//	range.end = parse_data.next_index;
			//}
		}
	}

	static void ProcessBlockRange(std::string_view substr, const ParseData& parse_data, BlockRange& range, BlockRangeType type, size_t offset = 0)
	{
		const auto marker = k_range_markers[static_cast<size_t>(type)];

		if (marker[0] == nullptr) {
			return;
		}

		if (range.start == std::string_view::npos) {
			ProcessBlockRangeStart(substr, parse_data, range, type, offset);

			if (range.start != std::string_view::npos) {
				ProcessBlockRangeEnd(substr, parse_data, range, type, range.start + 1 - parse_data.start_index);
			}

		} else {
			ProcessBlockRangeEnd(substr, parse_data, range, type, offset);
		}
	}

	static void ProcessPreprocessorDirective(std::string_view substr, ParseData& parse_data, BlockRange& range)
	{
		// Find opening '#' as normal.
		if (range.start == std::string_view::npos) {
			ProcessBlockRangeStart(substr, parse_data, range, BlockRangeType::PreprocessorDirective);

			if (range.start != std::string_view::npos) {
				parse_data.flags.set(ParseData::Flag::PreprocFirstToken);
			}
		}

		if (range.start != std::string_view::npos) {
			if (parse_data.flags.testAll(ParseData::Flag::PreprocFirstToken) && substr != "#") {
				if (substr.find("if") != std::string_view::npos) {
					parse_data.flags.set(ParseData::Flag::PreprocConditional);
					++parse_data.preproc_if_count;

				} else if (substr.find("define") != std::string_view::npos) {
					parse_data.flags.set(ParseData::Flag::PreprocDefine);
				}

				parse_data.flags.set(ParseData::Flag::PreprocFirstTokenClear);
			}

			if (range.end == std::string_view::npos) {
				if (parse_data.flags.testAll(ParseData::Flag::PreprocDefine) && substr.find('\\') != std::string_view::npos) {
					ProcessBlockRangeEnd(substr, parse_data, range, BlockRangeType::PreprocessorDirective);
					parse_data.flags.set(ParseData::Flag::PreprocSkipNewline);

				} else if (parse_data.flags.testAll(ParseData::Flag::PreprocConditional)) {
					if (!parse_data.flags.testAll(ParseData::Flag::PreprocFirstTokenClear)) {
						if (!parse_data.flags.testAll(ParseData::Flag::PreprocFirstToken) && substr.find("#") != std::string_view::npos) {
							parse_data.flags.set(ParseData::Flag::PreprocFirstToken);
						}

						if (parse_data.flags.testAll(ParseData::Flag::PreprocFirstToken) && substr != "#") {
							parse_data.flags.set(ParseData::Flag::PreprocFirstTokenClear);

							const size_t end_end_if_pos = substr.find("endif");
							const size_t end_if_pos = substr.find("if");

							// Count nested "#if*" directives.
							if (end_if_pos != std::string_view::npos && end_end_if_pos == std::string_view::npos) {
								++parse_data.preproc_if_count;

							} else if (end_end_if_pos != std::string_view::npos) {
								--parse_data.preproc_if_count;

								if (!parse_data.preproc_if_count) {
									range.end = parse_data.start_index + end_end_if_pos + 5;
								}
							}
						}
					}

				} else {
					ProcessBlockRangeEnd(substr, parse_data, range, BlockRangeType::PreprocessorDirective);
				}

				if (range.end != std::string_view::npos && parse_data.flags.testAll(ParseData::Flag::PreprocSkipNewline)) {
					parse_data.flags.clear(ParseData::Flag::PreprocSkipNewline);
					range.end = std::string_view::npos;
				}
			}
		}

		if (parse_data.flags.testAll(ParseData::Flag::PreprocFirstTokenClear)) {
			parse_data.flags.clear(ParseData::Flag::PreprocFirstTokenClear);
			parse_data.flags.clear(ParseData::Flag::PreprocFirstToken);
		}

		if (range.end != std::string_view::npos) {
			parse_data.flags.clearRange(ParseData::Flag::PreprocFirstToken, ParseData::Flag::PreprocSkipNewline);
		}
	}

	static BlockRange ProcessIgnoreRange(std::string_view substr, ParseData& parse_data)
	{
		// Get all ignore ranges and calculate the largest ignore range size.
		BlockRange ignore_range;
		ignore_range.end = 0;

		for (int32_t i = 0; i < static_cast<int32_t>(BlockRangeType::IgnoreBlocksCount); ++i) {
			BlockRange& range = parse_data.block_ranges[i];

			// Ignore this for now. We need to process this differently.
			if (i == static_cast<int32_t>(BlockRangeType::PreprocessorDirective)) {
				ProcessPreprocessorDirective(substr, parse_data, range);
			} else {
				ProcessBlockRange(substr, parse_data, range, static_cast<BlockRangeType>(i));
			}

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

		using ScopeFunc = void (*)(ParseData&);

		if (open_pos != std::string_view::npos) {
			const std::string_view before_scope = substr.substr(0, open_pos);
			const std::string_view after_scope = substr.substr(open_pos + strlen(marker[0]));

			// Process before new scope.
			parse_data.next_index = start_index + before_scope.size();
			Preproc_ParseSubstring(before_scope, parse_data, depth + 1);

			// Open new scope.
			ScopeRuntimeData scope_data;
			scope_data.range.start = start_index + open_pos;

			parse_data.scope_ranges.emplace_back(scope_data);

			// Process things that care about new scopes.
			constexpr ScopeFunc k_scope_open_funcs[] =
			{
				ProcessClassScopeOpen,
				ProcessEnumScopeOpen,
				ProcessNamespaceScopeOpen
			};

			for (ScopeFunc scope_func : k_scope_open_funcs) {
				scope_func(parse_data);
			}

			// Preserve open scope marker.
			if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
				parse_data.out_text += marker[0];
			}

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

			// Process things that care about closing scopes.
			parse_data.scope_ranges.back().range.end = start_index + close_pos;

			constexpr ScopeFunc k_scope_close_funcs[] =
			{
				ProcessClassScopeClose,
				ProcessEnumScopeClose,
				ProcessNamespaceScopeClose
			};

			for (ScopeFunc scope_func : k_scope_close_funcs) {
				scope_func(parse_data);
			}

			// Close previous scope.
			parse_data.scope_ranges.pop_back();

			// Preserve close scope marker.
			if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
				parse_data.out_text += marker[1];
			}

			// Process rest.
			parse_data.start_index = start_index + close_pos + strlen(marker[1]);
			parse_data.next_index = end_index;
			Preproc_ParseSubstring(after_scope, parse_data, depth + 1);

			parse_data.start_index = start_index;
			parse_data.next_index = end_index;

			append_text = false;
		}

		using ParseFunc = bool (*)(std::string_view, ParseData&);
		constexpr ParseFunc k_parse_funcs[] =
		{
			ParseClass,
			ParseMixin,
			ParseEnum,
			ParseNamespace,
			ParseTemplate,
			ParseFunction
		};

		for (ParseFunc parse_func : k_parse_funcs) {
			if (parse_func(substr, parse_data)) {
				break;
			}
		}

		if (append_text) {
			if (parse_data.flags.testAll(ParseData::Flag::WriteFile) &&
				!parse_data.flags.testAll(ParseData::Flag::DoNotWriteSubstring)) {

				parse_data.out_text += substr;
			}
		}

	} else {
		const std::string_view left_substr = substr.substr(0, ignore_range.start - parse_data.start_index);
		const std::string_view ignore_substr = substr.substr(ignore_range.start - parse_data.start_index, ignore_range.end - ignore_range.start);
		const std::string_view right_substr = substr.substr(ignore_range.end - parse_data.start_index);

		const size_t start_index = parse_data.start_index;
		const size_t end_index = parse_data.next_index;

		parse_data.next_index = start_index + left_substr.size();
		Preproc_ParseSubstring(left_substr, parse_data, depth + 1);

		if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
			parse_data.out_text += ignore_substr;
		}

		parse_data.start_index = parse_data.next_index + ignore_substr.size();
		parse_data.next_index = end_index;
		Preproc_ParseSubstring(right_substr, parse_data, depth + 1);

		parse_data.start_index = start_index;
		parse_data.next_index = end_index;
	}

	if (!depth && parse_data.next_index != std::string_view::npos) {
		if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
			parse_data.out_text += parse_data.file_text[parse_data.next_index]; // Preserve token character.
		}
	}
}

bool Preproc_ParseFile(ParseData& parse_data)
{
	//ParseRuntimeData runtime_data;

	parse_data.next_index = parse_data.file_text.find_first_of(k_parse_until_tokens);

	while (parse_data.next_index != std::string::npos) {
		// Started on whitespace. Append it to the output and move on.
		if (parse_data.start_index == parse_data.next_index) {
			if (parse_data.flags.testAll(ParseData::Flag::WriteFile)) {
				// Don't append whitespace characters if we're stripping from the beginning of the file.
				if (!parse_data.out_text.empty()) {
					parse_data.out_text += parse_data.file_text[parse_data.start_index];
				}
			}

		} else {
			constexpr std::string_view parse_tokens(k_parse_until_tokens);
			constexpr std::string_view newline_substr(k_newline_chars);

			for (size_t index = parse_data.next_index; index < parse_data.file_text.size(); ++index) {
				// Find a valid character. We're done parsing.
				if (parse_tokens.find(parse_data.file_text[index]) == std::string_view::npos) {
					break;
				}

				// We found a newline, this is the last token for this line.
				if (newline_substr.find(parse_data.file_text[index]) != std::string_view::npos) {
					parse_data.flags.set(ParseData::Flag::LastTokenOnLine);

					// Once we've finished processing the template arguments, clear the template data if we pass 2 newlines
					// and have not yet had anything consume the template data.
					if (parse_data.template_runtime.is_template && !parse_data.flags.testAll(ParseData::Flag::ProcessingTemplate)) {
						++parse_data.template_runtime.newline_count;

						if (parse_data.template_runtime.newline_count >= 2) {
							parse_data.template_runtime.clear();
						}
					}

					break;
				}
			}

			const std::string_view substr = parse_data.file_text.substr(parse_data.start_index, parse_data.next_index - parse_data.start_index);
			Preproc_ParseSubstring(substr, parse_data/*, runtime_data*/);

			parse_data.flags.clear(ParseData::Flag::LastTokenOnLine);
			parse_data.prev_substr = substr;
		}

		parse_data.start_index = parse_data.next_index + 1;
		parse_data.next_index = parse_data.file_text.find_first_of(k_parse_until_tokens, parse_data.start_index);
	}

	const std::string_view substr = parse_data.file_text.substr(parse_data.start_index);
	Preproc_ParseSubstring(substr, parse_data/*, runtime_data*/);
	parse_data.prev_substr = substr;

	return true;
}