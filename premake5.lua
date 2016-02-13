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
	trigger = "simd",
	description = "Generate solution with defines for using SIMD instructions."
}

newoption
{
	trigger = "simd_set_aligned",
	description = "Generate solution with defines for using SIMDStoreAligned and SIMDLoadAligned instructions."
}

newoption
{
	trigger = "symbols",
	description = "Builds symbol information into binaries."
}

newoption
{
	trigger = "fmod_path",
	value = "path",
	description = "The path to your FMOD installation. Use only if FMOD is not already in your global VC++ Directories. (Windows Only)"
}

newoption
{
	trigger = "brofiler",
	description = "Specify to compile and use the Brofiler intrusive profiler. (Windows Only)"
}

solution "Shibboleth"
	if _ACTION then
		location ("project/" .. _ACTION)
	end

	if os.get() == "windows" then
		configurations { "Debug_OpenGL", "Release_OpenGL", "Debug_Direct3D", "Release_Direct3D" }
	else
		configurations { "Debug_OpenGL", "Release_OpenGL" }
	end

	dofile("solution_settings.lua")

	local dependency_generators = os.matchfiles("dependencies/**/project_generator.lua")
	local project_generators = os.matchfiles("src/**/project_generator.lua")

	group "Dependencies"
	for i = 1, table.getn(dependency_generators) do
		dofile(dependency_generators[i])
	end

	for i = 1, table.getn(project_generators) do
		dofile(project_generators[i])
	end
