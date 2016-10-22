project "imgui"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	defines { "STATIC_PLUGINS" }
	files { "**.h", "**.cpp", "**.inl" }
