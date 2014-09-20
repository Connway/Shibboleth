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

solution "Temp"
	platforms { "x86", "x64" }

	configurations { "Debug", "Release" }
	warnings "Extra"
	flags { "Unicode", "NoRTTI" }

	nativewchar "Default"
	floatingpoint "Fast"

	filter { "platforms:x86" }
		architecture "x32"
		vectorextensions "SSE2"

	filter { "platforms:x64" }
		architecture "x64"

	filter { "configurations:Debug", "platforms:x86" }
		objdir "build/intermediate"
		targetdir "build/output/x86/Debug"

	filter { "configurations:Debug", "platforms:x64" }
		objdir "build/intermediate"
		targetdir "build/output/x64/Debug"

	filter { "configurations:Release", "platforms:x86" }
		objdir "build/intermediate"
		targetdir "build/output/x86/Release"

	filter { "configurations:Release", "platforms:x64" }
		objdir "build/intermediate"
		targetdir "build/output/x64/Release"

	filter { "language:C++", "action:gmake" }
		buildoptions { "-std=c++11", "-x c++" }

	filter { "configurations:Debug", "action:gmake", "options:not debug_optimization" }
		optimize "Off"

	filter { "configurations:Debug", "action:gmake", "options:debug_optimization" }
		optimize "Debug"

	filter {}

	configuration "vs*"
		buildoptions { "/sdl" }

	configuration "Debug"
		flags { "Symbols" }
		defines { "_DEBUG", "DEBUG" }

	configuration "Release"
		flags { "LinkTimeOptimization", "ReleaseRuntime" }
		optimize "Speed"

	configuration {}

local project_generators = os.matchfiles("dependencies/**/project_generator.lua")
local other_proj_generators = os.matchfiles("src/**/project_generator.lua")

for i = 1, table.getn(other_proj_generators) do
	table.insert(project_generators, other_proj_generators[i])
end

for i = 1, table.getn(project_generators) do
	dofile(project_generators[i])
end

local solution_generators = os.matchfiles("src/**/solution_generator.lua")

for i = 1, table.getn(solution_generators) do
	dofile(solution_generators[i])
end
