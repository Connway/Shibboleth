group "Dependencies/Unused By Engine"

project "zstd"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C"
	warnings "Extra"

	files { "**.h", "**.c", "**.S" }

	defines { "ZSTD_MULTITHREAD=1" }

	SetupConfigMap()

group "Dependencies"
