Group "Dependencies/Unused By Engine"

DepProject "sqlite"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()

Group "Dependencies"
