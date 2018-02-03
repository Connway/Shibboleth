newoption
{
	trigger = "debug_optimization",
	description = "If you are generating makefiles, specifies that your gcc version supports the -Og flag."
}

newoption
{
	trigger = "console_app",
	description = "Generates the 'App' project to be a ConsoleApp instead of a WindowApp"
}

newoption
{
	trigger = "fmod_path",
	value = "path",
	description = "The path to your FMOD installation. Use only if FMOD is not already in your global VC++ Directories. (Windows Only)"
}

newoption
{
	trigger = "physx",
	description = "Specify to generate the PhysX build project."
}

newoption
{
	trigger = "gen-clang",
	description = "Generate Clang configurations for Visual Studio projects."
}

-- Options for build action.
newoption
{
	trigger = "config",
	value = "CONFIG",
	description = "Configuration to build. ('build' only)",
	allowed = config_options,
	default = "Debug"
}

newoption
{
	trigger = "project",
	description = "The project to build. (Defaults to VS2017) ('build' only)",
	allowed =
	{
		{ "vs2015", "Visual Studio 2015" },
		{ "vs2017", "Visual Studio 2017" }
	}
}

newoption
{
	trigger = "clean",
	description = "Clean the build outputs. ('build' only)"
}

newoption
{
	trigger = "rebuild",
	description = "Cleans the build outputs and builds the project. ('build' only)"
}
