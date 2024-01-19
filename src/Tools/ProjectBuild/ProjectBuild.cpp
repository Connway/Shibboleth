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

#include "ProjBuild_UpdateModifiedDatabase.h"
#include "ProjBuild_GenerateHeaders.h"
#include "ProjBuild_GenerateProject.h"
#include "ProjBuild_BuildProject.h"
#include "ProjBuild_Preproc.h"
#include "ProjBuild_Errors.h"
#include <Gaff_String.h>
#include <argparse.hpp>

int main(int argc, const char** argv)
{
	argparse::ArgumentParser program("ProjectBuild");

	program.add_argument("--root_path", "-rp")
		.help("(Optional) The root directory of the project.")
		.default_value<std::string>("../..");

	program.add_argument("action")
		.help("Build action to perform. Ex: generate_headers, generate_project, preprocessor, compile, link")
		.default_value<std::string>("all");


	GenerateHeaders_AddArguments(program);
	Preproc_AddArguments(program);
	GenerateProject_AddArguments(program);
	BuildProject_AddArguments(program);
	//Compile_AddArguments(program);
	//Link_AddArguments(program);
	//UpdateModifiedDatabase_AddArguments(program);


	try {
		program.parse_args(argc, argv);

	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cout << program;
		return -1;
	}

	const std::string working_dir = program.get("--root_path");
	CONVERT_STRING(char8_t, temp, working_dir.data());
	Gaff::SetWorkingDir(temp);

	const std::string action = program.get("action");

	if (action == "generate_headers") {
		return GenerateHeaders_Run(program);

	} else if (action == "preprocessor") {
		return Preproc_Run(program);

	} else if (action == "generate_project") {
		return GenerateProject_Run(program);

	} else if (action == "build") {
		return BuildProject_Run(program);

	//} else if (action == "compile") {
		//return Compile_Run(program);

	//} else if (action == "link") {
		//return Link_Run(program);

	//} else if (action == "update_modified_database") {
		//return UpdateModifiedDatabase_Run(program);

	} else if (action == "all") {
		using ActionFunction = int (*)(const argparse::ArgumentParser&);
		constexpr ActionFunction k_action_order[] =
		{
			GenerateHeaders_Run,
			Preproc_Run,
			GenerateProject_Run//,
			//Compile_Run,
			//Link_Run,
			//UpdateModifiedDatabase_Run
		};

		for (ActionFunction func : k_action_order) {
			const int ret = func(program);

			if (ret) {
				return ret;
			}
		}

	} else {
		std::cout << program;
	}

	return static_cast<int>(Error::Success);
}
