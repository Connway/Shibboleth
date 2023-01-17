/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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
#include "ProjBuild_Errors.h"
#include <Gaff_Utils.h>
#include <argparse.hpp>
#include <chrono>
#include <thread>

void GenerateProject_AddArguments(argparse::ArgumentParser& program)
{
	program.add_argument("--no-luajit", "-nlj")
		.help("(Optional) Generates the project with the reference Lua interpreter instead of LuaJIT.")
		.implicit_value(true)
		.default_value(false);
}

int GenerateProject_Run(const argparse::ArgumentParser& program)
{
	char8_t curr_working_dir[2048] = { 0 };
	Gaff::GetWorkingDir(curr_working_dir, sizeof(curr_working_dir));
	Gaff::SetWorkingDir(u8"utils");

#ifdef PLATFORM_WINDOWS
	constexpr const char* const k_premake_action = "vs2022";
#elif defined(PLATFORM_MAC)
	constexpr const char* const k_premake_action = "xcode4";
#else
	constexpr const char* const k_premake_action = "gmake2";
#endif

	std::string gen_args = " --generate-preproc";

	if (program.get<bool>("--no-luajit")) {
		gen_args += " --no-luajit";
	}

	std::string proj_gen_cmd = std::string("premake5 ") + k_premake_action + gen_args;

	const int ret = std::system(proj_gen_cmd.data());

	Gaff::SetWorkingDir(curr_working_dir);

	return ret;
}
