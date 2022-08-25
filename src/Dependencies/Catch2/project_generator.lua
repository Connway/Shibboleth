DepProject "Catch2"
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
		"CATCH_AMALGAMATED_CUSTOM_MAIN"
	}

	SetupConfigMap()
