dofile("solution_configs.lua")
dofile("helper_functions.lua")
dofile("options.lua")

function RunFile(file)
	dofile(file)
end

local actions = os.matchfiles("*actions.lua")
table.foreachi(actions, RunFile)

solution "Shibboleth"
	if _ACTION then
		location ("../project/" .. _ACTION)
	end

	configurations(configs)

	dofile("solution_settings.lua")

	local dependency_generators = os.matchfiles("../dependencies/**/project_generator.lua")
	local framework_generators = os.matchfiles("../frameworks/**/project_generator.lua")
	local project_generators = os.matchfiles("../src/**/project_generator.lua")

	group "Dependencies"
	table.foreachi(dependency_generators, RunFile)

	group "Frameworks"
	table.foreachi(framework_generators, RunFile)

	group "Tests"
	dofile("../tests/project_generator.lua")

	table.foreachi(project_generators, RunFile)
