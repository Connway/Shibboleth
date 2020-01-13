project "Lua"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h" }
	excludes { "luac.c" }

	SetupConfigMap()
