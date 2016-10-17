project "glew"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	dofile("../../utils/default_configs.lua")
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C"
	warnings "Default"

	defines { "GLEW_STATIC" }

	files { "**.h", "**.c" }
	includedirs { "include" }

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration {}
