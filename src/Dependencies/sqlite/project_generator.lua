Group "Dependencies/Unused By Engine"

DepProject "sqlite"
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
