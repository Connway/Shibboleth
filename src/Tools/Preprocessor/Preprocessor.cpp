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
#include "Preproc_Errors.h"
#include <Gaff_String.h>
#include <argparse.hpp>

int main(int argc, const char** argv)
{
	argparse::ArgumentParser program("Preprocessor");

	program.add_argument("--root_path", "-rp")
		.help("(Optional) The root directory of the project.")
		.default_value<std::string>("../..");

	program.add_argument("--module", "-m")
		.help("(Optional) Given name is for a module. Looks only in module directory instead of searching.")
		.default_value(false)
		.implicit_value(true);

	program.add_argument("--tool", "-t")
		.help("(Optional) Given name is for a tool. Looks only in tool directory instead of searching.")
		.default_value(false)
		.implicit_value(true);

	program.add_argument("--engine", "-e")
		.help("(Optional) Given name is for an engine module. Looks only in engine directory instead of searching.")
		.default_value(false)
		.implicit_value(true);

	program.add_argument("--file", "-f")
		.help("(Optional) Process a specific file from the given module or tool. Requires --module, --tool, or --engine.");

	program.add_argument("--include", "-i")
		.help("(Optional) Specifies include directories. Paths can be relative or absolute.")
		.default_value< std::vector<std::string> >({})
		.append();

	program.add_argument<std::string>("module_or_tool_name")
		.help("Module or tool to do preprocessing on.");
	
	DoPreproc_AddArguments(program);

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

	if (program.is_used("module_or_tool_name")) {
		DoPreproc_Run(program);
	} else {
		std::cout << program;
	}

	return static_cast<int>(Error::Success);
}
