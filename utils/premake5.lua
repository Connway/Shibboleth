dofile("actions.lua")
dofile("options.lua")
dofile("helper_functions.lua")

function Generate(generator)
	dofile(generator)
end

solution "Shibboleth"
	dofile("solution_configs.lua")
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
