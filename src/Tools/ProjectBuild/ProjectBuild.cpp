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

#include "ProjBuild_Generate.h"
#include <Gaff_String.h>
#include <argparse.hpp>

int main(int argc, const char** argv)
{
	argparse::ArgumentParser program("ProjectBuild");

	program.add_argument("--root_path", "-rp")
		.help("(Optional) The root directory of the project.")
		.default_value<std::string>("../..");

	program.add_argument("action")
		.help("Build action to perform. Ex: generate, preprocessor, compile, link")
		.default_value<std::string>("all");


	Generate_AddArguments(program);
	//PreProc_AddArguments(program);
	//Compile_AddArguments(program);
	//Link_AddArguments(program);


	try {
		program.parse_args(argc, argv);

	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return -1;
	}

	const std::string working_dir = program.get("--root_path");
	CONVERT_STRING(char8_t, temp, working_dir.c_str());
	Gaff::SetWorkingDir(temp);

	const std::string action = program.get("action");

	if (action == "generate") {
		Generate_Run(program);

	} else if (action == "preprocessor") {
		//PreProc_Run(program);

	} else if (action == "compile") {
		//Compile_Run(program);

	} else if (action == "link") {
		//Link_Run(program);

	} else if (action == "all") {
		Generate_Run(program);
		//PreProc_Run(program);
		//Compile_Run(program);
		//Link_Run(program);

	} else {
		std::cout << program;
	}

	return 0;
}
