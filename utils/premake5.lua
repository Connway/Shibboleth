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
	trigger = "fmod_path",
	value = "path",
	description = "The path to your FMOD installation. Use only if FMOD is not already in your global VC++ Directories. (Windows Only)"
}

newoption
{
	trigger = "brofiler",
	description = "Specify to compile and use the Brofiler intrusive profiler. (Windows Only)"
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

solution "Shibboleth"
	if _ACTION then
		location ("../project/" .. _ACTION)
	end

	if _OPTIONS["gen-clang"] and _ACTION == "vs2015" then
		configurations
		{
			"Debug", "Release",
			"Debug_Clang", "Release_Clang",
			"Analyze",
			"Profile", "Profile_Clang",
			"Optimized_Debug", "Optimized_Debug_Clang"
		}
	elseif _ACTION == "vs2015" then
		configurations
		{
			"Debug", "Release", "Analyze",
			"Profile", "Optimized_Debug"
		}
	else
		configurations
		{
			"Debug", "Release",
			"Profile", "Optimized_Debug"
		}
	end

	dofile("solution_settings.lua")

	local dependency_generators = os.matchfiles("../dependencies/**/project_generator.lua")
	local framework_generators = os.matchfiles("../frameworks/**/project_generator.lua")
	local project_generators = os.matchfiles("../src/**/project_generator.lua")

	group "Dependencies"
	for i = 1, table.getn(dependency_generators) do
		dofile(dependency_generators[i])
	end

	group "Frameworks"
	for i = 1, table.getn(framework_generators) do
		dofile(framework_generators[i])
	end

	group "Tests"
	dofile("../tests/project_generator.lua")

	for i = 1, table.getn(project_generators) do
		dofile(project_generators[i])
	end
