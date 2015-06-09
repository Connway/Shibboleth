group "Dependencies"
	dofile("../../dependencies/jansson/include_external.lua")
	dofile("../../dependencies/Gleam/include_external.lua")
	dofile("../../dependencies/Gaff/include_external.lua")
	dofile("../../dependencies/glew/include_external.lua")

group "Common"
	dofile("../Shared/include_external.lua")
	dofile("../Memory/include_external.lua")

group "Graphics Modules"
	external "RenderPipelinesModule"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/renderpipelinesmodule")
		end

		kind "SharedLib"
		language "C++"
