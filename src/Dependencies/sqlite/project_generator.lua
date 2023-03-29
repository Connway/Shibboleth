Group "Dependencies/Unused by Engine"

DependencyProject "sqlite"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()

Group "Dependencies"
