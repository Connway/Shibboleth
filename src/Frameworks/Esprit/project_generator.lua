FrameworkProject "Esprit"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl" }

	IncludeDirs
	{
		"include",
		"../Gaff/include",
		"../Gleam/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/glm"
	}

	SetupConfigMap()
