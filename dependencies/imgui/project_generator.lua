project "imgui"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C++"

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	defines { "STATIC_PLUGINS" }
	files { "**.h", "**.cpp", "**.inl" }
