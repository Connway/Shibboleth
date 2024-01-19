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

#include "ProjBuild_GenerateHeaders.h"
#include "ProjBuild_Common.h"
#include "ProjBuild_Errors.h"
#include <Gaff_Utils.h>
#include <argparse.hpp>
#include <filesystem>

static constexpr const char* const k_code_gen_name = "CodeGenerator" TARGET_SUFFIX;

static int RunStaticCodeGen(const argparse::ArgumentParser& program)
{
	const std::string code_gen_cmd = std::string(k_code_gen_name) + " static_header --root_path " + program.get("--root_path");

	std::wcout.flush();

	char8_t curr_working_dir[2048] = { 0 };
	Gaff::GetWorkingDir(curr_working_dir, sizeof(curr_working_dir));
	Gaff::SetWorkingDir(u8"workingdir/tools");

	const int ret = std::system(code_gen_cmd.data());

	Gaff::SetWorkingDir(curr_working_dir);

	if (ret) {
		return static_cast<int>(Error::GenerateHeaders_FailedToGenerateStatic);
	}

	return 0;
}

static int RunCodeGen(const argparse::ArgumentParser& program, const std::filesystem::path& path, bool is_tool)
{
	std::vector<std::filesystem::path> directories;

	// $TODO: Thread this.
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		// Only care about directory names.
		if (!entry.is_directory()) {
			continue;
		}

		if (is_tool) {
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
		std::wcout << L"Generating Reflection Header for '" << module_path.filename().wstring() << '\'' << std::endl;

		std::string code_gen_cmd = std::string(k_code_gen_name);

		if (is_tool) {
			code_gen_cmd += " tool_header --no_write_if_empty --tool ";
		} else {
			code_gen_cmd += " module_header --module ";
		}

		code_gen_cmd += module_path.filename().string() + " --root_path " + program.get("--root_path");

		std::wcout.flush();

		ret = std::system(code_gen_cmd.data());

		if (ret) {
			ret = static_cast<int>((is_tool) ? Error::GenerateHeaders_FailedToGenerateTool : Error::GenerateHeaders_FailedToGenerateModule);
			break;
		}
	}

	Gaff::SetWorkingDir(curr_working_dir);
	return ret;
}

void GenerateHeaders_AddArguments(argparse::ArgumentParser& /*program*/)
{
}

int GenerateHeaders_Run(const argparse::ArgumentParser& program)
{
	static constexpr const char* const k_modules_path = "src/Modules";
	static constexpr const char* const k_tools_path = "src/Tools";

	if (std::filesystem::is_directory(k_modules_path)) {
		const int ret = RunCodeGen(program, k_modules_path, false);

		if (ret) {
			return ret;
		}
	}

	if (std::filesystem::is_directory(k_tools_path)) {
		const int ret = RunCodeGen(program, k_tools_path, true);

		if (ret) {
			return ret;
		}
	}

	const int ret = RunStaticCodeGen(program);
	return ret;
}
