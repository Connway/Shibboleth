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

#include "ProjBuild_GenerateProject.h"
#include "ProjBuild_Common.h"
#include "ProjBuild_Errors.h"
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <argparse.hpp>
#include <filesystem>

namespace
{
	static constexpr const char* const k_preproc_name = "Preprocessor" TARGET_SUFFIX;
	//static constexpr const char* const k_modules_path = "src/Modules";
	//static constexpr const char* const k_engine_path = "src/Engine";
	//static constexpr const char* const k_tools_path = "src/Tools";

	//struct ModuleData final
	//{
	//	std::vector<std::string> include_dirs;
	//	std::filesystem::path base_path;
	//};

	//struct PreprocData final
	//{
	//	std::vector<ModuleData> modules;
	//};
}

//static int RunPreproc(const argparse::ArgumentParser& /*program*/, const std::filesystem::path& path)
//{
//	std::vector<std::filesystem::path> directories;
//
//	// $TODO: Thread this.
//	for (const auto& entry : std::filesystem::directory_iterator(path)) {
//		// Only care about directory names.
//		if (!entry.is_directory()) {
//			continue;
//		}
//
//		if (path != k_modules_path) {
//			const std::string name = entry.path().stem().string();
//			bool ignore = false;
//
//			for (const char* const ignore_name : k_ignore_list) {
//				if (name == ignore_name) {
//					ignore = true;
//					break;
//				}
//			}
//
//			if (ignore) {
//				continue;
//			}
//		}
//
//		directories.emplace_back(entry.path());
//	}
//
//	char8_t curr_working_dir[2048] = { 0 };
//	Gaff::GetWorkingDir(curr_working_dir, sizeof(curr_working_dir));
//	Gaff::SetWorkingDir(u8"workingdir/tools");
//
//	int ret = static_cast<int>(Error::Success);
//
//	for (const std::filesystem::path& module_path : directories) {
//		std::wcout << L"Running Preprocessor for '" << module_path.filename().c_str() << '\'' << std::endl;
//
//		std::string preproc_cmd = std::string(k_preproc_name) + " " + module_path.filename().string();
//
//		if (path == k_modules_path) {
//			preproc_cmd += " --module";
//		} else if (path == k_tools_path) {
//			preproc_cmd += " --tool";
//		} else if (path == k_engine_path) {
//			preproc_cmd += " --engine";
//		}
//
//		const std::string proj_gen_path = "../../" + module_path.string() + "/project_generator.lua";
//		Gaff::File proj_gen_file(proj_gen_path.c_str(), Gaff::File::OpenMode::ReadBinary);
//
//		if (proj_gen_file.isOpen()) {
//			std::string proj_gen_text;
//
//			proj_gen_text.resize(proj_gen_file.getFileSize());
//			proj_gen_file.readEntireFile(proj_gen_text.data());
//
//			// Find all includedirs calls.
//			size_t pos = proj_gen_text.find("includedirs");
//
//			while (pos != std::string::npos) {
//				pos = proj_gen_text.find('{', pos);
//				const size_t end_pos = proj_gen_text.find('}', pos);
//
//				// Iterate over all include dirs. Ignore source_dir/base_dir additions. We can deduce that from the rest of the path.
//				std::string_view include_dirs = static_cast<std::string_view>(proj_gen_text).substr(pos, end_pos - pos + 1);
//				size_t dir_start = include_dirs.find('"');
//
//				while (dir_start != std::string_view::npos) {
//					size_t dir_end = include_dirs.find('"', dir_start + 1);
//
//					const std::string_view dir = include_dirs.substr(dir_start + 1, dir_end - dir_start - 1);
//					const std::filesystem::path abs_path = std::filesystem::absolute("../../" + module_path.string() + "/" + std::string(dir));
//
//					preproc_cmd += " -i " + abs_path.string();
//
//					dir_start = include_dirs.find('"', dir_end + 1);
//				}
//
//				pos = proj_gen_text.find("includedirs", end_pos);
//			}
//
//			proj_gen_file.close();
//
//		} else {
//			// $TODO: Log warning.
//		}
//
//		std::wcout.flush();
//
//		ret = std::system(preproc_cmd.c_str());
//
//		if (ret) {
//			if (path == k_modules_path) {
//				ret = static_cast<int>(Error::Preproc_FailedToPreprocModule);
//			} else if (path == k_engine_path) {
//				ret = static_cast<int>(Error::Preproc_FailedToPreprocEngine);
//			} else /*if (path == k_tools_path)*/ {
//				ret = static_cast<int>(Error::Preproc_FailedToPreprocTool);
//			}
//
//			break;
//		}
//	}
//
//	Gaff::SetWorkingDir(curr_working_dir);
//	return ret;
//}

void Preproc_AddArguments(argparse::ArgumentParser& /*program*/)
{
}

int Preproc_Run(const argparse::ArgumentParser& /*program*/)
{
	char8_t curr_working_dir[2048] = { 0 };
	Gaff::GetWorkingDir(curr_working_dir, sizeof(curr_working_dir));
	Gaff::SetWorkingDir(u8"workingdir/tools");

	std::wcout.flush();
	const int ret = std::system(k_preproc_name);

	Gaff::SetWorkingDir(curr_working_dir);

	return ret;

	//if (std::filesystem::is_directory(k_modules_path)) {
	//	const int ret = RunPreproc(program, k_modules_path);

	//	if (ret) {
	//		return ret;
	//	}
	//}

	//if (std::filesystem::is_directory(k_tools_path)) {
	//	const int ret = RunPreproc(program, k_tools_path);

	//	if (ret) {
	//		return ret;
	//	}
	//}

	//if (std::filesystem::is_directory(k_engine_path)) {
	//	const int ret = RunPreproc(program, k_engine_path);

	//	if (ret) {
	//		return ret;
	//	}
	//}

	//return static_cast<int>(Error::Success);
}
