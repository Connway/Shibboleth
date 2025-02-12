project "Esprit"
	location(GetFrameworkLocation())

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../Gleam/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/glm"
	}

	SetupConfigMap()
