group "Dependencies"
	dofile("../../dependencies/jansson/include_external.lua")
	dofile("../../dependencies/Gaff/include_external.lua")

group "Common"
	dofile("../Shared/include_external.lua")

group "Applications"
	external "Contrivance"
		if _ACTION then
			location ("./")
		end

		kind "WindowedApp"
		language "C++"
