project "Catch2"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C++"
	warnings "Extra"

	files { "**.hpp", "**.c", "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/optick",
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

		"EASTL",
		"optick"
	}

	SetupConfigMap()
