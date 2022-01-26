#include "catch_amalgamated.hpp"
#include <Shibboleth_App.h>

#ifdef PLATFORM_WINDOWS
	// Force machines with integrated graphics and discrete GPUs to favor discrete GPUs.
	// https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card/39047129#39047129
	extern "C"
	{
		__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
		__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
	}
#endif

namespace Catch
{
	extern LeakDetector leakDetector;
}

int RunTests(int argc, const char** argv)
{
	Shibboleth::App app;

	const size_t index = Gaff::FindLastOf(argv[0], '\\');
	Gaff::JSON& configs = app.getConfigs();

	if (index != Shibboleth::U8String::npos) {
		Shibboleth::U8String log_dir(Shibboleth::U8String::CtorSprintf(), u8"./tests/logs/%s", argv[0] + index + 1);
		log_dir.erase(log_dir.size() - 4);

		configs.setObject(u8"log_dir", Gaff::JSON::CreateString(log_dir.data()));

	} else {
		configs.setObject(u8"log_dir", Gaff::JSON::CreateString(u8"./tests/logs"));
	}

	configs.setObject(u8"working_dir", Gaff::JSON::CreateString(u8".."));

	// $TODO: Make this configurable.
	// No main loop or modules.
	configs.setObject(u8"no_load_modules", Gaff::JSON::CreateTrue());
	configs.setObject(u8"no_main_loop", Gaff::JSON::CreateTrue());

	if (!app.init(argc, argv)) {
		app.destroy();
		return -1;
	}

	// We want to force the linker not to discard the global variable
	// and its constructor, as it (optionally) registers leak detector
	(void)&Catch::leakDetector;

	const int ret = Catch::Session().run(argc, argv);

	//app.run();
	app.destroy();

	return ret;
}

#ifdef PLATFORM_WINDOWS
#include <shellapi.h>

int CALLBACK WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	int argc = 0;
	const wchar_t* const* const argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	Shibboleth::Vector< Shibboleth::String<char> > args;
	Shibboleth::Vector<const char*> args_raw;
	args_raw.set_capacity(argc);
	args.set_capacity(argc);

	// Load any extra configs and add their values to the _configs object.
	for (int i = 0; i < argc; ++i) {
		const wchar_t* tmp = argv[i];
		CONVERT_STRING(char, arg, tmp);

		args.emplace_back(arg);
		args_raw.emplace_back(args.back().data());
	}

	RunTests(argc, args_raw.data());
}
#endif

int main(int argc, const char** argv)
{
	RunTests(argc, argv);
}
