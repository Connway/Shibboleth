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

#include "CodeGen_ModuleGenerator.h"
#include "CodeGen_ReflectionHeaderGenerator.h"
#include "CodeGen_IncludeArgParse.h"
#include <Gaff_Utils.h>
#include <filesystem>

static int CreateProjectFiles(const std::string& /*path*/, const std::string& /*name*/, const argparse::ArgumentParser& /*program*/)
{
	return 0;
}

void ModuleGenerator_AddArguments(argparse::ArgumentParser& /*program*/)
{
	// No arguments to add.
}

int ModuleGenerator_Run(const argparse::ArgumentParser& program)
{
	const std::string action = program.get("action");
	std::string name;
	std::string path;

	if (action == "create_module") {
		if (!program.is_used(k_arg_module)) {
			std::cerr << "Did not provide '--module' parameter." << std::endl;
			return -2;
		}

		name = program.get(k_arg_module);
		path = "src/Modules/" + name;

	} else if (action == "create_tool") {
		if (!program.is_used(k_arg_tool)) {
			std::cerr << "Did not provide '--tool' parameter." << std::endl;
			return -2;
		}

		name = program.get(k_arg_tool);
		path = "src/Tools/" + name;
	
	} else {
		std::cerr << "Unknown action '" << action << "'." << std::endl;
		return -3;
	}

	if (std::filesystem::exists(path) && !std::filesystem::is_directory(path)) {
		std::cerr << "'" << path << "' is not a directory." << std::endl;
		return -4;
	}

	if (const auto out_dir = std::filesystem::absolute(path).u8string(); !Gaff::CreateDir(out_dir.data(), 0777)) {
		std::cerr << "Failed to create output directory '" << reinterpret_cast<const char*>(out_dir.data()) << "'." << std::endl;
		return -5;
	}

	return CreateProjectFiles(path, name, program);
}
