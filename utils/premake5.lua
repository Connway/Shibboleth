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

dofile("helper_functions.lua")
dofile("options.lua")

require("premake-qt/qt")

local actions = os.matchfiles("../**/*actions.lua")
table.foreachi(actions, RunFile)

local is_project_action = false
for _,v in pairs(supported_project_actions) do
	if _ACTION == v then
		is_project_action = true
		break
	end
end

if is_project_action == true then
	dofile("solution_configs.lua")

	function GenerateModules(file)
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
				location("../.generated/preproc/" .. os.target() .. "/" .. _ACTION)
			else
				location("../.generated/project/" .. os.target() .. "/" .. _ACTION)
			end
		end

		configurations(configs)

		dofile("solution_settings.lua")

		local dependency_generators = os.matchfiles("../src/Dependencies/**/project_generator.lua")
		local framework_generators = os.matchfiles("../src/Frameworks/**/project_generator.lua")
		local module_generators = os.matchfiles("../src/Modules/**/project_generator.lua")
		local engine_generators = os.matchfiles("../src/Engine/**/project_generator.lua")
		local tools_generators = os.matchfiles("../src/Tools/**/project_generator.lua")

		group "Dependencies"
		table.foreachi(dependency_generators, RunFile)

		group "Frameworks"
		table.foreachi(framework_generators, RunFile)

		group "Core"
		table.foreachi(engine_generators, RunFile)

		group "Modules"
		table.foreachi(module_generators, GenerateModules)

		group "Tools"
		table.foreachi(tools_generators, RunFile)

		group "Tests"
		dofile("../src/Tests/project_generator.lua")

		if _ACTION ~= "gmake2" then
			group "Project Files"
				project "Generators"
					kind "None"
					files { "../src/**.lua", "../src/Dependencies/DEPENDENCY_README.txt" }

				project "Utils"
					kind "None"
					files { "**.lua", "../.gitignore", "../azure-pipelines.yml" }
		end
end
