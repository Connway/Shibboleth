group "Dependencies"
	dofile("../../dependencies/jansson/include_external.lua")
	dofile("../../dependencies/Gleam/include_external.lua")
	dofile("../../dependencies/Gaff/include_external.lua")
	dofile("../../dependencies/glew/include_external.lua")

group "Common"
	dofile("../Shared/include_external.lua")

group "GraphicsModule"
	external "Graphics"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/graphics")
		end

		kind "SharedLib"
		language "C++"
