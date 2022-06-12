DepProject "Lua"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h" }
	excludes { "luac.c" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()
