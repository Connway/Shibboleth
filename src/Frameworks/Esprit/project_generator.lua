project "Esprit"
	if _ACTION then
		location(GetFrameworkLocation())
	end

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../Gleam/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/glm"
	}

	SetupConfigMap()
