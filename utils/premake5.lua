local unsupported_project_actions =
{
	"vs2005",
	"vs2008",
	"vs2010",
	"vs2012",
	"vs2013",
	"vs2015",
	"vs2017",
	"vs2019",
	"codelite",
	"gmake"
}

local supported_project_actions =
{
	"vs2022",
	"gmake2",
	"xcode4"
}

for _,v in pairs(unsupported_project_actions) do
	if _ACTION == v then
		print("We do not support generating/building on project platform " .. _ACTION)
		return
	end
end

require("premake-qt/qt")

local helper_functions = os.matchfiles("*_functions.lua")
local options = os.matchfiles("*_options.lua")
local actions = os.matchfiles("*_actions.lua")

table.foreachi(helper_functions, dofile)
table.foreachi(options, dofile)
table.foreachi(actions, dofile)

local is_project_action = false
for _,v in pairs(supported_project_actions) do
	if _ACTION == v then
		is_project_action = true
		break
	end
end

if is_project_action == true then
	dofile("solution_configs.lua")

	local generate_modules = function(file)
		local funcs = dofile(file)
		funcs.GenerateProject()
	end

	local workspace_name = "Shibboleth"

	if _OPTIONS["generate-preproc"] then
		workspace_name = "Shibboleth-Preproc"
	end

	workspace(workspace_name)
		if _ACTION then
			if _OPTIONS["generate-preproc"] then
				location("../.generated/preproc/project/" .. os.target() .. "/" .. _ACTION)
			else
				location("../.generated/project/" .. os.target() .. "/" .. _ACTION)
			end
		end

		configurations(configs)

		dofile("solution_settings.lua")

		local dependency_generators = os.matchfiles("../src/Dependencies/**/project_generator.lua")
		local framework_generators = os.matchfiles("../src/Frameworks/**/project_generator.lua")
		local module_generators = os.matchfiles("../src/Modules/**/project_generator.lua")
		local core_generators = os.matchfiles("../src/Core/**/project_generator.lua")
		local tools_generators = os.matchfiles("../src/Tools/**/project_generator.lua")
		local test_generators = os.matchfiles("../src/Tests/**/project_generator.lua")

		Group "Dependencies"
		table.foreachi(dependency_generators, dofile)

		Group "Frameworks"
		table.foreachi(framework_generators, dofile)

		Group "Core"
		table.foreachi(core_generators, dofile)

		Group "Modules"
		table.foreachi(module_generators, generate_modules)

		Group "Tools"
		table.foreachi(tools_generators, dofile)

		Group "Tests"
		table.foreachi(test_generators, dofile)

		if not _OPTIONS["generate-preproc"] then
			if _ACTION ~= "gmake2" then
				group "Project Files"
					project "Utils"
						kind "None"
						files { "**.lua", "../.gitignore", "../azure-pipelines.yml", "../src/Dependencies/DEPENDENCY_README.txt" }
			end
		end
end
