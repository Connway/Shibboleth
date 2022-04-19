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

#include "CodeGen_ReflectionHeaderGenerator.h"
#include "CodeGen_Utils.h"
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>
#include <Gaff_File.h>
#include <argparse.hpp>
#include <fmt/core.h>
#include <filesystem>

static constexpr const char/*8_t*/* const k_gen_header =
/*u8*/R"(// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

#ifdef SHIB_STATIC
{}

namespace
{{
	enum class InitMode : int8_t
	{{
		Enums,
		Attributes,
		Classes,

		Count
	}};
}}

namespace Gen::{}
{{
	template <class T>
	void RegisterOwningModule(void)
	{{
		if constexpr (std::is_enum<T>::value) {{
			Shibboleth::GetApp().getReflectionManager().registerEnumOwningModule(Refl::Reflection<T>::GetHash(), u8"{}");
		}} else {{
			Shibboleth::GetApp().getReflectionManager().registerOwningModule(Refl::Reflection<T>::GetHash(), u8"{}");
		}}
	}}

	template <class T>
	void InitReflectionT(InitMode mode)
	{{
		if constexpr (std::is_enum<T>::value) {{
			if (mode == InitMode::Enums) {{
				Refl::Reflection<T>::Init();
				RegisterOwningModule<T>();
			}}
		}} else if constexpr (std::is_base_of<Refl::IAttribute, T>::value) {{
			if (mode == InitMode::Attributes) {{
				Refl::Reflection<T>::Init();
				RegisterOwningModule<T>();
			}}
		}} else {{
			if (mode == InitMode::Classes) {{
				Refl::Reflection<T>::Init();
				RegisterOwningModule<T>();
			}}
		}}
	}}

	static void InitReflection(InitMode mode)
	{{
{}
	}}
}}
#endif

namespace Shibboleth
{{
	class IModule;
}}

namespace {}
{{
	Shibboleth::IModule* CreateModule(void);
}}
)";

static constexpr const char/*8_t*/* const k_gen_static_header =
/*u8*/R"(// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

#ifdef SHIB_STATIC
	#undef SHIB_STATIC
{}
	#define SHIB_STATIC

namespace Gen::Engine
{{
	static bool LoadModulesStatic(Shibboleth::App& app)
	{{
{}
		return true;
	}}
}}

#endif
)";



static void ProcessLine(std::vector<std::u8string>& file_classes, std::u8string_view line)
{
	constexpr const std::u8string_view k_refl_decl = u8"SHIB_REFLECTION_DECLARE";

	if (line.find(u8"#define") != std::u8string_view::npos) {
		return;
	}

	size_t start = line.find(k_refl_decl);

	if (start == std::u8string_view::npos) {
		return;
	}

	// Line is commented out, don't init the reflection.
	if (const size_t comment_index = line.find(u8"//");
		comment_index != std::u8string_view::npos && comment_index < start) {
		return;
	}

	start = line.find(u8'(', start) + 1;
	const size_t end = line.find(u8')', start);

	std::u8string_view class_name = line.substr(start, end - start);

	while (isspace(class_name[0])) {
		class_name = class_name.substr(1);
	}

	while (isspace(class_name.back())) {
		class_name = class_name.substr(0, class_name.size() - 1);
	}

	file_classes.emplace_back(class_name);
}

static int WriteFile(
	const std::map< std::u8string, std::vector<std::u8string> >& file_class_map,
	const std::filesystem::path& abs_path,
	const argparse::ArgumentParser& program)
{
	std::u8string init_func = (file_class_map.empty()) ? u8"\t\tGAFF_REF(mode);\n" : u8"";
	std::u8string include_files = u8"";

	size_t init_count = 0;
	size_t count = 0;

	const char8_t separator = Gaff::ConvertChar<char8_t>(std::filesystem::path::preferred_separator);

	for (const auto& entry : file_class_map) {
		++count;

		const std::u8string abs_path_u8 = abs_path.u8string();
		const size_t index = entry.first.rfind(abs_path.u8string());

		if (index == std::u8string::npos) {
			std::cerr << "Failed to find directory for file '" << reinterpret_cast<const char*>(entry.first.data()) << "'." << std::endl;
			continue;
		}

		std::u8string file_name(entry.first.data() + index + 1 + abs_path_u8.size());

		if (separator == u8'\\') {
			size_t dir_sep = file_name.find(separator);

			while (dir_sep != std::u8string::npos) {
				file_name[dir_sep] = u8'/';
				dir_sep = file_name.find(separator);
			}
		}

		include_files += u8"#include \"" + file_name + u8'"';

		if (count != file_class_map.size()) {
			include_files += u8'\n';
		}

		init_count += entry.second.size();
	}

	count = 0;

	for (const auto& entry : file_class_map) {
		for (const auto& class_name : entry.second) {
			++count;

			init_func += u8"\t\tInitReflectionT<" + class_name + u8">(mode);";

			if (count != init_count) {
				init_func += u8'\n';
			}
		}
	}

	const std::string module_name = (program.is_used(k_arg_module)) ?
		program.get(k_arg_module) :
		(program.is_used(k_arg_tool)) ?
		program.get(k_arg_module) :
		"Engine";

	const std::string final_text = GetLicenseText(program) + fmt::format(
		k_gen_header,
		reinterpret_cast<const char*>(include_files.data()),
		module_name.data(),
		module_name.data(),
		module_name.data(),
		reinterpret_cast<const char*>(init_func.data()),
		module_name.data(),
		module_name.data(),
		module_name.data()
	);

	const std::u8string gen_file_path = abs_path.u8string() + u8"/include/Gen_ReflectionInit.h";
	Gaff::File gen_file(gen_file_path.data(), Gaff::File::OpenMode::Read);

	if (gen_file.isOpen()) {
		std::string current_gen_file_text(gen_file.getFileSize() + 1, 0);

		gen_file.readEntireFile(current_gen_file_text.data());
		gen_file.close();

		// Strings are equal, no need to write data to file.
		if (!strcmp(current_gen_file_text.data(), final_text.data())) {
			return 0;
		}
	}

	if (!gen_file.open(gen_file_path.data(), Gaff::File::OpenMode::Write)) {
		std::cerr << "Failed to open output file '" << reinterpret_cast<const char*>(gen_file_path.data()) << "'." << std::endl;
		return -6;
	}

	if (!gen_file.writeString(final_text.data())) {
		std::cerr << "Failed to write to output file '" << reinterpret_cast<const char*>(gen_file_path.data()) << "'." << std::endl;
		return -7;
	}

	return 0;
}

static int GenerateStaticReflectionHeader(
	const std::filesystem::path& abs_module_path,
	const argparse::ArgumentParser& program)
{
	const char8_t separator = Gaff::ConvertChar<char8_t>(std::filesystem::path::preferred_separator);
	std::vector<std::u8string> modules;

	// $TODO: Thread this.
	// Find all the module names.
	for (const auto& entry : std::filesystem::recursive_directory_iterator(u8"src/Modules")) {
		if (!entry.is_regular_file()) {
			continue;
		}

		const auto abs_path = std::filesystem::absolute(entry);
		const std::u8string abs_path_u8 = abs_path.u8string();

		if (!abs_path_u8.ends_with(u8"project_generator.lua")) {
			continue;
		}

		const size_t end_index = abs_path_u8.rfind(separator);

		if (end_index == std::u8string::npos) {
			std::cerr << "Failed to find directory for file '" << abs_path.string().data() << "'." << std::endl;
			continue;
		}

		const size_t start_index = abs_path_u8.rfind(separator, end_index - 1);

		if (start_index == std::u8string::npos) {
			std::cerr << "Failed to find directory for file '" << abs_path.string().data() << "'." << std::endl;
			continue;
		}

		const std::u8string_view module_name(abs_path_u8.data() + start_index + 1, end_index - (start_index + 1));

		if (module_name.find(u8"Dev") != 0) {
			modules.emplace_back(module_name);
		}
	}

	// Load the module order.
	Gaff::JSON config;

	if (!config.parseFile(u8"workingdir/cfg/app.cfg")) {
		std::cerr << "Failed to open or parse 'workingdir/cfg/app.cfg'." << std::endl;
		return -1;
	}

	const Gaff::JSON module_load_order = config.getObject(u8"module_load_order");
	std::vector<std::u8string> init_order;

	if (module_load_order.isArray()) {
		init_order.reserve(module_load_order.size());

		module_load_order.forEachInArray([&](int32_t /*index*/, const Gaff::JSON& value) -> bool
		{
			init_order.emplace_back(value.getString());
			return false;
		});
	}

	// Write modules inits in the order of module_load_order and then the rest.
	const size_t init_count = modules.size();
	std::u8string includes = u8"";
	std::u8string inits = u8"";
	size_t count = 0;

	for (const auto& module_name : init_order) {
		++count;

		// $TODO: Make use of std::filesystem::path::relative
		includes += u8"\t#include <../../Modules/" + module_name + u8"/include/Gen_ReflectionInit.h>";
		inits += u8"\t\tGAFF_FAIL_RETURN(app.createModule(" + module_name + u8"::CreateModule, u8\"" + module_name + u8"\"), false)\n";

		if (count != init_count) {
			includes += u8'\n';
		}
	}

	for (const auto& module_name : modules) {
		if (std::find(init_order.begin(), init_order.end(), module_name) != init_order.end()) {
			continue;
		}

		++count;

		// $TODO: Make use of std::filesystem::path::relative
		includes += u8"\t#include <../../Modules/" + module_name + u8"/include/Gen_ReflectionInit.h>";
		inits += u8"\t\tGAFF_FAIL_RETURN(app.createModule(" + module_name + u8"::CreateModule, u8\"" + module_name + u8"\"), false)\n";

		if (count != init_count) {
			includes += u8'\n';
		}
	}

	const std::string final_text = GetLicenseText(program) + fmt::format(
		k_gen_static_header,
		reinterpret_cast<const char*>(includes.data()),
		reinterpret_cast<const char*>(inits.data())
	);

	const std::u8string gen_file_path = abs_module_path.u8string() + u8"/include/Gen_StaticReflectionInit.h";
	Gaff::File gen_file(gen_file_path.data(), Gaff::File::OpenMode::Read);

	if (gen_file.isOpen()) {
		std::string current_gen_file_text(gen_file.getFileSize() + 1, 0);

		gen_file.readEntireFile(current_gen_file_text.data());
		gen_file.close();

		// Strings are equal, no need to write data to file.
		if (!strcmp(current_gen_file_text.data(), final_text.data())) {
			return 0;
		}
	}

	if (!gen_file.open(gen_file_path.data(), Gaff::File::OpenMode::Write)) {
		std::cerr << "Failed to open output file '" << reinterpret_cast<const char*>(gen_file_path.data()) << "'." << std::endl;
		return -6;
	}

	return gen_file.writeString(final_text.data()) ? 0 : -7;
}

static int GenerateReflectionHeader(
	const std::filesystem::path& abs_module_path,
	const argparse::ArgumentParser& program)
{
	std::map< std::u8string, std::vector<std::u8string> > file_class_map;

	// $TODO: Thread this.
	for (const auto& entry : std::filesystem::recursive_directory_iterator(abs_module_path)) {
		if (!entry.is_regular_file()) {
			continue;
		}

		const auto abs_path = std::filesystem::absolute(entry);
		const auto extension = abs_path.extension();

		// Only care about header files.
		if (extension != ".h" && extension != ".hpp") {
			continue;
		}

		// Ignore generated header files or reflection headers.
		if (const auto file_name = abs_path.u8string();
			file_name.ends_with(u8"Gen_ReflectionInit.h") ||
			file_name.ends_with(u8"Gen_StaticReflectionInit.h") ||
			file_name.ends_with(u8"Shibboleth_ReflectionMacros.h")) {

			continue;
		}

		Gaff::File file(abs_path.u8string().data());

		if (!file.isOpen()) {
			std::cerr << "Failed to open '" << abs_path << "'." << std::endl;
			continue;
		}

		std::u8string text(file.getFileSize() + 1, 0);

		if (!file.readEntireFile(text.data())) {
			std::cout << reinterpret_cast<const char*>(text.data()) << std::endl;
			std::cerr << "Failed to read contents of '" << abs_path << "'." << std::endl;
			continue;
		}

		// Go line by line and get all the class names.
		size_t offset = 0;
		size_t index = text.find(u8'\n', offset);
		std::vector<std::u8string> file_classes;

		while (index != std::u8string::npos) {
			const std::u8string_view line(text.begin() + offset, text.begin() + index);
			ProcessLine(file_classes, line);

			offset = index + 1;
			index = text.find(u8'\n', offset);
		}

		if (!file_classes.empty()) {
			file_class_map[abs_path.u8string()] = std::move(file_classes);
		}
	}

	return WriteFile(file_class_map, abs_module_path, program);
}


void ReflectionHeaderGenerator_AddArguments(argparse::ArgumentParser& program)
{
	program.add_argument(k_arg_module, k_arg_module_short)
		.help("The module to generate files for.");

	program.add_argument(k_arg_tool, k_arg_tool_short)
		.help("The tool to generate files for.");
}

int ReflectionHeaderGenerator_Run(const argparse::ArgumentParser& program)
{
	const std::string action = program.get("action");
	std::string path;

	if (action == "module_header") {
		if (!program.is_used(k_arg_module)) {
			std::cerr << "Did not provide '--module' parameter." << std::endl;
			return -2;
		}

		path = "src/Modules/" + program.get(k_arg_module);

	} else if (action == "tool_header") {
		if (!program.is_used(k_arg_tool)) {
			std::cerr << "Did not provide '--tool' parameter." << std::endl;
			return -2;
		}

		path = "src/Tools/" + program.get(k_arg_tool);
	
	} else if (action == "static_header") {
		path = "src/Engine/Engine";

	} else {
		std::cerr << "Unknown action '" << action << "'." << std::endl;
		return -3;
	}

	if (!std::filesystem::is_directory(path)) {
		std::cerr << "'" << path << "' is not a directory." << std::endl;
		return -4;
	}

	const std::filesystem::path abs_module_path = std::filesystem::absolute(path);

	if (const auto out_dir = abs_module_path.u8string() + u8"/include"; !Gaff::CreateDir(out_dir.data(), 0777)) {
		std::cerr << "Failed to create output directory '" << reinterpret_cast<const char*>(out_dir.data()) << "'." << std::endl;
		return -5;
	}

	const int ret = GenerateReflectionHeader(abs_module_path, program);

	if (!ret && action == "static_header") {
		return GenerateStaticReflectionHeader(abs_module_path, program);
	}

	return ret;
}
