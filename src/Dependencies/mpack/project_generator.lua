DepProject "mpack"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C"
	-- warnings "Default"

	files { "**.c", "**.h", "**.inl", "**.sample" }

	defines { "MPACK_HAS_CONFIG" }

	SetupConfigMap()
