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

#include "ProjBuild_GenerateProject.h"
#include "ProjBuild_Common.h"
#include "ProjBuild_Errors.h"
#include <Gaff_Utils.h>
#include <argparse.hpp>
#include <filesystem>

static constexpr const char* const k_preproc_name = "Preprocessor" TARGET_SUFFIX;
static constexpr const char* const k_modules_path = "src/Modules";
static constexpr const char* const k_engine_path = "src/Engine";
static constexpr const char* const k_tools_path = "src/Tools";

static int RunPreproc(const argparse::ArgumentParser& /*program*/, const std::filesystem::path& path)
{
	std::vector<std::filesystem::path> directories;

	// $TODO: Thread this.
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		// Only care about directory names.
		if (!entry.is_directory()) {
			continue;
		}

		if (path != k_modules_path) {
			const std::string name = entry.path().stem().string();
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
		}

		directories.emplace_back(entry.path());
	}

	char8_t curr_working_dir[2048] = { 0 };
	Gaff::GetWorkingDir(curr_working_dir, sizeof(curr_working_dir));
	Gaff::SetWorkingDir(u8"workingdir/tools");

	int ret = static_cast<int>(Error::Success);

	for (const std::filesystem::path& module_path : directories) {
		std::wcout << L"Running Preprocessor for '" << module_path.filename().c_str() << '\'' << std::endl;

		std::string preproc_cmd = std::string(k_preproc_name) + " " + module_path.filename().string();

		if (path == k_modules_path) {
			preproc_cmd += " --module";
		} else if (path == k_tools_path) {
			preproc_cmd += " --tool";
		} else if (path == k_engine_path) {
			preproc_cmd += " --engine";
		}

		std::wcout.flush();

		ret = std::system(preproc_cmd.c_str());

		if (ret) {
			if (path == k_modules_path) {
				ret = static_cast<int>(Error::Preproc_FailedToPreprocModule);
			} else if (path == k_engine_path) {
				ret = static_cast<int>(Error::Preproc_FailedToPreprocEngine);
			} else /*if (path == k_tools_path)*/ {
				ret = static_cast<int>(Error::Preproc_FailedToPreprocTool);
			}

			break;
		}
	}

	Gaff::SetWorkingDir(curr_working_dir);
	return ret;
}

void Preproc_AddArguments(argparse::ArgumentParser& /*program*/)
{
}

int Preproc_Run(const argparse::ArgumentParser& program)
{
	if (std::filesystem::is_directory(k_modules_path)) {
		const int ret = RunPreproc(program, k_modules_path);

		if (ret) {
			return ret;
		}
	}

	if (std::filesystem::is_directory(k_tools_path)) {
		const int ret = RunPreproc(program, k_tools_path);

		if (ret) {
			return ret;
		}
	}

	if (std::filesystem::is_directory(k_engine_path)) {
		const int ret = RunPreproc(program, k_engine_path);

		if (ret) {
			return ret;
		}
	}

	return static_cast<int>(Error::Success);
}
