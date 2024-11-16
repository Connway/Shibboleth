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

#include <Shibboleth_App.h>

#ifdef _DEBUG
	#include <Gaff_Utils.h>
#endif

namespace
{
	static Shibboleth::App* g_app = nullptr;

	bool InitApp(int argc, const char** argv)
	{
	#ifdef _DEBUG
		for (int i = 0; i < argc; ++i) {
			if (!strcmp(argv[i], "--wait-for-debugger")) {
				while (!Gaff::IsDebuggerAttached()) {
				}

				break;
			}
		}
	#endif

		static Shibboleth::App app;
		g_app = &app;

		// Step out of bin directory into main directory.
		if (char8_t working_dir[1024]; Gaff::GetWorkingDir(working_dir, std::size(working_dir)) && Gaff::EndsWith(working_dir, u8"bin")) {
			Gaff::SetWorkingDir(u8"..");
		}

		if (!app.init(argc, argv)) {
			app.destroy();
			return false;
		}

		return true;
	}

	void RunApp(void)
	{
		g_app->run();
		g_app->destroy();
	}
}

#ifdef PLATFORM_WINDOWS
	#include <shellapi.h>

	// Force machines with integrated graphics and discrete GPUs to favor discrete GPUs.
	// https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card/39047129#39047129
	extern "C"
	{
		__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
		__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	}

	int CALLBACK WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
	{
		int argc = 0;
		const wchar_t* const* const argv_w = CommandLineToArgvW(GetCommandLineW(), &argc);

		Shibboleth::Vector<Shibboleth::U8String> arg_strings(argc);
		Shibboleth::Vector<const char8_t*> args(argc);

		// Load any extra configs and add their values to the _configs object.
		for (int i = 0; i < argc; ++i) {
			CONVERT_STRING(char8_t, temp, argv_w[i]);
			arg_strings[i] = temp;
			args[i] = arg_strings[i].data();
		}

		const char** const argv = reinterpret_cast<const char**>(args.data());

		if (!InitApp(argc, argv)) {
			return -1;
		}

		// Shibboleth::App::init() should not be holding onto references to the command-line args.
		arg_strings.clear();
		args.clear();

		RunApp();

		return 0;
	}

#else
	int main(int argc, const char** argv)
	{
		if (!InitApp(argc, argv)) {
			return -1;
		}

		RunApp();

		return 0;
	}
#endif
