Group "Dependencies/Unused By Engine"

DepProject "zstd"
	language "C"
	warnings "Extra"

	files { "**.h", "**.c", "**.S" }

	defines { "ZSTD_MULTITHREAD=1" }

	SetupConfigMap()

Group "Dependencies"
