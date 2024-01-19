/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Preproc_DoPreproc.h"
#include "Preproc_ParseNamespace.h"
#include "Preproc_ParseMixin.h"
#include "Preproc_ParseFile.h"
#include "Preproc_Common.h"
#include "Preproc_Errors.h"
#include <Gaff_String.h>
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <argparse.hpp>
#include <filesystem>

namespace
{
	static int DoPreproc_CopyFile(
		const argparse::ArgumentParser& /*program*/,
		const std::string& /*name*/,
		const std::string& dir,
		const std::string& gen_dir,
		const std::filesystem::path& path)
	{
		std::string output_file_path = path.string().replace(0, dir.size(), gen_dir);

		// Create output directory.
		size_t pos = output_file_path.find('\\');

		while (pos != std::string::npos) {
			output_file_path[pos] = '/';
			pos = output_file_path.find('\\');
		}

		pos = output_file_path.find('/', pos + 1);

		while (pos != std::string::npos) {
			const std::string out_dir = output_file_path.substr(0, pos);

			if (!Gaff::CreateDir(out_dir.data(), 0777)) {
				std::cerr << "Failed to create output directory '" << out_dir << "'." << std::endl;
				return static_cast<int>(Error::DoPreproc_FailedToCreateOutputDir);
			}

			pos = output_file_path.find('/', pos + 1);
		}

		// Copy file.
		std::error_code error;
		std::filesystem::copy(path, output_file_path, std::filesystem::copy_options::overwrite_existing, error);

		if (error) {
			std::cerr << error.message() << std::endl;
			return static_cast<int>(Error::DoPreproc_FailedToWriteOutputFile);
		}

		return static_cast<int>(Error::Success);
	}

	static int DoPreproc_ProcessFile(
		const argparse::ArgumentParser& program,
		GlobalRuntimeData& global_runtime_data,
		const std::string& dir,
		const std::string& gen_dir,
		const std::filesystem::path& path,
		bool write_file)
	{
		CONVERT_STRING(char8_t, temp, path.c_str());
		Gaff::File file(temp, Gaff::File::OpenMode::ReadBinary);

		if (!file.isOpen()) {
			std::cerr << "Failed to open file '" << path << "'." << std::endl;
			return static_cast<int>(Error::DoPreproc_FailedToOpenInputFile);
		}

		ParseData parse_data;
		parse_data.flags.set(write_file, ParseData::Flag::WriteFile);
		parse_data.global_runtime = &global_runtime_data;
		parse_data.file_path = path.u8string();

		size_t separator_index = parse_data.file_path.find(u8'\\');

		while (separator_index != std::u8string::npos) {
			parse_data.file_path.replace(separator_index, 1, u8"/");
			separator_index = parse_data.file_path.find(u8'\\');
		}

		std::string file_text;

		file_text.resize(file.getFileSize());
		file.readEntireFile(file_text.data());

		parse_data.include_dirs = program.get< std::vector<std::string> >("--include");
		parse_data.file_text = file_text;
		parse_data.out_text = file_text;

		// Currently we don't do any processing that can't be done as an insert.
		if (!write_file) {
			if (!Preproc_ParseFile(parse_data)) {
				std::cerr << "Failed to process file '" << path.string() << "'." << std::endl;
				return static_cast<int>(Error::DoPreproc_FailedToProcessFile);
			}

			using PostProcessFunc = void (*)(ParseData&);
			constexpr PostProcessFunc k_post_process_funcs[] =
			{
				PostProcessNamespaces
			};

			for (PostProcessFunc post_process_func : k_post_process_funcs) {
				post_process_func(parse_data);
			}
		}

		if (write_file) {
			using ModifyOutputFunc = void (*)(ParseData&);
			constexpr ModifyOutputFunc k_modify_output_funcs[] =
			{
				ModifyOutputMixin
			};

			for (ModifyOutputFunc modify_output_func : k_modify_output_funcs) {
				modify_output_func(parse_data);
			}

			// Add warning to source files.
			if (path.extension() == ".h" || path.extension() == ".hpp" || path.extension() == ".cpp") {
				parse_data.out_text = std::string("// THIS IS A GENERATED FILE! MODIFICATIONS WILL BE LOST!") +
					k_newline +
					k_newline +
					parse_data.out_text;
			}

			// Check and write output file.
			std::string output_file_path = path.string().replace(0, dir.size(), gen_dir);

			// Create output directory.
			size_t pos = output_file_path.find('\\');

			while (pos != std::string::npos) {
				output_file_path[pos] = '/';
				pos = output_file_path.find('\\');
			}

			pos = output_file_path.find('/', pos + 1);

			while (pos != std::string::npos) {
				const std::string out_dir = output_file_path.substr(0, pos);

				if (!Gaff::CreateDir(out_dir.data(), 0777)) {
					std::cerr << "Failed to create output directory '" << out_dir << "'." << std::endl;
					return static_cast<int>(Error::DoPreproc_FailedToCreateOutputDir);
				}

				pos = output_file_path.find('/', pos + 1);
			}

			if (std::filesystem::is_regular_file(output_file_path)) {
				Gaff::File output_file(output_file_path.data(), Gaff::File::OpenMode::ReadBinary);

				if (!output_file.isOpen()) {
					std::cerr << "Failed to open output file '" << output_file_path << "'." << std::endl;
					return static_cast<int>(Error::DoPreproc_FailedToOpenOutputFile);
				}

				std::string old_output_file_text;
				old_output_file_text.resize(output_file.getFileSize());

				if (!output_file.readEntireFile(old_output_file_text.data())) {
					std::cerr << "Failed to read output file '" << output_file_path << "'." << std::endl;
					return static_cast<int>(Error::DoPreproc_FailedToReadOutputFile);
				}

				// Don't write the output file if the resulting output is the same.
				// $TODO: Database of timestamps to make this more optimal? Could also be used for CodeGenerator.
				if (parse_data.out_text == old_output_file_text) {
					return static_cast<int>(Error::Success);
				}

				output_file.close();
			}

			Gaff::File output_file(output_file_path.data(), Gaff::File::OpenMode::WriteBinary);

			if (!output_file.isOpen()) {
				std::cerr << "Failed to open output file '" << output_file_path << "'." << std::endl;
				return static_cast<int>(Error::DoPreproc_FailedToOpenOutputFile);
			}

			if (!output_file.writeString(parse_data.out_text.data())) {
				std::cerr << "Failed to write output file '" << output_file_path << "'." << std::endl;
				return static_cast<int>(Error::DoPreproc_FailedToWriteOutputFile);
			}

		}

		return static_cast<int>(Error::Success);
	}

	static int DoPreproc_ProcessDirectoryAndPopulateClassData(
		const argparse::ArgumentParser& program,
		GlobalRuntimeData& global_runtime_data,
		const std::string& dir,
		const std::string& gen_dir)
	{
		if (!std::filesystem::is_directory(dir)) {
			std::cerr << '"' << dir << "\" is not a directory." << std::endl;
			return static_cast<int>(Error::DoPreproc_PathNotFound);
		}

		// Find all header and implementation files.
		std::vector<std::filesystem::path> header_files;
		std::vector<std::filesystem::path> impl_files;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
			if (!entry.is_regular_file()) {
				continue;
			}

			const std::filesystem::path& path = entry.path();

			// Ignore these filenames.
			if (const std::filesystem::path filename = path.filename();
				filename == "project_generator.lua" || filename == "Gen_ReflectionInit.h" || filename == "Gen_StaticReflectionInit.h") {

				continue;
			}

			const std::filesystem::path extension = path.extension();

			if (extension == ".h" || extension == ".hpp") {
				header_files.emplace_back(path);
			} else if (extension == ".cpp") {
				impl_files.emplace_back(path);
			}
		}

		// Process header files first.
		for (const std::filesystem::path& path : header_files) {
			//int ret = static_cast<int>(Error::Success);

			/*ret =*/ DoPreproc_ProcessFile(program, global_runtime_data, dir, gen_dir, path, false);

			//if (ret) {
			//	return ret;
			//}
		}

		// Process implementation files second.
		for (const std::filesystem::path& path : impl_files) {
			//int ret = static_cast<int>(Error::Success);

			/*ret =*/ DoPreproc_ProcessFile(program, global_runtime_data, dir, gen_dir, path, false);

			//if (ret) {
			//	return ret;
			//}
		}

		for (auto& data : global_runtime_data.class_data) {
			if (data.second.finished) {
				continue;
			}

			using ProcessClassStructFunc = void (*)(GlobalRuntimeData&, ClassData&);
			constexpr ProcessClassStructFunc k_process_funcs[] =
			{
				ProcessClassStructMixin
			};

			for (ProcessClassStructFunc process_func : k_process_funcs) {
				process_func(global_runtime_data, data.second);
			}

			data.second.finished = true;
		}

		return static_cast<int>(Error::Success);
	}

	static int DoPreproc_ProcessDirectory(
		const argparse::ArgumentParser& program,
		GlobalRuntimeData& global_runtime_data,
		const std::string& name,
		const std::string& dir,
		const std::string& gen_dir)
	{
		if (name.empty()) {
			int ret = static_cast<int>(Error::Success);

			// Iterate over all folders in dir.
			for (const auto& entry : std::filesystem::directory_iterator(dir)) {
				// Only care about directory names.
				if (!entry.is_directory()) {
					continue;
				}

				const std::string sub_name = entry.path().stem().string();
				bool ignore = false;

				for (const char* const ignore_name : k_ignore_list) {
					if (name == ignore_name) {
						ignore = true;
						break;
					}
				}

				if (ignore) {
					continue;
				}

				std::cout << "Running Preprocessor for '" << sub_name << '\'' << std::endl;

				const int new_ret = DoPreproc_ProcessDirectory(program, global_runtime_data, sub_name, dir, gen_dir);

				if (new_ret && !ret) {
					ret = new_ret;
				}
			}

			return ret;
		}

		const std::string dir_path = dir + "/" + name;

		if (!std::filesystem::is_directory(dir_path)) {
			std::cerr << '"' << dir_path << "\" is not a directory." << std::endl;
			return static_cast<int>(Error::DoPreproc_PathNotFound);
		}

		for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
			if (!entry.is_regular_file()) {
				continue;
			}

			const std::filesystem::path& path = entry.path();
			const std::filesystem::path extension = path.extension();
			const std::filesystem::path filename = path.filename();

			//int ret = static_cast<int>(Error::Success);

			if ((extension == ".h" || extension == ".hpp" || extension == ".cpp") &&
				filename != "Gen_ReflectionInit.h" && filename != "Gen_StaticReflectionInit.h") {

				/*ret =*/ DoPreproc_ProcessFile(program, global_runtime_data, dir, gen_dir, path, true);

			} else {
				/*ret =*/ DoPreproc_CopyFile(program, name, dir, gen_dir, path);
			}

			//if (ret) {
			//	return ret;
			//}
		}

		return static_cast<int>(Error::Success);
	}
}

void DoPreproc_AddArguments(argparse::ArgumentParser& /*program*/)
{
}

int DoPreproc_Run(const argparse::ArgumentParser& program)
{
	constexpr const char* const k_gen_module_dir = ".generated/preproc/Modules";
	constexpr const char* const k_gen_engine_dir = ".generated/preproc/Engine";
	constexpr const char* const k_gen_tool_dir = ".generated/preproc/Tools";
	constexpr const char* const k_module_dir = "src/Modules";
	constexpr const char* const k_engine_dir = "src/Engine";
	constexpr const char* const k_tool_dir = "src/Tools";

	const std::string name = program.get("module_or_tool_name");
	const bool process_pass = program.get<bool>("--process_pass");
	const bool force_module = program.get<bool>("--module");
	const bool force_engine = program.get<bool>("--engine");
	const bool force_tool = program.get<bool>("--tool");

	bool process_engine = false;
	bool process_module = false;
	bool process_tool = false;
	bool modify_engine = false;
	bool modify_module = false;
	bool modify_tool = false;

	std::string engine_path;
	std::string file_name;
	std::string gen_path;

	if (program.is_used("--file")) {
		if (name.empty()) {
			std::cerr << "--file was used, but no module or tool name was given." << std::endl;
			return static_cast<int>(Error::DoPreproc_NoModuleOrToolSpecified);
		}

		file_name = program.get<std::string>("--file");

		if (force_engine) {
			gen_path = k_gen_engine_dir;
			engine_path = k_engine_dir;
			process_engine = !process_pass;

		} else if (force_module) {
			gen_path = k_gen_module_dir;
			engine_path = k_module_dir;
			process_engine = true;
			process_module = !process_pass;

		} else if (force_tool) {
			gen_path = k_gen_tool_dir;
			engine_path = k_tool_dir;
			process_engine = true;
			process_module = true;
			process_tool = !process_pass;

		} else {
			std::cerr << "--module, --tool, or --engine not specified." << std::endl;
			return static_cast<int>(Error::DoPreproc_NoForceFlagSpecified);
		}

	} else {
		process_engine = (force_engine || force_module || force_tool) || (!force_engine && !force_module && !force_tool);
		process_module = (force_module || force_tool) || (!force_engine && !force_module && !force_tool);
		process_tool = force_tool || (!force_engine && !force_module && !force_tool);
		modify_engine = process_engine && !process_pass;
		modify_module = process_module && !process_pass;
		modify_tool = process_tool && !process_pass;
	}

	GlobalRuntimeData global_runtime_data;

	if (process_engine) {
		int ret = DoPreproc_ProcessDirectoryAndPopulateClassData(program, global_runtime_data, k_engine_dir, k_gen_engine_dir);

		if (ret) {
			return ret;
		}

		if (modify_engine) {
			ret = DoPreproc_ProcessDirectory(program, global_runtime_data, name, k_engine_dir, k_gen_engine_dir);

			if (ret) {
				return ret;
			}
		}
	}

	if (process_module) {
		int ret = DoPreproc_ProcessDirectoryAndPopulateClassData(program, global_runtime_data, k_module_dir, k_gen_module_dir);

		if (ret) {
			return ret;
		}

		if (modify_module) {
			ret = DoPreproc_ProcessDirectory(program, global_runtime_data, name, k_module_dir, k_gen_module_dir);

			if (ret) {
				return ret;
			}
		}
	}

	if (process_tool) {
		int ret = DoPreproc_ProcessDirectoryAndPopulateClassData(program, global_runtime_data, k_tool_dir, k_gen_tool_dir);

		if (ret) {
			return ret;
		}

		if (modify_tool) {
			ret = DoPreproc_ProcessDirectory(program, global_runtime_data, name, k_tool_dir, k_gen_tool_dir);

			if (ret) {
				return ret;
			}
		}
	}

	if (!file_name.empty()) {
		const std::string rel_path = engine_path + "/" + name + "/" + file_name;
		return DoPreproc_ProcessFile(program, global_runtime_data, engine_path, gen_path, rel_path, !process_pass);
	}

	return static_cast<int>(Error::Success);
}
