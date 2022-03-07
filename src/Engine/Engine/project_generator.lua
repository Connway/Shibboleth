project "Engine"
	if _ACTION then
		location(GetEngineLocation())
	end

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl", "**.lua" }

	includedirs
	{
		"include",
		"../Memory/include",
		"../../Dependencies/rapidjson",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/mpack",
		"../../Dependencies/glm",
		"../../Dependencies/zlib-ng",
		"../../Dependencies/libpng",
		"../../Dependencies/libtiff"
	}

	StaticHeaderGen()
	SetupConfigMap()
