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
#include <unordered_map>
#include <string>
#include <vector>
#include <map>

#ifdef PLATFORM_WINDOWS
	constexpr const char* const k_newline = "\r\n";
#else
	constexpr const char* const k_newline = "\n";
#endif

// Stop when we hit a newline or a semi-colon.
constexpr const char* const k_parse_until_tokens = " \n\f\r\t\v";
constexpr const char* const k_whitespace_chars = " \t\v";
constexpr const char* const k_newline_chars = "\n\f\r";

constexpr std::string_view k_parse_until_substr(k_parse_until_tokens);
constexpr std::string_view k_whitespace_substr(k_whitespace_chars);
constexpr std::string_view k_newline_substr(k_newline_chars);

enum class BlockRangeType
{
	// Ignore Ranges
	BlockComment,
	LineComment,

	RawStringLiteral,
	StringLiteral,
	CharLiteral,

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
	{ "R\"(", ")\"" },
	{ "\"", "\"" },
	{ "'", "'" },
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
	enum class Access
	{
		Default,
		Public,
		Private,
		Protected
	};

	struct InheritanceData final
	{
		std::string class_struct_name;
		Access access = Access::Default;
		bool is_virtual = false;
	};

	struct MixinData final
	{
		std::string class_struct_name;
		bool no_inherit = false;
	};

	const char* GetAccessText(Access access) const
	{
		switch (access) {
			case Access::Default:
				return (is_struct) ? "public" : "private";

			case Access::Public:
				return "public";

			case Access::Private:
				return "private";

			case Access::Protected:
				return "protected";
		}

		GAFF_ASSERT(false);
		return "";
	}

	Access GetAccess(Access access) const
	{
		if (access == Access::Default) {
			return (is_struct) ? Access::Public : Access::Private;
		}

		return access;
	}

	std::string name;
	std::string declaration_text; // Text between curly braces of class declaration.
	std::string definition_text; // Function implementations and static class variables.
	std::string mixin_definition_text; // The definition text accrued from mixins.
	std::u8string decl_file_path;
	std::u8string impl_file_path;
	std::string mixin_inheritance;
	std::vector<InheritanceData> inherits;
	std::vector<MixinData> mixin_classes;
	bool is_struct = false;
	bool is_final = false;
	bool finished = false;
};

struct ClassRuntimeData final
{
	enum class Flag
	{
		Struct,
		Anonymous,
		Template, // Flag for now, but will be unnecessary when we process template args.
		Final,
		MixinNoInherit,

		// Temp, runtime flags.
		ParsingInheritance,
		Virtual,

		Count
	};

	ClassData::Access GetAccess(ClassData::Access access) const
	{
		if (access == ClassData::Access::Default) {
			return (flags.testAll(Flag::Struct)) ? ClassData::Access::Public : ClassData::Access::Private;
		}

		return access;
	}

	std::vector<std::string> template_args; // $TODO: Process template classes.
	std::vector<ClassData::InheritanceData> inherits;
	std::string name;
	size_t scope_range_index = SIZE_T_FAIL;
	ClassData::Access curr_access = ClassData::Access::Default;
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

struct EnumData final
{
	std::string name;
	std::string storage_type;
	std::vector<std::string> entries;
};

struct EnumRuntimeData final
{
	enum class Flag
	{
		Valid,
		Anonymous,

		Count
	};

	EnumData data;
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
	size_t open_bracket_index = std::string_view::npos;
	size_t close_bracket_index = std::string_view::npos;
	size_t open_bracket_count = 0;
	size_t newline_count = 0;
	bool is_template = false;

	void clear(void)
	{
		args.clear();

		open_bracket_count = std::string_view::npos;
		close_bracket_index = std::string_view::npos;
		open_bracket_count = 0;
		newline_count = 0;
		is_template = false;
	}
};

// If copying runtime data becomes more of a hassle, use a struct for easy copying.
//struct ParseRuntimeData final
//{
//	size_t start_index = 0;
//	size_t next_index = 0;
//};

struct GlobalRuntimeData final
{
	struct ClassFileMapData final
	{
		bool has_decl = false;
		bool has_impl = false;
	};

	// class_file_map[file_path][class_name]
	std::unordered_map< std::u8string, std::unordered_map<std::string, ClassFileMapData> > class_file_map;
	std::unordered_map<std::string, std::string> namespace_mappings;
	std::unordered_map<std::string, ClassData> class_data;
	std::unordered_map<std::string, EnumData> enum_data;
};

struct ParseData final
{
	enum class Flag
	{
		ClassStructName,
		ClassFunctionDefinitionLine,
		ClassFunctionDefinition,

		MixinName,

		EnumName,

		NamespaceName,

		FunctionName,

		EnumStorage,
		EnumEntries,
		EnumNextEntry,

		PreprocFirstToken,
		PreprocFirstTokenClear,
		PreprocConditional,
		PreprocDefine,
		PreprocSkipNewline,

		ProcessingTemplate,

		LastTokenOnLine,

		DoNotWriteSubstring,
		WriteFile,

		Count,

		FirstRuntimeFlag = 0,
		LastRuntimeFlag = ProcessingTemplate
	};

	std::u8string file_path;
	std::string_view file_text;
	std::string out_text;

	std::string_view prev_substr;

	std::vector<std::string> include_dirs;

	// Runtime info.
	BlockRange block_ranges[static_cast<size_t>(BlockRangeType::IgnoreBlocksCount)];
	std::vector<ClassRuntimeData> class_stack;
	std::vector<ScopeRuntimeData> scope_ranges;
	NamespaceRuntimeData namespace_runtime; // Only have one instance of it as this data is temporary. It gets absorbed into ScopeRuntimeData after we process it enough.
	EnumRuntimeData enum_runtime;
	TemplateRuntimeData template_runtime;
	GlobalRuntimeData* global_runtime = nullptr;
	ClassData* curr_class_data = nullptr;

	int32_t preproc_if_count = 0;

	size_t start_of_line_index = 0;
	size_t prev_index = 0;
	size_t start_index = 0;
	size_t next_index = 0;

	Gaff::Flags<Flag> flags;
};

void Preproc_ParseSubstring(std::string_view substr, ParseData& parse_data, int32_t depth = 0);
bool Preproc_ParseFile(ParseData& parse_data);

// Helpers
size_t SplitSubstr(std::string_view substr, const char* delimeter, std::string_view& left, std::string_view& right, bool reverse_find = false);
size_t SplitSubstr(std::string_view substr, char delimeter, std::string_view& left, std::string_view& right, bool reverse_find = false);
void StripComments(std::string& text);
void TrimNewlines(std::string& text);
