dofile("solution_configs.lua")
dofile("helper_functions.lua")
dofile("options.lua")
dofile("actions.lua")

function Generate(generator)
	dofile(generator)
end

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
	table.foreachi(dependency_generators, Generate)

	group "Frameworks"
	table.foreachi(framework_generators, Generate)

	group "Tests"
	dofile("../tests/project_generator.lua")

	table.foreachi(project_generators, Generate)
