group "Dependencies/Unused By Engine"

project "zstd"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C"
	warnings "Extra"

	files { "**.h", "**.c" }

	defines { "ZSTD_MULTITHREAD=1" }

	SetupConfigMap()

group "Dependencies"
