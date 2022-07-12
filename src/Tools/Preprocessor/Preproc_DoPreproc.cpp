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

#include "Preproc_DoPreproc.h"
#include "Preproc_ParseFile.h"
#include "Preproc_Errors.h"
#include <Gaff_String.h>
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <argparse.hpp>
#include <filesystem>

int DoPreproc_CopyFile(
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

		if (!Gaff::CreateDir(out_dir.c_str(), 0777)) {
			std::cout << "Failed to create output directory '" << out_dir << "'." << std::endl;
			return static_cast<int>(Error::DoPreproc_FailedToCreateOutputDir);
		}

		pos = output_file_path.find('/', pos + 1);
	}

	// Copy file.
	std::error_code error;
	std::filesystem::copy(path, output_file_path, std::filesystem::copy_options::overwrite_existing, error);

	if (error) {
		std::cout << error.message() << std::endl;
		return static_cast<int>(Error::DoPreproc_FailedToWriteOutputFile);
	}

	return static_cast<int>(Error::Success);
}

int DoPreproc_ProcessFile(
	const argparse::ArgumentParser& program,
	const std::string& /*name*/,
	const std::string& dir,
	const std::string& gen_dir,
	const std::filesystem::path& path)
{
	CONVERT_STRING(char8_t, temp, path.c_str());
	Gaff::File file(temp, Gaff::File::OpenMode::ReadBinary);

	if (!file.isOpen()) {
		std::cout << "Failed to open file '" << path << "'." << std::endl;
		return static_cast<int>(Error::DoPreproc_FailedToOpenInputFile);
	}

	ParseData parse_data;
	std::string file_text;

	file_text.resize(file.getFileSize());
	file.readEntireFile(file_text.data());

	parse_data.include_dirs = program.get< std::vector<std::string> >("--include");
	parse_data.file_text = file_text;

	if (!Preproc_ParseFile(parse_data)) {
		std::cout << "Failed to process file '" << path.string() << "'." << std::endl;
		return static_cast<int>(Error::DoPreproc_FailedToProcessFile);
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

		if (!Gaff::CreateDir(out_dir.c_str(), 0777)) {
			std::cout << "Failed to create output directory '" << out_dir << "'." << std::endl;
			return static_cast<int>(Error::DoPreproc_FailedToCreateOutputDir);
		}

		pos = output_file_path.find('/', pos + 1);
	}

	if (std::filesystem::is_regular_file(output_file_path)) {
		Gaff::File output_file(output_file_path.c_str(), Gaff::File::OpenMode::ReadBinary);

		if (!output_file.isOpen()) {
			std::cout << "Failed to open output file '" << output_file_path << "'." << std::endl;
			return static_cast<int>(Error::DoPreproc_FailedToOpenOutputFile);
		}

		std::string old_output_file_text;
		old_output_file_text.resize(output_file.getFileSize());

		if (!output_file.readEntireFile(old_output_file_text.data())) {
			std::cout << "Failed to read output file '" << output_file_path << "'." << std::endl;
			return static_cast<int>(Error::DoPreproc_FailedToReadOutputFile);
		}

		// Don't write the output file if the resulting output is the same.
		// $TODO: Database of timestamps to make this more optimal? Could also be used for CodeGenerator.
		if (parse_data.out_text == old_output_file_text) {
			return static_cast<int>(Error::Success);
		}

		output_file.close();
	}

	Gaff::File output_file(output_file_path.c_str(), Gaff::File::OpenMode::WriteBinary);

	if (!output_file.isOpen()) {
		std::cout << "Failed to open output file '" << output_file_path << "'." << std::endl;
		return static_cast<int>(Error::DoPreproc_FailedToOpenOutputFile);
	}

	if (!output_file.writeString(parse_data.out_text.c_str())) {
		std::cout << "Failed to write output file '" << output_file_path << "'." << std::endl;
		return static_cast<int>(Error::DoPreproc_FailedToWriteOutputFile);
	}

	return static_cast<int>(Error::Success);
}

int DoPreproc_ProcessDirectory(
	const argparse::ArgumentParser& program,
	const std::string& name,
	const std::string& dir,
	const std::string& gen_dir)
{
	const std::string path = dir + "/" + name;

	if (!std::filesystem::is_directory(path)) {
		std::cout << '"' << path << "\" is not a directory." << std::endl;
		return static_cast<int>(Error::DoPreproc_PathNotFound);
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (!entry.is_regular_file()) {
			continue;
		}

		if (entry.path().filename() == "project_generator.lua") {
			continue;
		}

		//int ret = static_cast<int>(Error::Success);

		if ((entry.path().extension() == ".h" || entry.path().extension() == ".hpp" || entry.path().extension() == ".cpp") &&
			entry.path().filename() != "Gen_ReflectionInit.h" && entry.path().filename() != "Gen_StaticReflectionInit.h") {

			/*ret =*/ DoPreproc_ProcessFile(program, name, dir, gen_dir, entry.path());

		} else {
			/*ret =*/ DoPreproc_CopyFile(program, name, dir, gen_dir, entry.path());
		}

		//if (ret) {
		//	return ret;
		//}
	}

	return static_cast<int>(Error::Success);
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
	const bool force_module = program.get<bool>("--module");
	const bool force_engine = program.get<bool>("--engine");
	const bool force_tool = program.get<bool>("--tool");

	int ret = static_cast<int>(Error::Success);

	if (program.is_used("--file")) {
		const std::string file_name = program.get<std::string>("--file");

		if (force_module) {
			const std::string rel_path = std::string(k_module_dir) + "/" + name + "/" + file_name;
			//const std::filesystem::path abs_path = std::filesystem::absolute(rel_path);

			ret = DoPreproc_ProcessFile(program, name, k_module_dir, k_gen_module_dir, rel_path);

		} else if (force_tool) {
			const std::string rel_path = std::string(k_tool_dir) + "/" + name + "/" + file_name;
			const std::filesystem::path abs_path = std::filesystem::absolute(rel_path);

			ret = DoPreproc_ProcessFile(program, name, k_tool_dir, k_gen_tool_dir, rel_path);

		} else if (force_engine) {
			const std::string rel_path = std::string(k_engine_dir) + "/" + name + "/" + file_name;
			const std::filesystem::path abs_path = std::filesystem::absolute(rel_path);

			ret = DoPreproc_ProcessFile(program, name, k_engine_dir, k_gen_engine_dir, rel_path);
		}

	} else {
		if (force_module) {
			ret = DoPreproc_ProcessDirectory(program, name, k_module_dir, k_gen_module_dir);

		} else if (force_tool) {
			ret = DoPreproc_ProcessDirectory(program, name, k_tool_dir, k_gen_tool_dir);

		} else if (force_engine) {
			ret = DoPreproc_ProcessDirectory(program, name, k_engine_dir, k_gen_engine_dir);

		} else {
			ret = DoPreproc_ProcessDirectory(program, name, k_module_dir, k_gen_module_dir);

			if (ret) {
				ret = DoPreproc_ProcessDirectory(program, name, k_tool_dir, k_gen_tool_dir);
			}

			if (ret) {
				ret = DoPreproc_ProcessDirectory(program, name, k_engine_dir, k_gen_engine_dir);
			}
		}
	}

	return ret;
}
