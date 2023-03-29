DependencyProject "Lua"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h", "**.hpp" }
	removefiles { "luac.c" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()
