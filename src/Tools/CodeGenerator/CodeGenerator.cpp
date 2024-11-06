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

#include "CodeGen_ReflectionHeaderGenerator.h"
#include <Gaff_String.h>
#include <Gaff_Utils.h>
#include <argparse.hpp>

int main(int argc, const char** argv)
{
	argparse::ArgumentParser program("CodeGenerator");

	program.add_argument("--license_file", "-lf")
		.help("(Optional) File containing the license text to put at the top of generated files.");

	program.add_argument("--root_path", "-rp")
		.help("(Optional) The root directory of the project.")
		.default_value<std::string>("../..");

	program.add_argument("--output_files", "-of")
		.help("(Optional) Defines the output file(s) the given operation should write to.")
		.nargs(argparse::nargs_pattern::at_least_one);

	program.add_argument("action")
		.help("Generate action to perform. Examples: module_header, tool_header, static_header");


	ReflectionHeaderGenerator_AddArguments(program);


	try {
		program.parse_args(argc, argv);

	} catch(const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cout << program;
		return -1;
	}

	const std::vector<std::string> output_files = program.get< std::vector<std::string> >("--output_files");
	std::vector<std::filesystem::path> output_file_paths;

	output_file_paths.reserve(output_files.size());

	for (const std::string& path : output_files) {
		output_file_paths.emplace_back(std::filesystem::absolute(path));
	}

	const std::string action = program.get("action");

	if (action == "engine_header" || action == "engine_static_header") {
		Gaff::SetWorkingDir(u8"..");

	} else {
		const std::string working_dir = program.get("--root_path");
		CONVERT_STRING(char8_t, temp, working_dir.data());
		Gaff::SetWorkingDir(temp);
	}

	if (action == "module_header" || action == "tool_header" || action == "static_header" || action == "engine_header" || action == "engine_static_header") {
		return ReflectionHeaderGenerator_Run(program, output_file_paths);
	} else {
		std::cout << program;
	}

	return 0;
}
