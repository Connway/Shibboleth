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

#pragma once

#include <Gaff_Flags.h>
#include <string>
#include <vector>
#include <map>

// Stop when we hit a newline or a semi-colon.
constexpr const char* const k_parse_until_tokens = " \n\f\r\t\v";
constexpr const char* const k_whitespace_chars = " \t\v";
constexpr const char* const k_newline_chars = "\n\f\r";

enum class BlockRangeType
{
	// Ignore Ranges
	BlockComment,
	LineComment,

	StringLiteral,
	CharLiteral,

	//TemplateArgs1,
	//TemplateArgs2,

	PreprocessorDirective,

	// Scope Ranges
	Scope,

	Count,

	IgnoreBlocksStart = BlockComment,
	IgnoreBlocksEnd = PreprocessorDirective,
	IgnoreBlocksCount = IgnoreBlocksEnd - IgnoreBlocksStart + 1,

	//EmbeddableBlocksStart = Scope,
	//EmbeddableBlocksEnd = Scope,
	//EmbeddableBlocksCount = EmbeddableBlocksStart - EmbeddableBlocksEnd + 1,
};

constexpr const char* k_range_markers[static_cast<size_t>(BlockRangeType::Count)][2] =
{
	// Ignore Ranges
	{ "/*", "*/" },
	{ "//", nullptr },
	{ "\"", "\"" },
	{ "'", "'" },
	//{ "template<", ">" },
	//{ "template <", ">" },
	{ "#", nullptr }, // Requires special processing.

	// Scope Ranges
	{ "{", "}" }
};

struct BlockRange final
{
	size_t start = std::string_view::npos;
	size_t end = std::string_view::npos;
};

struct ClassData final
{
	std::string name;
	std::string declaration_text; // Text between curly braces.
	std::string definition_text; // Text in cpp file.
	std::vector<std::string> mixin_classes;
	bool is_struct = false;
	bool finished = false;
};

struct ClassRuntimeData final
{
	enum class Visibility
	{
		Public,
		Private,
		Protected
	};

	enum class Flag
	{
		Struct,
		Anonymous,

		Count
	};

	std::string name;
	size_t scope_range_index = SIZE_T_FAIL;
	Visibility curr_visibility = Visibility::Private;
	Gaff::Flags<Flag> flags;
};

struct ScopeRuntimeData final
{
	enum class Type
	{
		Unknown,
		Class,
		Enum,
		Namespace,
		Conditional,	// Currently unused as we have no detection for it.
		Loop			// Currently unused as we have no detection for it.
	};

	std::string name;
	BlockRange range;
	Type type = Type::Unknown;
};

//struct EnumData final
//{
//	std::string name;
//};

struct EnumRuntimeData final
{
	enum class Flag
	{
		Valid,
		Anonymous,

		Count
	};


	std::string name;
	size_t scope_range_index = SIZE_T_FAIL;
	Gaff::Flags<Flag> flags;
};

struct NamespaceRuntimeData final
{
	std::string name;
	bool valid = false;
};

struct TemplateRuntimeData final
{
	std::vector<std::string> args;
	bool is_template = false;
};

// If copying runtime data becomes more of a hassle, use a struct for easy copying.
//struct ParseRuntimeData final
//{
//	size_t start_index = 0;
//	size_t next_index = 0;
//};

//struct FileTextData final
//{
//	std::string output_path;
//	std::string input_path;
//	std::string text;
//};

struct GlobalRuntimeData final
{
	//std::map<size_t, FileTextData> file_text;
	std::map<size_t, ClassData> class_data;
};

struct ParseData final
{
	enum class Flag
	{
		ClassStructName,
		MixinName,
		EnumName,
		NamespaceName,

		PreprocFirstToken,
		PreprocFirstTokenClear,
		PreprocConditional,
		PreprocDefine,
		PreprocSkipNewline,

		LastTokenOnLine,

		DoNotWriteSubstring,
		WriteFile,

		Count
	};

	std::string_view file_text;
	std::string out_text;

	std::vector<std::string> include_dirs;

	//std::map<size_t, EnumData> enum_data;

	// Runtime info.
	BlockRange block_ranges[static_cast<size_t>(BlockRangeType::IgnoreBlocksCount)];
	std::vector<ClassRuntimeData> class_stack;
	std::vector<ScopeRuntimeData> scope_ranges;
	NamespaceRuntimeData namespace_runtime; // Only have one instance of it as this data is temporary. It gets absorbed into ScopeRuntimeData after we process it enough.
	EnumRuntimeData enum_runtime;
	TemplateRuntimeData template_runtime;
	GlobalRuntimeData* global_runtime = nullptr;

	int32_t preproc_if_count = 0;

	size_t start_index = 0;
	size_t next_index = 0;

	Gaff::Flags<Flag> flags;
};

void Preproc_ParseSubstring(std::string_view substr, ParseData& parse_data, int32_t depth = 0);
bool Preproc_ParseFile(ParseData& parse_data);
