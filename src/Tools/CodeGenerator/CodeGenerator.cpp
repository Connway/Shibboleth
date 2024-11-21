/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

	program.add_argument("--output_file", "-of")
		.help("(Optional) Defines the output file the given operation should write to.");

	program.add_argument("--print_to_stdout", "-pts")
		.help("(Optional) Prints resulting file to standard output.")
		.default_value<bool>(false);

	program.add_argument("--capture_output_files_pre_cwd_change", "-cofpcc")
		.help("(Optional) Captures the output file paths before changing the current working diretory.")
		.default_value<bool>(true);

	program.add_argument("action")
		.help("Generate action to perform. Examples: module_header, tool_header, engine_header, engine_static_header");

	ReflectionHeaderGenerator_AddArguments(program);


	try {
		program.parse_args(argc, argv);

	} catch(const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cout << program;
		return -1;
	}

	const std::string output_file = program.get<std::string>("--output_file");
	const bool capture_pre_cwd = program.get<bool>("-cofpcc");
	std::filesystem::path output_file_path;

	if (capture_pre_cwd) {
		output_file_path = std::filesystem::absolute(output_file);

		if (output_file_path.empty()) {
			std::cerr << "Failed to find valid absolute path for '" << output_file << "'." << std::endl;
			return -8;
		}
	}

	const std::string working_dir = program.get("--root_path");
	CONVERT_STRING(char8_t, temp, working_dir.data());
	Gaff::SetWorkingDir(temp);

	if (!capture_pre_cwd) {
		output_file_path = std::filesystem::absolute(output_file);

		if (output_file_path.empty()) {
			std::cerr << "Failed to find valid absolute path for '" << output_file << "'." << std::endl;
			return -8;
		}
	}

	const std::string action = program.get<std::string>("action");

	if (action == "module_header" || action == "tool_header" || action == "engine_header" || action == "engine_static_header") {
		return ReflectionHeaderGenerator_Run(program, output_file_path);
	} else {
		std::cout << program;
	}

	return 0;
}
