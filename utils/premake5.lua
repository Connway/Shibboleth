dofile("options.lua")
dofile("helper_functions.lua")

solution "Shibboleth"
	dofile("solution_configs.lua")
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
