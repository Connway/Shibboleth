group "Dependencies/Unused By Engine"

project "sqlite"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h" }
	-- includedirs { "include" }

	-- filter { "system:windows" }
	-- 	defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }

	-- filter {}

	SetupConfigMap()

group "Dependencies"
