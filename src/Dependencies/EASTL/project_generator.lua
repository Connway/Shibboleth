project "EASTL"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C++"
	warnings "Extra"

	files
	{
		"source/EAAssert/*.cpp",
		"source/EAStdC/*.cpp",
		"source/EAThread/*.cpp",
		"source/*.cpp",
		"**.h",
		"**.inl"
	}

	includedirs { "include" }

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }

	filter {}

	SetupConfigMap()
