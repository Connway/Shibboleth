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

-- newoption
-- {
-- 	trigger = "fmod_path",
-- 	value = "path",
-- 	description = "The path to your FMOD installation. Use only if FMOD is not already in your global VC++ Directories. (Windows Only)"
-- }

newoption
{
	trigger = "gen-clang",
	description = "Generate Clang configurations for Visual Studio projects."
}
