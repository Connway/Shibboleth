project "imgui"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	defines { "STATIC_PLUGINS" }
	files { "**.h", "**.cpp", "**.inl" }
