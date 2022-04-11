project "Lua"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h" }
	excludes { "luac.c" }

	SetupConfigMap()
