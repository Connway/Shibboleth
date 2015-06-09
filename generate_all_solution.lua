solution "All"
	if _ACTION then
		location ("project/" .. _ACTION)
	end

	if os.get() == "windows" then
		configurations { "Debug_OpenGL", "Release_OpenGL", "Debug_Direct3D", "Release_Direct3D" }
	else
		configurations { "Debug_OpenGL", "Release_OpenGL" }
	end

	dofile("solution_settings.lua")

	local all_includes = os.matchfiles("src/**/all_solution_include.lua")

	for i = 1, table.getn(all_includes) do
		dofile(all_includes[i])
	end
