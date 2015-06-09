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
	trigger = "ltcg",
	description = "Enables Link-Time Code Generation."
}

newoption
{
	trigger = "symbols",
	description = "Builds symbol information into binaries."
}

solution "Temp"
	configurations { "Debug", "Release" }
	dofile("solution_settings.lua")

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

dofile("generate_all_solution.lua")
