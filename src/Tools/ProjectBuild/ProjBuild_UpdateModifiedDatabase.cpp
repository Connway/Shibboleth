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
#include <Gaff_JSON.h>
#include <argparse.hpp>
#include <filesystem>

void UpdateModifiedDatabase_AddArguments(argparse::ArgumentParser& /*program*/)
{
}

int UpdateModifiedDatabase_Run(const argparse::ArgumentParser& /*program*/)
{
#ifdef PLATFORM_WINDOWS
	static constexpr const char8_t* const k_platform_folder = u8"windows/vs2022";
#elif defined(PLATFORM_MAC)
	static constexpr const char8_t* const k_platform_folder = u8"macosx/xcode4";
#else // PLATFORM_LINUX
	static constexpr const char8_t* const k_platform_folder = u8"linux/gmake2";
#endif

	static constexpr const char* const k_modules_dir = "src/Modules";
	static constexpr const char* const k_tools_dir = "src/Tools";

	// $TODO: Convert to message pack.
	const std::u8string project_dir = u8".generated/preproc/project/" + std::u8string(k_platform_folder);
	const std::u8string database_file = project_dir + u8"/modified_database.json";

	Gaff::JSON database = Gaff::JSON::CreateObject();

	for (const auto& entry : std::filesystem::recursive_directory_iterator(k_modules_dir)) {
		if (!entry.is_regular_file()) {
			continue;
		}

		const auto root_dir = entry.path().parent_path();
		std::u8string dir = root_dir.u8string();

		size_t pos = dir.find(u8'\\');

		while (pos != std::u8string::npos) {
			dir[pos] = u8'/';
			pos = dir.find(u8'\\');
		}

		if (database.getObject(dir.data()).isNull()) {
			database.setObject(dir.data(), Gaff::JSON::CreateObject());
		}

		Gaff::JSON db_entry = database.getObject(dir.data());
		
		const int64_t write_time = entry.last_write_time().time_since_epoch().count();
		db_entry.setObject(entry.path().filename().u8string().data(), Gaff::JSON::CreateInt64(write_time));
	}

	if (std::filesystem::exists(database_file)) {
		std::filesystem::remove(database_file);
	}

	database.dumpToFile(database_file.data());

	return static_cast<int>(Error::Success);
}
