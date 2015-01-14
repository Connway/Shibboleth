group "Dependencies"
	dofile("../../dependencies/jansson/include_external.lua")
	dofile("../../dependencies/Gaff/include_external.lua")

group "Common"
	dofile("../Shared/include_external.lua")
	dofile("../Memory/include_external.lua")

group "Application"
	external "App"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/app")
		end

		if _OPTIONS["console_app"] then
			kind "ConsoleApp"
		else
			kind "WindowedApp"
		end

		language "C++"
