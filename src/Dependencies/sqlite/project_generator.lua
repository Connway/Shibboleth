Group "Dependencies/Unused By Engine"

project "sqlite"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()

Group "Dependencies"
