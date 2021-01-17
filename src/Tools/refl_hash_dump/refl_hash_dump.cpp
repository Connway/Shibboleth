/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include <Shibboleth_App.h>
//#include <Gaff_MessagePack.h>
//#include <Gaff_String.h>
//#include <Gaff_JSON.h>
//#include <Gaff_File.h>
//#include <cstdio>

int main(int argc, const char** argv)
{
	Shibboleth::App app;

	app.getConfigs().setObject("working_dir", Gaff::JSON::CreateString(".."));
	app.getConfigs().setObject("no_main_loop", Gaff::JSON::CreateBool(true));
	app.getConfigs().setObject("no_managers", Gaff::JSON::CreateBool(true));

	if (!app.init(argc, argv)) {
		app.destroy();
		return -1;
	}

	const Shibboleth::ReflectionManager& refl_mgr = app.getReflectionManager();
	const auto* const class_refl = refl_mgr.getTypeBucket(Gaff::FNV1aHash64Const("*"));

	if (!class_refl) {
		printf("No reflected classes.");
		return 0;
	}

	Gaff::File csv_out;

	if (!csv_out.open("reflection_hashes.csv", Gaff::File::OpenMode::Write)) {
		printf("Failed to open output file.");
		return 0;
	}

	csv_out.printf("Class Name,Hash\n");

	for (const Gaff::IReflectionDefinition* ref_def : *class_refl) {
		const Gaff::Hash64 hash = ref_def->getReflectionInstance().getHash();
		const char* const name = ref_def->getReflectionInstance().getName();

		csv_out.printf("%s,%llu\n", name, hash);
	}

	app.destroy();
	return 0;
}
