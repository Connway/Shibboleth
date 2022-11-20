DepProject "Lua"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h", "**.hpp" }
	excludes { "luac.c" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()
