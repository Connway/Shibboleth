Group "Dependencies/Unused by Engine"

DependencyProject "zstd"
	language "C"
	warnings "Extra"

	files { "**.h", "**.c", "**.S" }

	defines { "ZSTD_MULTITHREAD=1" }

	SetupConfigMap()

Group "Dependencies"
