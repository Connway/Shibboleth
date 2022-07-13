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

#include "ProjBuild_BuildProject.h"
#include "ProjBuild_Errors.h"
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <argparse.hpp>

void BuildProject_AddArguments(argparse::ArgumentParser& program)
{
	program.add_argument("--project", "-p")
		.help("The project to build.");

	program.add_argument("--config", "-cf")
		.help("The config of project to build.");

	program.add_argument("--clean", "-cl")
		.help("Clean the project.")
		.default_value(false)
		.implicit_value(true);
}

int BuildProject_Run(const argparse::ArgumentParser& program)
{
#ifdef PLATFORM_WINDOWS
	static constexpr const char* const k_platform_folder = "windows/vs2022";
#elif defined(PLATFORM_MAC)
	static constexpr const char* const k_platform_folder = "macosx/xcode4";
#else // PLATFORM_LINUX
	static constexpr const char* const k_platform_folder = "linux/gmake2";
#endif


	//if (!program.is_used("--project")) {
	//	std::cerr << "--project not specified." << std::endl;
	//	return static_cast<int>(Error::BuildProject_NoProjectSpecified);
	//}

	if (!program.is_used("--config")) {
		std::cerr << "--config not specified." << std::endl;
		return static_cast<int>(Error::BuildProject_NoConfigSpecified);
	}

#ifdef PLATFORM_WINDOWS
	FILE* const pipe = _popen("\"C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -latest -property installationPath", "r");

	if (!pipe) {
		std::cerr << "Failed to run vswhere." << std::endl;
		return static_cast<int>(Error::BuildProject_FailedToOpenVSDir);
	}

	std::string vs_dir;
	char buffer[128] = { 0 };

	while (fgets(buffer, sizeof(buffer), pipe)) {
		vs_dir += buffer;
	}

	_pclose(pipe);

	if (vs_dir.empty()) {
		std::cerr << "Failed to find VS directory." << std::endl;
		return static_cast<int>(Error::BuildProject_FailedToReadVSDir);
	}

	vs_dir.pop_back(); // Remove newline.

	const std::string vcvars = "\"" + vs_dir + "\\VC\\Auxiliary\\Build\\vcvarsall.bat\" amd64";
	std::string msbuild = "msbuild .generated/preproc/project/" + std::string(k_platform_folder) +
		"/Shibboleth-Preproc.sln -m -p:Configuration=" + program.get<std::string>("--config");

	if (program.is_used("--project")) {
		msbuild += " -t:" + program.get<std::string>("--project");

		if (program.get<bool>("--clean")) {
			msbuild += ":Clean";
		}

	} else if (program.get<bool>("--clean")) {
		msbuild += " /t:Clean";
	}

	const std::string final_command = vcvars + " && " + msbuild;

#elif defined(PLATFORM_MAC)
	#error "'build' command not yet implemented for Mac!"
#else // PLATFORM_LINUX
	#error "'build' command not yet implemented for Linux!"
#endif


	return system(final_command.c_str());
}
