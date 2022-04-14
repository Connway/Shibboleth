project "Engine"
	location(GetEngineLocation())

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		".",
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

	filter { "configurations:*Static*" }
		defines { "SHIB_STATIC" }

	filter {}


	StaticHeaderGen()
	SetupConfigMap()
