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
#include "ProjBuild_Errors.h"
#include <Gaff_Utils.h>
//#include <Gaff_File.h>
#include <argparse.hpp>
//#include <filesystem>

void PreProc_AddArguments(argparse::ArgumentParser& /*program*/)
{
}

int PreProc_Run(const argparse::ArgumentParser& /*program*/)
{
	static constexpr const char* const k_dependencies_dir = "../../Dependencies";
	static constexpr const char* const k_frameworks_dir = "../../Frameworks";

	//static constexpr const char* const k_preproc_modules_path = ".generated/preproc/src/Modules";
	//static constexpr const char* const k_preproc_tools_path = ".generated/preproc/src/Tools";
	//static constexpr const char* const k_preproc_path = ".generated/preproc";
	//static constexpr const char* const k_modules_path = "src/Modules";
	//static constexpr const char* const k_tools_path = "src/Tools";

	//if (!Gaff::CreateDir(k_preproc_modules_path, 0777)) {
	//	return static_cast<int>(Error::PreProc_FailedToCreateModulesDir);
	//}

	//if (!Gaff::CreateDir(k_preproc_tools_path, 0777)) {
	//	return static_cast<int>(Error::PreProc_FailedToCreateToolsDir);
	//}

	return static_cast<int>(Error::Success);
}
