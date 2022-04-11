project "Catch2"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C++"
	warnings "Extra"

	files { "**.hpp", "**.c", "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Frameworks/Gaff/include",
		"../../Engine/Engine/include",
		"../../Engine/Memory/include"
	}

	defines
	{
		"DO_NOT_GENERATE_MAIN"
	}

	links
	{
		"Gaff",
		"Engine",
		"Memory",

		"EASTL"
	}

	SetupConfigMap()
