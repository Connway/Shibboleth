dofile("solution_configs.lua")
dofile("helper_functions.lua")
dofile("options.lua")

local actions = os.matchfiles("../**/*actions.lua")
table.foreachi(actions, RunFile)

function GenerateModules(file)
	local funcs = dofile(file)
	funcs.GenerateProject()
end

solution "Shibboleth"
	if _ACTION then
		location("../.generated/project/" .. os.target() .. "/" .. _ACTION)
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
