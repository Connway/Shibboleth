DependencyProject "Catch2"
	language "C++"
	warnings "Extra"

	files { "**.hpp", "**.c", "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Frameworks/Gaff/include",
		"../../Core/Engine/include",
		"../../Core/Memory/include"
	}

	defines
	{
		"CATCH_AMALGAMATED_CUSTOM_MAIN"
	}

	SetupConfigMap()
