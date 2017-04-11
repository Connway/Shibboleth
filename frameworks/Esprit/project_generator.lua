project "Esprit"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/frameworks")
	end

	kind "StaticLib"
	language "C++"

	filter { "configurations:not *Clang", "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../Gleam/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/glm"
	}
