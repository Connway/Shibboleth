project "nuklear"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	dofile("../../utils/default_configs.lua")
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"
	flags { "FatalWarnings" }

	files { "**.cpp", "**.h" }

	includedirs
	{
		"../../frameworks/Gaff/include",
		"../../src/Shared/include"
	}
